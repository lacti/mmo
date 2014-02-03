#pragma once

struct rs_render_tile_params;

namespace client {;

class ShaderManager;
class TextureManager;
class VertexBuffer;
class Texture;

typedef std::shared_ptr<TextureManager> TextureManagerSPtr;

class rendering_thread {
public:
    rendering_thread();
    ~rendering_thread();

    int             initialize();
    void            frame_move(double dt);

    void            setup_projection_matrix(int width, int height);
    void            setup_lookat_matrix();

    void            register_shader_program(const std::string& vert, const std::string& frag);
    void            register_texture(int resource_name, const std::string& tex);
    void            register_vertex_buffer_rectangle(const std::string& name, double width, double height);
    void            register_vertex_buffer_cube(const std::string& name, double width, double height, double depth);

    void            clear_screen();
    void            render_tile(const rs_render_tile_params* params);
    void            render_image(double x, double y, double z, const char* vb_name, int resource_name);
    void            render_image(double x, double y, double z, const char* vb_name, int resource_name, int anim_uindex, int anim_vindex);
    void            clear_rendering_context();

private:
    void            set_mvp(const matrix& m);
    void            set_font(bool v);
    void            set_alpha_add(float v);
    void            set_alpha_mult(float v);
    void            set_tile_scale(float x, float y);
    void            set_tile_count(float x, float y);
    void            set_tile_index(int x, int y);
    const Texture*  get_texture(int resource_name);

    std::shared_ptr<ShaderManager>          shader_manager;
    TextureManagerSPtr                      texture_manager;
    std::map<std::string, VertexBuffer>     vb_list;
    std::wstring                            fps_str;
    double                                  last_fps_updated;
    matrix                                  projection;
    matrix                                  lookat;
};
}
