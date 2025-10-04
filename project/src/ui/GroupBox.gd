@tool
extends MarginContainer
class_name GroupBox

@export var title := "Group":
	set(value):
		title = value
		_update_title()

@onready var title_label := %Title

func _ready() -> void:
	_update_title()

func _update_title() -> void:
	if title_label:
		title_label.text = title
		title_label.visible = not title.is_empty()
