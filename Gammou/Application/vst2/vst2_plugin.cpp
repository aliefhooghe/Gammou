
#include "vst2_plugin.h"

#define MIDI_NOTE_OFF	0x80u
#define MIDI_NOTE_ON	0x90u

namespace Gammou  {
    
    namespace VST2 {

        plugin::plugin()
        :   m_synthesizer_mutex(),
            m_master_circuit_processor(),
            m_polyphonic_circuit_processor(),
            m_synthesizer(
                m_master_circuit_processor,
                m_polyphonic_circuit_processor,
                GAMMOU_VST2_INPUT_COUNT,
                GAMMOU_VST2_OUTPUT_COUNT,
                GAMMOU_VST2_CHANNEL_COUNT,
                GAMMOU_VST2_PARAMETER_COUNT),
            m_gui(&m_synthesizer, &m_synthesizer_mutex),
            m_display(m_gui)
        {
            DEBUG_PRINT("Gammou Vst2 Plugin CTOR\n");

			//	Initialize effect instance

            m_aeffect = new AEffect;

            m_aeffect->magic = kEffectMagic;
            m_aeffect->dispatcher = dispatcher_proc;
            m_aeffect->process = nullptr;

            m_aeffect->setParameter = set_parameter_proc;
            m_aeffect->getParameter = get_parameter_proc;

            m_aeffect->numPrograms = 0u;
            m_aeffect->numParams = GAMMOU_VST2_PARAMETER_COUNT;
            m_aeffect->numInputs = GAMMOU_VST2_INPUT_COUNT;
            m_aeffect->numOutputs = GAMMOU_VST2_OUTPUT_COUNT;

            m_aeffect->flags = 
                effFlagsHasEditor       |
                effFlagsCanReplacing    |
                effFlagsProgramChunks   |
                effFlagsIsSynth         |
                effFlagsCanDoubleReplacing;

            m_aeffect->resvd1 = 0u;
            m_aeffect->resvd2 = 0u;
            m_aeffect->initialDelay = 0u;
            m_aeffect->realQualities = 0u;
            m_aeffect->offQualities = 0u;
            m_aeffect->ioRatio = 0.0;
            m_aeffect->object = nullptr;
            m_aeffect->user = this;
            m_aeffect->uniqueID = 1342151;  //  TODO
            m_aeffect->version = kVstVersion;
            m_aeffect->processReplacing = process_replacing_proc;
            m_aeffect->processDoubleReplacing = process_double_replacing_proc;
        
			//	Initialize window rect
			m_window_rect.left = 0;
			m_window_rect.right = m_display.get_display_width();
			m_window_rect.top = 0;
			m_window_rect.bottom = m_display.get_display_height();
		}

        plugin::~plugin()
        {

        }

        AEffect *plugin::get_AEffect_instance()
        {
            return m_aeffect;
        }

		void plugin::handle_event(VstEvent & ev)
		{
			if (ev.type != kVstMidiType)
				return;

			VstMidiEvent* midi_ev = (VstMidiEvent*)(&ev);

			// ignore channel
			const uint8_t cmd =
				(midi_ev->midiData[0] & 0xf0);

			const uint8_t note =
				(midi_ev->midiData[1] & 0x7f);

			const uint8_t velocity =
				(midi_ev->midiData[2] & 0x7f);

			if (cmd == MIDI_NOTE_OFF ||
				(cmd == MIDI_NOTE_ON && velocity == 0u))
				m_synthesizer.send_note_off(note, static_cast<float>(velocity) / 127.0f);
			else if (cmd == MIDI_NOTE_ON)
				m_synthesizer.send_note_on(note, static_cast<float>(velocity) / 127.0f);
		}

		void plugin::get_param_name(char * str, const unsigned int index)
		{
			std::string param_name = ("Parameter " + std::to_string(index));
			std::strcpy(str, param_name.c_str());
		}

        AEffect *plugin::create_AEffect_instance()
        {
            plugin *p = new plugin;
            return p->get_AEffect_instance();
        }

