/* this code is auto-generated. */
#include <clientpch.h>
#include <data_expression.h>
#include <data_center.h>
#include "anim_state_data.h"
#include "sprite_data.h"
#include "static_object_data.h"
#include "character_data.h"
#include "npc_name_data.h"
#include "npc_data.h"
#include "world_name_data.h"
#include "world_data.h"
#include "ps_data.h"
#include "config_data.h"
#include "entity_template_data.h"
#include "attribute_template_data.h"
#include "interact_template_data.h"

#pragma warning( disable : 4996 )

using namespace data;

template <> typename data_center<anim_state_t>::storage_t* data_center<anim_state_t>::storage = nullptr;
template <> typename data_center<sprite_t>::storage_t* data_center<sprite_t>::storage = nullptr;
template <> typename data_center<static_object_t>::storage_t* data_center<static_object_t>::storage = nullptr;
template <> typename data_center<character_t>::storage_t* data_center<character_t>::storage = nullptr;
template <> typename data_center<npc_name_t>::storage_t* data_center<npc_name_t>::storage = nullptr;
template <> typename data_center<npc_t>::storage_t* data_center<npc_t>::storage = nullptr;
template <> typename data_center<world_name_t>::storage_t* data_center<world_name_t>::storage = nullptr;
template <> typename data_center<world_t>::storage_t* data_center<world_t>::storage = nullptr;
template <> typename data_center<ps_t>::storage_t* data_center<ps_t>::storage = nullptr;
template <> typename data_center<config_t>::storage_t* data_center<config_t>::storage = nullptr;
template <> typename data_center<entity_template_t>::storage_t* data_center<entity_template_t>::storage = nullptr;
template <> typename data_center<attribute_template_t>::storage_t* data_center<attribute_template_t>::storage = nullptr;
template <> typename data_center<interact_template_t>::storage_t* data_center<interact_template_t>::storage = nullptr;
template <> data_linker_t data_center<anim_state_t>::linker;
template <> data_linker_t data_center<sprite_t>::linker;
template <> data_linker_t data_center<static_object_t>::linker;
template <> data_linker_t data_center<character_t>::linker;
template <> data_linker_t data_center<npc_name_t>::linker;
template <> data_linker_t data_center<npc_t>::linker;
template <> data_linker_t data_center<world_name_t>::linker;
template <> data_linker_t data_center<world_t>::linker;
template <> data_linker_t data_center<ps_t>::linker;
template <> data_linker_t data_center<config_t>::linker;
template <> data_linker_t data_center<entity_template_t>::linker;
template <> data_linker_t data_center<attribute_template_t>::linker;
template <> data_linker_t data_center<interact_template_t>::linker;

static void parse_anim_state(TiXmlElement* root_node);
static void parse_anim_state_state(TiXmlElement* root_node, anim_state_t* parent);
static void parse_sprite(TiXmlElement* root_node);
static void parse_static_object(TiXmlElement* root_node);
static void parse_character(TiXmlElement* root_node);
static void parse_npc_name(TiXmlElement* root_node);
static void parse_npc(TiXmlElement* root_node);
static void parse_world_name(TiXmlElement* root_node);
static void parse_world(TiXmlElement* root_node);
static void parse_world_world_npc(TiXmlElement* root_node, world_t* parent);
static void parse_ps(TiXmlElement* root_node);
static void parse_config(TiXmlElement* root_node);
static void parse_entity_template(TiXmlElement* root_node);
static void parse_entity_template_attribute(TiXmlElement* root_node, entity_template_t* parent);
static void parse_attribute_template(TiXmlElement* root_node);
static void parse_attribute_template_field(TiXmlElement* root_node, attribute_template_t* parent);
static void parse_interact_template(TiXmlElement* root_node);
static void parse_interact_template_effect(TiXmlElement* root_node, interact_template_t* parent);

static void parse_anim_state(TiXmlElement* root_node)
{
    if (root_node == nullptr) return;
    for (TiXmlElement* each_node = root_node->FirstChildElement(); each_node != nullptr; each_node = each_node->NextSiblingElement()) {
        anim_state_t* ptr = new anim_state_t;
        each_node->Attribute("id", &ptr->id);
        parse_anim_state_state(each_node->FirstChildElement(), ptr);
        
        anim_state_data::add(ptr);
    }
}
static void parse_anim_state_state(TiXmlElement* root_node, anim_state_t* parent)
{
    if (root_node == nullptr) return;
    for (TiXmlElement* each_node = root_node->FirstChildElement(); each_node != nullptr; each_node = each_node->NextSiblingElement()) {
        anim_state_t::state_t* ptr = new anim_state_t::state_t;
        ptr->name = std::string(each_node->Attribute("name"));
        
        parent->state_list.push_back(ptr);
    }
}

