#include <serverpch.h>
#include <data_expression.h>
#include <data_center.h>
#include "anim_state_data.h"
#include "sprite_data.h"
#include "character_data.h"
#include "npc_name_data.h"
#include "npc_data.h"
#include "world_name_data.h"
#include "world_data.h"
#include "ps_data.h"
#include "config_data.h"

using namespace data;

template <> typename data_center<anim_state_t>::storage_t* data_center<anim_state_t>::storage = nullptr;
template <> typename data_center<sprite_t>::storage_t* data_center<sprite_t>::storage = nullptr;
template <> typename data_center<character_t>::storage_t* data_center<character_t>::storage = nullptr;
template <> typename data_center<npc_name_t>::storage_t* data_center<npc_name_t>::storage = nullptr;
template <> typename data_center<npc_t>::storage_t* data_center<npc_t>::storage = nullptr;
template <> typename data_center<world_name_t>::storage_t* data_center<world_name_t>::storage = nullptr;
template <> typename data_center<world_t>::storage_t* data_center<world_t>::storage = nullptr;
template <> typename data_center<ps_t>::storage_t* data_center<ps_t>::storage = nullptr;
template <> typename data_center<config_t>::storage_t* data_center<config_t>::storage = nullptr;
template <> data_linker_t data_center<anim_state_t>::linker;
template <> data_linker_t data_center<sprite_t>::linker;
template <> data_linker_t data_center<character_t>::linker;
template <> data_linker_t data_center<npc_name_t>::linker;
template <> data_linker_t data_center<npc_t>::linker;
template <> data_linker_t data_center<world_name_t>::linker;
template <> data_linker_t data_center<world_t>::linker;
template <> data_linker_t data_center<ps_t>::linker;
template <> data_linker_t data_center<config_t>::linker;

static void parse_anim_state(TiXmlElement* root_node);
static void parse_anim_state_state(TiXmlElement* root_node, anim_state_t* parent);
static void parse_sprite(TiXmlElement* root_node);
static void parse_character(TiXmlElement* root_node);
static void parse_npc_name(TiXmlElement* root_node);
static void parse_npc(TiXmlElement* root_node);
static void parse_world_name(TiXmlElement* root_node);
static void parse_world(TiXmlElement* root_node);
static void parse_world_world_npc(TiXmlElement* root_node, world_t* parent);
static void parse_ps(TiXmlElement* root_node);
static void parse_config(TiXmlElement* root_node);

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
        
        sprite_data::add(ptr);
        
        id_t anim_state_ref;
        each_node->Attribute("anim-state-ref", &anim_state_ref);
        sprite_data::linker.add<anim_state_t>(&ptr->anim_state, anim_state_ref);
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
        parse_world_world_npc(each_node->FirstChildElement(), ptr);
        
        world_data::add(ptr);
        
        id_t world_name_ref;
        each_node->Attribute("world-name-ref", &world_name_ref);
        world_data::linker.add<world_name_t>(&ptr->world_name, world_name_ref);
    }
}
static void parse_world_world_npc(TiXmlElement* root_node, world_t* parent)
{
    if (root_node == nullptr) return;
    for (TiXmlElement* each_node = root_node->FirstChildElement(); each_node != nullptr; each_node = each_node->NextSiblingElement()) {
        world_t::world_npc_t* ptr = new world_t::world_npc_t;
        each_node->Attribute("id", &ptr->id);
        ptr->npc = nullptr;
        parse_pos_t(each_node->Attribute("loc"), &ptr->loc);
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


void data::__data_load(data_type_t<anim_state_t>)
{
    TiXmlDocument doc;
    doc.LoadFile(user_defined_path_resolver("C:\\mmo4\\resources\\data\\mmo-database.xml"));
    TiXmlElement* root_node = doc.FirstChildElement("mmo-database");
    parse_anim_state(root_node->FirstChildElement("anim-state-list"));
}

void data::__data_load(data_type_t<sprite_t>)
{
    TiXmlDocument doc;
    doc.LoadFile(user_defined_path_resolver("C:\\mmo4\\resources\\data\\mmo-database.xml"));
    TiXmlElement* root_node = doc.FirstChildElement("mmo-database");
    parse_sprite(root_node->FirstChildElement("sprite-list"));
}

void data::__data_load(data_type_t<character_t>)
{
    TiXmlDocument doc;
    doc.LoadFile(user_defined_path_resolver("C:\\mmo4\\resources\\data\\mmo-database.xml"));
    TiXmlElement* root_node = doc.FirstChildElement("mmo-database");
    parse_character(root_node->FirstChildElement("character-list"));
}

void data::__data_load(data_type_t<npc_name_t>)
{
    TiXmlDocument doc;
    doc.LoadFile(user_defined_path_resolver("C:\\mmo4\\resources\\data\\mmo-database.xml"));
    TiXmlElement* root_node = doc.FirstChildElement("mmo-database");
    parse_npc_name(root_node->FirstChildElement("npc-name-list"));
}

void data::__data_load(data_type_t<npc_t>)
{
    TiXmlDocument doc;
    doc.LoadFile(user_defined_path_resolver("C:\\mmo4\\resources\\data\\mmo-database.xml"));
    TiXmlElement* root_node = doc.FirstChildElement("mmo-database");
    parse_npc(root_node->FirstChildElement("npc-list"));
}

void data::__data_load(data_type_t<world_name_t>)
{
    TiXmlDocument doc;
    doc.LoadFile(user_defined_path_resolver("C:\\mmo4\\resources\\data\\mmo-database.xml"));
    TiXmlElement* root_node = doc.FirstChildElement("mmo-database");
    parse_world_name(root_node->FirstChildElement("world-name-list"));
}

void data::__data_load(data_type_t<world_t>)
{
    TiXmlDocument doc;
    doc.LoadFile(user_defined_path_resolver("C:\\mmo4\\resources\\data\\mmo-database.xml"));
    TiXmlElement* root_node = doc.FirstChildElement("mmo-database");
    parse_world(root_node->FirstChildElement("world-list"));
}

void data::__data_load(data_type_t<ps_t>)
{
    TiXmlDocument doc;
    doc.LoadFile(user_defined_path_resolver("C:\\mmo4\\resources\\data\\mmo-database.xml"));
    TiXmlElement* root_node = doc.FirstChildElement("mmo-database");
    parse_ps(root_node->FirstChildElement("ps-list"));
}

void data::__data_load(data_type_t<config_t>)
{
    TiXmlDocument doc;
    doc.LoadFile(user_defined_path_resolver("C:\\mmo4\\resources\\data\\mmo-database.xml"));
    TiXmlElement* root_node = doc.FirstChildElement("mmo-database");
    parse_config(root_node->FirstChildElement("config-list"));
}

static struct __initializer {
    __initializer()
    {
        data::__data_load(data_type_t<anim_state_t>());
        data::__data_load(data_type_t<sprite_t>());
        data::__data_load(data_type_t<character_t>());
        data::__data_load(data_type_t<npc_name_t>());
        data::__data_load(data_type_t<npc_t>());
        data::__data_load(data_type_t<world_name_t>());
        data::__data_load(data_type_t<world_t>());
        data::__data_load(data_type_t<ps_t>());
        data::__data_load(data_type_t<config_t>());
        
        anim_state_data::linker.link();
        sprite_data::linker.link();
        character_data::linker.link();
        npc_name_data::linker.link();
        npc_data::linker.link();
        world_name_data::linker.link();
        world_data::linker.link();
        ps_data::linker.link();
        config_data::linker.link();
    }
} ___init;