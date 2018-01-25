#ifndef GUI_COMPONENT_H_
#define GUI_COMPONENT_H_

#include <map>
#include "view.h"
#include "synthesizer.h"


namespace Gammou {

	namespace Gui {

		void draw_link(cairo_t *cr, const float x_input, const float y_input,
			const float x_output, const float y_output, const View::color color);

		class abstract_gui_component : public View::panel<> {

			friend class abstract_gui_component_map;

		public:
			abstract_gui_component(const unsigned int x, const unsigned int y, 
				const unsigned int initial_input_count, const unsigned int initial_output_count);

			virtual ~abstract_gui_component() {}

			// Parent coordinate system
			virtual bool get_input_pos(const unsigned int input_id, float& x, float& y);
			virtual bool get_output_pos(const unsigned int output_id, float& x, float& y);
			virtual int output_id_by_pos(const float x, const float y) const;
			virtual int input_id_by_pos(const float x, const float y) const;

			virtual unsigned int save_sound_component_state(Sound::data_destination& data) { return 0; }
			virtual unsigned int get_sound_component_factory_id() const { return Sound::NO_FACTORY;  }

			virtual Process::abstract_component<double> *get_component() const = 0;

			//	Self coordinate system
			virtual bool on_mouse_drag(const View::mouse_button button, const int x, const int y, const int dx, const int dy) override;
			virtual bool on_mouse_drag_start(const View::mouse_button, const int x, const int y) override;
			virtual bool on_mouse_drag_end(const View::mouse_button, const int x, const int y) override;

		protected:
			virtual void draw(cairo_t *cr) override;

			bool is_frozen();
			void set_frozen(const bool state = true);

		private:
			bool m_frozen;

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

		/*
		
		*/
		class default_gui_component : public abstract_gui_component {

		public:
			default_gui_component(Process::abstract_component<double> *component,
				const unsigned int x, const unsigned int y)
				: abstract_gui_component(x, y, component->get_input_count(), component->get_output_count()),
				m_component(component) {}

			virtual ~default_gui_component() {}

			Process::abstract_component<double> *get_component() const override 
			{ 
				return m_component; 
			}
		
		protected:
			Process::abstract_component<double> *m_component;
		};

		/*
		
		*/
		class abstract_gui_component_map : public View::panel<abstract_gui_component> {

		public:
			abstract_gui_component_map(const unsigned int x, const unsigned int y,
				const unsigned int width, const unsigned int height, const View::color background = View::cl_dimgray);
			abstract_gui_component_map(const View::rectangle& rect,
				const View::color background = View::cl_dimgray);
			virtual ~abstract_gui_component_map() {}

			void add_gui_component(abstract_gui_component *component);

			virtual bool on_mouse_drag(const View::mouse_button button, const int x, const int y,
				const int dx, const int dy) override;
			virtual bool on_mouse_drag_start(const View::mouse_button button, const int x, const int y) override;
			virtual bool on_mouse_drag_end(const View::mouse_button button, const int x, const int y) override;

		protected:
			virtual void draw(cairo_t *cr);

			virtual void connect(abstract_gui_component* src, const unsigned int output_id,
				abstract_gui_component *dst, const unsigned int input_id);
		private:

			unsigned int input_count(abstract_gui_component *component);
			abstract_gui_component *get_input_src(abstract_gui_component *component, const unsigned int input_id, 
				unsigned int& src_output_id);

			std::map<Process::abstract_component<double>*, abstract_gui_component*> m_component_association;

			bool m_is_linking;
			abstract_gui_component *m_linking_component;
			unsigned int m_linking_output_id;
			float m_linking_x;
			float m_linking_y;
		};

	} /* Gui */

} /* Gammou */

#endif