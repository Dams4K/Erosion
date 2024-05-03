#include "droplet_emitter.h"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/godot.hpp>

#include <godot_cpp/classes/random_number_generator.hpp>

#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

void DropletEmitter::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_terrain_path", "p_terrain_path"), &DropletEmitter::set_terrain_path);
    ClassDB::bind_method(D_METHOD("get_terrain_path"), &DropletEmitter::get_terrain_path);
    ADD_PROPERTY(PropertyInfo(Variant::NODE_PATH, "terrain_path", PROPERTY_HINT_NODE_PATH_VALID_TYPES, "Terrain"), "set_terrain_path", "get_terrain_path");

    ClassDB::bind_method(D_METHOD("get_terrain"), &DropletEmitter::get_terrain);

    ClassDB::bind_method(D_METHOD("simulate_droplet"), &DropletEmitter::simulate_droplet);
}

DropletEmitter::DropletEmitter() {

}

DropletEmitter::~DropletEmitter() {

}

void DropletEmitter::set_terrain_path(const NodePath p_terrain_path) { terrain_path = p_terrain_path; }
NodePath DropletEmitter::get_terrain_path() const { return terrain_path; }

Terrain* DropletEmitter::get_terrain() const {
    Node* node = get_node_or_null(terrain_path);
    Terrain* terrain = Object::cast_to<Terrain>(node);
    
    return terrain;
}

void DropletEmitter::simulate_droplet() {
    Terrain* terrain = get_terrain();
    Ref<MeshDataTool> mdt = terrain->get_mdt();

    // Random position for the droplet
    Ref<RandomNumberGenerator> rng;
    rng.instantiate();
    rng->randomize();

    int terrain_size = terrain->get_mesh_size();
    int half_terrain_size = terrain_size/2;
    int x = rng->randf_range(-half_terrain_size, half_terrain_size);
    int y = rng->randf_range(-half_terrain_size, half_terrain_size);

    int lifetime = 30; //TODO: exported var
    for (int i = 0; i < lifetime; i++) {

        
    }
}

PackedVector3Array DropletEmitter::get_surroundings(float x, float y, Ref<MeshDataTool> mdt, int size) {
        int ix = (int) x;
        int iy = (int) y;

        // Gradient
        float u = x - ix;
        float v = y - iy;

        // The indices of the vertices surrounding the droplet
        int v00_i = iy * (size+1) + ix;
        int v10_i = v00_i + 1;
        int v01_i = v00_i + (size+1);
        int v11_i = v01_i + 1;

        Vector3 v00 = mdt->get_vertex(v00_i);
        Vector3 v10 = mdt->get_vertex(v10_i);
        Vector3 v01 = mdt->get_vertex(v01_i);
        Vector3 v11 = mdt->get_vertex(v11_i);

        PackedVector3Array arr = PackedVector3Array();
        arr.append(v00);
        arr.append(v10);
        arr.append(v01);
        arr.append(v11);
        return arr;
}