@tool
extends Terrain


@export var generate: bool = false : set = set_generate

func set_generate(v):
	generate_mesh()