static void parse_sprite(TiXmlElement* root_node)
{
    if (root_node == nullptr) return;
    for (TiXmlElement* each_node = root_node->FirstChildElement(); each_node != nullptr; each_node = each_node->NextSiblingElement()) {
        sprite_t* ptr = new sprite_t;
        each_node->Attribute("id", &ptr->id);
        ptr->name = std::string(each_node->Attribute("name"));
        ptr->anim_state = nullptr;
        each_node->Attribute("umax", &ptr->umax);
        each_node->Attribute("vmax", &ptr->vmax);
        each_node->Attribute("scale", &ptr->scale);
        each_node->Attribute("count", &ptr->count);
        each_node->Attribute("tile_size", &ptr->tile_size);
        
        sprite_data::add(ptr);
        
        id_t anim_state_ref;
        each_node->Attribute("anim-state-ref", &anim_state_ref);
        sprite_data::linker.add<anim_state_t>(&ptr->anim_state, anim_state_ref);
    }
}

static void parse_static_object(TiXmlElement* root_node)
{
    if (root_node == nullptr) return;
    for (TiXmlElement* each_node = root_node->FirstChildElement(); each_node != nullptr; each_node = each_node->NextSiblingElement()) {
        static_object_t* ptr = new static_object_t;
        each_node->Attribute("id", &ptr->id);
        ptr->name = std::string(each_node->Attribute("name"));
        ptr->dds = std::string(each_node->Attribute("dds"));
        each_node->Attribute("scale", &ptr->scale);
        
        static_object_data::add(ptr);
    }
}

static void parse_character(TiXmlElement* root_node)
{
    if (root_node == nullptr) return;
    for (TiXmlElement* each_node = root_node->FirstChildElement(); each_node != nullptr; each_node = each_node->NextSiblingElement()) {
        character_t* ptr = new character_t;
        each_node->Attribute("id", &ptr->id);
        ptr->sprite = nullptr;
        
        character_data::add(ptr);
        
        id_t sprite_ref;
        each_node->Attribute("sprite-ref", &sprite_ref);
        character_data::linker.add<sprite_t>(&ptr->sprite, sprite_ref);
    }
}

static void parse_npc_name(TiXmlElement* root_node)
{
    if (root_node == nullptr) return;
    for (TiXmlElement* each_node = root_node->FirstChildElement(); each_node != nullptr; each_node = each_node->NextSiblingElement()) {
        npc_name_t* ptr = new npc_name_t;
        each_node->Attribute("id", &ptr->id);
        ptr->name = std::string(each_node->GetText() != nullptr? each_node->GetText(): "");
        
        npc_name_data::add(ptr);
    }
}

static void parse_npc(TiXmlElement* root_node)
{
    if (root_node == nullptr) return;
    for (TiXmlElement* each_node = root_node->FirstChildElement(); each_node != nullptr; each_node = each_node->NextSiblingElement()) {
        npc_t* ptr = new npc_t;
        each_node->Attribute("id", &ptr->id);
        ptr->character = nullptr;
        ptr->npc_name = nullptr;
        
        npc_data::add(ptr);
        
        id_t character_ref;
        each_node->Attribute("character-ref", &character_ref);
        npc_data::linker.add<character_t>(&ptr->character, character_ref);
        id_t npc_name_ref;
        each_node->Attribute("npc-name-ref", &npc_name_ref);
        npc_data::linker.add<npc_name_t>(&ptr->npc_name, npc_name_ref);
    }
}

static void parse_world_name(TiXmlElement* root_node)
{
    if (root_node == nullptr) return;
    for (TiXmlElement* each_node = root_node->FirstChildElement(); each_node != nullptr; each_node = each_node->NextSiblingElement()) {
        world_name_t* ptr = new world_name_t;
        each_node->Attribute("id", &ptr->id);
        ptr->name = std::string(each_node->GetText() != nullptr? each_node->GetText(): "");
        
        world_name_data::add(ptr);
    }
}

