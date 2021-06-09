
#include "vst2_plugin.h"

#include "gui/main_gui.h"
#include "plugin_system/package_loader.h"
#include "builtin_plugins/load_builtin_plugins.h"
#include "gui/control_node_widgets/load_control_plugins.h"
#include "synthesizer/midi_parser.h"
#include "backends/common/configuration.h"

#include <algorithm>
#include <cstdlib>
#include <cstring>

namespace Gammou {

    vst2_plugin::vst2_plugin(audioMasterCallback master)
    :   _synthesizer{_llvm_context},
        _node_factory{_llvm_context}
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

        //  Load builtin plugins into factory
        load_builtin_plugins(_node_factory);

        //  Load control plugins into factory
        load_control_plugins(_synthesizer, _node_factory);

        //  Load packages into factory
        load_all_packages(configuration::get_packages_directory_path(), _node_factory);

        //  Prepare synthesizer
        _synthesizer.add_module(_node_factory.module());

        //  Build gui
        _main_gui = std::make_unique<main_gui>(_synthesizer, _node_factory);

        //  Initialize display
        _display = View::create_vst2_display(_main_gui->widget(), 1);

        _window_rect.left = 0u;
        _window_rect.right = _display->px_width();
        _window_rect.top = 0u;
        _window_rect.bottom = _display->px_height();
    }

    AEffect *vst2_plugin::create_AEffect_instance(audioMasterCallback master)
    {
        auto plugin = new vst2_plugin(master);
        return plugin->_effect;
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

            case effEditKey:
                return 1;
                break;

            case effGetNumMidiInputChannels:
                return 1;
                break;

            case effGetVendorString:
                std::strcpy(reinterpret_cast<char*>(ptr), "Arthur Liefhooghe");
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

            if (_main_gui->deserialize(json_object))
                return size;
            else
                return 0u;
        }
        catch(const std::exception& e)
        {
            LOG_ERROR("Failed to load VST2 state : %s\n", e.what());
            return 0u;
        }
        catch(...)
        {
            LOG_ERROR("Failed to load VST2 state : unknown error\n");
            return 0u;
        }
    }

    std::size_t vst2_plugin::_save_state(void **chunk_ptr)
    {
        LOG_INFO("Save VST2 state\n");

        // For VST serialize json state as cbor (Concise Binary Object Representation)
        try {

            const auto json_object = _main_gui->serialize();
            const auto binary_data = nlohmann::json::to_cbor(json_object);

            if ((*chunk_ptr = std::malloc(binary_data.size())) == nullptr)
                throw std::bad_alloc();

            std::memcpy(*chunk_ptr, binary_data.data(), binary_data.size());
            LOG_INFO("Successfully saved VST2 sate (size = %llu)\n", binary_data.size());

            return binary_data.size();
        }
        catch(const std::exception& e)
        {
            LOG_ERROR("Failed to save VST2 state : %s\n", e.what());
            return 0u;
        }
        catch(...)
        {
            LOG_ERROR("Failed to save VST2 state (unknown error)\n");
            return 0u;
        }
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