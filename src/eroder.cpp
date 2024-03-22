#include "eroder.h"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/godot.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/engine.hpp>

#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/classes/array_mesh.hpp>
#include <godot_cpp/classes/surface_tool.hpp>

#include <godot_cpp/classes/sphere_mesh.hpp>
#include <godot_cpp/classes/standard_material3d.hpp>

#include <godot_cpp/classes/noise.hpp>
#include <godot_cpp/classes/noise_texture2d.hpp>
#include <godot_cpp/classes/image_texture.hpp>
#include <godot_cpp/classes/image.hpp>
#include <godot_cpp/classes/gradient_texture2d.hpp>

#include <godot_cpp/classes/random_number_generator.hpp>

using namespace godot;

void Eroder::_bind_methods() {
    ClassDB::bind_method(D_METHOD("convert_mesh_base"), &Eroder::convert_mesh_base);
    ClassDB::bind_method(D_METHOD("apply_height"), &Eroder::apply_height);
    ClassDB::bind_method(D_METHOD("start_simulation", "mesh_size"), &Eroder::start_simulation);

    ADD_GROUP("Mesh", "mesh");
    ClassDB::bind_method(D_METHOD("set_mesh_base", "p_mesh"), &Eroder::set_mesh_base);
    ClassDB::bind_method(D_METHOD("get_mesh_base"), &Eroder::get_mesh_base);
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "mesh_base", PROPERTY_HINT_RESOURCE_TYPE, "Mesh"), "set_mesh_base", "get_mesh_base");

    ClassDB::bind_method(D_METHOD("set_height_maps", "p_array"), &Eroder::set_height_maps);
    ClassDB::bind_method(D_METHOD("get_height_maps"), &Eroder::get_height_maps);
    ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "mesh_height_maps", PROPERTY_HINT_ARRAY_TYPE, vformat("%s/%s:%s", Variant::OBJECT, PROPERTY_HINT_RESOURCE_TYPE, Texture2D::get_class_static())), "set_height_maps", "get_height_maps");

    ClassDB::bind_method(D_METHOD("set_mesh_height", "p_height"), &Eroder::set_mesh_height);
    ClassDB::bind_method(D_METHOD("get_mesh_height"), &Eroder::get_mesh_height);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "mesh_height"), "set_mesh_height", "get_mesh_height");

    ClassDB::bind_method(D_METHOD("get_mdt"), &Eroder::get_mdt);
    
    ClassDB::bind_method(D_METHOD("set_nb_droplets", "p_nb_droplets"), &Eroder::set_nb_droplets);
    ClassDB::bind_method(D_METHOD("get_nb_droplets"), &Eroder::get_nb_droplets);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "nb_droplets", PROPERTY_HINT_NONE), "set_nb_droplets", "get_nb_droplets");

    ClassDB::bind_method(D_METHOD("get_droplet"), &Eroder::get_droplet);
    ClassDB::bind_method(D_METHOD("set_droplet", "p_droplet"), &Eroder::set_droplet);
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "droplet", PROPERTY_HINT_RESOURCE_TYPE, "Droplet"), "set_droplet", "get_droplet");

    ClassDB::bind_method(D_METHOD("set_droplets_simulated", "p_droplets_simulated"), &Eroder::set_droplets_simulated);
    ClassDB::bind_method(D_METHOD("get_droplets_simulated"), &Eroder::get_droplets_simulated);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "droplets_simulated", PROPERTY_HINT_NONE, "droplets_simulated", PROPERTY_USAGE_NO_EDITOR), "set_droplets_simulated", "get_droplets_simulated");

    ClassDB::bind_method(D_METHOD("set_show_trails", "p_show"), &Eroder::set_show_trails);
    ClassDB::bind_method(D_METHOD("get_show_trails"), &Eroder::get_show_trails);
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "show_trails"), "set_show_trails", "get_show_trails");
}

