#include "droplet.h"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/godot.hpp>
#include <godot_cpp/variant/utility_functions.hpp> // TODO: for debugging
#include <godot_cpp/classes/engine.hpp>

#include <godot_cpp/classes/random_number_generator.hpp>

using namespace godot;

int Droplet::current_radius = -1;
int Droplet::current_mesh_size = -1;
int **Droplet::brush_indices = new int*[0];
float **Droplet::brush_weights = new float*[0];
int *Droplet::brush_indices_sub_len = new int[0];
int Droplet::brush_dynarrays_len = 0;

void Droplet::_bind_methods() {
    ClassDB::bind_static_method("Droplet", D_METHOD("generate_brushs", "mesh_size", "raduis"), &Droplet::generate_brushs);
    ClassDB::bind_method(D_METHOD("erode", "mdt", "mesh_size"), &Droplet::erode);

    ClassDB::bind_method(D_METHOD("set_erosion_radius", "p_radius"), &Droplet::set_erosion_radius);
    ClassDB::bind_method(D_METHOD("get_erosion_radius"), &Droplet::get_erosion_radius);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "erosion_radius", PROPERTY_HINT_RANGE, "2,8,1,or_greater"), "set_erosion_radius", "get_erosion_radius");
    
    ClassDB::bind_method(D_METHOD("set_inertia", "p_inertia"), &Droplet::set_inertia);
    ClassDB::bind_method(D_METHOD("get_inertia"), &Droplet::get_inertia);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "inertia", PROPERTY_HINT_RANGE, "0,1"), "set_inertia", "get_inertia");
    
    ClassDB::bind_method(D_METHOD("set_erode_speed", "p_erode_speed"), &Droplet::set_erode_speed);
    ClassDB::bind_method(D_METHOD("get_erode_speed"), &Droplet::get_erode_speed);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "erode_speed", PROPERTY_HINT_RANGE, "0,1"), "set_erode_speed", "get_erode_speed");
    
    ClassDB::bind_method(D_METHOD("set_deposit_speed", "p_deposit_speed"), &Droplet::set_deposit_speed);
    ClassDB::bind_method(D_METHOD("get_deposit_speed"), &Droplet::get_deposit_speed);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "deposit_speed", PROPERTY_HINT_RANGE, "0,1"), "set_deposit_speed", "get_deposit_speed");

    ClassDB::bind_method(D_METHOD("set_evaporate_speed", "p_evaporate_speed"), &Droplet::set_evaporate_speed);
    ClassDB::bind_method(D_METHOD("get_evaporate_speed"), &Droplet::get_evaporate_speed);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "evaporate_speed", PROPERTY_HINT_RANGE, "0,1"), "set_evaporate_speed", "get_evaporate_speed");
    
    ClassDB::bind_method(D_METHOD("set_speed", "p_speed"), &Droplet::set_speed);
    ClassDB::bind_method(D_METHOD("get_speed"), &Droplet::get_speed);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "speed", PROPERTY_HINT_NONE), "set_speed", "get_speed");
    
    ClassDB::bind_method(D_METHOD("set_position", "p_position"), &Droplet::set_position);
    ClassDB::bind_method(D_METHOD("get_position"), &Droplet::get_position);
    ADD_PROPERTY(PropertyInfo(Variant::VECTOR2, "position", PROPERTY_HINT_NONE), "set_position", "get_position");
    
    ClassDB::bind_method(D_METHOD("set_direction", "p_direction"), &Droplet::set_direction);
    ClassDB::bind_method(D_METHOD("get_direction"), &Droplet::get_direction);
    ADD_PROPERTY(PropertyInfo(Variant::VECTOR2, "direction", PROPERTY_HINT_NONE), "set_direction", "get_direction");
    
    ClassDB::bind_method(D_METHOD("set_gravity", "p_gravity"), &Droplet::set_gravity);
    ClassDB::bind_method(D_METHOD("get_gravity"), &Droplet::get_gravity);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "gravity", PROPERTY_HINT_NONE), "set_gravity", "get_gravity");
    
    ClassDB::bind_method(D_METHOD("set_lifetime", "p_lifetime"), &Droplet::set_lifetime);
    ClassDB::bind_method(D_METHOD("get_lifetime"), &Droplet::get_lifetime);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "lifetime", PROPERTY_HINT_NONE), "set_lifetime", "get_lifetime");
    
    ClassDB::bind_method(D_METHOD("set_min_capacity", "p_min_capacity"), &Droplet::set_min_capacity);
    ClassDB::bind_method(D_METHOD("get_min_capacity"), &Droplet::get_min_capacity);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "min_capacity", PROPERTY_HINT_NONE), "set_min_capacity", "get_min_capacity");
    
    ClassDB::bind_method(D_METHOD("set_min_slope", "p_min_slope"), &Droplet::set_min_slope);
    ClassDB::bind_method(D_METHOD("get_min_slope"), &Droplet::get_min_slope);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "min_slope", PROPERTY_HINT_NONE), "set_min_slope", "get_min_slope");
    
    ClassDB::bind_method(D_METHOD("set_sediment", "p_sediment"), &Droplet::set_sediment);
    ClassDB::bind_method(D_METHOD("get_sediment"), &Droplet::get_sediment);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "sediment", PROPERTY_HINT_NONE), "set_sediment", "get_sediment");
    
    ClassDB::bind_method(D_METHOD("set_water", "p_water"), &Droplet::set_water);
    ClassDB::bind_method(D_METHOD("get_water"), &Droplet::get_water);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "water", PROPERTY_HINT_NONE), "set_water", "get_water");
}