static void parse_world(TiXmlElement* root_node)
{
    if (root_node == nullptr) return;
    for (TiXmlElement* each_node = root_node->FirstChildElement(); each_node != nullptr; each_node = each_node->NextSiblingElement()) {
        world_t* ptr = new world_t;
        each_node->Attribute("id", &ptr->id);
        ptr->world_name = nullptr;
        ptr->sprite = nullptr;
        parse_world_world_npc(each_node->FirstChildElement(), ptr);
        
        world_data::add(ptr);
        
        id_t world_name_ref;
        each_node->Attribute("world-name-ref", &world_name_ref);
        world_data::linker.add<world_name_t>(&ptr->world_name, world_name_ref);
        id_t sprite_ref;
        each_node->Attribute("sprite-ref", &sprite_ref);
        world_data::linker.add<sprite_t>(&ptr->sprite, sprite_ref);
    }
}
static void parse_world_world_npc(TiXmlElement* root_node, world_t* parent)
{
    if (root_node == nullptr) return;
    for (TiXmlElement* each_node = root_node->FirstChildElement(); each_node != nullptr; each_node = each_node->NextSiblingElement()) {
        world_t::world_npc_t* ptr = new world_t::world_npc_t;
        each_node->Attribute("id", &ptr->id);
        ptr->npc = nullptr;
        parse_data_xyz(each_node->Attribute("loc"), &ptr->loc);
        ptr->rot = expression_parser_t(each_node->Attribute("rot")).result();
        
        parent->world_npc_map.insert(std::make_pair(ptr->id, ptr));
        
        id_t npc_ref;
        each_node->Attribute("npc-ref", &npc_ref);
        world_data::linker.add<npc_t>(&ptr->npc, npc_ref);
    }
}

static void parse_ps(TiXmlElement* root_node)
{
    if (root_node == nullptr) return;
    for (TiXmlElement* each_node = root_node->FirstChildElement(); each_node != nullptr; each_node = each_node->NextSiblingElement()) {
        ps_t* ptr = new ps_t;
        each_node->Attribute("id", &ptr->id);
        each_node->Attribute("count", &ptr->count);
        
        ps_data::add(ptr);
    }
}

static void parse_config(TiXmlElement* root_node)
{
    if (root_node == nullptr) return;
    for (TiXmlElement* each_node = root_node->FirstChildElement(); each_node != nullptr; each_node = each_node->NextSiblingElement()) {
        config_t* ptr = new config_t;
        each_node->Attribute("id", &ptr->id);
        each_node->Attribute("winx", &ptr->winx);
        each_node->Attribute("winy", &ptr->winy);
        
        config_data::add(ptr);
    }
}

static void parse_entity_template(TiXmlElement* root_node)
{
    if (root_node == nullptr) return;
    for (TiXmlElement* each_node = root_node->FirstChildElement(); each_node != nullptr; each_node = each_node->NextSiblingElement()) {
        entity_template_t* ptr = new entity_template_t;
        {
            const char* _temp = each_node->Attribute("id");
            if (stricmp(_temp, "SYSTEM") == 0) ptr->id = entity_template_t::SYSTEM;
            if (stricmp(_temp, "USER") == 0) ptr->id = entity_template_t::USER;
            if (stricmp(_temp, "NPC") == 0) ptr->id = entity_template_t::NPC;
            if (stricmp(_temp, "ITEM") == 0) ptr->id = entity_template_t::ITEM;
            if (stricmp(_temp, "MONSTER") == 0) ptr->id = entity_template_t::MONSTER;
            if (stricmp(_temp, "FACTORY") == 0) ptr->id = entity_template_t::FACTORY;
        }
        parse_entity_template_attribute(each_node->FirstChildElement(), ptr);
        
        entity_template_data::add(ptr);
    }
}
static void parse_entity_template_attribute(TiXmlElement* root_node, entity_template_t* parent)
{
    if (root_node == nullptr) return;
    for (TiXmlElement* each_node = root_node->FirstChildElement(); each_node != nullptr; each_node = each_node->NextSiblingElement()) {
        entity_template_t::attribute_t* ptr = new entity_template_t::attribute_t;
        ptr->name = std::string(each_node->Attribute("name"));
        
        parent->attribute_list.push_back(ptr);
    }
}

static void parse_attribute_template(TiXmlElement* root_node)
{
    if (root_node == nullptr) return;
    for (TiXmlElement* each_node = root_node->FirstChildElement(); each_node != nullptr; each_node = each_node->NextSiblingElement()) {
        attribute_template_t* ptr = new attribute_template_t;
        each_node->Attribute("id", &ptr->id);
        ptr->name = std::string(each_node->Attribute("name"));
        parse_attribute_template_field(each_node->FirstChildElement(), ptr);
        
        attribute_template_data::add(ptr);
    }
}
static void parse_attribute_template_field(TiXmlElement* root_node, attribute_template_t* parent)
{
    if (root_node == nullptr) return;
    for (TiXmlElement* each_node = root_node->FirstChildElement(); each_node != nullptr; each_node = each_node->NextSiblingElement()) {
        attribute_template_t::field_t* ptr = new attribute_template_t::field_t;
        ptr->name = std::string(each_node->Attribute("name"));
        ptr->value = std::string(each_node->Attribute("value"));
        
        parent->field_list.push_back(ptr);
    }
}

