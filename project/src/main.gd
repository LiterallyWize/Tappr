extends Panel

const THEME_DARK := preload("res://assets/themes/dark.tres")
const THEME_LIGHT := preload("res://assets/themes/light.tres")

@onready var clicker := Clicker.new()

@onready var button_toggle := %ButtonToggle
@onready var input_hotkey := %InputHotkey

@onready var interval_keys: Dictionary[String, IntBox] = {
	"milliseconds": %InputMilliseconds,
	"seconds": %InputSeconds,
	"minutes": %InputMinutes,
	"hours": %InputHours
}

@onready var to_disable: Array[Control] = [
	%InputHours, %InputMinutes, %InputSeconds, %InputMilliseconds,
	%InputMouseButton, %InputRepeatCount, %InputClickCount,
	%InputHotkey
]

var clicking := false

func _ready() -> void:
	DisplayServer.set_system_theme_change_callback(_check_theme)
	_check_theme()

	clicker.connect("click_started", _on_click_started, CONNECT_DEFERRED)
	clicker.connect("click_stopped", _on_click_stopped, CONNECT_DEFERRED)
	clicker.connect("hotkey_failed", input_hotkey.hotkey_fail, CONNECT_DEFERRED)

	for key in interval_keys:
		var input := interval_keys[key]
		input.value_changed.connect(_set_interval)
	_set_interval()

	clicker.set_hotkey(input_hotkey.hotkey_event)
	clicker.set_handle(DisplayServer.window_get_native_handle(DisplayServer.WINDOW_HANDLE))
	clicker.start_listen()

func _on_click_started() -> void:
	clicking = true
	button_toggle.text = "Stop"

	for obj in to_disable:
		if obj is BaseButton:
			obj.disabled = true
		elif obj is LineEdit:
			obj.editable = false
		elif obj is SpinBox:
			obj.editable = false

func _on_click_stopped() -> void:
	clicking = false
	button_toggle.text = "Start"

	for obj in to_disable:
		if obj is BaseButton:
			obj.disabled = false
		elif obj is LineEdit:
			obj.editable = true
		elif obj is SpinBox:
			obj.editable = true

func _set_interval(_value: float = 0.0) -> void:
	clicker.set_interval(
		floori(interval_keys["milliseconds"].value),
		floori(interval_keys["seconds"].value),
		floori(interval_keys["minutes"].value),
		floori(interval_keys["hours"].value),
	)

func _check_theme() -> void:
	if DisplayServer.is_dark_mode():
		theme = THEME_DARK
	else:
		theme = THEME_LIGHT

func _on_toggle() -> void:
	if clicking:
		clicker.stop_click()
	else:
		clicker.start_click()

func _hotkey_submitted(event: InputEventKey) -> void:
	clicker.set_hotkey(event)

func _hotkey_focused() -> void:
	if not input_hotkey.editable: return
	clicker.stop_listen()

func _hotkey_unfocused() -> void:
	if not input_hotkey.editable: return
	clicker.start_listen()

func _set_mouse_button(index: int) -> void:
	clicker.mouse_button = index

func _set_repeat_count(value: float) -> void:
	clicker.repeat_count = floori(value)

func _set_click_count(value: float) -> void:
	clicker.click_count = floori(value)
