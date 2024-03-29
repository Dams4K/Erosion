#include "terrain.h"

#include "terrain_modifier.h"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/godot.hpp>

#include <godot_cpp/classes/plane_mesh.hpp>
#include <godot_cpp/classes/array_mesh.hpp>
#include <godot_cpp/classes/surface_tool.hpp>
#include <godot_cpp/classes/mesh.hpp>

using namespace godot;

void Terrain::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_modifiers", "p_modifiers"), &Terrain::set_modifiers);
    ClassDB::bind_method(D_METHOD("get_modifiers"), &Terrain::get_modifiers);
    ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "modifiers", PROPERTY_HINT_ARRAY_TYPE, "24/17:TerrainModifier"), "set_modifiers", "get_modifiers");

    ClassDB::bind_method(D_METHOD("generate_mesh"), &Terrain::generate_mesh);

    ClassDB::bind_method(D_METHOD("set_size", "p_size"), &Terrain::set_size);
    ClassDB::bind_method(D_METHOD("get_size"), &Terrain::get_size);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "size"), "set_size", "get_size");

    ClassDB::bind_method(D_METHOD("set_resolution_scale", "p_scale"), &Terrain::set_resolution_scale);
    ClassDB::bind_method(D_METHOD("get_resolution_scale"), &Terrain::get_resolution_scale);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "resolution_scale"), "set_resolution_scale", "get_resolution_scale");
}

Terrain::Terrain() {
    mesh_instance = memnew(MeshInstance3D);
    add_child(mesh_instance);
}

Terrain::~Terrain() {}

void Terrain::set_modifiers(const Array p_modifiers) { modifiers = p_modifiers; }
Array Terrain::get_modifiers() const { return modifiers; }

void Terrain::set_size(const int p_size) { size = p_size; }
int Terrain::get_size() const { return size; };

void Terrain::set_resolution_scale(const float p_scale) { resolution_scale = p_scale; }
float Terrain::get_resolution_scale() const { return resolution_scale; }

void Terrain::initialize_mesh_instance() {
    Ref<PlaneMesh> mesh;
    mesh.instantiate();

    mesh_instance->set_mesh(mesh);
}

void Terrain::apply_modifiers() {
    
}

void Terrain::generate_mesh() {
    Ref<PlaneMesh> plane_mesh;

    Vector2 v_size = Vector2(size, size);
    Vector2 sub_size = v_size * resolution_scale;

    plane_mesh.instantiate();
    plane_mesh->set_size(v_size);
    plane_mesh->set_subdivide_depth(sub_size.x-1);
    plane_mesh->set_subdivide_width(sub_size.y-1);

    SurfaceTool* st = memnew(SurfaceTool);
    st->create_from(plane_mesh, 0);
    Ref<ArrayMesh> mesh = st->commit();

    mdt.instantiate();
    mdt->create_from_surface(mesh, 0);

    for (int vert_i = 0; vert_i < mdt->get_vertex_count(); vert_i++) {
        Vector3 vertex = mdt->get_vertex(vert_i);
        for (int mod_i = 0; mod_i < modifiers.size(); mod_i++) {
            Ref<TerrainModifier> modifier = modifiers[mod_i];
            if (modifier == nullptr) { continue; }

            double height = modifier->get_height((int) (vertex.x * resolution_scale), (int) (vertex.z * resolution_scale), sub_size);
            switch (modifier->get_mode()) {
            case TerrainModifier::ModifierMode::ADD:
                vertex.y += height;
                break;
            case TerrainModifier::ModifierMode::SUB:
                vertex.y -= height;
                break;
            case TerrainModifier::ModifierMode::MUL:
                vertex.y *= height;
                break;
            }
        }

        mdt->set_vertex(vert_i, vertex);
    }

    // Calculate the normals
    for (int i = 0; i < mdt->get_face_count(); i++) {
        int a = mdt->get_face_vertex(i, 0);
        int b = mdt->get_face_vertex(i, 1);
        int c = mdt->get_face_vertex(i, 2);

        Vector3 ap = mdt->get_vertex(a);
        Vector3 bp = mdt->get_vertex(b);
        Vector3 cp = mdt->get_vertex(c);

        Vector3 n = (bp - cp).cross(ap - bp).normalized();

        mdt->set_vertex_normal(a, n + mdt->get_vertex_normal(a));
        mdt->set_vertex_normal(b, n + mdt->get_vertex_normal(b));
        mdt->set_vertex_normal(c, n + mdt->get_vertex_normal(c));
    }

    for (int i = 0; i < mdt->get_vertex_count(); i++) {
        Vector3 v = mdt->get_vertex_normal(i).normalized();
        mdt->set_vertex_normal(i, v);
        mdt->set_vertex_color(i, Color(v.x, v.y, v.z));
    }

    mesh->clear_surfaces();
    mdt->commit_to_surface(mesh);

    mesh_instance->set_mesh(mesh);
}