Droplet::Droplet() {
    erosion_radius = 3.0;

    inertia = 0.05;

    erode_speed = 0.3;
    deposit_speed = 0.3;
    evaporate_speed = 0.01;
    speed = 1.0;

    position = Vector2(-1, -1);

    // direction = Vector2(0, 0);
    
    gravity = 4.0;
    lifetime = 30;

    min_capacity = 0.1;
    min_slope = 1.0;

    sediment = 0.0;
    water = 1.0;
}

Droplet::~Droplet() {
    // Droplet::clear_brushs();
}

void Droplet::clear_brushs() {
    for (int i = 0; i < Droplet::brush_dynarrays_len; i++) {
        delete[] Droplet::brush_indices[i];
        delete[] Droplet::brush_weights[i];
    }
    delete[] Droplet::brush_indices;
    delete[] Droplet::brush_weights;
}

void Droplet::generate_brushs(int mesh_size, int radius) {
    Droplet::current_mesh_size = mesh_size;
    Droplet::current_radius = radius;

    Droplet::clear_brushs();

    Droplet::brush_indices = new int*[mesh_size * mesh_size];
    Droplet::brush_weights = new float*[mesh_size * mesh_size];
    Droplet::brush_indices_sub_len = new int[mesh_size * mesh_size];
    Droplet::brush_dynarrays_len = mesh_size * mesh_size;

    int x_offsets[radius * radius * 4];
    int y_offsets[radius * radius * 4];
    float weights[radius * radius * 4];

    float weight_sum = 0.0;
    int add_idx = 0;

    for (int i = 0; i < Droplet::brush_dynarrays_len; i++) {
        int center_x = i % mesh_size;
        int center_y = i / mesh_size;

        if (center_y <= radius || center_y >= mesh_size - radius
        || center_x <= radius + 1 || center_x >= mesh_size - radius) {
            weight_sum = 0;
            add_idx = 0;

            for (int y = -radius; y <= radius; y++) {
                for (int x = -radius; x <= radius; x++) {
                    float sqr_dst = x*x + y*y;

                    if (sqr_dst < radius * radius) {
                        int coord_x = center_x + x;
                        int coord_y = center_y + y;

                        if (coord_x >= 0 && coord_x < mesh_size
                        && coord_y >= 0 && coord_y < mesh_size) {
                            float weight = 1 - Math::sqrt(sqr_dst) / radius;
                            weight_sum += weight;
                            weights[add_idx] = weight;
                            x_offsets[add_idx] = x;
                            y_offsets[add_idx] = y;
                            add_idx++;
                        }
                    }
                }
            }
        }

        int num_entries = add_idx;
        Droplet::brush_indices[i] = new int[num_entries];
        Droplet::brush_weights[i] = new float[num_entries];
        Droplet::brush_indices_sub_len[i] = num_entries;

        for (int j = 0; j < num_entries; j++) {
            Droplet::brush_indices[i][j] = (y_offsets[j] + center_y) * mesh_size + x_offsets[j] + center_x;
            Droplet::brush_weights[i][j] = weights[j] / weight_sum;
        }
    }
}

