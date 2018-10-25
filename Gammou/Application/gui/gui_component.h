#ifndef GUI_COMPONENT_H_
#define GUI_COMPONENT_H_

#include <map>
#include <mutex>
#include <algorithm>
#include <view.h>

#include "synthesizer.h"

#include "gui_properties.h"

namespace Gammou {

	namespace Gui {

		/*
		
		*/

        class abstract_gui_component : public View::edit_widget {

		public:
			abstract_gui_component(
				const int x, const int y, 
                const unsigned int initial_input_count,
                const unsigned int initial_output_count);

			virtual ~abstract_gui_component() {}

			// Parent coordinate system
			virtual bool get_input_pos(const unsigned int input_id, float& x, float& y) const;
			virtual bool get_output_pos(const unsigned int output_id, float& x, float& y) const;
			virtual int get_output_id_by_pos(const float x, const float y) const;
			virtual int get_input_id_by_pos(const float x, const float y) const;

			virtual unsigned int save_sound_component_state(Sound::data_output_stream& data) { return 0; }
			virtual unsigned int get_sound_component_factory_id() const { return Sound::NO_FACTORY;  }

			virtual Process::abstract_component<double> *get_component() const = 0;

            virtual bool on_mouse_move(const int x, const int y) override;
			virtual bool on_mouse_exit() override;

			virtual void draw(cairo_t *cr) override;

            void update_size();
		protected:
			void set_autosize(const bool state = true);

		private:
			static unsigned int component_height_by_socket_count(const unsigned int socket_count);

            bool m_autosize{true};

            int m_focused_output_id{-1}; // -1 if nothing

			// cf schema
			float m_l1;
			float m_socket_size;
			float m_name_height;
		};

		/*
		
		*/

		class abstract_gui_component_map : 
            public View::edit_panel<abstract_gui_component> {

		public:
			abstract_gui_component_map(
				const int x, 
				const int y,
				const unsigned int width,
				const unsigned int height, 
				const View::color background = GuiProperties::background);

			abstract_gui_component_map(
				const View::rectangle& rect,
				const View::color background = GuiProperties::background);

			virtual ~abstract_gui_component_map() {}

			virtual void add_gui_component(std::unique_ptr<abstract_gui_component> && component);

            virtual bool on_mouse_drag_start(
                const View::mouse_button button,
                const int x, const int y) override;

            virtual bool on_mouse_drag(
                const View::mouse_button button,
                const int x, const int y,
				const int dx, const int dy) override;

            virtual bool on_mouse_drag_end(
                const View::mouse_button button,
                const int x, const int y) override;

            virtual bool on_mouse_button_down(
                const View::mouse_button button,
                const int x, const int y) override;

			virtual void remove_widget(abstract_gui_component *component) override;

		protected:
			void draw_content(cairo_t *cr) override;

			void connect(abstract_gui_component* src, const unsigned int output_id,
				abstract_gui_component *dst, const unsigned int input_id);
			void disconnect(abstract_gui_component *component, const unsigned int input_id);

            virtual void lock_circuit() = 0;
            virtual void unlock_circuit() = 0;

			unsigned int get_input_count(const std::unique_ptr<abstract_gui_component> &component);
			abstract_gui_component *get_input_src(
				const std::unique_ptr<abstract_gui_component> &component, 
				const unsigned int input_id, 
				unsigned int& src_output_id);

		private:
			std::map<Process::abstract_component<double>*, abstract_gui_component*> m_component_association;

			bool m_is_linking;
			abstract_gui_component *m_linking_component;
			unsigned int m_linking_output_id;
			float m_linking_x;
			float m_linking_y;

            bool m_socket_highlighting;
			float m_highlighted_socket_x;
			float m_highlighted_socket_y;
		};

	} /* Gui */

} /* Gammou */

#endif
