#ifndef ERODER_H
#define ERODER_H

#include <godot_cpp/classes/mesh_instance3d.hpp>
#include <godot_cpp/classes/mesh.hpp>
#include <godot_cpp/classes/texture2d.hpp>
#include <godot_cpp/classes/mesh_data_tool.hpp>
#include <godot_cpp/classes/thread.hpp>

#include "droplet.h"

namespace godot {
    class Eroder : public MeshInstance3D {
        GDCLASS(Eroder, MeshInstance3D)

        private:
            Ref<Thread> thread;
            bool can_simulate;
            int droplets_simulated;
            Ref<Mesh> mesh_base;
            Array mesh_height_maps;
            float mesh_height;
            float ms;

            Ref<MeshDataTool> mdt;

            Ref<Droplet> droplet;
            int nb_droplets;

            bool show_trails;
        protected:
            static void _bind_methods();
        
        public:
            Eroder();
            ~Eroder();

            void _ready() override;
            void _process(double delta) override;

            void convert_mesh_base();
            void apply_height();
            void start_simulation(int mesh_size);
            Ref<MeshDataTool> simulation(Ref<MeshDataTool> o_mdt, int mesh_size, int nb_droplets);
            void stop_simulation();
            void simulation_ended();

            void set_mesh_base(const Ref<Mesh> p_mesh);
            Ref<Mesh> get_mesh_base() const;
            void set_height_maps(const Array p_array);
            Array get_height_maps() const;
            void set_mesh_height(const float p_height);
            float get_mesh_height() const;

            void set_mdt(const Ref<MeshDataTool> p_mdt);
            Ref<MeshDataTool> get_mdt() const;

            void set_nb_droplets(const int p_nb_droplets);
            int get_nb_droplets() const;

            void set_droplet(const Ref<Droplet> p_droplet);
            Ref<Droplet> get_droplet() const;

            void set_droplets_simulated(const int p_droplets_simulated);
            int get_droplets_simulated() const;

            void set_show_trails(const bool p_show);
            bool get_show_trails() const;
    };
}

#endif