static void parse_interact_template(TiXmlElement* root_node)
{
    if (root_node == nullptr) return;
    for (TiXmlElement* each_node = root_node->FirstChildElement(); each_node != nullptr; each_node = each_node->NextSiblingElement()) {
        interact_template_t* ptr = new interact_template_t;
        {
            const char* _temp = each_node->Attribute("id");
            if (stricmp(_temp, "ATTACK") == 0) ptr->id = interact_template_t::ATTACK;
            if (stricmp(_temp, "HEAL") == 0) ptr->id = interact_template_t::HEAL;
            if (stricmp(_temp, "RECOVERY") == 0) ptr->id = interact_template_t::RECOVERY;
        }
        parse_interact_template_effect(each_node->FirstChildElement(), ptr);
        
        interact_template_data::add(ptr);
    }
}
static void parse_interact_template_effect(TiXmlElement* root_node, interact_template_t* parent)
{
    if (root_node == nullptr) return;
    for (TiXmlElement* each_node = root_node->FirstChildElement(); each_node != nullptr; each_node = each_node->NextSiblingElement()) {
        interact_template_t::effect_t* ptr = new interact_template_t::effect_t;
        {
            const char* _temp = each_node->Attribute("type");
            if (stricmp(_temp, "ATTRIBUTE") == 0) ptr->type = interact_template_t::effect_t::ATTRIBUTE;
        }
        {
            const char* _temp = each_node->Attribute("direction");
            if (stricmp(_temp, "TARGET") == 0) ptr->direction = interact_template_t::effect_t::TARGET;
            if (stricmp(_temp, "OWNER") == 0) ptr->direction = interact_template_t::effect_t::OWNER;
        }
        ptr->attribute = std::string(each_node->Attribute("attribute"));
        ptr->field = std::string(each_node->Attribute("field"));
        {
            const char* _temp = each_node->Attribute("function");
            if (stricmp(_temp, "MINUS") == 0) ptr->function = interact_template_t::effect_t::MINUS;
            if (stricmp(_temp, "PLUS") == 0) ptr->function = interact_template_t::effect_t::PLUS;
        }
        each_node->Attribute("value", &ptr->value);
        
        parent->effect_list.push_back(ptr);
    }
}


void data::__data_load(data_type_t<anim_state_t>)
{
    TiXmlDocument document;
    document.LoadFile(user_defined_path_resolver("C:\\Users\\John\\Documents\\mmo-code\\resources\\data\\mmo-data.xml"));
    TiXmlElement* root_node = document.FirstChildElement("mmo-data");
    parse_anim_state(root_node->FirstChildElement("anim-state-list"));
}

void data::__data_load(data_type_t<sprite_t>)
{
    TiXmlDocument document;
    document.LoadFile(user_defined_path_resolver("C:\\Users\\John\\Documents\\mmo-code\\resources\\data\\mmo-data.xml"));
    TiXmlElement* root_node = document.FirstChildElement("mmo-data");
    parse_sprite(root_node->FirstChildElement("sprite-list"));
}

void data::__data_load(data_type_t<static_object_t>)
{
    TiXmlDocument document;
    document.LoadFile(user_defined_path_resolver("C:\\Users\\John\\Documents\\mmo-code\\resources\\data\\mmo-data.xml"));
    TiXmlElement* root_node = document.FirstChildElement("mmo-data");
    parse_static_object(root_node->FirstChildElement("static-object-list"));
}

void data::__data_load(data_type_t<character_t>)
{
    TiXmlDocument document;
    document.LoadFile(user_defined_path_resolver("C:\\Users\\John\\Documents\\mmo-code\\resources\\data\\mmo-data.xml"));
    TiXmlElement* root_node = document.FirstChildElement("mmo-data");
    parse_character(root_node->FirstChildElement("character-list"));
}

void data::__data_load(data_type_t<npc_name_t>)
{
    TiXmlDocument document;
    document.LoadFile(user_defined_path_resolver("C:\\Users\\John\\Documents\\mmo-code\\resources\\data\\mmo-data.xml"));
    TiXmlElement* root_node = document.FirstChildElement("mmo-data");
    parse_npc_name(root_node->FirstChildElement("npc-name-list"));
}

