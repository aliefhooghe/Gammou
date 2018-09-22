
#include <algorithm>

#include "gui_component.h"



namespace Gammou {

	namespace Gui {

		void draw_link(
			cairo_t *cr, const float x_input, const float y_input, 
			const float x_output, const float y_output, const View::color color)
		{
			const float D = std::min<float>(std::fabs(x_output - x_input), 70.0);

			cairo_move_to(cr, x_input, y_input);
			cairo_curve_to(cr, x_input + D, y_input, x_output - D, y_output, x_output, y_output);

			cairo_set_line_width(cr, GuiProperties::link_width);
			View::cairo_helper::set_source_color(cr, color);
			cairo_stroke(cr);
		}

		//

		abstract_gui_component::abstract_gui_component(
			const int x, const int y, const unsigned int initial_input_count, const unsigned int initial_output_count)
			:	View::panel<View::widget>(
					x, y, 
					GuiProperties::component_width, 
					component_height_by_socket_count(std::max(initial_input_count, initial_output_count))),
					m_is_linking(false),
					m_is_moving(false),
					m_autosize(true),
					m_focused_output_id(-1)
		{
			// Todo this goto properties
			m_l1 = 40;
			m_socket_size = 9;
			m_name_height = GuiProperties::component_font_size;
		}

		bool abstract_gui_component::get_input_pos(const unsigned int input_id, float & x, float & y) const
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

		bool abstract_gui_component::get_output_pos(const unsigned int output_id, float & x, float & y) const
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

		int abstract_gui_component::get_output_id_by_pos(const float x, const float y) const
		{
			const Process::abstract_component<double> *component = get_component();
			const float self_x = x - static_cast<float>(get_x());
			const float self_y = y - static_cast<float>(get_y());

			if (component != nullptr
				&& ((get_width() - self_x) < m_l1 / 2.0)) {
				const int oc = component->get_output_count();	//	int because of (test ret comapred with oc)
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

		int abstract_gui_component::get_input_id_by_pos(const float x, const float y) const
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
			Process::abstract_component<double> *component = get_component();

			// draw component
			View::color component_background;
			float border_width;

			if (m_is_moving) {
				border_width = GuiProperties::moving_component_border_width;
				component_background = GuiProperties::moving_component_background;
			}
			else {
				border_width = GuiProperties::component_border_width;
				component_background = GuiProperties::component_background;
			}


			View::cairo_helper::rounded_rectangle(cr, 0, 0, get_width(), get_height(), 
				GuiProperties::component_rectangle_corner_radius);
			View::cairo_helper::set_source_color(cr, component_background);
			cairo_fill_preserve(cr);

			View::cairo_helper::set_source_color(cr, GuiProperties::component_border);
			cairo_set_line_width(cr, border_width);
			cairo_stroke(cr);

			if (component == nullptr)
				return;

			// draw name

			cairo_set_font_size(cr, GuiProperties::component_font_size);
			cairo_select_font_face(cr, "sans-serif", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
			View::cairo_helper::set_source_color(cr, GuiProperties::component_font_color);
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
					View::color socket_color;

					// socket
					View::cairo_helper::circle(cr, cx, cy, GuiProperties::component_socket_radius);
					
					if (component->get_input_src(i) == nullptr)
						socket_color = GuiProperties::component_input_socket_color;
					else
						socket_color = GuiProperties::component_linked_input_socket_color;

					View::cairo_helper::set_source_color(cr, socket_color);
					cairo_fill(cr);

					// name
					View::cairo_helper::set_source_color(cr, GuiProperties::component_font_color);
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
					View::color socket_color;
					float socket_radius;

					// socket

					if (static_cast<int>(i) == m_focused_output_id) {
						socket_radius = GuiProperties::focused_component_socket_radius;
						socket_color = GuiProperties::focused_component_output_socket_color;
					}
					else {
						socket_radius = GuiProperties::component_socket_radius;
						socket_color = GuiProperties::component_output_socket_color;
					}

					View::cairo_helper::circle(cr, cx, cy, socket_radius);
					View::cairo_helper::set_source_color(cr, socket_color);
					cairo_fill(cr);

					// name
					View::cairo_helper::set_source_color(cr, GuiProperties::component_font_color);
					View::cairo_helper::show_right_aligned_text(cr, View::rectangle(static_cast<float>(get_width()) - m_l1, ty,m_l1 - m_socket_size,
						socket_rect_height), component->get_output_name(i));
				}
			}

			// Contained widget
			View::panel<>::draw_widgets(cr);
		}

		void abstract_gui_component::set_autosize(const bool state)
		{
			m_autosize = state;
		}

		bool abstract_gui_component::is_linking()
		{
			return m_is_linking;
		}

		void abstract_gui_component::set_linking(const bool state)
		{
			if( !state )
				m_focused_output_id = -1;
			m_is_linking = state;
		}

