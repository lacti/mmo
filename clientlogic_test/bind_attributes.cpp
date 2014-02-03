/* this code is auto-generated. */

#include "serverpch.h"
#include "singleton.h"
#include "cbes/attribute.h"

#pragma warning (disable: 4189 4100 4505)

#include "pos_attribute.h"
#include "motion_attribute.h"
#include "displayable_attribute.h"
#include "fightable_attribute.h"
#include "nameplate_attribute.h"
#include "npc_attribute.h"
#include "player_attribute.h"
#include "removable_attribute.h"

#pragma region /* factory-register */
static struct __initializer1 {
    __initializer1()
    {
        sys<attribute_factory>().factory_map.insert(std::make_pair("pos", [=] () { return attribute_ref(new pos_t); }));
        sys<attribute_factory>().factory_map.insert(std::make_pair("motion", [=] () { return attribute_ref(new motion_t); }));
        sys<attribute_factory>().factory_map.insert(std::make_pair("displayable", [=] () { return attribute_ref(new displayable_t); }));
        sys<attribute_factory>().factory_map.insert(std::make_pair("fightable", [=] () { return attribute_ref(new fightable_t); }));
        sys<attribute_factory>().factory_map.insert(std::make_pair("nameplate", [=] () { return attribute_ref(new nameplate_t); }));
        sys<attribute_factory>().factory_map.insert(std::make_pair("npc", [=] () { return attribute_ref(new npc_t); }));
        sys<attribute_factory>().factory_map.insert(std::make_pair("player", [=] () { return attribute_ref(new player_t); }));
        sys<attribute_factory>().factory_map.insert(std::make_pair("removable", [=] () { return attribute_ref(new removable_t); }));
    }
} ___init2;
#pragma endregion

#pragma region /* pos */
void pos_t::from_bson(bson_iterator*)
{
}
void pos_t::to_bson(bson* obj)
{
    bson_append_start_object(obj, "pos");
    bson_append_string(obj, "x", boost::lexical_cast<std::string>(x).c_str());
    bson_append_string(obj, "y", boost::lexical_cast<std::string>(y).c_str());
    bson_append_string(obj, "z", boost::lexical_cast<std::string>(z).c_str());
    bson_append_finish_object(obj);
}
void pos_t::from_xml(TiXmlElement* node)
{
    const char* attr_value = nullptr;
    node->Attribute("x", &x);
    node->Attribute("y", &y);
    node->Attribute("z", &z);
}
void pos_t::to_xml(std::ostream& out)
{
    out << "<pos";
        out << " x=\"" << x << "\"";
        out << " y=\"" << y << "\"";
        out << " z=\"" << z << "\"";
    out << "/>";
    out << std::endl;
}
#pragma endregion

#pragma region /* motion */
void motion_t::from_bson(bson_iterator*)
{
}
void motion_t::to_bson(bson* obj)
{
    bson_append_start_object(obj, "motion");
    bson_append_string(obj, "dir", boost::lexical_cast<std::string>(dir).c_str());
    bson_append_string(obj, "speed", boost::lexical_cast<std::string>(speed).c_str());
    bson_append_finish_object(obj);
}
void motion_t::from_xml(TiXmlElement* node)
{
    const char* attr_value = nullptr;
    node->Attribute("dir", &dir);
    node->Attribute("speed", &speed);
}
void motion_t::to_xml(std::ostream& out)
{
    out << "<motion";
        out << " dir=\"" << dir << "\"";
        out << " speed=\"" << speed << "\"";
    out << "/>";
    out << std::endl;
}
#pragma endregion

#pragma region /* displayable */
void displayable_t::from_bson(bson_iterator*)
{
}
void displayable_t::to_bson(bson* obj)
{
    bson_append_start_object(obj, "displayable");
    bson_append_string(obj, "visible", boost::lexical_cast<std::string>(visible).c_str());
    bson_append_string(obj, "resource_id", boost::lexical_cast<std::string>(resource_id).c_str());
    bson_append_finish_object(obj);
}
void displayable_t::from_xml(TiXmlElement* node)
{
    const char* attr_value = nullptr;
    attr_value = node->Attribute("visible");
    if (attr_value != nullptr) visible = boost::lexical_cast<bool>(attr_value);
    node->Attribute("resource_id", &resource_id);
}
void displayable_t::to_xml(std::ostream& out)
{
    out << "<displayable";
        out << " visible=\"" << visible << "\"";
        out << " resource_id=\"" << resource_id << "\"";
    out << "/>";
    out << std::endl;
}
#pragma endregion

