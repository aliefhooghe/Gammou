
#include "vst2_plugin.h"

#include "gui/main_gui.h"
#include "plugin_system/package_loader.h"
#include "synthesizer/midi_parser.h"
#include "backends/common/configuration.h"

#include <algorithm>
#include <cstdlib>

namespace Gammou {

    vst2_plugin::vst2_plugin(audioMasterCallback master)
    :   _synthesizer{_llvm_context},
        _node_factory{_llvm_context}
    {
        //  Allocate effect instance
        _effect = static_cast<AEffect*>(std::malloc(sizeof(AEffect)));
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

        //  Load packages into factory
        load_all_packages(get_packages_directory_path(), _node_factory);

        //  Prepare synthesizer
        _synthesizer.add_module(_node_factory.module());

        //  Build gui
        _window = make_synthesizer_gui(_synthesizer, _node_factory);

        //  Initialize display
        _display = std::make_unique<View::native_vst2_display>(*_window, 12);

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

            case effCanDo:
                return 1;
                break;

            case effEditMouse:
                return 1;
                break;

            case effEditKey:
                return 1;
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

        auto output_left_buffer = outputs[0];
        auto output_right_buffer = outputs[1];

        for (auto i = 0u; i < sample_count; ++i) {
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