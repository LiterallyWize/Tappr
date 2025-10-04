extends LineEdit
class_name HotkeyBox

const HOTKEY_DEFAULT := preload("res://assets/hotkey_default.tres")

@export var hotkey_event: InputEventKey
signal hotkey_submitted(event: InputEventKey)

var pending_event: InputEventKey

func hotkey_fail() -> void:
	text = "Hotkey failed to register!"

func _ready() -> void:
	hotkey_event = HOTKEY_DEFAULT
	_update_text()

	self.focus_entered.connect(_focus_entered)
	self.focus_exited.connect(_focus_exited)
	self.text_changed.connect(_text_changed)

func _gui_input(event: InputEvent) -> void:
	if not editable: return
	if event is InputEventKey:
		if event.is_echo() or not event.is_pressed():
			return

		if event.keycode == KEY_ENTER or event.keycode == KEY_KP_ENTER:
			if pending_event and _is_modifier(pending_event.keycode):
				hotkey_event = null
			else:
				hotkey_event = pending_event

			if hotkey_event:
				emit_signal("hotkey_submitted", hotkey_event)

			pending_event = null
			call_deferred("release_focus")
			return

		if event.keycode == KEY_ESCAPE:
			pending_event = null
			call_deferred("release_focus")
			return

		pending_event = InputEventKey.new()
		pending_event.keycode = event.keycode
		pending_event.physical_keycode = event.physical_keycode
		pending_event.shift_pressed = event.shift_pressed
		pending_event.ctrl_pressed = event.ctrl_pressed
		pending_event.alt_pressed = event.alt_pressed
		_update_text()

func _update_text() -> void:
	if pending_event:
		text = pending_event.as_text().to_upper().remove_char(0x20)
	elif hotkey_event:
		text = hotkey_event.as_text().to_upper().remove_char(0x20)
	else:
		text = "None"

func _is_modifier(key: Key) -> bool:
	match key:
		KEY_SHIFT: return true
		KEY_CTRL: return true
		KEY_ALT: return true
		KEY_META: return true
	return false

func _focus_entered() -> void:
	if not editable: return
	text = "..."

func _focus_exited() -> void:
	if not editable: return
	pending_event = null
	_update_text()

func _text_changed(_input: String) -> void:
	_update_text()
