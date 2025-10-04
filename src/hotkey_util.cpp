#include "hotkey_util.hpp"

#include <unordered_map>

using namespace godot;

UINT HotkeyUtil::get_vk(const Ref<InputEventKey> &event) {
	static const std::unordered_map<Key, UINT> key_map = {
		{KEY_F1, VK_F1}, {KEY_F2, VK_F2},
		{KEY_F3, VK_F3}, {KEY_F4, VK_F4},
		{KEY_F5, VK_F5}, {KEY_F6, VK_F6},
		{KEY_F7, VK_F7}, {KEY_F8, VK_F8},
		{KEY_F9, VK_F9}, {KEY_F10, VK_F10},
		{KEY_F11, VK_F11}, {KEY_F12, VK_F12},
		{KEY_F13, VK_F13}, {KEY_F14, VK_F14},
		{KEY_F15, VK_F15}, {KEY_F16, VK_F16},
		{KEY_F17, VK_F17}, {KEY_F18, VK_F18},
		{KEY_F19, VK_F19}, {KEY_F20, VK_F20},
		{KEY_F21, VK_F21}, {KEY_F22, VK_F22},
		{KEY_F23, VK_F23}, {KEY_F24, VK_F24},

		{KEY_0, '0'}, {KEY_1, '1'}, {KEY_2, '2'}, {KEY_3, '3'}, {KEY_4, '4'},
		{KEY_5, '5'}, {KEY_6, '6'}, {KEY_7, '7'}, {KEY_8, '8'}, {KEY_9, '9'},

		{KEY_A, 'A'}, {KEY_B, 'B'},
		{KEY_C, 'C'}, {KEY_D, 'D'},
		{KEY_E, 'E'}, {KEY_F, 'F'},
		{KEY_G, 'G'}, {KEY_H, 'H'},
		{KEY_I, 'I'}, {KEY_J, 'J'},
		{KEY_K, 'K'}, {KEY_L, 'L'},
		{KEY_M, 'M'}, {KEY_N, 'N'},
		{KEY_O, 'O'}, {KEY_P, 'P'},
		{KEY_Q, 'Q'}, {KEY_R, 'R'},
		{KEY_S, 'S'}, {KEY_T, 'T'},
		{KEY_U, 'U'}, {KEY_V, 'V'},
		{KEY_W, 'W'}, {KEY_X, 'X'},
		{KEY_Y, 'Y'}, {KEY_Z, 'Z'},

		{KEY_QUOTELEFT, VK_OEM_3}, {KEY_TAB, VK_TAB}, {KEY_BACKSPACE, VK_BACK},

		{KEY_MINUS, VK_OEM_MINUS}, {KEY_EQUAL, VK_OEM_PLUS},
		{KEY_BRACKETLEFT, VK_OEM_4}, {KEY_BRACKETRIGHT, VK_OEM_6},
		{KEY_BACKSLASH, VK_OEM_5}, {KEY_SEMICOLON, VK_OEM_1},
		{KEY_APOSTROPHE, VK_OEM_7}, {KEY_COMMA, VK_OEM_COMMA},
		{KEY_PERIOD, VK_OEM_PERIOD}, {KEY_SLASH, VK_OEM_2},

		{KEY_INSERT, VK_INSERT}, {KEY_HOME, VK_HOME}, {KEY_PAGEUP, VK_PRIOR},
		{KEY_DELETE, VK_DELETE}, {KEY_END, VK_END}, {KEY_PAGEDOWN, VK_NEXT},

		{KEY_LEFT, VK_LEFT}, {KEY_UP, VK_UP},
		{KEY_RIGHT, VK_RIGHT}, {KEY_DOWN, VK_DOWN},

		{KEY_KP_DIVIDE, VK_DIVIDE}, {KEY_KP_MULTIPLY, VK_MULTIPLY}, {KEY_KP_SUBTRACT, VK_SUBTRACT},
		{KEY_KP_ADD, VK_ADD}, {KEY_KP_PERIOD, VK_DECIMAL}, {KEY_KP_ENTER, VK_RETURN},

		{KEY_KP_7, VK_NUMPAD7}, {KEY_KP_8, VK_NUMPAD8}, {KEY_KP_9, VK_NUMPAD9},
		{KEY_KP_4, VK_NUMPAD4}, {KEY_KP_5, VK_NUMPAD5}, {KEY_KP_6, VK_NUMPAD6},
		{KEY_KP_1, VK_NUMPAD1}, {KEY_KP_2, VK_NUMPAD2}, {KEY_KP_3, VK_NUMPAD3},
		{KEY_KP_0, VK_NUMPAD0}
	};

	const Key key = event->get_physical_keycode();
	const auto it = key_map.find(key);
	if (it != key_map.end()) {
		return it->second;
	}

	if (key >= 0x30 && key <= 0x5A) return static_cast<UINT>(key);
	return 0;
}

UINT HotkeyUtil::get_modifiers(const Ref<InputEventKey> &event) {
	UINT modifiers = 0;
	if (event->is_shift_pressed()) modifiers |= MOD_SHIFT;
	if (event->is_ctrl_pressed()) modifiers |= MOD_CONTROL;
	if (event->is_alt_pressed()) modifiers |= MOD_ALT;
	if (event->is_meta_pressed()) modifiers |= MOD_WIN;

	return modifiers;
}
