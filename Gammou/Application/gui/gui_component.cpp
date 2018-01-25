
#include "gui_component.h"

namespace Gammou {

	namespace Gui {

		abstract_gui_component::abstract_gui_component(const unsigned int x, const unsigned int y, const unsigned int initial_input_count, const unsigned int initial_output_count)
			: View::panel<View::widget>(x, y, 80, 10 + 15 * max(initial_input_count, initial_output_count))
		{
			m_l1 = 40.0;
			m_rec_radius = 5.0;
			m_socket_radius = 3.0;
			m_socket_size = 9;
			m_socket_color = View::cl_firebrick;
			m_component_color = View::cl_silver;
			m_font_size = 11;
			m_component_border_color = View::cl_black;
			m_font_color = View::cl_black;
			m_name_height = m_font_size;
			m_line_width = 0.8;
		}

		bool abstract_gui_component::get_input_pos(const unsigned int input_id, float & x, float & y)
		{
			const unsigned int ic = get_component()->get_input_count();

			if (input_id >= ic) {
				return false;
			}

			const float socket_rect_height = static_cast<float>(get_height() - m_name_height) / static_cast<float>(ic);

			x = get_x() + m_socket_size / 2.0;
			y = get_y() + m_name_height + (static_cast<float>(input_id) + 0.5) * socket_rect_height;

			return true;
		}

		bool abstract_gui_component::get_output_pos(const unsigned int output_id, float & x, float & y)
		{
			const unsigned int oc = get_component()->get_output_count();

			if (output_id > oc) {
				return false;
			}

			const float socket_rect_height = static_cast<float>(get_height() - m_name_height) / static_cast<float>(oc);

			x = get_x() + static_cast<float>(get_width()) - (m_socket_size / 2.0);
			y = get_y() + m_name_height + (static_cast<float>(output_id) + 0.5) * socket_rect_height;

			return true;
		}

		void abstract_gui_component::draw(cairo_t * cr)
		{
			const Process::abstract_component<double> *component = get_component();

			// draw component

			View::cairo_helper::rounded_rectangle(cr, 0, 0, get_width(), get_height(), m_rec_radius);

			View::cairo_helper::set_source_color(cr, m_component_color);
			cairo_fill_preserve(cr);

			View::cairo_helper::set_source_color(cr, m_component_border_color);
			cairo_set_line_width(cr, m_line_width);
			cairo_stroke(cr);

			// draw name

			cairo_set_font_size(cr, m_font_size);
			cairo_select_font_face(cr, "sans-serif", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
			View::cairo_helper::set_source_color(cr, m_font_color);
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
					View::cairo_helper::set_source_color(cr, m_socket_color);
					cairo_fill(cr);

					// name
					View::cairo_helper::set_source_color(cr, m_font_color);
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
					View::cairo_helper::set_source_color(cr, m_socket_color);
					cairo_fill(cr);

					// name
					View::cairo_helper::set_source_color(cr, m_font_color);
					View::cairo_helper::show_right_aligned_text(cr, View::rectangle(static_cast<float>(get_width()) - m_l1, ty, m_l1 - m_socket_size,
						socket_rect_height), component->get_output_name(i));

				}
			}
		}

		bool abstract_gui_component::on_mouse_drag(const View::mouse_button button, const int x, const int y, const int dx, const int dy)
		{
			const View::rectangle rect = get_absolute_rect().translate(dx, dy);
			if (get_parent()->contains(rect)) {
				set_rect(rect);
				redraw_parent();
			}
			return true;
		}

		bool abstract_gui_component::on_mouse_drag_start(const View::mouse_button, const int x, const int y)
		{
			m_component_border_color = View::cl_blue;
			m_component_color = View::cl_lightgrey;
			redraw();
			return false;
		}

		bool abstract_gui_component::on_mouse_drag_end(const View::mouse_button, const int x, const int y)
		{
			m_component_border_color = View::cl_black;
			m_component_color = View::cl_silver;
			redraw();
			return false;
		}

		/*
		 *		ABSTRACT GUI COMPONENT MAP IMPLEMENTATION
		 */

		abstract_gui_component_map::abstract_gui_component_map(const unsigned int x, const unsigned int y, const unsigned int width, 
			const unsigned int height, const View::color background)
			: panel<abstract_gui_component>(x, y, width, height, background)
		{
		}

		abstract_gui_component_map::abstract_gui_component_map(const View::rectangle & rect, const View::color background)
			: panel<abstract_gui_component>(rect, background)
		{
		}

		void abstract_gui_component_map::add_gui_component(abstract_gui_component * component)
		{
			Process::abstract_component<double> *cmpt = component->get_component();
			
			add_widget(component);
			m_component_association[cmpt] = component;
		}

		void abstract_gui_component_map::draw(cairo_t * cr)
		{
			draw_background(cr);

			// Draw links

			for (abstract_gui_component *gui_component : m_widgets){
				const unsigned int ic = input_count(gui_component);

				for (unsigned int i = 0; i < ic; ++i) {
					unsigned int output_id;
					abstract_gui_component *src = get_input_src(gui_component, i, output_id);
					
					if (src != nullptr) {
						float x1, y1, x2, y2, D;

						src->get_output_pos(output_id, x1, y1);
						gui_component->get_input_pos(i, x2, y2);

						D = min(std::fabs(x2 - x1), 70.0);

						cairo_move_to(cr, x1, y1);
						cairo_curve_to(cr, x1 + D, y1, x2 - D, y2, x2, y2);

						cairo_set_line_width(cr, 3.0);
						View::cairo_helper::set_source_color(cr, View::cl_crimson);
						cairo_stroke(cr);
					}
				}

			}

			draw_widgets(cr);
		}

		unsigned int abstract_gui_component_map::input_count(abstract_gui_component * component)
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