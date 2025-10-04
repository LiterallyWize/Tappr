@tool
extends SpinBox
class_name IntBox

const FONT := preload("res://assets/fonts/GoogleSansCode-Regular.ttf")

@export_custom(PROPERTY_HINT_RANGE, "0,0,1,or_greater,hide_slider") var max_length := 5:
	set(length):
		max_length = length
		_update_max_length()

@onready var line_edit := self.get_line_edit()

func _ready() -> void:
	_update_max_length()
	line_edit.caret_blink = true
	line_edit.context_menu_enabled = false
	line_edit.add_theme_font_override("font", FONT)

	line_edit.text_changed.connect(_text_changed)
	line_edit.text_submitted.connect(_text_submitted)

func _text_changed(input: String) -> void:
	var column := line_edit.caret_column
	var output := ""

	for c in input:
		if c.is_subsequence_of("0123456789"):
			output += c
		else:
			column -= 1

	if output != input:
		line_edit.text = output
		line_edit.caret_column = column
	if output.is_empty():
		value = 0.0

func _text_submitted(_text: String) -> void:
	if line_edit.has_focus():
		line_edit.call_deferred("release_focus")

func _update_max_length() -> void:
	if line_edit:
		line_edit.max_length = max_length