#pragma region /* fightable */
void fightable_t::from_bson(bson_iterator*)
{
}
void fightable_t::to_bson(bson* obj)
{
    bson_append_start_object(obj, "fightable");
    bson_append_string(obj, "max_hp", boost::lexical_cast<std::string>(max_hp).c_str());
    bson_append_string(obj, "hp", boost::lexical_cast<std::string>(hp).c_str());
    bson_append_string(obj, "alive", boost::lexical_cast<std::string>(alive).c_str());
    bson_append_finish_object(obj);
}
void fightable_t::from_xml(TiXmlElement* node)
{
    const char* attr_value = nullptr;
    node->Attribute("max_hp", &max_hp);
    node->Attribute("hp", &hp);
    attr_value = node->Attribute("alive");
    if (attr_value != nullptr) alive = boost::lexical_cast<bool>(attr_value);
}
void fightable_t::to_xml(std::ostream& out)
{
    out << "<fightable";
        out << " max_hp=\"" << max_hp << "\"";
        out << " hp=\"" << hp << "\"";
        out << " alive=\"" << alive << "\"";
    out << "/>";
    out << std::endl;
}
#pragma endregion

#pragma region /* nameplate */
void nameplate_t::from_bson(bson_iterator*)
{
}
void nameplate_t::to_bson(bson* obj)
{
    bson_append_start_object(obj, "nameplate");
    bson_append_string(obj, "title", boost::lexical_cast<std::string>(title).c_str());
    bson_append_string(obj, "name", boost::lexical_cast<std::string>(name).c_str());
    bson_append_finish_object(obj);
}
void nameplate_t::from_xml(TiXmlElement* node)
{
    const char* attr_value = nullptr;
    for (TiXmlElement* each_node = node->FirstChildElement(); each_node != nullptr; each_node = each_node->NextSiblingElement()) {
        const char* node_name = each_node->Value();
        if (stricmp("title", node_name) == 0) {
            title = std::string(each_node->GetText() != nullptr? each_node->GetText(): "");
        }
        if (stricmp("name", node_name) == 0) {
            name = std::string(each_node->GetText() != nullptr? each_node->GetText(): "");
        }
    }
}
void nameplate_t::to_xml(std::ostream& out)
{
    out << "<nameplate";
    out << ">";
        out << "<title>" << title << "</title>";
        out << "<name>" << name << "</name>";
    out << "</nameplate>";
    out << std::endl;
}
#pragma endregion

#pragma region /* npc */
void npc_t::from_bson(bson_iterator*)
{
}
void npc_t::to_bson(bson* obj)
{
    bson_append_start_object(obj, "npc");
    bson_append_string(obj, "interactive", boost::lexical_cast<std::string>(interactive).c_str());
    bson_append_finish_object(obj);
}
void npc_t::from_xml(TiXmlElement* node)
{
    const char* attr_value = nullptr;
    attr_value = node->Attribute("interactive");
    if (attr_value != nullptr) interactive = boost::lexical_cast<bool>(attr_value);
}
void npc_t::to_xml(std::ostream& out)
{
    out << "<npc";
        out << " interactive=\"" << interactive << "\"";
    out << "/>";
    out << std::endl;
}
#pragma endregion

#pragma region /* player */
void player_t::from_bson(bson_iterator* iterator)
{
    while (bson_iterator_next(iterator)) {
        const char* field_name = bson_iterator_key(iterator);
        const char* field_value = bson_iterator_string(iterator);
    }
}
void player_t::to_bson(bson* obj)
{
    bson_append_start_object(obj, "player");
    bson_append_finish_object(obj);
}
void player_t::from_xml(TiXmlElement*)
{
}
void player_t::to_xml(std::ostream& out)
{
    out << "<player/>";
    out << std::endl;
}
#pragma endregion

#pragma region /* removable */
void removable_t::from_bson(bson_iterator*)
{
}
void removable_t::to_bson(bson* obj)
{
    bson_append_start_object(obj, "removable");
    bson_append_string(obj, "action", boost::lexical_cast<std::string>(action).c_str());
    bson_append_string(obj, "touch", boost::lexical_cast<std::string>(touch).c_str());
    bson_append_finish_object(obj);
}
void removable_t::from_xml(TiXmlElement* node)
{
    const char* attr_value = nullptr;
    attr_value = node->Attribute("action");
    if (attr_value != nullptr) action = boost::lexical_cast<bool>(attr_value);
    attr_value = node->Attribute("touch");
    if (attr_value != nullptr) touch = boost::lexical_cast<bool>(attr_value);
}
void removable_t::to_xml(std::ostream& out)
{
    out << "<removable";
        out << " action=\"" << action << "\"";
        out << " touch=\"" << touch << "\"";
    out << "/>";
    out << std::endl;
}
#pragma endregion
