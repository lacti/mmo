#include "stdafx.h"
#include "texture_manager.h"
#include "texture.h"

using namespace client;

int TextureManager::add(int resource_name, const std::string& name)
{
    if (texture_list.find(resource_name) == texture_list.end())
    {
	    texture_list[resource_name] = Texture::create_from_file( get_file( name.c_str() ) );
        return 0;
    }

    return 1;
}

GLuint TextureManager::get_glname_from_mname(int resource_name) const
{
    auto v = std::find_if(texture_list.cbegin(), texture_list.cend(), [resource_name](const std::pair<int, Texture>& each) -> bool {
        return each.first == resource_name;
    });

    return (v != texture_list.end()) ? v->second.name : 0;
}

const Texture* TextureManager::get_tex_from_mname(int resource_name) const
{
	auto v = std::find_if(texture_list.cbegin(), texture_list.cend(), [resource_name](const std::pair<int, Texture>& each) -> bool {
        return each.first == resource_name;
    });

    return (v != texture_list.end()) ? &v->second : nullptr;
}
