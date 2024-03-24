#ifndef TERRAIN_MODIFIER_H
#define TERRAIN_MODIFIER_H

#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/classes/texture2d.hpp>

using namespace godot;

class TerrainModifier : public Resource {
    GDCLASS(TerrainModifier, Resource);

    protected:
        static void _bind_methods();

    public:
        TerrainModifier();
        ~TerrainModifier();

        enum ModifierMode {
            ADD,
            SUB,
            MUL
        };

        enum TextureMode {
            SCALE,
            REPEAT
        };

        void set_texture(const Ref<Texture2D> p_texture);
        Ref<Texture2D> get_texture() const;

        void set_texture_mode(const TextureMode p_mode);
        TextureMode get_texture_mode() const;

        void set_weight(const double weight);
        double get_weight() const;

        void set_mode(const ModifierMode p_mode);
        ModifierMode get_mode() const;

        double get_height(int x, int y, Vector2i size);
    
    private:
        Ref<Texture2D> texture;
        TextureMode texture_mode = TextureMode::SCALE;
        double weight = 1.0;
        ModifierMode mode = ModifierMode::ADD;
};

VARIANT_ENUM_CAST(TerrainModifier::ModifierMode);
VARIANT_ENUM_CAST(TerrainModifier::TextureMode);

#endif