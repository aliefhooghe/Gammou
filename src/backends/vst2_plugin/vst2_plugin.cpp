
#include "vst2_plugin.h"
#include "synthesizer/midi_parser.h"
#include "backends/common/default_configuration.h"

#include <DSPJIT/log.h>

#include <algorithm>
#include <cstdlib>
#include <cstring>

namespace Gammou {

    vst2_plugin::vst2_plugin(audioMasterCallback master)
    :   _synthesizer{_llvm_context, synthesizer::configuration{}},
        _master_callback{master}
    {
        //  Allocate effect instance
        _effect = static_cast<AEffect*>(std::malloc(sizeof(AEffect)));

        if (_effect == nullptr)
            throw std::bad_alloc();

        std::memset(_effect, 0u, sizeof(AEffect));

        _effect->magic = kEffectMagic;
        _effect->dispatcher = dispatcher_proc;
        _effect->setParameter = set_parameter_proc;
        _effect->getParameter = get_parameter_proc;
        _effect->numParams = 0u;
        _effect->numInputs = _synthesizer.get_input_count();
        _effect->numOutputs = _synthesizer.get_output_count();

        _effect->flags = effFlagsHasEditor | effFlagsCanReplacing |
                         effFlagsProgramChunks | effFlagsIsSynth/* |
                         effFlagsCanDoubleReplacing*/;

        _effect->user = this;
        _effect->uniqueID = 0xFA5DAD42u;    //  todo better ?
        _effect->version = kVstVersion;
        _effect->processReplacing = process_replacing_proc;

        //  Build application with default settings
        const application::configuration options
        {
            default_configuration::get_packages_directory_path(),
            default_configuration::get_patch_path()
        };

        _application = std::make_unique<application>(options, _synthesizer);

        //  Initialize display
        _display = View::create_vst2_display(_application->main_gui(), 1);
    }

    AEffect *vst2_plugin::create_AEffect_instance(audioMasterCallback master)
    {
        try {
            auto plugin = new vst2_plugin(master);
            return plugin->_effect;
        }
        catch (...)
        {
            // Avoid throwing a c++ exception to a non c++ program
            return nullptr;
        }
    }

    intptr_t vst2_plugin::dispatcher_proc(
        AEffect *fx, int32_t opcode,
        int32_t index, intptr_t value,
        void *ptr, float opt)
    {
        auto plugin =
            reinterpret_cast<vst2_plugin*>(fx->user);

        switch (opcode)
        {
            case effOpen: break;

            case effClose:
                delete plugin;
                break;

            case effEditGetRect:
            {
                ERect **rect_ref = (ERect **)ptr;
                plugin->_update_windows_rect();
                *rect_ref = &(plugin->_window_rect);
            }
                break;

            case effEditOpen:
                plugin->_display->open(ptr);
                break;

            case effEditClose:
                plugin->_display->close();
                break;

            case effProcessEvents:
            {
                auto list =
                    reinterpret_cast<const VstEvents*>(ptr);

                std::for_each_n(
                    list->events, list->numEvents,
                    [plugin](const VstEvent *ev) { plugin->_handle_event(*ev); }
                );
            }
            break;

            case effGetChunk:
            {
                if (index == 0)
                    return plugin->_save_state((void **)ptr);
            }
            break;

            case effSetChunk:
            {
                if (index == 0)
                    return plugin->_load_state(ptr, value);
            }
            break;

            case effCanDo:
                return 1;
                break;

            case effEditMouse:
                return 1;
                break;

            case effEditKeyDown:
            {
                const bool used = plugin->_display->text_input(index, value, opt);
                return static_cast<int>(used);
            }
                break;

            case effEditKeyUp:
                return 0;
                break;

            case effGetNumMidiInputChannels:
                return 1;
                break;

            case effGetVendorString:
                std::strcpy(reinterpret_cast<char*>(ptr), "Arthur Liefhooghe");
                break;

            case effGetPlugCategory:
                return kPlugCategSynth;
                break;

            case effGetEffectName:
                std::strcpy(reinterpret_cast<char*>(ptr), "Gammou");
                break;

            case effSetBlockSizeAndSampleRate:
            case effSetSampleRate:
                LOG_INFO("[vst2_plugin) Set samplerate to %f\n", opt);
                plugin->_synthesizer.set_sample_rate(opt);
                break;

            default:
                return 0u;
        }

        return 0u;
    }

