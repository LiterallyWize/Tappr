#pragma once

#include <condition_variable>
#include <mutex>
#include <atomic>
#include <chrono>

#include <windows.h>

#include <godot_cpp/godot.hpp>
#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/classes/thread.hpp>
#include <godot_cpp/classes/input_event_key.hpp>

namespace godot {

class Clicker : public RefCounted {
	GDCLASS(Clicker, RefCounted);

public:
	Clicker();
	~Clicker();

	void start_listen();
	void stop_listen();
	void start_click();
	void stop_click();

	void set_handle(const int64_t handle);
	void set_hotkey(const Ref<InputEventKey> &event);
	void set_interval(
		const int64_t milliseconds,
		const int64_t seconds,
		const int64_t minutes,
		const int64_t hours
	);

	void set_mouse_button(const int64_t p_mouse_button);
	int64_t get_mouse_button() const;

	void set_repeat_count(const int64_t p_repeat_count);
	int64_t get_repeat_count() const;

	void set_click_count(const int64_t p_click_count);
	int64_t get_click_count() const;

private:
	Ref<Thread> listener_thread;
	std::atomic<DWORD> listener_thread_id{0};
	std::atomic<bool> listener_running{false};

	Ref<Thread> clicker_thread;
	std::atomic<bool> clicker_running{false};
	std::mutex clicker_mutex;
	std::condition_variable clicker_cvar; 

	std::atomic<HWND> hwnd;

	std::atomic<std::chrono::milliseconds> interval;
	std::atomic<UINT> hotkey_modifiers{0};
	std::atomic<UINT> hotkey_vk{0};

	std::atomic<int64_t> mouse_button{0};
	std::atomic<int64_t> repeat_count{0};
	std::atomic<int64_t> click_count{1};

	void listen_loop();
	static void pass_hotkey(const UINT modifiers, const UINT vk);
	void click_loop();
	static void send_click(const int64_t button);

protected:
	static void _bind_methods();
};

}
