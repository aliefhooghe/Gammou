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

		void draw_link(cairo_t *cr, const float x_input, const float y_input,
			const float x_output, const float y_output, const View::color color);


		/*
		
		*/

		class abstract_gui_component : public View::panel<> {

			friend class abstract_gui_component_map;

		public:
			abstract_gui_component(
				std::mutex *circuit_mutex,
				const int x, const int y, 
				const unsigned int initial_input_count, const unsigned int initial_output_count);

			virtual ~abstract_gui_component() {}

			// Parent coordinate system
			virtual bool get_input_pos(const unsigned int input_id, float& x, float& y) const;
			virtual bool get_output_pos(const unsigned int output_id, float& x, float& y) const;
			virtual int get_output_id_by_pos(const float x, const float y) const;
			virtual int get_input_id_by_pos(const float x, const float y) const;

			virtual unsigned int save_sound_component_state(Sound::data_sink& data) { return 0; }
			virtual unsigned int get_sound_component_factory_id() const { return Sound::NO_FACTORY;  }

			virtual Process::abstract_component<double> *get_component() const = 0;

			//	Self coordinate system
			virtual bool on_mouse_drag(const View::mouse_button button, const int x, const int y, const int dx, const int dy) override;
			virtual bool on_mouse_drag_start(const View::mouse_button, const int x, const int y) override;
			virtual bool on_mouse_drag_end(const View::mouse_button, const int x, const int y) override;
			virtual bool on_mouse_move(const int x, const int y) override;
			virtual bool on_mouse_exit() override;

		protected:
			virtual void draw(cairo_t *cr) override;

			bool is_linking();
			void set_linking(const bool state = true);

			inline void lock_circuit() { m_circuit_mutex->lock(); }
			inline void unlock_circuit() { m_circuit_mutex->unlock(); }

		private:
			void update_size();
			static unsigned int component_height_by_socket_count(const unsigned int socket_count);

			bool m_is_linking;
			bool m_is_moving;
			std::mutex *m_circuit_mutex;

			int m_focused_output_id; // -1 if nothing 

			// cf schema
			float m_l1;
			float m_socket_size;
			float m_name_height;
		};

		/*
		
		*/

		class default_gui_component : public abstract_gui_component {

		public:
			default_gui_component(
				Process::abstract_component<double> *component,
				std::mutex *circuit_mutex,
				const int x, const int y)
				: abstract_gui_component(circuit_mutex, x, y, component->get_input_count(), component->get_output_count()),
				m_component(component) {}

			virtual ~default_gui_component() {}

			Process::abstract_component<double> *get_component() const override 
			{ 
				return m_component; 
			}
		
		protected:
			Process::abstract_component<double> *const m_component;
		};

		/*
		
		*/

		class abstract_gui_component_map : public View::scrollable_panel<abstract_gui_component> {

		public:
			abstract_gui_component_map(
				std::mutex *circuit_mutex,
				const int x, 
				const int y,
				const unsigned int width,
				const unsigned int height, 
				const View::color background = GuiProperties::background);

			abstract_gui_component_map(
				std::mutex *circuit_mutex, 
				const View::rectangle& rect,
				const View::color background = GuiProperties::background);

			virtual ~abstract_gui_component_map() {}

			virtual void add_gui_component(abstract_gui_component *component);

			virtual bool on_mouse_drag(const View::mouse_button button, const int x, const int y,
				const int dx, const int dy) override;
			virtual bool on_mouse_drag_start(const View::mouse_button button, const int x, const int y) override;
			virtual bool on_mouse_drag_end(const View::mouse_button button, const int x, const int y) override;

			virtual bool on_mouse_button_down(const View::mouse_button button, const int x, const int y) override;

		protected:
			void draw_content(cairo_t *cr) override;

			void connect(abstract_gui_component* src, const unsigned int output_id,
				abstract_gui_component *dst, const unsigned int input_id);
			void disconnect(abstract_gui_component *component, const unsigned int input_id);

			inline void lock_circuit() { m_circuit_mutex->lock();  }
			inline void unlock_circuit() { m_circuit_mutex->unlock(); }

			unsigned int get_input_count(abstract_gui_component *component);
			abstract_gui_component *get_input_src(abstract_gui_component *component, const unsigned int input_id, 
				unsigned int& src_output_id);

		private:
			std::map<Process::abstract_component<double>*, abstract_gui_component*> m_component_association;
			std::mutex *m_circuit_mutex;

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