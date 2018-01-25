#ifndef GAMMOU_COMMON_WIDGET_H_
#define GAMMOU_COMMON_WIDGET_H_

#include <string>
#include <functional>
#include "widget.h"
#include "panel.h"
#include "cairo_helper.h"

namespace Gammou {

	namespace View {

		class control : public widget {

		public:
			control(const unsigned int x, const unsigned int y, const unsigned int width, const unsigned int height);
			control(const rectangle& rect);
			virtual ~control() {};

			bool is_enabled(void) const;
			virtual void set_enabled(const bool state = true);

			virtual bool on_mouse_enter(void) override;
			virtual bool on_mouse_exit(void) override;

			// widget override
		protected:
			virtual void draw(cairo_t *cr) override;

			//	control features
			bool is_focused(void) const;
		private:
			bool m_focused;
			bool m_enabled;
		};

		//--------------

		class movable_control : public control {

		public:
			movable_control(const unsigned int x, const unsigned int y, const unsigned int width, const unsigned int height);
			movable_control(const rectangle& rect);
			virtual ~movable_control() {}

			bool is_movable() const;
			void set_movable(bool movable);

			virtual bool on_mouse_drag(const mouse_button button, const int x, const int y, const int dx, const int dy) override;

			bool on_mouse_wheel(const float d) override
			{
				resize(get_width() + 5 * d, get_height() + 5 * d);
				return true;
			}

		private:
			bool m_movable;
		};

		//--------------

		class push_button : public control {

		public: 
			push_button(std::function<void()> push_action, const std::string& text, const unsigned int x, const unsigned int y, const unsigned int width = 70, const unsigned int height = 20, const unsigned int font_size = 10);
			virtual ~push_button() {};

			virtual bool on_mouse_drag_end(const mouse_button button, const int x, const int y) override;
			virtual bool on_mouse_button_down(const mouse_button button, const int cx, const int cy) override;
			virtual bool on_mouse_button_up(const mouse_button button, const int cx, const int cy) override;
			virtual bool on_mouse_enter(void);
			virtual bool on_mouse_exit(void);

		protected:
			virtual void draw(cairo_t *cr) override;
			void set_text(std::string& text);

			bool is_pushed(void) const;
		private:
			std::function<void()> m_push_action;
			std::string m_text;
			const unsigned int m_font_size;
			bool m_pushed;
		};

		//-----------


		//	TODO :	Slider, knob, toolbar, tabpanel, .... 

	} /* View */

} /* Gammou */

#endif