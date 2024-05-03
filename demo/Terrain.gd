@tool
extends Terrain


@export var generate: bool = false : set = set_generate

@export var debug_vertices: bool = false : set = set_debug_vertices

func set_generate(v):
	generate_mesh()

func set_debug_vertices(v):
	check_vertices_index()
