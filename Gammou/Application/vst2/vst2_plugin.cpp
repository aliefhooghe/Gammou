
#include "vst2_plugin.h"


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
            m_midi_driver(m_synthesizer),
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
                effFlagsHasEditor         |
                effFlagsCanReplacing      |
                effFlagsProgramChunks     |
                effFlagsIsSynth           |
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
			m_midi_driver.handle_midi_event(midi_ev->midiData);
		}

		void plugin::get_param_name(char * str, const unsigned int index)
		{
			std::string param_name = ("Parameter " + std::to_string(index));
			std::strcpy(str, param_name.c_str());
		}

		unsigned int plugin::save_state(void **data)
		{
			//	reset buffer
            m_chunk_buffer.resize(0);

            Persistence::gammou_state state;

            try {
                m_gui.save_state(state);

                //  Write state in chunk buffer
                Persistence::buffer_output_stream stream(m_chunk_buffer);
                Persistence::gammou_file<Persistence::gammou_state>::save(stream, state);

                const unsigned int data_size =
                    static_cast<unsigned int>(m_chunk_buffer.size());

                if ((*data = std::malloc(data_size)) == nullptr)
                    return 0u;

                //	Copy chunk buffer data to allocated chunk
                std::memcpy(*data, m_chunk_buffer.data(), data_size);

                return data_size;
            }
            catch(...)
            {
                return 0;
            }
        }

		unsigned int plugin::load_state(void *data, const unsigned int size)
		{
			raw_data_source source(data, size);
            Persistence::gammou_state state;

            try {
                Persistence::gammou_file<Persistence::gammou_state>::load(source, state);
                m_gui.load_state(state);
                return size;
            }
            catch(...) {
                return 0;
            }

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
                    DEBUG_PRINT("Effect Open\n");
                    break;

                case effClose:
                    DEBUG_PRINT("Effect CLose\n");
					delete self;
                    break;

                case effGetParamLabel:
                    DEBUG_PRINT("GetParamLabel\n");
                    break;

                case effGetParamDisplay:
                    DEBUG_PRINT("GetParamDisplay\n");
                    break;

                case effGetParamName:
					self->get_param_name((char*)ptr, index);
                    break;

                case effSetSampleRate:
                    self->m_synthesizer.set_sample_rate(opt);
                    break;

                case effEditGetRect:
                {
					ERect **rect_ref = (ERect**)ptr;
					*rect_ref = &(self->m_window_rect);
                    DEBUG_PRINT("Edit Get Rect\n");
                }
                    break;

                case effEditOpen:
                    DEBUG_PRINT("Edit Open\n");
                    self->m_display.open(ptr);
                    break;

                case effEditClose:
                    DEBUG_PRINT("Edit Close\n");
                    self->m_display.close();
                    break;

                case effGetChunk:
                    DEBUG_PRINT("Get chunk (index = %u)\n", index);
					if (index == 0)
						return self->save_state((void**)ptr);
                    break;

                case effSetChunk:
					if (index == 0)
						return self->load_state(ptr, value);
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
				}
                    break;

                case effCanBeAutomated:
                    return 1;   //  Every Parameters Can be automated
                    break;

                case effGetVendorString:
                    strcpy((char*)ptr, "Arthur Liefhooghe");
                    break;

                case effCanDo:
                    DEBUG_PRINT("Got Cando '%s' ?\n", (char*)ptr);
                    break;
                
                case effGetNumMidiInputChannels:
                    DEBUG_PRINT("effGetNumMidiInputChannels received\n");
                    return 1;
                    break;

                default:
                   // DEBUG_PRINT("Unknown VST opcode : %u\n", opcode);
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

            self->m_synthesizer_mutex.lock();

            for (int i = 0; i < sample_count; ++i) {
                double input[2] = {inputs[0][i], inputs[1][i]};
                double output[2];
                synthesizer.process(input, output);
                outputs[0][i] = static_cast<float>(output[0]);
                outputs[1][i] = static_cast<float>(output[1]);
            }

            self->m_synthesizer_mutex.unlock();
        }

        void plugin::process_double_replacing_proc(
            AEffect *fx,
            double **inputs,
            double **outputs,
            int32_t sample_count)
        {
            plugin *self = (plugin*)(fx->user);
            auto& synthesizer = self->m_synthesizer;

            self->m_synthesizer_mutex.lock();

            for (int i = 0; i < sample_count; ++i) {
                double input[2] = {inputs[0][i], inputs[1][i]};
                double output[2];
                synthesizer.process(input, output);
                outputs[0][i] = output[0];
                outputs[1][i] = output[1];
            }

            self->m_synthesizer_mutex.unlock();
        }

		//	Raw data source implementation

		raw_data_source::raw_data_source(
			const void *raw_data, 
			const unsigned int size_limit)
			:	m_begin((uint8_t*)raw_data),
				m_size_limit(size_limit),
				m_cursor(0)
		{
		}
		
		raw_data_source::~raw_data_source()
		{
		}

		bool raw_data_source::seek(
			const int offset, 
			Sound::abstract_data_stream::seek_mode mode)
		{
			using seek_mode = Sound::data_input_stream::seek_mode;

			int new_cursor;
			
			switch (mode) {

				case seek_mode::CURRENT:
					new_cursor =
						static_cast<int>(m_cursor) +
						offset;
					break;

				case seek_mode::SET:
					new_cursor = offset;
					break;

				case seek_mode::END:
					new_cursor = 
						static_cast<int>(m_size_limit) + 
						offset;
					break;

			}

			if (new_cursor > 0 &&
				new_cursor < static_cast<int>(m_size_limit)) {
				m_cursor = static_cast<unsigned int>(new_cursor);
				return true;
			}
			else {
				return false;
			}
		}
		
		unsigned int raw_data_source::tell()
		{
			return m_cursor;
		}

		unsigned int raw_data_source::read(
			void *data, 
			const unsigned int size)
		{
			const int new_cursor =
                static_cast<int>(m_cursor + size);

			if (new_cursor <= static_cast<int>(m_size_limit)) {
				std::memcpy(data, m_begin + m_cursor, size);
				m_cursor = static_cast<unsigned int>(new_cursor);
				return size;
			}
			else {
				return 0;
			}
		}


    }   /*  vst2 */

}   /* Gammou */


extern "C" {

    AEffect* VSTPluginMain(audioMasterCallback)
    {
        return Gammou::VST2::plugin::create_AEffect_instance();
    }

}
