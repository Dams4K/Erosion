#ifndef DROPLET_H
#define DROPLET_H

#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/classes/mesh_data_tool.hpp>

namespace godot {
    class Droplet : public Resource {
        GDCLASS(Droplet, Resource)

        private:
            static int current_radius;
            static int current_mesh_size;
            static int **brush_indices;
            static float **brush_weights;
            static int *brush_indices_sub_len;
            static int brush_dynarrays_len;

            int erosion_radius;

            float inertia;
            float erode_speed;
            float deposit_speed;
            float evaporate_speed;
            float speed;
            Vector2 position;
            Vector2 direction;

            float gravity;
            int lifetime;

            float min_capacity; // In my brain it's renamed p_capacity
            float min_slope;

            float sediment;
            float water;
        
        protected:
            static void _bind_methods();

        public:
            Array pos;
            Array pos3;
            
            Droplet();
            ~Droplet();

            static void clear_brushs();
            static void generate_brushs(int mesh_size, int radius);

            void erode(Ref<MeshDataTool> mdt, int mesh_size);

            Vector2 calculate_gradient(Ref<MeshDataTool> mdt, int mesh_size);
            float calculate_height(Ref<MeshDataTool> mdt, int mesh_size);

            void set_erosion_radius(const int p_radius);
            int get_erosion_radius() const;

            void set_inertia(const float p_inertia);
            float get_inertia() const;

            void set_erode_speed(const float p_speed);
            float get_erode_speed() const;

            void set_deposit_speed(const float p_speed);
            float get_deposit_speed() const;

            void set_evaporate_speed(const float p_speed);
            float get_evaporate_speed() const;

            void set_speed(const float p_speed);
            float get_speed() const;

            void set_position(const Vector2 p_position);
            Vector2 get_position() const;

            void set_direction(const Vector2 p_direction);
            Vector2 get_direction() const;


            void set_gravity(const float p_gravity);
            float get_gravity() const;

            void set_lifetime(const int p_lifetime);
            int get_lifetime() const;


            void set_min_capacity(const float p_capacity);
            float get_min_capacity() const;

            void set_min_slope(const float p_slope);
            float get_min_slope() const;


            void set_sediment(const float p_sediment);
            float get_sediment() const;

            void set_water(const float p_water);
            float get_water() const;
    };
}

#endif