Eroder::Eroder() {
    // Constructor
    nb_droplets = 10;

    mesh_height = 10;
    mdt.instantiate();

    can_simulate = false;
    droplets_simulated = 0;
}

Eroder::~Eroder() {
    // Destructor
}

void Eroder::_ready() {
    apply_height();
}

void Eroder::_process(double delta) {
    if (!can_simulate) return;
    if (!thread.is_null() && !thread->is_alive()) {
        UtilityFunctions::print("finish");
        mdt = thread->wait_to_finish();
        
        ArrayMesh* mesh = Object::cast_to<ArrayMesh>(get_mesh().ptr());
        mesh->clear_surfaces();
        mdt->commit_to_surface(mesh);
        set_mesh(mesh);

        thread.unref();
    }





    return;
    if (droplets_simulated >= nb_droplets) return;

    Ref<Droplet> dpt = droplet->duplicate();
    
    Ref<RandomNumberGenerator> rng;
    rng.instantiate();
    rng->randomize();
    
    if (dpt->get_position() == Vector2(-1, -1)) {
        float x = rng->randf_range(0, ms-2);
        float y = rng->randf_range(0, ms-2);
        dpt->set_position(Vector2(x, y));
    }

    // UtilityFunctions::print(dpt->get_direction());
    dpt->erode(get_mdt(), ms);

    droplets_simulated++;

    if (droplets_simulated == nb_droplets) {
        ArrayMesh* mesh = Object::cast_to<ArrayMesh>(get_mesh().ptr());
        // Generate all normals
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

        // Generate colors + normalize normals???
        for (int i = 0; i < mdt->get_vertex_count(); i++) {
            Vector3 v = mdt->get_vertex_normal(i).normalized();
            mdt->set_vertex_normal(i, v);
            mdt->set_vertex_color(i, Color(v.x, v.y, v.z));
        }

        mesh->clear_surfaces();
        mdt->commit_to_surface(mesh);
        set_mesh(mesh);

        if (show_trails) {
            // for (int k = 0; k < dpt->pos.size(); k++) {
            //     MeshInstance3D *meshinst = memnew(MeshInstance3D);

            //     SphereMesh *sph = memnew(SphereMesh);
            //     sph->set_radius((k+1)/8.0);
            //     meshinst->set_mesh(sph);
            //     meshinst->set_position(dpt->pos[k]);
            //     meshinst->set_position(meshinst->get_position());

            //     Ref<StandardMaterial3D> mat;
            //     mat.instantiate();
            //     mat->set_albedo(Color(((float) (k % 4)) / 4.0, ((float) ((k + 1) % 4)) / 4.0, ((float) ((k + 2) % 4)) / 4.0 ));

            //     meshinst->set_material_overlay(mat);

            //     add_child(meshinst);
            // }


            for (int k = 0; k < dpt->pos.size(); k++) {
                MeshInstance3D *meshinst = memnew(MeshInstance3D);

                SphereMesh *sph = memnew(SphereMesh);
                meshinst->set_mesh(sph);
                meshinst->set_position(dpt->pos[k]);
                meshinst->set_position(meshinst->get_position());

                Ref<StandardMaterial3D> mat;
                mat.instantiate();
                mat->set_albedo(Color(1.0, ((float) k)/dpt->pos.size(), 0.0));

                meshinst->set_material_overlay(mat);

                get_parent()->add_child(meshinst);
            }

            // for (int k = 0; k < dpt->pos3.size(); k++) {
            //     MeshInstance3D *meshinst = memnew(MeshInstance3D);

            //     SphereMesh *sph = memnew(SphereMesh);
            //     meshinst->set_mesh(sph);
            //     meshinst->set_position(dpt->pos3[k]);
            //     meshinst->set_position(meshinst->get_position());

            //     Ref<StandardMaterial3D> mat;
            //     mat.instantiate();
            //     mat->set_albedo(Color(0.0, ((float) k)/dpt->pos3.size(), 1.0));

            //     meshinst->set_material_overlay(mat);

            //     get_parent()->add_child(meshinst);
            // }
        }
        // UtilityFunctions::print(dpt->pos);
    }
}

