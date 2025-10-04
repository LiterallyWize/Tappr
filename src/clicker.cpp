#include "clicker.hpp"

#include <vector>
#include <thread>

#include <godot_cpp/core/class_db.hpp>

#include "hotkey_util.hpp"

using namespace godot;

Clicker::Clicker() {
	listener_thread.instantiate();
	clicker_thread.instantiate();

	interval = std::chrono::milliseconds(0);
}

Clicker::~Clicker() {
	stop_listen();
	stop_click();
}

void Clicker::start_listen() {
	if (listener_thread->is_started()) return;
	if (hotkey_vk.load() == 0) return;
	listener_thread->start(callable_mp(this, &Clicker::listen_loop));
}

void Clicker::stop_listen() {
	if (!listener_thread->is_started()) return;
	listener_running.store(false);

	const DWORD thread_id = listener_thread_id.load();
	if (thread_id != 0) {
		PostThreadMessage(thread_id, WM_QUIT, 0, 0);
	}

	listener_thread->wait_to_finish();
}

void Clicker::listen_loop() {
	const UINT modifiers = hotkey_modifiers.load();
	const UINT vk = hotkey_vk.load();
	if (!RegisterHotKey(nullptr, 1, modifiers, vk)) {
		emit_signal("hotkey_failed");
		return;
	}

	listener_thread_id.store(GetCurrentThreadId());
	listener_running.store(true);

	MSG dummy;
	PeekMessage(&dummy, nullptr, 0, 0, PM_NOREMOVE);

	MSG msg;
	while (listener_running.load()
		&& (GetMessage(&msg, nullptr, 0, 0) != 0)
	) {
		if (msg.message == WM_HOTKEY) {
			UnregisterHotKey(nullptr, 1);
			pass_hotkey(modifiers, vk);
			
			if (!RegisterHotKey(nullptr, 1, modifiers, vk)) {
				emit_signal("hotkey_failed");
				listener_running.store(false);
				listener_thread_id.store(0);
				return;
			}

			if (clicker_running.load()) {
				stop_click();
			} else {
				start_click();
			}
		}
	}

	UnregisterHotKey(nullptr, 1);
	listener_running.store(false);
	listener_thread_id.store(0);
}

inline static INPUT create_kbinput(const UINT vk, const bool up) {
	INPUT input{};
	input.type = INPUT_KEYBOARD;
	input.ki.wVk = vk;
	if (up) input.ki.dwFlags = KEYEVENTF_KEYUP;

	return input;
}

void Clicker::pass_hotkey(const UINT modifiers, const UINT vk) {
	std::vector<INPUT> inputs;

	if (modifiers & MOD_CONTROL) {
		inputs.push_back(create_kbinput(VK_CONTROL, false));
	}
	if (modifiers & MOD_SHIFT) {
		inputs.push_back(create_kbinput(VK_SHIFT, false));
	}
	if (modifiers & MOD_ALT) {
		inputs.push_back(create_kbinput(VK_MENU, false));
	}

	inputs.push_back(create_kbinput(vk, false));
	inputs.push_back(create_kbinput(vk, true));

	if (modifiers & MOD_ALT) {
		inputs.push_back(create_kbinput(VK_MENU, true));
	}
	if (modifiers & MOD_SHIFT) {
		inputs.push_back(create_kbinput(VK_SHIFT, true));
	}
	if (modifiers & MOD_CONTROL) {
		inputs.push_back(create_kbinput(VK_CONTROL, true));
	}

	SendInput(static_cast<UINT>(inputs.size()), inputs.data(), sizeof(INPUT));
}

void Clicker::start_click() {
	if (clicker_thread->is_started()) return;
	emit_signal("click_started");
	clicker_thread->start(callable_mp(this, &Clicker::click_loop));
}

void Clicker::stop_click() {
	if (!clicker_thread->is_started()) return;
	clicker_running.store(false);

	clicker_cvar.notify_all();
	clicker_thread->wait_to_finish();
}

void Clicker::click_loop() {
	const HWND _hwnd = hwnd.load();

	const std::chrono::milliseconds _interval = interval.load();
	const std::chrono::milliseconds sleep_time =
		(_interval.count() > 0) ? _interval : std::chrono::milliseconds(1);

	const int64_t _mouse_button = mouse_button.load();
	const int64_t _repeat_count = repeat_count.load();
	const int64_t _click_count = click_count.load();
	int64_t c = 0;

	clicker_running.store(true);

	while (clicker_running.load()
		&& (_repeat_count == 0 || c < _repeat_count)
	) {
		POINT cursor_pos;
		GetCursorPos(&cursor_pos);

		const HWND window_hovered = WindowFromPoint(cursor_pos);
		if (_hwnd == nullptr || window_hovered != _hwnd) {
			for (int64_t i = 0; i < _click_count; ++i) {
				send_click(_mouse_button);
			}
			++c;
		}

		std::unique_lock<std::mutex> lock(clicker_mutex);
		clicker_cvar.wait_for(lock, sleep_time, [&]() {
			return !clicker_running.load();
		});
	}

	clicker_running.store(false);
	emit_signal("click_stopped");
}

