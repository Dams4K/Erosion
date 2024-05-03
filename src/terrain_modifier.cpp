#include "terrain_modifier.h"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/godot.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include <godot_cpp/classes/image.hpp>

using namespace godot;

void TerrainModifier::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_texture", "p_texture"), &TerrainModifier::set_texture);
    ClassDB::bind_method(D_METHOD("get_texture"), &TerrainModifier::get_texture);
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "texture", PROPERTY_HINT_RESOURCE_TYPE, "Texture2D"), "set_texture", "get_texture");

    ClassDB::bind_method(D_METHOD("set_texture_mode", "p_mode"), &TerrainModifier::set_texture_mode);
    ClassDB::bind_method(D_METHOD("get_texture_mode"), &TerrainModifier::get_texture_mode);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "texture_mode", PROPERTY_HINT_ENUM, "Scale,Repeat"), "set_texture_mode", "get_texture_mode");

    ClassDB::bind_method(D_METHOD("set_weight", "p_weight"), &TerrainModifier::set_weight);
    ClassDB::bind_method(D_METHOD("get_weight"), &TerrainModifier::get_weight);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "weight"), "set_weight", "get_weight");
    
    ClassDB::bind_method(D_METHOD("set_mode", "p_mode"), &TerrainModifier::set_mode);
    ClassDB::bind_method(D_METHOD("get_mode"), &TerrainModifier::get_mode);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "mode", PROPERTY_HINT_ENUM, "Add,Substract,Multiply"), "set_mode", "get_mode");

    BIND_ENUM_CONSTANT(ADD);
    BIND_ENUM_CONSTANT(SUB);
    BIND_ENUM_CONSTANT(MUL);

    BIND_ENUM_CONSTANT(SCALE);
    BIND_ENUM_CONSTANT(REPEAT);
}

TerrainModifier::TerrainModifier() {
    
}

TerrainModifier::~TerrainModifier() {
    
}

void TerrainModifier::set_texture(const Ref<Texture2D> p_texture) { texture = p_texture; }
Ref<Texture2D> TerrainModifier::get_texture() const { return texture; }

void TerrainModifier::set_texture_mode(const TextureMode p_mode) { texture_mode = p_mode; }
TerrainModifier::TextureMode TerrainModifier::get_texture_mode() const { return texture_mode; }

void TerrainModifier::set_weight(const double p_weight) { weight = p_weight; }
double TerrainModifier::get_weight() const { return weight; }

void TerrainModifier::set_mode(const ModifierMode p_mode) { mode = p_mode; };
TerrainModifier::ModifierMode TerrainModifier::get_mode() const { return mode; }


double TerrainModifier::get_height(int x, int y, Vector2i size) {
    if (texture == nullptr) {
        UtilityFunctions::printerr("texture is nullptr");
        return 0;
    }

    Ref<Image> img = texture->get_image();
    if (img == nullptr) {
        UtilityFunctions::printerr("img is nullptr");
        return 0;
    }

    int rx = x + size.x/2;
    int ry = y + size.y/2;

    float fx = rx / (float) (size.x+1);
    float fy = ry / (float) (size.y+1);

    switch (texture_mode) {
    case SCALE:
        return img->get_pixel((int) (fx * img->get_width()), (int) (fy * img ->get_height())).r * weight;
    
    default:
        return img->get_pixel(rx, ry).r * weight;
    }
}