
#include "abstract_vst2_display.h"

namespace Gammou {

    namespace View {

		enum VstVirtualKey
		{
			VKEY_BACK = 1,
			VKEY_TAB,
			VKEY_CLEAR,
			VKEY_RETURN,
			VKEY_PAUSE,
			VKEY_ESCAPE,
			VKEY_SPACE,
			VKEY_NEXT,
			VKEY_END,
			VKEY_HOME,
			VKEY_LEFT,
			VKEY_UP,
			VKEY_RIGHT,
			VKEY_DOWN,
			VKEY_PAGEUP,
			VKEY_PAGEDOWN,
			VKEY_SELECT,
			VKEY_PRINT,
			VKEY_ENTER,
			VKEY_SNAPSHOT,
			VKEY_INSERT,
			VKEY_DELETE,
			VKEY_HELP,
			VKEY_NUMPAD0,
			VKEY_NUMPAD1,
			VKEY_NUMPAD2,
			VKEY_NUMPAD3,
			VKEY_NUMPAD4,
			VKEY_NUMPAD5,
			VKEY_NUMPAD6,
			VKEY_NUMPAD7,
			VKEY_NUMPAD8,
			VKEY_NUMPAD9,
			VKEY_MULTIPLY,
			VKEY_ADD,
			VKEY_SEPARATOR,
			VKEY_SUBTRACT,
			VKEY_DECIMAL,
			VKEY_DIVIDE,
			VKEY_F1,
			VKEY_F2,
			VKEY_F3,
			VKEY_F4,
			VKEY_F5,
			VKEY_F6,
			VKEY_F7,
			VKEY_F8,
			VKEY_F9,
			VKEY_F10,
			VKEY_F11,
			VKEY_F12,
			VKEY_NUMLOCK,
			VKEY_SCROLL,
			VKEY_SHIFT,
			VKEY_CONTROL,
			VKEY_ALT,
			VKEY_EQUALS
		};

        abstract_vst2_display::abstract_vst2_display(
            View::widget& root_widget)
        :   abstract_display(root_widget)
        {
        }

        abstract_vst2_display::~abstract_vst2_display()
        {
        }

		int abstract_vst2_display::on_effEditKeyUp(const int32_t index, const int64_t value)
		{
			return static_cast<int>(sys_key_up(convert_key(index, value)));
		}

		int abstract_vst2_display::on_effEditKeyDown(const int32_t index, const int64_t value)
		{
			return static_cast<int>(sys_key_down(convert_key(index, value)));
		}

#define TRANSLATE_KEY(k1, k2) case k1: return k2;

		keycode abstract_vst2_display::convert_key(const int32_t index, const int64_t value)
		{
			DEBUG_PRINT("%u\n", index);

			if (index >= 'a' && index <= 'z') {
				return static_cast<keycode>(
					static_cast<unsigned int>(key_a) + (index - 'a'));
			}
			else {
				switch (value) {
					TRANSLATE_KEY(VKEY_BACK, key_backspace)
						TRANSLATE_KEY(VKEY_TAB, key_tab)
						TRANSLATE_KEY(VKEY_RETURN, key_return)
						TRANSLATE_KEY(VKEY_PAUSE, key_pause_break)
						TRANSLATE_KEY(VKEY_ESCAPE, key_escape)
						TRANSLATE_KEY(VKEY_SPACE, key_space)
						//TRANSLATE_KEY(VKEY_NEXT, key_page_down)
						//TRANSLATE_KEY(VKEY_END, key_page_up)
						TRANSLATE_KEY(VKEY_HOME, key_home)
						TRANSLATE_KEY(VKEY_LEFT, key_left_arrow)
						TRANSLATE_KEY(VKEY_UP, key_up_arrow)
						TRANSLATE_KEY(VKEY_RIGHT, key_right_arrow)
						TRANSLATE_KEY(VKEY_DOWN, key_down_arrow)
						TRANSLATE_KEY(VKEY_PAGEUP, key_page_up)
						TRANSLATE_KEY(VKEY_PAGEDOWN, key_page_down)
						TRANSLATE_KEY(VKEY_SELECT, key_select)
						//TRANSLATE_KEY(VKEY_PRINT,
						TRANSLATE_KEY(VKEY_ENTER, key_enter)
						//TRANSLATE_KEY(VKEY_SNAPSHOT, 
						TRANSLATE_KEY(VKEY_INSERT, key_insert)
						TRANSLATE_KEY(VKEY_DELETE, key_delete)
						//TRANSLATE_KEY(VKEY_HELP, 
						TRANSLATE_KEY(VKEY_NUMPAD0, key_numpad_0)
						TRANSLATE_KEY(VKEY_NUMPAD1, key_numpad_1)
						TRANSLATE_KEY(VKEY_NUMPAD2, key_numpad_2)
						TRANSLATE_KEY(VKEY_NUMPAD3, key_numpad_3)
						TRANSLATE_KEY(VKEY_NUMPAD4, key_numpad_4)
						TRANSLATE_KEY(VKEY_NUMPAD5, key_numpad_5)
						TRANSLATE_KEY(VKEY_NUMPAD6, key_numpad_6)
						TRANSLATE_KEY(VKEY_NUMPAD7, key_numpad_7)
						TRANSLATE_KEY(VKEY_NUMPAD8, key_numpad_8)
						TRANSLATE_KEY(VKEY_NUMPAD9, key_numpad_9)
						TRANSLATE_KEY(VKEY_MULTIPLY, key_multiply)
						TRANSLATE_KEY(VKEY_ADD, key_add)
						TRANSLATE_KEY(VKEY_SEPARATOR, key_comma)
						TRANSLATE_KEY(VKEY_SUBTRACT, key_subtract)
						TRANSLATE_KEY(VKEY_DECIMAL, key_decimal_point)
						TRANSLATE_KEY(VKEY_DIVIDE, key_divide)
						TRANSLATE_KEY(VKEY_F1, key_f1)
						TRANSLATE_KEY(VKEY_F2, key_f2)
						TRANSLATE_KEY(VKEY_F3, key_f3)
						TRANSLATE_KEY(VKEY_F4, key_f4)
						TRANSLATE_KEY(VKEY_F5, key_f5)
						TRANSLATE_KEY(VKEY_F6, key_f6)
						TRANSLATE_KEY(VKEY_F7, key_f7)
						TRANSLATE_KEY(VKEY_F8, key_f8)
						TRANSLATE_KEY(VKEY_F9, key_f9)
						TRANSLATE_KEY(VKEY_F10, key_f10)
						TRANSLATE_KEY(VKEY_F11, key_f11)
						TRANSLATE_KEY(VKEY_F12, key_f12)
						TRANSLATE_KEY(VKEY_NUMLOCK, key_num_lock)
						//TRANSLATE_KEY(VKEY_SCROLL, 
						TRANSLATE_KEY(VKEY_SHIFT, key_shift)
						TRANSLATE_KEY(VKEY_CONTROL, key_ctrl)
						TRANSLATE_KEY(VKEY_ALT, key_alt)
						TRANSLATE_KEY(VKEY_EQUALS, key_equal_sign)
				default:
					return key_unknown;
				}
			}
		}
    } /* View */

} /* Gammou */