void Eroder::convert_mesh_base() {
    if (mesh_base == nullptr) return; // Skip if it's null
    if (Object::cast_to<ArrayMesh>(mesh_base.ptr()) != nullptr) return; // Skip if it's already an array mesh
    
    SurfaceTool *st = memnew(SurfaceTool);
    
    st->create_from(mesh_base, 0);
    set_mesh(st->commit());
    
    memdelete(st);
}

void Eroder::apply_height() {
    convert_mesh_base();
    ArrayMesh* mesh = Object::cast_to<ArrayMesh>(get_mesh().ptr());

    if (mdt == nullptr) {
        UtilityFunctions::push_error("mdt can't be nullptr");
    }
    int err = mdt->create_from_surface(mesh, 0);

    if (err != OK) {
        UtilityFunctions::push_error("Create from surface failed");
    }

    // Move all vertices
    for (int i = 0; i < mdt->get_vertex_count(); i++) {
        Vector3 vertex = mdt->get_vertex(i);

        float noise_value = 0.0;
        for (int j = 0; j < mesh_height_maps.size(); j++) {
            Ref<Texture2D> tex = mesh_height_maps[j];
            if (NoiseTexture2D *noise_tex = Object::cast_to<NoiseTexture2D>(tex.ptr())) {
                if (Noise *noise = Object::cast_to<Noise>(noise_tex->get_noise().ptr())) {
                    noise_value += noise->get_noise_2d(vertex.x, vertex.z) / (j+1);
                }
            }

            if (ImageTexture *img_tex = Object::cast_to<ImageTexture>(tex.ptr())) {
                noise_value += img_tex->get_image()->get_pixel(vertex.x, vertex.z).r / (j+1);
            }

            if (GradientTexture2D *grad_2d = Object::cast_to<GradientTexture2D>(tex.ptr())) {
                if (Image *img = Object::cast_to<Image>(grad_2d->get_image().ptr())) {
                    int coord_x = ((int) Math::abs(vertex.x)) % img->get_width();
                    int coord_y = ((int) Math::abs(vertex.z)) % img->get_height();
                    noise_value += img->get_pixel(coord_x, coord_y).r / (j+1);
                } else {
                    UtilityFunctions::push_error("Image is null");
                }
                // noise_value += ->get_pixel(vertex.x, vertex.z).r / (j+1);
            }
        }
        // UtilityFunctions::print(vertex);
        // vertex = vertex.round();
        vertex.y = noise_value * mesh_height;
        mdt->set_vertex(i, vertex);
    }

    // Generate all normals
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

    // Generate colors + normalize normals???
    for (int i = 0; i < mdt->get_vertex_count(); i++) {
        Vector3 v = mdt->get_vertex_normal(i).normalized();
        mdt->set_vertex_normal(i, v);
        mdt->set_vertex_color(i, Color(v.x, v.y, v.z));
    }

    mesh->clear_surfaces();
    mdt->commit_to_surface(mesh);
    set_mesh(mesh);
}


