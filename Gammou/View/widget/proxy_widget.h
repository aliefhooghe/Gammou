#ifndef PROXY_WIDGET_H_
#define PROXY_WIDGET_H_

#include "panel.h"

namespace Gammou {

	namespace View {

		class proxy_widget : public abstract_panel {

			public:
				proxy_widget(widget& w);
				~proxy_widget();

				virtual bool on_key_up(const keycode key) override;
				virtual bool on_key_down(const keycode key) override;

				virtual bool on_mouse_enter(void) override;
				virtual bool on_mouse_exit(void) override;
				virtual bool on_mouse_move(const int x, const int y) override;
				virtual bool on_mouse_wheel(const float distance) override;

				virtual bool on_mouse_button_down(const mouse_button button, const int x, const int y) override;
				virtual bool on_mouse_button_up(const mouse_button button, const int x, const int y) override;
				virtual bool on_mouse_dbl_click(const int x, const int y) override;

				virtual bool on_mouse_drag(const mouse_button button, const int x, const int y, const int dx, const int dy) override;
				virtual bool on_mouse_drag_start(const mouse_button button, const int x, const int y) override;
				virtual bool on_mouse_drag_end(const mouse_button button, const int x, const int y) override;

				virtual void draw(cairo_t *cr) override;

				virtual void set_pos(const int x, const int y) override;
				virtual void resize(const unsigned int width, const unsigned int height) override;
				virtual void set_rect(const rectangle& rect) override;

			private:
				widget& m_widget;
		};

	}

}

#endif
