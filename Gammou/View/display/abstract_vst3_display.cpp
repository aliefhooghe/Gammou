
#include "abstract_vst3_display.h"
#include "pluginterfaces/base/keycodes.h"

#define TRANSLATE_KEY(vst3key, gammoukey) case Steinberg::vst3key: return gammoukey

namespace Gammou {

	namespace View {

		static keycode vst3_keycode_to_gammou_keycode(Steinberg::char16 key, Steinberg::int16 keyMsg)
		{
			if (keyMsg >= 'A' + 80 && keyMsg <= 'Z' + 80) {
				return (keycode)((unsigned int)key_A + (keyMsg - 'A' - 80));
			}
			else if (keyMsg >= '0' + 80 && keyMsg <= '9' + 80) {
				return (keycode)((unsigned int)key_0 + (keyMsg - '0' - 80));
			}
			else
			{
				switch (keyMsg) {
					TRANSLATE_KEY(KEY_BACK, key_backspace);
					TRANSLATE_KEY(KEY_TAB, key_tab);
					//TRANSLATE_KEY(KEY_CLEAR, );
					//TRANSLATE_KEY(KEY_RETURN, key_enter);
					TRANSLATE_KEY(KEY_PAUSE, key_pause_break);
					TRANSLATE_KEY(KEY_ESCAPE, key_escape);
					TRANSLATE_KEY(KEY_SPACE, key_space);
					//TRANSLATE_KEY(KEY_NEXT, key_page_up);
					//TRANSLATE_KEY(KEY_END, key_page_down);
					TRANSLATE_KEY(KEY_HOME, key_home);

					TRANSLATE_KEY(KEY_LEFT, key_left_arrow);
					TRANSLATE_KEY(KEY_UP, key_up_arrow);
					TRANSLATE_KEY(KEY_RIGHT, key_right_arrow);
					TRANSLATE_KEY(KEY_DOWN, key_down_arrow);
					TRANSLATE_KEY(KEY_PAGEUP, key_page_up);
					TRANSLATE_KEY(KEY_PAGEDOWN, key_page_down);

					TRANSLATE_KEY(KEY_SELECT, key_select);
					//TRANSLATE_KEY(KEY_PRINT, key_prin
					TRANSLATE_KEY(KEY_ENTER, key_enter);
					//TRANSLATE_KEY(KEY_SNAPSHOT, 
					TRANSLATE_KEY(KEY_INSERT, key_insert);
					TRANSLATE_KEY(KEY_DELETE, key_delete);
					//TRANSLATE_KEY(KEY_HELP, key_hel
					TRANSLATE_KEY(KEY_NUMPAD0, key_numpad_0);
					TRANSLATE_KEY(KEY_NUMPAD1, key_numpad_1);
					TRANSLATE_KEY(KEY_NUMPAD2, key_numpad_2);
					TRANSLATE_KEY(KEY_NUMPAD3, key_numpad_3);
					TRANSLATE_KEY(KEY_NUMPAD4, key_numpad_4);
					TRANSLATE_KEY(KEY_NUMPAD5, key_numpad_5);
					TRANSLATE_KEY(KEY_NUMPAD6, key_numpad_6);
					TRANSLATE_KEY(KEY_NUMPAD7, key_numpad_7);
					TRANSLATE_KEY(KEY_NUMPAD8, key_numpad_8);
					TRANSLATE_KEY(KEY_NUMPAD9, key_numpad_9);
					TRANSLATE_KEY(KEY_MULTIPLY, key_multiply);
					TRANSLATE_KEY(KEY_ADD, key_add);
					//TRANSLATE_KEY(KEY_SEPARATOR, key_decimal_point);
					TRANSLATE_KEY(KEY_SUBTRACT, key_subtract);
					TRANSLATE_KEY(KEY_DECIMAL, key_decimal_point);
					TRANSLATE_KEY(KEY_DIVIDE, key_divide);
					TRANSLATE_KEY(KEY_F1, key_f1);
					TRANSLATE_KEY(KEY_F2, key_f2);
					TRANSLATE_KEY(KEY_F3, key_f3);
					TRANSLATE_KEY(KEY_F4, key_f4);
					TRANSLATE_KEY(KEY_F5, key_f5);
					TRANSLATE_KEY(KEY_F6, key_f6);
					TRANSLATE_KEY(KEY_F7, key_f7);
					TRANSLATE_KEY(KEY_F8, key_f8);
					TRANSLATE_KEY(KEY_F9, key_f9);
					TRANSLATE_KEY(KEY_F10, key_f10);
					TRANSLATE_KEY(KEY_F11, key_f11);
					TRANSLATE_KEY(KEY_F12, key_f12);
					TRANSLATE_KEY(KEY_NUMLOCK, key_num_lock);
					TRANSLATE_KEY(KEY_SCROLL, key_scroll_lock);

					TRANSLATE_KEY(KEY_SHIFT, key_shift);
					TRANSLATE_KEY(KEY_CONTROL, key_ctrl);
					TRANSLATE_KEY(KEY_ALT, key_alt);

				default:
					return key_unknown;
				}
			}
		}

		/*
		*	Abstract vst3 display implementation
		*/

		abstract_vst3_display::abstract_vst3_display(View::widget& root_widget)
		: abstract_display(root_widget)
		{
		}

		abstract_vst3_display::~abstract_vst3_display()
		{
		}

		/*
		*	Abstrat vst3 view implementation
		*/

		abstract_vst3_view::abstract_vst3_view(
			abstract_vst3_display *display)
			: m_display(display)
		{
		}

		abstract_vst3_view::~abstract_vst3_view()
		{
		}

		void abstract_vst3_view::set_size(const unsigned int width, const unsigned int height)
		{
			if (plugFrame != nullptr) {
				Steinberg::ViewRect rect(0, 0, width, height);
				plugFrame->resizeView(this, &rect);
			}
		}

		Steinberg::tresult PLUGIN_API abstract_vst3_view::onWheel(float distance)
		{
			return m_display->sys_mouse_wheel(distance);
		}

		Steinberg::tresult PLUGIN_API abstract_vst3_view::onKeyDown(Steinberg::char16 key, Steinberg::int16 keyMsg, Steinberg::int16 modifiers)
		{
			DEBUG_PRINT("KEY\n");
			const keycode k = vst3_keycode_to_gammou_keycode(key, keyMsg);
			return m_display->sys_key_down(k) ? Steinberg::kResultTrue : Steinberg::kResultFalse;
		}

		Steinberg::tresult PLUGIN_API abstract_vst3_view::onKeyUp(Steinberg::char16 key, Steinberg::int16 keyMsg, Steinberg::int16 modifiers)
		{
			const keycode k = vst3_keycode_to_gammou_keycode(key, keyMsg);
			return m_display->sys_key_up(k) ? Steinberg::kResultTrue : Steinberg::kResultFalse;

		}

		void abstract_vst3_view::attachedToParent()
		{
			DEBUG_PRINT("Attached to parent\n");
			if (m_display != nullptr) {
				m_display->open(systemWindow);
				set_size(
					m_display->get_display_width(),
					m_display->get_display_height());
			}
		}

		void abstract_vst3_view::removedFromParent()
		{
			if (m_display != nullptr)
				m_display->close();
		}


		

} /* View */

} /* Gammou */