
#include "gui_component.h"

namespace Gammou {

	namespace Gui {

		void draw_link(cairo_t *cr, const float x_input, const float y_input, const float x_output, const float y_output, const View::color color)
		{
			const float D = std::min<float>(std::fabs(x_output - x_input), 70.0);

			cairo_move_to(cr, x_input, y_input);
			cairo_curve_to(cr, x_input + D, y_input, x_output - D, y_output, x_output, y_output);

			cairo_set_line_width(cr, 3.0);
			View::cairo_helper::set_source_color(cr, color);
			cairo_stroke(cr);
		}

		//

		abstract_gui_component::abstract_gui_component(
			std::mutex *circuit_mutex,
			const unsigned int x, const unsigned int y, const unsigned int initial_input_count, const unsigned int initial_output_count)
			: View::panel<View::widget>(x, y, 90, 10 + 15 * max(initial_input_count, initial_output_count)),
				m_frozen(false),
				m_circuit_mutex(circuit_mutex),
			m_is_moving(false)
		{
			m_l1 = 40;
			m_rec_radius = 5.0;
			m_socket_radius = 3.0;
			m_socket_size = 9;
			m_font_size = 11;
			m_name_height = m_font_size;
			m_border_width = 1.0;
		}

		bool abstract_gui_component::get_input_pos(const unsigned int input_id, float & x, float & y)
		{
			const Process::abstract_component<double> *component = get_component();

			if (component != nullptr) {
				const unsigned int ic = component->get_input_count();

				if (input_id < ic) {
					const float socket_rect_height = static_cast<float>(get_height() - m_name_height) / static_cast<float>(ic);

					x = get_x() + m_socket_size / 2.0;
					y = get_y() + m_name_height + (static_cast<float>(input_id) + 0.5) * socket_rect_height;

					return true;
				}
			}

			return false;
		}

		bool abstract_gui_component::get_output_pos(const unsigned int output_id, float & x, float & y)
		{
			const Process::abstract_component<double> *component = get_component();

			if (component != nullptr) {
				const unsigned int oc = component->get_output_count();

				if (output_id < oc) {
					const float socket_rect_height = static_cast<float>(get_height() - m_name_height) / static_cast<float>(oc);

					x = get_x() + static_cast<float>(get_width()) - (m_socket_size / 2.0);
					y = get_y() + m_name_height + (static_cast<float>(output_id) + 0.5) * socket_rect_height;

					return true;
				}

				
			}

			return false;
		}

		int abstract_gui_component::output_id_by_pos(const float x, const float y) const
		{
			const Process::abstract_component<double> *component = get_component();
			const float self_x = x - get_x();
			const float self_y = y - get_y();

			if (component != nullptr
				&& ((get_width() - self_x) < m_l1 / 2.0)) {
				const int oc = component->get_output_count();	//	int because of (ret >= oc)
				const float socket_rect_height = static_cast<float>(get_height() - m_name_height) / static_cast<float>(oc);

				if (oc != 0) {
					const int ret =  (int)((self_y - m_name_height) / socket_rect_height);
					
					if (ret < 0)
						return ret;
					else if (ret >= oc)
						return (oc - 1);
					else
						return ret;
				}
			}

			return -1;
		}

		int abstract_gui_component::input_id_by_pos(const float x, const float y) const
		{
			const Process::abstract_component<double> *component = get_component();
			const float self_x = x - get_x();
			const float self_y = y - get_y();

			if (component != nullptr
				&& (self_x < m_l1)) {
				const int ic = component->get_input_count();
				const float socket_rect_height = static_cast<float>(get_height() - m_name_height) / static_cast<float>(ic);

				if (ic != 0) {
					const int ret = (int)((self_y - m_name_height) / socket_rect_height);
					
					if (ret < 0)
						return ret;
					else if (ret >= ic)
						return (ic - 1);
					else
						return ret;
				}
			}

			return -1;
		}

