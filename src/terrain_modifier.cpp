#include "terrain_modifier.h"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/godot.hpp>

#include <godot_cpp/classes/image.hpp>

using namespace godot;

void TerrainModifier::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_texture", "p_texture"), &TerrainModifier::set_texture);
    ClassDB::bind_method(D_METHOD("get_texture"), &TerrainModifier::get_texture);
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "texture", PROPERTY_HINT_RESOURCE_TYPE, "Texture2D"), "set_texture", "get_texture");

    ClassDB::bind_method(D_METHOD("set_weight", "p_weight"), &TerrainModifier::set_weight);
    ClassDB::bind_method(D_METHOD("get_weight"), &TerrainModifier::get_weight);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "weight"), "set_weight", "get_weight");
    
    ClassDB::bind_method(D_METHOD("set_mode", "p_mode"), &TerrainModifier::set_mode);
    ClassDB::bind_method(D_METHOD("get_mode"), &TerrainModifier::get_mode);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "mode", PROPERTY_HINT_ENUM, "Add,Substract,Multiply"), "set_mode", "get_mode");

    BIND_ENUM_CONSTANT(ADD);
    BIND_ENUM_CONSTANT(SUB);
    BIND_ENUM_CONSTANT(MUL);
}

TerrainModifier::TerrainModifier() {  }
TerrainModifier::~TerrainModifier() {  }

void TerrainModifier::set_texture(const Ref<Texture2D> p_texture) { texture = p_texture; }
Ref<Texture2D> TerrainModifier::get_texture() const { return texture; }

void TerrainModifier::set_weight(const double p_weight) { weight = p_weight; }
double TerrainModifier::get_weight() const { return weight; }

void TerrainModifier::set_mode(const ModifierMode p_mode) { mode = p_mode; };
TerrainModifier::ModifierMode TerrainModifier::get_mode() const { return mode; }


double TerrainModifier::get_height(int x, int y) {
    Ref<Image> img = texture->get_image();

    return img->get_pixel(x, y).r * weight;
}