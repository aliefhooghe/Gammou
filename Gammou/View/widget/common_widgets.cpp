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

		// push_button implementation

		push_button::push_button(std::function<void(push_button*)> push_action, const std::string& text, const int x, const int y, const unsigned int width, const unsigned int height, const unsigned int font_size)
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
			// On recoit ce msg si le drag a commencé sur le bouton
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

		void push_button::set_text(std::string & text)
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

		knob::knob(const int x, const int y, const unsigned int size)
			: control(x, y, size, size), m_angle(0.0f)
		{
		}

		void knob::draw(cairo_t * cr)
		{
			const float offset = 0.5f * static_cast<float>(get_width());
			const float radius = 0.4f * static_cast<float>(get_width());
			
			const float theta = 0.2;
			
			cairo_set_line_width(cr, 3.5);

			cairo_helper::set_source_color(cr, cl_lightgrey);
			cairo_arc(cr, offset, offset, radius, 1.57 + theta, 1.57 - theta);
			cairo_stroke(cr);

			cairo_helper::set_source_color(cr, cl_blueviolet);

			if( m_angle >= 0.0)
				cairo_arc(cr, offset, offset, radius, -1.57, -1.57 + m_angle);
			else
				cairo_arc_negative(cr, offset, offset, radius, -1.57 + 0.0, -1.57 + m_angle);

			cairo_line_to(cr, offset, offset);

			cairo_stroke(cr);
		}

		bool knob::on_mouse_drag(const mouse_button button, const int x, const int y, const int dx, const int dy)
		{
			const float borne = 3.14 - 0.2;

			m_angle -= (dy * 0.1);

			if (m_angle < -borne)
				m_angle = -borne;
			else if( m_angle > borne)
				m_angle = borne;

			redraw();

			const float value = (m_angle + borne) / (2.0 * borne);

			DEBUG_PRINT("value = %f dB\n",20 * log10f(value));
			return true;
		}

} /* View */

} /* Gammou */


