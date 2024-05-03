#ifndef DROPLET_EMITTER_H
#define DROPLET_EMITTER_H

#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/classes/collision_shape3d.hpp>
// #include <godot_cpp/

#include "terrain.h"

using namespace godot;

class DropletEmitter : public Node3D {
    GDCLASS(DropletEmitter, Node3D)

    protected:
        static void _bind_methods();

    public:
        DropletEmitter();
        ~DropletEmitter();

        void set_terrain_path(const NodePath p_terrain_path);
        NodePath get_terrain_path() const;

        Terrain* get_terrain() const;

        void simulate_droplet();

        PackedVector3Array get_surroundings(float x, float y, Ref<MeshDataTool> mdt, int size);
    
    private:
        NodePath terrain_path;

        CollisionShape3D source;
        CollisionShape3D boundaries;
};

#endif