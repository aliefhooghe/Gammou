#ifndef GUI_SYNTHESIZER_CIRCUITS_H_
#define GUI_SYNTHESIZER_CIRCUITS_H_

#include "../../Synthesizer/plugin_management/main_factory.h"
#include "../persistence/synthesizer_persistence.h"
#include "gui_sound_component.h"
#include "gui_component_main_factory.h"

namespace Gammou {

	namespace Gui {

        class abstract_gui_circuit : public abstract_gui_component_map {

            public:
                abstract_gui_circuit(
                    gui_component_main_factory& factory,
                    const unsigned int components_channel_count,
                    const int x,
                    const int y,
                    const unsigned int width,
                    const unsigned int height,
                    const View::color background = GuiProperties::background);

                abstract_gui_circuit(
                    gui_component_main_factory& factory,
                    const unsigned int components_channel_count,
                    const View::rectangle& rect,
                    const View::color background = GuiProperties::background);

                virtual ~abstract_gui_circuit();

                //  widget override
                virtual bool on_mouse_dbl_click(const int x, const int y) override;

                void select_component_creation_factory_id(const unsigned int factory_id);

                bool save_state(Sound::data_output_stream& data);
                bool load_state(Sound::data_input_stream& data);
                void reset_content();

            protected:
                virtual void add_sound_component_to_frame(Sound::abstract_sound_component *sound_component) = 0;
                virtual abstract_gui_component *gui_component_by_internal_id(const uint32_t id) = 0;

            private:
                void save_component(Sound::data_output_stream& data, abstract_gui_component *component);
                abstract_gui_component *load_component(Sound::data_input_stream& data); // Add the process_component on the frame
                void save_link(
                     Sound::data_output_stream& data,
                     const unsigned int src_record_id,
                     const unsigned int output_id,
                     const unsigned int dst_record_id,
                     const unsigned int input_id);

                gui_component_main_factory& m_complete_component_factory;
                const unsigned int m_components_channel_count;
                unsigned int m_creation_factory_id;
        };


        class abstract_gui_synthesizer_circuit : public abstract_gui_circuit {

		public:
			abstract_gui_synthesizer_circuit(
                gui_component_main_factory& complete_component_factory,
				const unsigned int components_channel_count,
				Sound::synthesizer *synthesizer, 
				std::mutex *synthesizer_mutex,
                const int x,
                const int y,
				const unsigned int width, 
				const unsigned int height,
				const View::color background = GuiProperties::background);

			abstract_gui_synthesizer_circuit(
                gui_component_main_factory& complete_component_factory,
				const unsigned int component_channel_count,
				Sound::synthesizer *synthesizer, 
				std::mutex *synthesizer_mutex,
				const View::rectangle& rect, 
				const View::color background = GuiProperties::background);

			virtual ~abstract_gui_synthesizer_circuit() {}

		protected:
            void lock_circuit() override;
            void unlock_circuit() override;

			Sound::synthesizer *const m_synthesizer;

        private:
            std::mutex *const m_synthesizer_mutex;
		};


		/*
		
		*/


		class internal_gui_component : public abstract_gui_component {

		public:
			internal_gui_component(
				Process::abstract_component<double> *component, 
				const uint32_t internal_id,
                const int x,
                const int y);

            virtual ~internal_gui_component();

            Process::abstract_component<double> *get_component() const override;

			// FACTORY stuff
            unsigned int get_sound_component_factory_id() const override;
            unsigned int save_sound_component_state(
                    Sound::data_output_stream& data) override;

		protected:
			const uint32_t m_internal_component_id;
			Process::abstract_component<double> *const m_component;
		};



	} /* Gui */

} /* Gammou */


#endif
