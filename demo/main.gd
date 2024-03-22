extends Node3D
@onready var mountain_eroder: Eroder = $MountainEroder
@onready var animation_player: AnimationPlayer = $AnimationPlayer
@onready var label: Label = $CanvasLayer/Label

func _ready() -> void:
	print(Vector2.ZERO.normalized())
	animation_player.play("rotate_camera")
	#RenderingServer.set_debug_generate_wireframes(true)
	#get_viewport().debug_draw = Viewport.DEBUG_DRAW_WIREFRAME
	mountain_eroder.start_simulation(129)

func _process(delta: float) -> void:
	label.text = "%s droplets | %s fps" % [mountain_eroder.droplets_simulated, Engine.get_frames_per_second()]