		void abstract_gui_component::update_size()
		{
			const Process::abstract_component<double> *const component = get_component();

			if (m_autosize && component != nullptr) {
				const unsigned int ic = component->get_input_count();
				const unsigned int oc = component->get_output_count();
				const unsigned int socket_count = std::max<unsigned int>(ic, oc);

				resize(get_width(), component_height_by_socket_count(socket_count));
			}
		}

		unsigned int abstract_gui_component::component_height_by_socket_count(const unsigned int socket_count)
		{
			return GuiProperties::component_font_size + 15 * socket_count;
		}

		bool abstract_gui_component::on_mouse_drag(const View::mouse_button button, const int x, const int y, const int dx, const int dy)
		{
			if (!View::panel<>::on_mouse_drag(button, x, y, dx, dy)) {
				if (!is_linking()) {
					set_rect(get_absolute_rect().translate(dx, dy));
					//	No need to redraw here, this is done by component map
				}
			}
			
			return true;
		}

		bool abstract_gui_component::on_mouse_drag_start(const View::mouse_button button, const int x, const int y)
		{
			
			if (!View::panel<>::on_mouse_drag_start(button, x, y)) {
				if (!is_linking()) {
					m_is_moving = true;
					redraw();
				}
			}

			return true;
		}

		bool abstract_gui_component::on_mouse_drag_end(const View::mouse_button button, const int x, const int y)
		{
			if (!View::panel<>::on_mouse_drag_end(button, x, y)) {
				if (!is_linking()) {
					m_is_moving = false;
					redraw();
					return true;
				}
			}

			return false;
		}

		bool abstract_gui_component::on_mouse_move(const int x, const int y)
		{
			if (!View::panel<>::on_mouse_move(x, y)) {
				if (!m_is_linking) {
					const int output_id = get_output_id_by_pos(get_x() + x, get_y() + y);

					if (m_focused_output_id != output_id) {
						m_focused_output_id = output_id;
						redraw();
					}
				}
			}

			return true;
		}

		bool abstract_gui_component::on_mouse_exit()
		{
			View::panel<>::on_mouse_exit();

			if ( !m_is_linking 
				&& m_focused_output_id != -1) {
				m_focused_output_id = -1;
				redraw();
			}

			return true;
		}

		/*
		 *		ABSTRACT GUI COMPONENT MAP IMPLEMENTATION
		 */

		abstract_gui_component_map::abstract_gui_component_map(
			const int x,
			const int y, 
			const unsigned int width,
			const unsigned int height, 
			const View::color background)
			: scrollable_panel<abstract_gui_component>(x, y, width, height, background),
			m_is_linking(false),
			m_linking_component(nullptr),
			m_linking_output_id(0),
			m_linking_x(0.0),
			m_linking_y(0.0),
			m_socket_highlighting(false),
			m_highlighted_socket_x(0.0),
			m_highlighted_socket_y(0.0)
		{
			set_sroll_method(View::scrollable_panel<abstract_gui_component>::scroll_method::DRAG_SCROLL);
		}

		abstract_gui_component_map::abstract_gui_component_map(
			const View::rectangle & rect, 
			const View::color background)
			: scrollable_panel<abstract_gui_component>(rect, background),
			m_is_linking(false),
			m_linking_component(nullptr),
			m_linking_output_id(0),
			m_linking_x(0.0),
			m_linking_y(0.0),
			m_socket_highlighting(false),
			m_highlighted_socket_x(0.0),
			m_highlighted_socket_y(0.0)
		{
			set_sroll_method(View::scrollable_panel<abstract_gui_component>::scroll_method::DRAG_SCROLL);
		}

		void abstract_gui_component_map::add_gui_component(std::unique_ptr<abstract_gui_component> && component)
		{
			Process::abstract_component<double> *cmpt = component->get_component();

			if (cmpt != nullptr) {
                m_component_association[cmpt] = component.get();
				add_widget(std::move(component));
			}
		}

