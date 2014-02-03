#pragma once

#include <map>
#include "texture.h"

namespace client {
    class Texture;
    class TextureManager {
    public:
		TextureManager()
		{
		}
		~TextureManager()
		{
		}

		int add(int resource_name, const std::string& name);
        GLuint get_glname_from_mname(int resource_name) const;
		const Texture* get_tex_from_mname(int resource_name) const;

    private:
        std::map<int, Texture> texture_list;

        friend class rendering_thread;
    };
}
