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

                bool save_state(Persistence::circuit_state& state);
                bool load_state(const Persistence::circuit_state& state);
                void reset_content();

				bool on_key_down(const View::keycode key) override;
            protected:
				template<class container>
				void save_components(container& gui_components, Persistence::circuit_state& state);
				void insert_components(const Persistence::circuit_state& state);

                virtual void add_sound_component_to_frame(Sound::abstract_sound_component *sound_component) = 0;
                virtual abstract_gui_component *gui_component_by_internal_id(const uint32_t id) = 0;

            private:
                void save_component(Persistence::component_state& state, abstract_gui_component *component);
                abstract_gui_component *load_component(const Persistence::component_state& state); // Add the process_component on the frame

                gui_component_main_factory& m_complete_component_factory;
                const unsigned int m_components_channel_count;
                unsigned int m_creation_factory_id;

				Persistence::circuit_state m_clipboard{};
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

		/*

		*/

		template<class container>
		void abstract_gui_circuit::save_components(container & gui_components, Persistence::circuit_state & state)
		{
			// component -> record_id association
			std::map<abstract_gui_component*, uint32_t> component_record_id;

			uint32_t component_counter = 0u;
			uint32_t link_counter = 0u;

			state.components.resize(0);
			state.links.resize(0);

			// save components
			for (auto component : gui_components) {
				component_record_id[component] = component_counter++;
				state.components.push_back({});
				save_component(state.components.back(), component);
			}

			// save links

			for (auto component : gui_components) {
				// component is dst

				const unsigned int ic = component->get_component()->get_input_count();

				//	Getting component record_id
				const uint32_t dst_record_id = component_record_id[component];

				for (unsigned int input_id = 0; input_id < ic; ++input_id) {
					// for each dst input
					unsigned int output_id;
					abstract_gui_component *src = get_input_src(*component, input_id, output_id);

					// something linked to input
					if (src != nullptr) {
						const uint32_t src_record_id = component_record_id[src];

						state.links.emplace_back(
							Persistence::link_state{
								src_record_id, output_id,
								dst_record_id, input_id });
						link_counter++;
					}
				}
			}
		}

} /* Gui */

} /* Gammou */


#endif