		void abstract_gui_component_map::draw_content(cairo_t * cr)
		{
			// Background drawn by scrolable_panel

			// Draw links

			for (auto& gui_component : m_widgets){
				const unsigned int ic = get_input_count(gui_component);

				for (unsigned int i = 0; i < ic; ++i) {
					unsigned int output_id;
					abstract_gui_component *src = get_input_src(gui_component, i, output_id);
					
					if (src != nullptr) {
						float x1, y1, x2, y2;

						src->get_output_pos(output_id, x1, y1);
						gui_component->get_input_pos(i, x2, y2);

						draw_link(cr, x1, y1, x2, y2, GuiProperties::link_color);
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
					draw_link(cr, x, y, m_linking_x, m_linking_y, GuiProperties::linking_color);
				}
			}

			draw_widgets(cr);

			// Highlight socket
			
			if (m_socket_highlighting) {
				View::cairo_helper::set_source_color(cr, GuiProperties::highlight_socket_color);
				View::cairo_helper::circle(cr, m_highlighted_socket_x, m_highlighted_socket_y, 4.0);
				cairo_fill(cr);
			}

		}

		void abstract_gui_component_map::connect(abstract_gui_component * src, const unsigned int src_output_id, 
			abstract_gui_component * dst, const unsigned int dst_input_id)
		{
			Process::abstract_component<double> *c_src = src->get_component();
			Process::abstract_component<double> *c_dst = dst->get_component();

			if (c_src != nullptr && c_dst != nullptr) {

				if (c_src->get_output_count() <= src_output_id) {
					DEBUG_PRINT("Unable to link, %s has not output_id %u\n", c_src->get_name().c_str(), src_output_id);
				}
				else if (c_dst->get_input_count() <= dst_input_id)
				{
					DEBUG_PRINT("Unable to link, %s has not input_id %u\n", c_dst->get_name().c_str(), dst_input_id);
				}
				else {
					lock_circuit();
					c_src->connect_to(src_output_id, c_dst, dst_input_id);
					unlock_circuit();
					dst->update_size();
				}
			}
		}

		void abstract_gui_component_map::disconnect(abstract_gui_component * component, const unsigned int input_id)
		{
			Process::abstract_component<double> *c_component = component->get_component();

			if (c_component != nullptr) {
				lock_circuit();
				c_component->disconnect_input(input_id);
				unlock_circuit();
				component->update_size();
			}
		}

		bool abstract_gui_component_map::on_mouse_drag(const View::mouse_button button, const int x, const int y, const int dx, const int dy)
		{
			m_socket_highlighting = false;

			if (m_is_linking) {
				const abstract_gui_component *const component = get_focused_widget();

				const int map_x = convert_x(x);
				const int map_y = convert_y(y);

				if (component != nullptr) {
					// x,y converted into map coordinate
					const int input_id = component->get_input_id_by_pos(map_x, map_y);

					if (input_id != -1) {
						float fx, fy;
						component->get_input_pos(input_id, fx, fy);/// already in map coordinate
						m_socket_highlighting = true;
						m_highlighted_socket_x = fx;
						m_highlighted_socket_y = fy;
					}
				}

				m_linking_x = map_x;
				m_linking_y = map_y;
				redraw();
			}
			
			View::scrollable_panel<abstract_gui_component>::on_mouse_drag(button, x, y, dx, dy);
			return true;
		}

		bool abstract_gui_component_map::on_mouse_drag_start(const View::mouse_button button, const int x, const int y)
		{
			abstract_gui_component *focused_component = get_focused_widget();

			if (focused_component != nullptr) {
				const int map_x = convert_x(x);
				const int map_y = convert_y(y);

				int output_id = focused_component->get_output_id_by_pos(map_x, map_y);

				if (output_id != -1) {
					m_is_linking = true;
					m_linking_output_id = output_id;
					m_linking_component = focused_component;

					DEBUG_PRINT("Start linking from output_id = %d\n", output_id);

					set_scrollable(false); // prevent drag from scrolling the map
					focused_component->set_linking(); // prenvent from moving while being linked
				}
			}

			return View::scrollable_panel<abstract_gui_component>::on_mouse_drag_start(button, x, y);
		}

		bool abstract_gui_component_map::on_mouse_drag_end(const View::mouse_button button, const int x, const int y)
		{
			if (m_is_linking) {
				abstract_gui_component *dst = get_focused_widget();
				
				m_socket_highlighting = false;

				if (dst != nullptr) {
					const int map_x = convert_x(x);
					const int map_y = convert_y(y);

					const int input_id = dst->get_input_id_by_pos(map_x, map_y);
					
					DEBUG_PRINT("INPUT ID = %d\n", input_id);

					if (input_id != -1)
						connect(m_linking_component, m_linking_output_id, dst, input_id);
				}

				m_is_linking = false;
				m_linking_component->set_linking(false);
				set_scrollable(true);
				redraw();
			}

			return View::scrollable_panel<abstract_gui_component>::on_mouse_drag_end(button, x, y);
		}

		bool abstract_gui_component_map::on_mouse_button_down(const View::mouse_button button, const int x, const int y)
		{

			if (button == View::mouse_button::RightButton) {
				abstract_gui_component *component = get_focused_widget();

				if (component != nullptr) {
					const int input_id = component->get_input_id_by_pos(convert_x(x), convert_y(y));

					DEBUG_PRINT("INPUTID = %d\n", input_id);

					if (input_id != -1) {
						disconnect(component, input_id);
						redraw();
						return true;
					}
				}

			}

			return View::scrollable_panel<abstract_gui_component>::on_mouse_button_down(button, x, y);
		}

		void abstract_gui_component_map::remove_widget(abstract_gui_component * component)
		{
			m_component_association.erase(component->get_component());
			panel<abstract_gui_component>::remove_widget(component);
		}

		unsigned int abstract_gui_component_map::get_input_count(const std::unique_ptr<abstract_gui_component> &component)
		{
			Process::abstract_component<double> *cpmt = component->get_component();
			
			if (cpmt != nullptr)
				return cpmt->get_input_count();
			else
				return 0;
		}

		abstract_gui_component * abstract_gui_component_map::get_input_src(
				const std::unique_ptr<abstract_gui_component> &component, 
				const unsigned int input_id, 
				unsigned int& src_output_id)
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
