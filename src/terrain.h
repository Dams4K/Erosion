#ifndef TERRAIN_H
#define TERRAIN_H

#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/classes/mesh_instance3d.hpp>
#include <godot_cpp/classes/mesh_data_tool.hpp>

using namespace godot;

class Terrain : public Node3D {
    GDCLASS(Terrain, Node3D)

    protected:
        static void _bind_methods();

    public:
        Terrain();
        ~Terrain();

        void set_modifiers(const Array p_modifiers);
        Array get_modifiers() const;

        void set_mesh_instance(const Ref<MeshInstance3D> p_mesh_instance);
        Ref<MeshInstance3D> get_mesh_instance() const;

        void set_size(const int p_size);
        int get_size() const;

        void set_resolution_scale(const float p_scale);
        float get_resolution_scale() const;

        void initialize_mesh_instance();
        void apply_modifiers();
        void generate_mesh();
    
    private:
        MeshInstance3D* mesh_instance;
        Ref<MeshDataTool> mdt;
        Array modifiers;

        int size = 128;
        float resolution_scale = 1.0;
};

#endif