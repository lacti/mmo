#pragma once

namespace client {
    class ShaderManager {
    public:
        ShaderManager();
        ~ShaderManager();

        void register_shader_program(const std::string&, const std::string&);

        GLuint program_name;
        GLint u_mvp;
        GLint u_diffuse;
        GLint u_tile_count;
        GLint u_tile_index;
        GLint u_tile_size;
        GLint u_is_font_vs;
        GLint u_is_font_fs;
        GLint u_is_outline_fs;
        GLint u_alpha_add;
        GLint u_alpha_mult;
        GLint a_v_coord;
        GLint u_fbo_texture;
        GLint a_v_coord_scale;
    };
}

GLuint create_shader_from_file(GLenum type, const std::string& filename);
