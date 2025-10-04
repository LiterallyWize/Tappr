#pragma once

#include <windows.h>

#include <godot_cpp/godot.hpp>
#include <godot_cpp/classes/input_event_key.hpp>

namespace godot {

class HotkeyUtil {
public:
	static UINT get_vk(const Ref<InputEventKey> &event);
	static UINT get_modifiers(const Ref<InputEventKey> &event);
};

}
