@tool
extends DropletEmitter

@export var simulate_one: bool = false : set = set_simulate_one


func set_simulate_one(v):
	simulate_droplet()
