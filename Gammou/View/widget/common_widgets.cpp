
#include <cmath>

#include "../../debug.h"
#include "common_widgets.h"

namespace Gammou {

	namespace View {

		// Control implementation

		control::control(const int x, const int y, const unsigned int width, const unsigned int height)
			: widget(x, y, width, height), m_focused(false), m_enabled(true)
		{

		}

		control::control(const rectangle & rect)
			: widget(rect), m_focused(false), m_enabled(true)
		{
		}


		void control::draw(cairo_t * cr)
		{
			cairo_set_source_rgb(cr, 0, 0, 0);
			cairo_set_line_width(cr, 1);
			cairo_rectangle(cr, 0, 0, get_width(), get_height());
			cairo_stroke(cr);
		}

		bool control::on_mouse_enter(void)
		{
			m_focused = true;
			return true;
		}

		bool control::on_mouse_exit(void)
		{
			m_focused = false;
			return true;
		}

		bool control::is_focused(void) const
		{
			return m_focused;
		}

		bool control::is_enabled(void) const
		{
			return m_enabled;
		}

		void control::set_enabled(const bool state)
		{
			m_enabled = state;
		}

		// Label Implementation

		label::label(
			const std::string & text, 
			const int x, const int y, 
			const unsigned int width, 
			const unsigned int height, 
			const color color,
			const unsigned int font_size)
			: widget(x, y, width, height),
				m_text(text),
				m_font_size(font_size),
				m_color(color)
		{
		}

		label::label(
			const std::string & text, 
			const rectangle & rect, 
			const unsigned int font_size,
			const color color)
			: widget(rect),
				m_text(text),
				m_font_size(font_size),
				m_color(color)
		{
		}

		void label::set_text(const std::string & text)
		{
			m_text = text;
			redraw();
		}

		void label::set_color(const color color)
		{
			m_color = color;
		}

		void label::set_font_size(const unsigned int size)
		{
			m_font_size = size;
			redraw();
		}