    void vst2_plugin::set_parameter_proc(
        AEffect *, int32_t, float)
    {

    }

    float vst2_plugin::get_parameter_proc(AEffect *fx, int32_t index)
    {
        return 0.f;
    }

    void vst2_plugin::process_replacing_proc(
        AEffect *fx, float ** /*inputs*/, float **outputs, int32_t sample_count)
    {
        auto plugin =
            reinterpret_cast<vst2_plugin*>(fx->user);
        plugin->_synthesizer.update_program();

        auto output_left_buffer = outputs[0];
        auto output_right_buffer = outputs[1];

        for (auto i = 0; i < sample_count; ++i) {
            float tmp[2];
            plugin->_synthesizer.process_sample(nullptr, tmp);
            output_left_buffer[i] = tmp[0];
            output_right_buffer[i] = tmp[1];
        }
    }

    void vst2_plugin::_call_master_callback(
            int32_t opcode,
            int32_t index,
            intptr_t value,
            void *ptr,
            float opt)
    {
        _master_callback(_effect, opcode, index, value, ptr, opt);
    }

    void vst2_plugin::_handle_event(const VstEvent &ev)
    {
        if (ev.type == kVstMidiType) {
            auto midi_ev = reinterpret_cast<const VstMidiEvent*>(&ev);
            execute_midi_msg(
                _synthesizer,
                reinterpret_cast<const uint8_t*>(midi_ev->midiData),
                4u);
        }
    }

    std::size_t vst2_plugin::_load_state(const void *chunk, std::size_t size)
    {
        LOG_INFO("Loading VST2 state (size = %llu)\n", size);
        const auto data = reinterpret_cast<const uint8_t*>(chunk);

        try {
            const auto json_object = nlohmann::json::from_cbor(data, data + size);
            _application->deserialize(json_object);
            return size;
        }
        catch(const std::exception& e)
        {
            LOG_ERROR("Failed to deserialize vst2 state: %s\n", e.what());
            return 0u;
        }
        catch(...)
        {
            LOG_ERROR("Failed to deserialize vst2 state: unknown error\n");
            return 0u;
        }
    }

    std::size_t vst2_plugin::_save_state(void **chunk_ptr)
    {
        LOG_INFO("Save VST2 state\n");

        // For VST serialize json state as cbor (Concise Binary Object Representation)
        try {

            const auto json_object = _application->serialize();
            const auto binary_data = nlohmann::json::to_cbor(json_object);

            if ((*chunk_ptr = std::malloc(binary_data.size())) == nullptr)
                throw std::bad_alloc();

            std::memcpy(*chunk_ptr, binary_data.data(), binary_data.size());
            LOG_INFO("Successfully saved VST2 sate (size = %llu)\n", binary_data.size());

            return binary_data.size();
        }
        catch(const std::exception& e)
        {
            LOG_ERROR("Failed to save VST2 state: %s\n", e.what());
            return 0u;
        }
        catch(...)
        {
            LOG_ERROR("Failed to save VST2 state: unknown error\n");
            return 0u;
        }
    }

    void vst2_plugin::_update_windows_rect()
    {
        _window_rect.left = 0u;
        _window_rect.right = _display->px_width();
        _window_rect.top = 0u;
        _window_rect.bottom = _display->px_height();
    }

} // namespace Gammou

/*
 *
 *      Export a VST2 compatible ABI
 *
 */

extern "C" {

    AEffect *VSTPluginMain(audioMasterCallback master)
    {
        return Gammou::vst2_plugin::create_AEffect_instance(master);
    }

}