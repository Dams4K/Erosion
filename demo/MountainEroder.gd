@tool
extends Eroder

@export var start: bool = false : set = set_start

func set_start(value):
	start_simulation(129)