void data::__data_load(data_type_t<npc_t>)
{
    TiXmlDocument document;
    document.LoadFile(user_defined_path_resolver("C:\\Users\\John\\Documents\\mmo-code\\resources\\data\\mmo-data.xml"));
    TiXmlElement* root_node = document.FirstChildElement("mmo-data");
    parse_npc(root_node->FirstChildElement("npc-list"));
}

void data::__data_load(data_type_t<world_name_t>)
{
    TiXmlDocument document;
    document.LoadFile(user_defined_path_resolver("C:\\Users\\John\\Documents\\mmo-code\\resources\\data\\mmo-data.xml"));
    TiXmlElement* root_node = document.FirstChildElement("mmo-data");
    parse_world_name(root_node->FirstChildElement("world-name-list"));
}

void data::__data_load(data_type_t<world_t>)
{
    TiXmlDocument document;
    document.LoadFile(user_defined_path_resolver("C:\\Users\\John\\Documents\\mmo-code\\resources\\data\\mmo-data.xml"));
    TiXmlElement* root_node = document.FirstChildElement("mmo-data");
    parse_world(root_node->FirstChildElement("world-list"));
}

void data::__data_load(data_type_t<ps_t>)
{
    TiXmlDocument document;
    document.LoadFile(user_defined_path_resolver("C:\\Users\\John\\Documents\\mmo-code\\resources\\data\\mmo-data.xml"));
    TiXmlElement* root_node = document.FirstChildElement("mmo-data");
    parse_ps(root_node->FirstChildElement("ps-list"));
}

void data::__data_load(data_type_t<config_t>)
{
    TiXmlDocument document;
    document.LoadFile(user_defined_path_resolver("C:\\Users\\John\\Documents\\mmo-code\\resources\\data\\mmo-data.xml"));
    TiXmlElement* root_node = document.FirstChildElement("mmo-data");
    parse_config(root_node->FirstChildElement("config-list"));
}

void data::__data_load(data_type_t<entity_template_t>)
{
    TiXmlDocument document;
    document.LoadFile(user_defined_path_resolver("C:\\Users\\John\\Documents\\mmo-code\\resources\\data\\mmo-data.xml"));
    TiXmlElement* root_node = document.FirstChildElement("mmo-data");
    parse_entity_template(root_node->FirstChildElement("entity-template-list"));
}

void data::__data_load(data_type_t<attribute_template_t>)
{
    TiXmlDocument document;
    document.LoadFile(user_defined_path_resolver("C:\\Users\\John\\Documents\\mmo-code\\resources\\data\\mmo-data.xml"));
    TiXmlElement* root_node = document.FirstChildElement("mmo-data");
    parse_attribute_template(root_node->FirstChildElement("attribute-template-list"));
}

void data::__data_load(data_type_t<interact_template_t>)
{
    TiXmlDocument document;
    document.LoadFile(user_defined_path_resolver("C:\\Users\\John\\Documents\\mmo-code\\resources\\data\\mmo-data.xml"));
    TiXmlElement* root_node = document.FirstChildElement("mmo-data");
    parse_interact_template(root_node->FirstChildElement("interact-template-list"));
}

static struct __initializer100 {
    __initializer100()
    {
        data::__data_load(data_type_t<anim_state_t>());
        data::__data_load(data_type_t<sprite_t>());
        data::__data_load(data_type_t<static_object_t>());
        data::__data_load(data_type_t<character_t>());
        data::__data_load(data_type_t<npc_name_t>());
        data::__data_load(data_type_t<npc_t>());
        data::__data_load(data_type_t<world_name_t>());
        data::__data_load(data_type_t<world_t>());
        data::__data_load(data_type_t<ps_t>());
        data::__data_load(data_type_t<config_t>());
        data::__data_load(data_type_t<entity_template_t>());
        data::__data_load(data_type_t<attribute_template_t>());
        data::__data_load(data_type_t<interact_template_t>());
        
        anim_state_data::linker.link();
        sprite_data::linker.link();
        static_object_data::linker.link();
        character_data::linker.link();
        npc_name_data::linker.link();
        npc_data::linker.link();
        world_name_data::linker.link();
        world_data::linker.link();
        ps_data::linker.link();
        config_data::linker.link();
        entity_template_data::linker.link();
        attribute_template_data::linker.link();
        interact_template_data::linker.link();
    }
} ___init101;

#pragma warning( default : 4996 )