void Droplet::erode(Ref<MeshDataTool> mdt, int mesh_size) {
    // UtilityFunctions::print("----");

    // Check if brushs are working for this droplet
    if (Droplet::brush_dynarrays_len == 0
    || Droplet::current_radius != erosion_radius
    || Droplet::current_mesh_size != mesh_size) {
        generate_brushs(mesh_size, erosion_radius);    
    }

    //TODO: create weights if needed
    for (int i = 0; i < lifetime; i++) {
        Vector2i coords = Vector2i(position);
        float u = position.x - coords.x;
        float v = position.y - coords.y;

        int vertex_00_idx = coords.y * (mesh_size+1) + coords.x;
        int vertex_10_idx = vertex_00_idx + 1;
        int vertex_01_idx = vertex_00_idx + (mesh_size+1);
        int vertex_11_idx = vertex_00_idx + (mesh_size+1) + 1;

        Vector3 vertex_00 = mdt->get_vertex(vertex_00_idx);
        Vector3 vertex_10 = mdt->get_vertex(vertex_10_idx);
        Vector3 vertex_01 = mdt->get_vertex(vertex_01_idx);
        Vector3 vertex_11 = mdt->get_vertex(vertex_11_idx);


        if (pos.is_empty()) {
            pos.append(vertex_00);
            // pos.append(vertex_10);
            // pos.append(vertex_01);
            // pos.append(vertex_11);
            // UtilityFunctions::printt(position, pos, vertex_00_idx, vertex_01_idx, vertex_10_idx, vertex_11_idx);
        }

        float old_height = calculate_height(mdt, mesh_size);

        Vector2 g = calculate_gradient(mdt, mesh_size);

        Ref<RandomNumberGenerator> rng;
        rng.instantiate();
        rng->randomize();
        
        // g = Vector2(rng->randf_range(-1, 1), rng->randf_range(-1, 1));
        // g = Vector2(0, 1);
        set_direction(direction * inertia - g * (1 - inertia)); // The direction is automatically normalized*
        // set_direction();
        // UtilityFunctions::print(position.angle());
        position += direction;

        if (direction == Vector2(0, 0)
        || position.x < 0 || position.x >= mesh_size - 1
        || position.y < 0 || position.y >= mesh_size - 1) {
            break; // The droplet is not moving or out of bounds
        }

        float new_height = calculate_height(mdt, mesh_size);
        float diff_height = new_height - old_height;
        float capacity = Math::max(-diff_height, min_slope) * speed * water * min_capacity;


        pos.append(vertex_00);
        // pos1.append(Vector3(vertex_10.x, vertex_10.y, vertex_10.z));
        // pos2.append(Vector3(vertex_01.x, vertex_01.y, vertex_01.z));
        // pos3.append(vertex_01);

        // UtilityFunctions::printt(capacity, sediment);
        // UtilityFunctions::printt(capacity, diff_height, speed, water, sediment, gravity);

        if (sediment > capacity || diff_height > 0) {
            // Deposition
            // float amount_to_deposit = (sediment - capacity) * deposit_speed;
            float amount_to_deposit = (diff_height > 0) ? Math::min(diff_height, sediment) : (sediment - capacity) * deposit_speed;
            // amount_to_deposit = 1.0;
            sediment -= amount_to_deposit;

            // UtilityFunctions::print(amount_to_deposit);
            mdt->set_vertex(vertex_00_idx, vertex_00 + Vector3(0, amount_to_deposit * (1 - u) * (1 - v), 0));
            mdt->set_vertex(vertex_10_idx, vertex_10 + Vector3(0, amount_to_deposit * u * (1 - v), 0));
            mdt->set_vertex(vertex_01_idx, vertex_01 + Vector3(0, amount_to_deposit * (1 - u) * v, 0));
            mdt->set_vertex(vertex_11_idx, vertex_11 + Vector3(0, amount_to_deposit * u * v, 0));
        } else {
            // Erosion
            float amount_to_erode = Math::min((capacity - sediment) * erode_speed, -diff_height);

            for (int brush_point_idx = 0; brush_point_idx < Droplet::brush_indices_sub_len[vertex_00_idx]; brush_point_idx++) {
                int node_idx = Droplet::brush_indices[vertex_00_idx][brush_point_idx];

                float weighed_erode_amount = amount_to_erode * Droplet::brush_weights[vertex_00_idx][brush_point_idx];
            
                Vector3 node = mdt->get_vertex(node_idx);


                float delta_sediment = (node.y < weighed_erode_amount) ? node.y : weighed_erode_amount;
                delta_sediment = weighed_erode_amount * 100;

                sediment += delta_sediment;
                // UtilityFunctions::printt(delta_sediment, weighed_erode_amount, amount_to_erode, Droplet::brush_weights[vertex_00_idx][brush_point_idx]);
                mdt->set_vertex(node_idx, node - Vector3(0, delta_sediment, 0));
            }
        }

        speed = Math::sqrt(Math::max(speed * speed + diff_height * gravity, (float) 0.0));
        water *= (1 - evaporate_speed);
    }
}