		void label::draw(cairo_t * cr)
		{
			cairo_select_font_face(cr, "sans-serif", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
			cairo_set_font_size(cr, static_cast<double>(m_font_size));
			cairo_helper::set_source_color(cr, m_color);
			cairo_helper::show_centered_text(cr, get_relative_rect(), m_text);
		}

		// push_button implementation

		push_button::push_button(
			std::function<void(push_button*)> push_action, 
			const std::string& text, 
			const int x, const int y, 
			const unsigned int width, 
			const unsigned int height, 
			const unsigned int font_size,
			const color pushed_color,
			const color background_color,
			const color font_color)
		:	m_pushed(false),  
			m_push_action(push_action), 
			m_text(text), 
			m_font_size(font_size), 
			m_pushed_color(pushed_color),
			m_background_color(background_color),
			m_font_color(font_color),
			control(x, y, width, height)
		{
		}

		void push_button::draw(cairo_t * cr)
		{
			if (m_pushed) {
				cairo_select_font_face(cr, "sans-serif", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
				cairo_helper::set_source_color(cr, m_pushed_color);
			}
			else {
				cairo_select_font_face(cr, "sans-serif", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
				cairo_helper::set_source_color(cr, m_background_color);
			}

			cairo_helper::simple_rectangle(cr, get_relative_rect());
			cairo_fill(cr);

			if (is_focused()) {
				cairo_helper::simple_rectangle(cr, get_relative_rect());
				cairo_helper::set_source_color(cr, m_pushed_color);
				cairo_set_line_width(cr, 3.0);
				cairo_set_line_join(cr, CAIRO_LINE_JOIN_ROUND);
				cairo_stroke(cr);
			}	

			cairo_helper::set_source_color(cr, m_font_color);
			cairo_set_font_size(cr, m_font_size);
			cairo_helper::show_centered_text(cr, get_relative_rect(), m_text.c_str());
		}

		bool push_button::on_mouse_drag_end(const mouse_button button, const int x, const int y)
		{
			// On recoit ce msg si le drag a commenc� sur le bouton
			if (button == mouse_button::LeftButton && is_enabled()) {
				m_push_action(this);
				m_pushed = false;
				redraw();
				return true;
			}
			else {
				return false;
			}
		}

		bool push_button::on_mouse_button_down(mouse_button button, const int cx, const int cy)
		{
			if (button == mouse_button::LeftButton && is_enabled()) {
				m_pushed = true;
				redraw();
				return true;
			}
			else {
				return false;
			}
		}

		bool push_button::on_mouse_button_up(mouse_button button, const int cx, const int cy)
		{
			if (button == mouse_button::LeftButton && is_enabled()) {
				m_push_action(this);
				m_pushed = false;
				redraw();
				return true;
			}
			else {
				return false;
			}
		}

		bool push_button::on_mouse_enter(void)
		{
			control::on_mouse_enter();
			redraw();
			return true;
		}

		bool push_button::on_mouse_exit(void)
		{
			control::on_mouse_exit();
			redraw();
			return true;
		}

		void push_button::set_text(const std::string & text)
		{
			m_text = text;
			redraw();
		}

		bool push_button::is_pushed(void) const
		{
			return m_pushed;
		}

		/*
		*	Knob
		*/

		const float knob::theta = 0.2f;
		const float knob::angle_max = 2.0f * (3.14f - knob::theta);

		knob::knob(
				std::function<void(knob *kn)> change_action, 
				const int x, const int y, 
				const color on_color, 
				const color off_color,
				const unsigned int size)
			: control(x, y, size, size), 
				m_change_action(change_action),
				m_angle(0.0f),
				m_normalized_value(0.0f),
				m_on_color(on_color), 
				m_off_color(off_color)
		{
		}


		void knob::draw(cairo_t * cr)
		{
			const float offset = 0.5f * static_cast<float>(get_width());
			const float radius = 0.4f * static_cast<float>(get_width());
			
			const float start_angle = 1.57f + theta;
			const float end_angle = 1.57f - theta;
			const float cur_angle = start_angle + m_angle; 
			
			//	Off part
			cairo_set_line_width(cr, 3.0f);

			cairo_helper::set_source_color(cr, m_off_color);
			cairo_new_path(cr);	//	should not be needed, to check
			cairo_arc(cr, offset, offset, radius, start_angle, end_angle);
			cairo_stroke(cr);

			//	On part
			cairo_helper::set_source_color(cr, m_on_color);
			cairo_arc(cr, offset, offset, radius, start_angle, cur_angle);
			cairo_stroke(cr);
			
			//	Cursor
			const float cur_dist = 0.6f * radius;
			const float cur_cx = offset + cur_dist * std::cos(cur_angle);
			const float cur_cy = offset + cur_dist * std::sin(cur_angle);

			cairo_helper::set_source_color(cr, m_on_color);
			cairo_helper::circle(cr, cur_cx, cur_cy, 3.5f);
			cairo_fill(cr);
		}

		bool knob::on_mouse_drag(
			const mouse_button button, 
			const int x, const int y, 
			const int dx, const int dy)
		{
			if (is_enabled()) {
				on_change(dy * 0.1f);
				return true;
			}
			else {
				return false;
			}
		}

		bool knob::on_mouse_wheel(const float distance)
		{
			if (is_enabled()) {
				on_change(-distance * 0.2f);
				return true;
			}
			else {
				return false;
			}
		}

		void knob::set_normalized_value(const float normalized_value)
		{
			m_normalized_value = normalized_value;
			m_angle = normalized_value * angle_max;
			m_change_action(this);
			redraw();
		}

		float knob::get_normalized_value() const
		{
			return m_normalized_value;
		}

		void knob::on_change(const float angle_change)
		{
			m_angle -= angle_change;

			if (m_angle < 0.0f)
				m_angle = 0.0f;
			else if (m_angle > angle_max)
				m_angle = angle_max;

			m_normalized_value = m_angle / angle_max;
			m_change_action(this);
			redraw();
		}

		
		/*
		*	Slider
		*/

		const unsigned int slider::cursor_radius = 5;

		slider::slider(
			std::function<void(slider&)> change_action,
			const int x, const int y,
			const unsigned int width,
			const color on_color, 
			const color off_color)
		:	control(x, y, width, 24),
			m_change_action(change_action),
			m_cursor(0u),
			m_cursor_max(width - 2 * cursor_radius),
			m_on_color(on_color), 
			m_off_color(off_color),
			m_cursor_is_moving(false)
		{
		}

		void slider::draw(cairo_t *cr)
		{
			const unsigned int middle = get_height() / 2;

			cairo_set_line_width(cr, 3);

			//	On part
			cairo_helper::set_source_color(cr, m_on_color);
			cairo_move_to(cr, cursor_radius, middle);
			cairo_line_to(cr, cursor_radius + m_cursor, middle);
			cairo_stroke(cr);

			//	Off part
			cairo_helper::set_source_color(cr, m_off_color);
			cairo_move_to(cr, cursor_radius + m_cursor, middle);
			cairo_line_to(cr, get_width() - cursor_radius, middle);
			cairo_stroke(cr);

			//	Cursor
			cairo_helper::set_source_color(cr, m_on_color);
			cairo_helper::circle(
				cr, cursor_radius + m_cursor, middle, 
				m_cursor_is_moving ? cursor_radius - 2 : cursor_radius);
			cairo_fill(cr);
		}

		bool slider::on_mouse_drag(
			const mouse_button button, 
			const int x, const int y,
			const int dx, const int dy)
		{
			change_cursor_value(x - cursor_radius);
			return true;
		}

		bool slider::on_mouse_drag_start(
			const mouse_button button, 
			const int x, const int y)
		{
			m_cursor_is_moving = true;
			redraw(); 
			return true;
		}

		bool slider::on_mouse_drag_end(
			const mouse_button button, 
			const int x, const int y)
		{
			m_cursor_is_moving = false;
			redraw();
			return true;
		}

		bool slider::on_mouse_button_down(
			const mouse_button button, 
			const int x, const int y)
		{ 
			m_cursor_is_moving = true; 
			change_cursor_value(x - cursor_radius);
			redraw();
			return true;
		} 

		bool slider::on_mouse_button_up(
			const mouse_button button, 
			const int x, const int y)
		{ 
			if (m_cursor_is_moving) {
				m_cursor_is_moving = false;
				redraw();
			}

			return true;
		}
		
		bool slider::on_mouse_wheel(const float distance)
		{
			change_cursor_value(m_cursor + distance * 12.0f);
			return false;
		}

		void slider::set_normalized_value(const float normalized_value)
		{
			if (normalized_value < 0.0f)
				m_cursor = 0u;
			else if (normalized_value > 1.0f)
				m_cursor = m_cursor_max;
			else
				m_cursor = 
					static_cast<unsigned int>(normalized_value * 
						static_cast<float>(m_cursor_max));

			m_change_action(*this);
			redraw();
		}
		
		float slider::get_normalized_value() const
		{
			return 
				static_cast<float>(m_cursor) /
				static_cast<float>(m_cursor_max);
		}
		
		void slider::change_cursor_value(const int v)
		{
			unsigned int new_value;

			if (v < 0)
				new_value = 0;
			else if (v > m_cursor_max)
				new_value = m_cursor_max;
			else
				new_value = v;

			if (new_value != m_cursor) {
				m_cursor = new_value;
				m_change_action(*this);
				redraw();
			}
		}

} /* View */

} /* Gammou */