        intptr_t plugin::dispatcher_proc(
            AEffect *fx, 
            int32_t opcode, 
            int32_t index, 
            intptr_t value, 
            void *ptr, 
            float opt)
        {
            plugin *self = (plugin*)(fx->user);

            switch (opcode) {

                case effOpen:
                    //  TODO : no arg
                    DEBUG_PRINT("Effect Open\n");
                    break;

                case effClose:
                    //  TODO : no arg
                    DEBUG_PRINT("Effect CLose\n");
					delete self;
                    break;

                case effGetParamLabel:
                    //  TODo : param, 0, str
                    DEBUG_PRINT("GetParamLabel\n");
                    break;

                case effGetParamDisplay:
                    //  TODO : param, 0, str
                    DEBUG_PRINT("GetParamDisplay\n");
                    break;

                case effGetParamName:
                    //  TODO : param, 0, str
                    DEBUG_PRINT("GetParamName\n");
					self->get_param_name((char*)ptr, index);
                    break;

                case effSetSampleRate:
                    DEBUG_PRINT("SampleRate set to %u\n", (unsigned int)opt);
                    self->m_synthesizer.set_sample_rate(opt);
                    break;

                case effSetBlockSize:
                    //  TODO opt = blocksize
                    DEBUG_PRINT("Block Size set to %u\n", (unsigned int)opt);
                    break;

                case effMainsChanged:
                    DEBUG_PRINT("Edit Main Changed\n");
                    break;

                case effEditGetRect:
                {
					ERect **rect_ref = (ERect**)ptr;
					*rect_ref = &(self->m_window_rect);
                    DEBUG_PRINT("Edit Get Rect\n");
                }
                    break;

                case effEditOpen:
                    //  TODO : arg = 0, 0, winid, 0
                    DEBUG_PRINT("Edit Open\n");
                    self->m_display.open(ptr);
                    break;

                case effEditClose:
                    DEBUG_PRINT("Edit Close\n");
                    self->m_display.close();
                    break;

                case effEditIdle:
                    DEBUG_PRINT("effEditIdle\n");
                    break;

                case effEditTop:
                    DEBUG_PRINT("effEditTop\n");
                    break;

                case effGetChunk:
                    //  TODO : arg 0 0 &ptr
                    DEBUG_PRINT("Get chunk\n");
                    break;

                case effSetChunk:
                    //  TODO : args = 0, size, data
                    DEBUG_PRINT("Set chunks\n");
                    break;

                case effProcessEvents:
				{
					VstEvents *list = (VstEvents*)ptr;

					const unsigned int event_count =
						list->numEvents;
					VstEvent **events =
						list->events;

					for (unsigned int i = 0; i < event_count; ++i) 
						self->handle_event(*(events[i]));
	
					DEBUG_PRINT("Received events\n");
				}
                    break;

                case effStartProcess:
                    DEBUG_PRINT("Start Process\n");
                    break;

                case effStopProcess:
                    DEBUG_PRINT("Stop Process\n");
                    break;
                    
                case effSetProcessPrecision:
                    DEBUG_PRINT("Precision was set to %u bit\n", 
                        value == kVstProcessPrecision64 ? 64 : 32);
                    break;

                default:
                    DEBUG_PRINT("Unknown VST opcode : %u\n", opcode);
                    break;
            }

            return 0u;
        }

        void plugin::set_parameter_proc(
            AEffect *fx, 
            int32_t index,
            float value)
        {
            plugin *self = (plugin*)(fx->user);
            self->m_synthesizer.set_parameter_value(value, index);
        }

        float plugin::get_parameter_proc(
            AEffect *fx,
            int32_t index)
        {
            plugin *self = (plugin*)(fx->user);
            return self->m_synthesizer.get_parameter_value(index);
        }

        void plugin::process_replacing_proc(
            AEffect *fx,
            float **inputs,
            float **outputs,
            int32_t sample_count)
        {
            plugin *self = (plugin*)(fx->user);
            auto& synthesizer = self->m_synthesizer;

            for (int i = 0; i < sample_count; ++i) {
                double input[2] = {inputs[0][i], inputs[1][i]};
                double output[2];
                synthesizer.process(input, output);
                outputs[0][i] = static_cast<float>(output[0]);
                outputs[1][i] = static_cast<float>(output[1]);
            }
        }

        void plugin::process_double_replacing_proc(
            AEffect *fx,
            double **inputs,
            double **outputs,
            int32_t sample_count)
        {
            plugin *self = (plugin*)(fx->user);
            auto& synthesizer = self->m_synthesizer;

            for (int i = 0; i < sample_count; ++i) {
                double input[2] = {inputs[0][i], inputs[1][i]};
                double output[2];
                synthesizer.process(input, output);
                outputs[0][i] = output[0];
                outputs[1][i] = output[1];
            }
        }

    }   /*  vst2 */

}   /* Gammou */


extern "C" {

    AEffect* VSTPluginMain(audioMasterCallback audioMaster)
    {
        return Gammou::VST2::plugin::create_AEffect_instance();
    }

}