		void abstract_gui_component::draw(cairo_t * cr)
		{
			const Process::abstract_component<double> *component = get_component();

			// draw component
			View::color component_background;

			if (m_is_moving)
				component_background = Palette::moving_component_background;
			else
				component_background = Palette::component_background;


			View::cairo_helper::rounded_rectangle(cr, 0, 0, get_width(), get_height(), m_rec_radius);
			View::cairo_helper::set_source_color(cr, component_background);
			cairo_fill_preserve(cr);

			View::cairo_helper::set_source_color(cr, Palette::component_border);
			cairo_set_line_width(cr, m_border_width);
			cairo_stroke(cr);

			// draw name

			cairo_set_font_size(cr, m_font_size);
			cairo_select_font_face(cr, "sans-serif", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
			View::cairo_helper::set_source_color(cr, Palette::component_font_color);
			View::cairo_helper::show_centered_text(cr, View::rectangle(0, 0, get_width(), m_name_height * 1.5)
				, component->get_name());


			cairo_select_font_face(cr, "sans-serif", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);

			const unsigned int ic = component->get_input_count();
			const unsigned int oc = component->get_output_count();


			if (ic != 0) {
				// draw Inputs sockets
				const float socket_rect_height = static_cast<float>(get_height() - m_name_height) / static_cast<float>(ic);
				const float cx = m_socket_size / 2.0;

				for (unsigned int i = 0; i < ic; ++i) {
					const float ty = m_name_height + static_cast<float>(i) * socket_rect_height;
					const float cy = m_name_height + (static_cast<float>(i) + 0.5) * socket_rect_height;

					// socket
					View::cairo_helper::circle(cr, cx, cy, m_socket_radius);
					View::cairo_helper::set_source_color(cr, Palette::component_socket_color);
					cairo_fill(cr);

					// name
					View::cairo_helper::set_source_color(cr, Palette::component_font_color);
					View::cairo_helper::show_left_aligned_text(cr, View::rectangle(m_socket_size, ty, m_l1 - m_socket_size, socket_rect_height),
						component->get_input_name(i));

				}
			}

			if (oc != 0) {
				// draw Output sockets
				const float socket_rect_height = static_cast<float>(get_height() - m_name_height) / static_cast<float>(oc);
				const float cx = static_cast<float>(get_width()) - (m_socket_size / 2.0);

				for (unsigned int i = 0; i < oc; ++i) {
					const float ty = m_name_height + static_cast<float>(i) * socket_rect_height;
					const float cy = m_name_height + (static_cast<float>(i) + 0.5) * socket_rect_height;

					// socket
					View::cairo_helper::circle(cr, cx, cy, m_socket_radius);
					View::cairo_helper::set_source_color(cr, Palette::component_socket_color);
					cairo_fill(cr);

					// name
					View::cairo_helper::set_source_color(cr, Palette::component_font_color);
					View::cairo_helper::show_right_aligned_text(cr, View::rectangle(static_cast<float>(get_width()) - m_l1, ty, m_l1 - m_socket_size,
						socket_rect_height), component->get_output_name(i));

				}
			}

		}

		bool abstract_gui_component::is_frozen()
		{
			return m_frozen;
		}

		void abstract_gui_component::set_frozen(const bool state)
		{
			m_frozen = state;
		}

		bool abstract_gui_component::on_mouse_drag(const View::mouse_button button, const int x, const int y, const int dx, const int dy)
		{
			if (!is_frozen()) {
				const View::rectangle rect = get_absolute_rect().translate(dx, dy);
				if (get_parent()->contains(rect)) {
					set_rect(rect);
					redraw_parent();
				}
			}
			
			return true;
		}

		bool abstract_gui_component::on_mouse_drag_start(const View::mouse_button, const int x, const int y)
		{
			if (!is_frozen()) {
				m_border_width = 2.0;
				m_is_moving = true;
				redraw();
			}

			return true;
		}

		bool abstract_gui_component::on_mouse_drag_end(const View::mouse_button, const int x, const int y)
		{
			if (!is_frozen()) {
				m_border_width = 1.0;
				m_is_moving = false;
				redraw();
				return true;
			}

			return false;
		}

		bool abstract_gui_component::on_mouse_dbl_click(const int x, const int y)
		{
			bool ret;
			const unsigned int input_id = input_id_by_pos(x + get_x(), y + get_y());

			if (input_id != -1) {
				ret = true; // event handled
				DEBUG_PRINT("Disconect input %d\n", input_id);

				lock_circuit();
				get_component()->disconnect_input(input_id);
				unlock_circuit();

				redraw_parent();
			}
			else {
				DEBUG_PRINT("Not on an input\n");
				ret = false;
			}
				
			return ret;
		}


		/*
		 *		ABSTRACT GUI COMPONENT MAP IMPLEMENTATION
		 */

		abstract_gui_component_map::abstract_gui_component_map(
			std::mutex *circuit_mutex, const unsigned int x, const unsigned int y, const unsigned int width,
			const unsigned int height, const View::color background)
			: panel<abstract_gui_component>(x, y, width, height, background),
			m_circuit_mutex(circuit_mutex),
			m_is_linking(false),
			m_linking_component(nullptr),
			m_linking_output_id(0),
			m_linking_x(0.0),
			m_linking_y(0.0)
		{
		}

		abstract_gui_component_map::abstract_gui_component_map(
			std::mutex *circuit_mutex, const View::rectangle & rect, const View::color background)
			: panel<abstract_gui_component>(rect, background),
			m_circuit_mutex(circuit_mutex),
			m_is_linking(false),
			m_linking_component(nullptr),
			m_linking_output_id(0),
			m_linking_x(0.0),
			m_linking_y(0.0)
		{
		}

		void abstract_gui_component_map::add_gui_component(abstract_gui_component * component)
		{
			Process::abstract_component<double> *cmpt = component->get_component();

			if (cmpt != nullptr) {
				add_widget(component);
				m_component_association[cmpt] = component;
			}
		}

		void abstract_gui_component_map::draw(cairo_t * cr)
		{
			draw_background(cr);

			//lock_circuit();

			// Draw links

			for (abstract_gui_component *gui_component : m_widgets){
				const unsigned int ic = get_input_count(gui_component);

				for (unsigned int i = 0; i < ic; ++i) {
					unsigned int output_id;
					abstract_gui_component *src = get_input_src(gui_component, i, output_id);
					
					if (src != nullptr) {
						float x1, y1, x2, y2;

						src->get_output_pos(output_id, x1, y1);
						gui_component->get_input_pos(i, x2, y2);

						draw_link(cr, x1, y1, x2, y2, Palette::link_color);
					}
				}
			}

			// linking link
			if (m_is_linking) {
				float x, y;
					
				if (!(m_linking_component->get_output_pos(m_linking_output_id, x, y))) {
					DEBUG_PRINT("ERROR OutputID %d\n", m_linking_output_id);
				}
				else{
					draw_link(cr, x, y, m_linking_x, m_linking_y, Palette::linking_color);
				}
			}

			draw_widgets(cr);
			//unlock_circuit();
		}

		void abstract_gui_component_map::connect(abstract_gui_component * src, const unsigned int src_output_id, 
			abstract_gui_component * dst, const unsigned int dst_input_id)
		{
			Process::abstract_component<double> *c_src = src->get_component();
			Process::abstract_component<double> *c_dst = dst->get_component();

			if( c_src != nullptr && c_dst != nullptr)
				c_src->connect_to(src_output_id, c_dst, dst_input_id);
		}

		bool abstract_gui_component_map::on_mouse_drag(const View::mouse_button button, const int x, const int y, const int dx, const int dy)
		{
			if (m_is_linking) {
				m_linking_x = x;
				m_linking_y = y;
				redraw();
			}

			return View::panel<abstract_gui_component>::on_mouse_drag(button, x, y, dx, dy);
		}

		bool abstract_gui_component_map::on_mouse_drag_start(const View::mouse_button button, const int x, const int y)
		{
			abstract_gui_component *focused_component = get_focused_widget();

			if (focused_component != nullptr) {
				//lock_circuit();
				int output_id = focused_component->output_id_by_pos(x, y);
				//unlock_circuit();

				DEBUG_PRINT("Start linking from output_id = %d\n", output_id);

				if (output_id != -1) {
					m_is_linking = true;
					m_linking_output_id = output_id;
					m_linking_component = focused_component;
					focused_component->set_frozen(); // prenvent from moving while being linked
				}
			}

			return View::panel<abstract_gui_component>::on_mouse_drag_start(button, x, y);
		}

		bool abstract_gui_component_map::on_mouse_drag_end(const View::mouse_button button, const int x, const int y)
		{
			if (m_is_linking) {
				abstract_gui_component *dst = get_focused_widget();
				
				if (dst != nullptr) {
					const int input_id = dst->input_id_by_pos(x, y);
					
					DEBUG_PRINT("INPUT ID = %d\n", input_id);

					if (input_id != -1) {
						lock_circuit();
						connect(m_linking_component, m_linking_output_id, dst, input_id);
						unlock_circuit();
					}
				}

				m_is_linking = false;
				m_linking_component->set_frozen(false);
				redraw();
			}

			return View::panel<abstract_gui_component>::on_mouse_drag_end(button, x, y);
		}

		unsigned int abstract_gui_component_map::get_input_count(abstract_gui_component * component)
		{
			Process::abstract_component<double> *cpmt = component->get_component();
			
			if (cpmt != nullptr)
				return cpmt->get_input_count();
			else
				return 0;
		}

		abstract_gui_component * abstract_gui_component_map::get_input_src(abstract_gui_component * component, const unsigned int input_id, unsigned int& src_output_id)
		{
			Process::abstract_component<double> *cmpt = component->get_component();

			if (cmpt != nullptr) {
				Process::abstract_component<double> *src = cmpt->get_input_src(input_id, src_output_id);

				if (src != nullptr
					&& m_component_association.find(src) != m_component_association.end()) {
					return m_component_association[src];
				}
			}

			return nullptr;
		}





} /* Gui */

} /* Gammou */