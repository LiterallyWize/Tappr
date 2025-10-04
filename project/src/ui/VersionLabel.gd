@tool
extends Label
class_name VersionLabel

func _ready():
	ProjectSettings.settings_changed.connect(_update_label)
	_update_label()

func _update_label():
	text = "v" + ProjectSettings.get_setting("application/config/version")