Vector2 Droplet::calculate_gradient(Ref<MeshDataTool> mdt, int mesh_size) {
    Vector2i coords = Vector2i(position);

    float u = position.x - coords.x;
    float v = position.y - coords.y;


    int vertex_00_idx = coords.y * (mesh_size+1) + coords.x;
    int vertex_10_idx = vertex_00_idx + 1;
    int vertex_01_idx = vertex_00_idx + (mesh_size+1);
    int vertex_11_idx = vertex_00_idx + (mesh_size+1) + 1;
    
    float height_00 = mdt->get_vertex(vertex_00_idx).y;
    float height_10 = mdt->get_vertex(vertex_10_idx).y;
    float height_01 = mdt->get_vertex(vertex_01_idx).y;
    float height_11 = mdt->get_vertex(vertex_11_idx).y;

    // UtilityFunctions::printt(mdt->get_vertex(vertex_00_idx), mdt->get_vertex(vertex_10_idx), mdt->get_vertex(vertex_01_idx), mdt->get_vertex(vertex_11_idx));

    // v = 1;
    // u = 1;

    float gx = (height_10 - height_00) * (1 - v) + (height_11 - height_01) * v;
    float gy = (height_01 - height_00) * (1 - u) + (height_11 - height_10) * u;

    // float gx = height_00 + height_01 - height_10 - height_11;
    // float gy = height_00 + height_10 - height_01 - height_11;

    return Vector2(gx, gy);
}

float Droplet::calculate_height(Ref<MeshDataTool> mdt, int mesh_size) {
    Vector2i coords = Vector2i(position);

    float u = position.x - coords.x;
    float v = position.y - coords.y;

    int vertex_00_idx = coords.y * (mesh_size+1) + coords.x;
    int vertex_10_idx = vertex_00_idx + 1;
    int vertex_01_idx = vertex_00_idx + (mesh_size+1);
    int vertex_11_idx = vertex_00_idx + (mesh_size+1) + 1;
    float height_00 = mdt->get_vertex(vertex_00_idx).y;
    float height_10 = mdt->get_vertex(vertex_10_idx).y;
    float height_01 = mdt->get_vertex(vertex_01_idx).y;
    float height_11 = mdt->get_vertex(vertex_11_idx).y;

    return height_00 * (1 - u) * (1 - v) + height_10 * u * (1 - v) + height_01 * (1 - u) * v + height_11 * u * v;
}



void Droplet::set_erosion_radius(const int p_radius) { erosion_radius = p_radius; }

void Droplet::set_inertia(const float p_inertia) { inertia = p_inertia; }
void Droplet::set_erode_speed(const float p_erode_speed) { erode_speed = p_erode_speed; }
void Droplet::set_deposit_speed(const float p_deposit_speed) { deposit_speed = p_deposit_speed; }
void Droplet::set_evaporate_speed(const float p_evaporate_speed) { evaporate_speed = p_evaporate_speed; }
void Droplet::set_speed(const float p_speed) { speed = p_speed; }
void Droplet::set_position(const Vector2 p_position) { position = p_position; }
void Droplet::set_direction(const Vector2 p_direction) { direction = p_direction.normalized(); }

void Droplet::set_gravity(const float p_gravity) { gravity = p_gravity; }
void Droplet::set_lifetime(const int p_lifetime) { lifetime = p_lifetime; }

void Droplet::set_min_capacity(const float p_min_capacity) { min_capacity = p_min_capacity; }
void Droplet::set_min_slope(const float p_min_slope) { min_slope = p_min_slope; }

void Droplet::set_sediment(const float p_sediment) { sediment = p_sediment; }
void Droplet::set_water(const float p_water) { water = p_water; }


int Droplet::get_erosion_radius() const { return erosion_radius; }

float Droplet::get_inertia() const { return inertia; }
float Droplet::get_erode_speed() const { return erode_speed; }
float Droplet::get_deposit_speed() const { return deposit_speed; }
float Droplet::get_evaporate_speed() const { return evaporate_speed; }
float Droplet::get_speed() const { return speed; }
Vector2 Droplet::get_position() const { return position; }
Vector2 Droplet::get_direction() const { return direction; }

float Droplet::get_gravity() const { return gravity; }
int Droplet::get_lifetime() const { return lifetime; }

float Droplet::get_min_capacity() const { return min_capacity; }
float Droplet::get_min_slope() const { return min_slope; }

float Droplet::get_sediment() const { return sediment; }
float Droplet::get_water() const { return water; }