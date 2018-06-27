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

		class label : public widget {

		public:
			label(
				const std::string& text, 
				const int x, const int y, 
				const unsigned int width, 
				const unsigned int height,
				const color color,
				const unsigned int font_size = 10);

			label(
				const std::string& text, 
				const rectangle& rect,
				const color color,
				const unsigned int font_size = 10);
			~label() {}

			void set_text(const std::string& text);
			void set_color(const color color);
			void set_font_size(const unsigned int size);
		protected:
			virtual void draw(cairo_t *cr) override;


			std::string m_text;
			color m_color;
			unsigned m_font_size;
		};

		//---------------

		// Todo : Rien de control utilis� (Enabled)
		class push_button : public control {

		public: 
			push_button(
				std::function<void(push_button*)> push_action, const std::string& text, const int x, const int y, 
				const unsigned int width = 95, const unsigned int height = 27, const unsigned int font_size = 10);
			// TODO rectangle ctor
			virtual ~push_button() {};

			virtual bool on_mouse_drag_end(const mouse_button button, const int x, const int y) override;
			virtual bool on_mouse_button_down(const mouse_button button, const int cx, const int cy) override;
			virtual bool on_mouse_button_up(const mouse_button button, const int cx, const int cy) override;
			virtual bool on_mouse_enter(void);
			virtual bool on_mouse_exit(void);

			void set_text(const std::string& text);
		
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

		class knob : public control {

		public:
			knob(std::function<void(knob *kn)> change_action, 
				const int x, const int y, 
				const color on_color = cl_blueviolet, const color off_color = cl_lightgrey, 
				const unsigned int size = 50);

			virtual ~knob() {}

			virtual void draw(cairo_t *cr) override;
			virtual bool on_mouse_drag(const mouse_button button, const int x, const int y,
				const int dx, const int dy) override;
			virtual bool on_mouse_wheel(const float distance) override;

			void set_normalized_value(const float normalized_value);
			double get_normalized_value();
		private:
			void on_change(const float angle_change);

			std::function<void(knob *kn)> m_change_action;
			float m_angle;
			float m_normalized_value;
			
			const color m_on_color;
			const color m_off_color;

			static const float theta;
			static const float angle_max;
		};


	} /* View */

} /* Gammou */

#endif