#ifndef GUI_COMPONENT_H_
#define GUI_COMPONENT_H_

#include <map>
#include "view.h"
#include "synthesizer.h"


namespace Gammou {

	namespace Gui {


		class abstract_gui_component : public View::panel<> {

		public:
			abstract_gui_component(const unsigned int x, const unsigned int y, 
				const unsigned int initial_input_count, const unsigned int initial_output_count);

			virtual ~abstract_gui_component() {}

			// Parent coordinate system
			virtual bool get_input_pos(const unsigned int input_id, float& x, float& y);
			virtual bool get_output_pos(const unsigned int output_id, float& x, float& y);

			virtual unsigned int save_sound_component_state(Sound::data_destination& data) { return 0; }
			virtual unsigned int get_sound_component_factory_id() const { return Sound::NO_FACTORY;  }

			virtual Process::abstract_component<double> *get_component() = 0;
		protected:
			virtual void draw(cairo_t *cr) override;

			virtual bool on_mouse_drag(const View::mouse_button button, const int x, const int y, const int dx, const int dy) override;
			virtual bool on_mouse_drag_start(const View::mouse_button, const int x, const int y) override;
			virtual bool on_mouse_drag_end(const View::mouse_button, const int x, const int y) override;
		
		private:
			// cf schema
			float m_l1;
			float m_rec_radius;
			float m_socket_radius;
			float m_socket_size;
			float m_font_size;
			float m_name_height;
			float m_line_width;
			View::color m_socket_color;
			View::color m_font_color;
			View::color m_component_border_color;
			View::color m_component_color;
		};

		class abstract_gui_component_map : public View::panel<abstract_gui_component> {

		public:
			abstract_gui_component_map(const unsigned int x, const unsigned int y,
				const unsigned int width, const unsigned int height, const View::color background = View::cl_dimgray);
			abstract_gui_component_map(const View::rectangle& rect,
				const View::color background = View::cl_dimgray);
			virtual ~abstract_gui_component_map() {}

			void add_gui_component(abstract_gui_component *component);

		protected:
			virtual void draw(cairo_t *cr);

			unsigned int input_count(abstract_gui_component *component);
			abstract_gui_component *get_input_src(abstract_gui_component *component, const unsigned int input_id, 
				unsigned int& src_output_id);
		private:
			std::map<Process::abstract_component<double>*, abstract_gui_component*> m_component_association;
		};

	} /* Gui */

} /* Gammou */

#endif