void Clicker::send_click(const int64_t button) {
	static constexpr struct {
		DWORD down;
		DWORD up;
	} button_map[3] = {
		{MOUSEEVENTF_LEFTDOWN, MOUSEEVENTF_LEFTUP},
		{MOUSEEVENTF_RIGHTDOWN, MOUSEEVENTF_RIGHTUP},
		{MOUSEEVENTF_MIDDLEDOWN, MOUSEEVENTF_MIDDLEUP}
	};

	INPUT inputs[2] = {};
	inputs[0].type = INPUT_MOUSE;
	inputs[1].type = INPUT_MOUSE;

	inputs[0].mi.dwFlags = button_map[button].down;
	inputs[1].mi.dwFlags = button_map[button].up;

	SendInput(2, inputs, sizeof(INPUT));
}

void Clicker::set_handle(const int64_t handle) {
	hwnd.store(reinterpret_cast<HWND>(handle));
}

void Clicker::set_hotkey(const Ref<InputEventKey> &event) {
	hotkey_modifiers.store(HotkeyUtil::get_modifiers(event));
	hotkey_vk.store(HotkeyUtil::get_vk(event));
}

void Clicker::set_interval(
	const int64_t milliseconds,
	const int64_t seconds,
	const int64_t minutes,
	const int64_t hours
) {
	interval.store(
		std::chrono::milliseconds((milliseconds < 0) ? 0 : milliseconds)
		+ std::chrono::seconds((seconds < 0) ? 0 : seconds)
		+ std::chrono::minutes((minutes < 0) ? 0 : minutes)
		+ std::chrono::hours((hours < 0) ? 0 : hours)
	);
}

void Clicker::set_mouse_button(const int64_t p_mouse_button) {
	mouse_button.store((p_mouse_button < 0 || p_mouse_button > 2) ? 0 : p_mouse_button);
}
int64_t Clicker::get_mouse_button() const {
	return mouse_button.load();
}

void Clicker::set_repeat_count(const int64_t p_repeat_count) {
	repeat_count.store((p_repeat_count < 0) ? 0 : p_repeat_count);
}
int64_t Clicker::get_repeat_count() const {
	return repeat_count.load();
}

void Clicker::set_click_count(const int64_t p_count) {
	click_count.store((p_count < 1) ? 1 : p_count);
}
int64_t Clicker::get_click_count() const {
	return click_count.load();
}

void Clicker::_bind_methods() {
	ClassDB::bind_method(D_METHOD("start_click"), &Clicker::start_click);
	ClassDB::bind_method(D_METHOD("stop_click"), &Clicker::stop_click);
	ClassDB::bind_method(D_METHOD("start_listen"), &Clicker::start_listen);
	ClassDB::bind_method(D_METHOD("stop_listen"), &Clicker::stop_listen);

	ClassDB::bind_method(D_METHOD("set_handle", "handle"), &Clicker::set_handle);
	ClassDB::bind_method(D_METHOD("set_hotkey", "hotkey"), &Clicker::set_hotkey);
	ClassDB::bind_method(D_METHOD("set_interval", "milliseconds", "seconds", "minutes", "hours"), &Clicker::set_interval);

	ClassDB::bind_method(D_METHOD("get_mouse_button"), &Clicker::get_mouse_button);
	ClassDB::bind_method(D_METHOD("set_mouse_button", "p_mouse_button"), &Clicker::set_mouse_button);
	ADD_PROPERTY(PropertyInfo(Variant::INT, "mouse_button"), "set_mouse_button", "get_mouse_button");

	ClassDB::bind_method(D_METHOD("get_repeat_count"), &Clicker::get_repeat_count);
	ClassDB::bind_method(D_METHOD("set_repeat_count", "p_repeat_count"), &Clicker::set_repeat_count);
	ADD_PROPERTY(PropertyInfo(Variant::INT, "repeat_count"), "set_repeat_count", "get_repeat_count");

	ClassDB::bind_method(D_METHOD("get_click_count"), &Clicker::get_click_count);
	ClassDB::bind_method(D_METHOD("set_click_count", "p_click_count"), &Clicker::set_click_count);
	ADD_PROPERTY(PropertyInfo(Variant::INT, "click_count"), "set_click_count", "get_click_count");

	ADD_SIGNAL(MethodInfo("hotkey_failed"));
	ADD_SIGNAL(MethodInfo("click_started"));
	ADD_SIGNAL(MethodInfo("click_stopped"));
}
