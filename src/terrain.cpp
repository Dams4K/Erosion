#include "terrain.h"

#include "terrain_modifier.h"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/godot.hpp>

#include <godot_cpp/classes/plane_mesh.hpp>
#include <godot_cpp/classes/array_mesh.hpp>
#include <godot_cpp/classes/mesh.hpp>

using namespace godot;

void Terrain::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_modifiers", "p_modifiers"), &Terrain::set_modifiers);
    ClassDB::bind_method(D_METHOD("get_modifiers"), &Terrain::get_modifiers);
    ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "modifiers", PROPERTY_HINT_ARRAY_TYPE, "24/17:TerrainModifier"), "set_modifiers", "get_modifiers");
}

Terrain::Terrain() {
    mesh_instance = memnew(MeshInstance3D);
    add_child(mesh_instance);
}

Terrain::~Terrain() {}

void Terrain::set_modifiers(const Array p_modifiers) { modifiers = p_modifiers; }
Array Terrain::get_modifiers() const { return modifiers; }


void Terrain::initialize_mesh_instance() {
    Ref<PlaneMesh> mesh;
    mesh.instantiate();

    mesh_instance->set_mesh(mesh);
}

void Terrain::apply_modifiers() {
    
}

void Terrain::generate_mesh() {
    Array surface_array;
    surface_array.resize(Mesh::ARRAY_MAX);

    PackedVector3Array verts;
    PackedVector2Array uvs;
    PackedVector3Array normals;
    PackedInt32Array indices;

    // Fill the different arrays

    // Create mesh
    surface_array[Mesh::ARRAY_VERTEX] = verts;
    surface_array[Mesh::ARRAY_TEX_UV] = uvs;
    surface_array[Mesh::ARRAY_NORMAL] = normals;
    surface_array[Mesh::ARRAY_INDEX] = indices;

    Ref<ArrayMesh> mesh;
    mesh.instantiate();
    mesh->add_surface_from_arrays(Mesh::PRIMITIVE_TRIANGLES, surface_array);

    
}