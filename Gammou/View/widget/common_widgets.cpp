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
			const unsigned int font_size)
			: widget(x, y, width, height),
				m_text(text),
				m_font_size(font_size)
		{
		}

		label::label(
			const std::string & text, 
			const rectangle & rect, 
			const unsigned int font_size)
			: widget(rect),
				m_text(text),
				m_font_size(font_size)
		{
		}

		void label::set_text(const std::string & text)
		{
			m_text = text;
		}

		void label::set_font_size(const unsigned int size)
		{
			m_font_size = size;
		}

		void label::draw(cairo_t * cr)
		{
			cairo_select_font_face(cr, "sans-serif", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
			cairo_set_font_size(cr, static_cast<double>(m_font_size));
			cairo_helper::show_centered_text(cr, get_relative_rect(), m_text);
		}

		// push_button implementation

		push_button::push_button(
			std::function<void(push_button*)> push_action, 
			const std::string& text, 
			const int x, const int y, 
			const unsigned int width, const unsigned int height, 
			const unsigned int font_size)
			: m_pushed(false),  m_push_action(push_action), 
			m_text(text), m_font_size(font_size), control(x, y, width, height)
		{
		}

		void push_button::draw(cairo_t * cr)
		{
			float offset;

			if (m_pushed) {
				cairo_select_font_face(cr, "sans-serif", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
				offset = 1.5;
			}
			else {
				cairo_select_font_face(cr, "sans-serif", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
				offset = 1.0;
			}

			if (is_enabled()) {
				if (is_focused())
					cairo_helper::set_source_color(cr, cl_lightblue);
				else
					cairo_helper::set_source_color(cr, cl_gainsboro);
			}
			else {
				cairo_helper::set_source_color(cr, cl_gray);
			}
			
			cairo_helper::rounded_rectangle(cr, offset / 2.0f, offset / 2.0f, get_width() - offset, get_height() - offset, 2.0f * offset);
			cairo_fill_preserve(cr);
			cairo_helper::set_source_color(cr, cl_black);

			cairo_set_line_width(cr, offset);
			cairo_set_line_join(cr, CAIRO_LINE_JOIN_ROUND);
			cairo_stroke(cr);

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
				m_on_color(on_color), m_off_color(off_color)
		{
		}


		void knob::draw(cairo_t * cr)
		{
			const float offset = 0.5f * static_cast<float>(get_width());
			const float radius = 0.4f * static_cast<float>(get_width());
			
			cairo_set_line_width(cr, 3);

			cairo_helper::set_source_color(cr, m_off_color);
			cairo_new_path(cr);
			cairo_arc(cr, offset, offset, radius, 1.57 + theta, 1.57 - theta);
			cairo_stroke(cr);

			cairo_helper::set_source_color(cr, m_on_color);

			cairo_set_line_width(cr, 3.6);
			cairo_arc(cr, offset, offset, radius, 1.57 + theta, 1.57 + theta + m_angle);


			cairo_line_to(cr, offset, offset);

			cairo_stroke(cr);
		}

		bool knob::on_mouse_drag(const mouse_button button, const int x, const int y, const int dx, const int dy)
		{
			on_change(dy * 0.1f);
			//DEBUG_PRINT("value = %f dB\n",20 * log10f(value));
			return true;
		}

		bool knob::on_mouse_wheel(const float distance)
		{
			on_change(-distance * 0.25f);
			return true;
		}

		void knob::set_normalized_value(const float normalized_value)
		{
			m_normalized_value = normalized_value;
			m_angle = normalized_value * angle_max;
			m_change_action(this);
			redraw();
		}

		double knob::get_normalized_value()
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

		

		

} /* View */

} /* Gammou */