Ref<MeshDataTool> Eroder::simulation(Ref<MeshDataTool> o_mdt, int mesh_size, int nb_droplets) {
    for (int i = 0; i < nb_droplets; i++) {
        Ref<Droplet> dpt = droplet->duplicate();
    
        Ref<RandomNumberGenerator> rng;
        rng.instantiate();
        rng->randomize();
        
        if (dpt->get_position() == Vector2(-1, -1)) {
            float x = rng->randf_range(0, ms-2);
            float y = rng->randf_range(0, ms-2);
            dpt->set_position(Vector2(x, y));
        }

        // UtilityFunctions::print(dpt->get_direction());
        dpt->erode(o_mdt, ms);

        droplets_simulated++;
    }

    // Generate all normals
    for (int i = 0; i < o_mdt->get_face_count(); i++) {
        int a = o_mdt->get_face_vertex(i, 0);
        int b = o_mdt->get_face_vertex(i, 1);
        int c = o_mdt->get_face_vertex(i, 2);

        Vector3 ap = o_mdt->get_vertex(a);
        Vector3 bp = o_mdt->get_vertex(b);
        Vector3 cp = o_mdt->get_vertex(c);

        Vector3 n = (bp - cp).cross(ap - bp).normalized();

        o_mdt->set_vertex_normal(a, n + o_mdt->get_vertex_normal(a));
        o_mdt->set_vertex_normal(b, n + o_mdt->get_vertex_normal(b));
        o_mdt->set_vertex_normal(c, n + o_mdt->get_vertex_normal(c));
    }

    // Generate colors + normalize normals???
    for (int i = 0; i < o_mdt->get_vertex_count(); i++) {
        Vector3 v = o_mdt->get_vertex_normal(i).normalized();
        o_mdt->set_vertex_normal(i, v);
        o_mdt->set_vertex_color(i, Color(v.x, v.y, v.z));
    }

    // call_deferred("simulation_ended");
    // simulation_ended();
    // call_deferred("simulation_ended");
    return o_mdt;
} 

void Eroder::simulation_ended() {
    UtilityFunctions::print("finish");
    // mdt = thread->wait_to_finish();
    UtilityFunctions::print(thread->wait_to_finish());
    // ArrayMesh* mesh = Object::cast_to<ArrayMesh>(get_mesh().ptr());
    // mesh->clear_surfaces();
    // mdt->commit_to_surface(mesh);
    // set_mesh(mesh);

    thread.unref();
}

void Eroder::start_simulation(int mesh_size) {
    apply_height();
    ms = mesh_size;

    thread.instantiate();
    can_simulate = true;
    // thread->connect("finished", Callable(this, "on_thread_finished"));
    Callable call = callable_mp(this, &Eroder::simulation).bind(mdt, mesh_size, nb_droplets);
    UtilityFunctions::print(call.is_valid());
    UtilityFunctions::print(thread->start(call, Thread::PRIORITY_HIGH));
}

void Eroder::stop_simulation() {
    can_simulate = false;
}

void Eroder::set_show_trails(const bool p_show) {
    show_trails = p_show;
}

bool Eroder::get_show_trails() const {
    return show_trails;
}

void Eroder::set_droplets_simulated(const int p_droplets_simulated) {
    droplets_simulated = droplets_simulated;
}
int Eroder::get_droplets_simulated() const {
    return droplets_simulated;
}

void Eroder::set_mesh_base(const Ref<Mesh> p_mesh) {
    mesh_base = p_mesh;
    apply_height();
}
Ref<Mesh> Eroder::get_mesh_base() const { return mesh_base; }

void Eroder::set_height_maps(const Array p_array) {
    mesh_height_maps = p_array;
    apply_height();
}
Array Eroder::get_height_maps() const { return mesh_height_maps; }

void Eroder::set_mesh_height(const float p_height) {
    mesh_height = p_height;
    apply_height();
}
float Eroder::get_mesh_height() const { return mesh_height; }

void Eroder::set_mdt(const Ref<MeshDataTool> p_mdt) { mdt = p_mdt; }
Ref<MeshDataTool> Eroder::get_mdt() const { return mdt; }


void Eroder::set_nb_droplets(const int p_nb_droplets) {
    nb_droplets = p_nb_droplets;
}

int Eroder::get_nb_droplets() const {
    return nb_droplets;
}

void Eroder::set_droplet(const Ref<Droplet> p_droplet) {
    droplet = p_droplet;
}

Ref<Droplet> Eroder::get_droplet() const {
    return droplet;
}