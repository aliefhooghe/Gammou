#ifndef GAMMOU_COMMON_WIDGET_H_
#define GAMMOU_COMMON_WIDGET_H_

#include <string>
#include <functional>
#include "widget.h"
#include "panel.h"
#include "cairo_helper.h"

namespace Gammou {

	namespace View {


		// Todo : Utile ?
		class control : public widget {

		public:
			control(const int x, const int y, const unsigned int width, const unsigned int height);
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

		// Todo : Rien de control utilisé (Enabled)
		class push_button : public control {

		public: 
			push_button(std::function<void(push_button*)> push_action, const std::string& text, const int x, const int y, const unsigned int width = 95, const unsigned int height = 27, const unsigned int font_size = 10);
			// todo rectangle ctor
			virtual ~push_button() {};

			virtual bool on_mouse_drag_end(const mouse_button button, const int x, const int y) override;
			virtual bool on_mouse_button_down(const mouse_button button, const int cx, const int cy) override;
			virtual bool on_mouse_button_up(const mouse_button button, const int cx, const int cy) override;
			virtual bool on_mouse_enter(void);
			virtual bool on_mouse_exit(void);

			void set_text(std::string& text);
		
		protected:
			virtual void draw(cairo_t *cr) override;
			bool is_pushed(void) const;

		private:
			std::function<void(push_button*)> m_push_action;
			std::string m_text;
			const unsigned int m_font_size;
			bool m_pushed;
		};

		//-----------


		//	TODO :	Slider, knob, toolbar, tabpanel, .... 

		class knob : public control {



		public:
			knob(std::function<void(knob *kn)> change_action, 
				const int x, const int y, const unsigned int size = 50);
			virtual ~knob() {}

			virtual void draw(cairo_t *cr) override;
			virtual bool on_mouse_drag(const mouse_button button, const int x, const int y,
				const int dx, const int dy) override;
			virtual bool on_mouse_wheel(const float distance) override;

			void set_normalized_value(const double normalized_value);
			double get_normalized_value();
		private:
			void on_change(const float angle_change);

			std::function<void(knob *kn)> m_change_action;
			float m_angle;
			float m_normalized_value;
			
			static const float theta;
			static const float angle_max;
		};


	} /* View */

} /* Gammou */

#endif