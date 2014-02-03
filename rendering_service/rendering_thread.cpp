#include "stdafx.h"
#include "rendering_thread.h"
#include "shader_manager.h"
#include "texture_manager.h"
#include "texture.h"
#include "vertex_buffer.h"
#include "rendering_service.h"

using namespace client;

static bool yreorder_compare(const std::pair<size_t, double>& lhs,
                             const std::pair<size_t, double>& rhs)
{
    return lhs.second > rhs.second;
}

rendering_thread::rendering_thread()
    : shader_manager(new ShaderManager)
    , texture_manager(new TextureManager)
    , last_fps_updated(0)
{
}

rendering_thread::~rendering_thread()
{
}

int rendering_thread::initialize()
{
    /*GLenum err = glewInit();
    if (GLEW_OK != err) {
    return -1;
    }*/

    glClearColor(1,0.5,0,1);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_ALPHA_TEST);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //glAlphaFunc(GL_GREATER, 0.0f);

    //glEnable(GL_BLEND);
    //glBlendFunc(GL_SRC_COLOR, GL_DST_COLOR);
    //glDepthMask(GL_FALSE);

    return 0;
}

void rendering_thread::register_shader_program(const std::string& vert, const std::string& frag)
{
    if (shader_manager)
    {
        shader_manager->register_shader_program(vert, frag);
    }
}

void rendering_thread::register_texture(int resource_name, const std::string& tex)
{
    if (texture_manager)
    {
        texture_manager->add(resource_name, tex.c_str());
    }
}

void rendering_thread::register_vertex_buffer_rectangle(const std::string& name, double width, double height)
{
    vb_list[name] = VertexBuffer::create_from_rectangle(width, height);
}

void rendering_thread::register_vertex_buffer_cube(const std::string& name, double width, double height, double depth)
{
    vb_list[name] = VertexBuffer::create_from_cube(width, height, depth);
}

void rendering_thread::set_font(bool v)
{
    if (v) {
        glUniform1f(shader_manager->u_is_font_vs, 1.0f);
        glUniform1f(shader_manager->u_is_font_fs, 1.0f);

        set_alpha_add(0);

    } else {
        glUniform1f(shader_manager->u_is_font_vs, 0.0f);
        glUniform1f(shader_manager->u_is_font_fs, 0.0f);
    }
}

void rendering_thread::set_alpha_add(float v)
{
    glUniform1f(shader_manager->u_alpha_add, v);
}

void rendering_thread::set_alpha_mult(float v)
{
    glUniform1f(shader_manager->u_alpha_mult, v);
}

void rendering_thread::set_tile_scale(float x, float y)
{
    glUniform2f(shader_manager->u_tile_size, x, y);
}

void rendering_thread::set_tile_count(float x, float y)
{
    glUniform2f(shader_manager->u_tile_count, x, y);
}

void rendering_thread::set_tile_index(int x, int y)
{
    glUniform2f(shader_manager->u_tile_index, static_cast<float>(x), static_cast<float>(y));
}

void rendering_thread::set_mvp(const matrix& m)
{
    glUniformMatrix4fv(shader_manager->u_mvp, 1, GL_FALSE, m.data());
}

const Texture* rendering_thread::get_texture(int resource_name)
{
    if (resource_name == 0)
        resource_name = 1; // 제..젠장! 0으로 올 줄이야!

    auto t = texture_manager->texture_list.find(resource_name);
    if (t != texture_manager->texture_list.end())
        return &t->second;

    return nullptr;
}

void rendering_thread::setup_projection_matrix(int width, int height)
{
    cml::matrix_orthographic_RH(projection,
        static_cast<float>(width),
        static_cast<float>(height),
        -1000.0f, 1000.0f, cml::z_clip_neg_one);
}

void rendering_thread::setup_lookat_matrix()
{   
    cml::matrix_look_at_RH(lookat,
        0.0f, -0.5f, 0.5f,
        0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f);
}



void rendering_thread::render_tile(const rs_render_tile_params* params)
{
    glUseProgram(shader_manager->program_name);
    glUniform1i(shader_manager->u_diffuse, 0);

    set_font(false);
    set_alpha_add(1.0f);
    set_tile_scale(1.0f, 1.0f);
    set_tile_count(1.0f / params->xrepeat, 1.0f / params->yrepeat);
    set_tile_index(params->uindex, params->vindex);

    //matrix tile_scale;
    float tile_scale[4][4] = {
        { static_cast<float>(params->xrepeat), 0, 0, 0 },
        { 0, static_cast<float>(params->yrepeat), 0, 0 },
        { 0, 0, 1, 0 },
        { 0, 0, 0, 1 }
    };

    float tile_translation[4][4] = {
        { 1, 0, 0, static_cast<float>(params->x) },
        { 0, 1, 0, static_cast<float>(params->y) },
        { 0, 0, 1, static_cast<float>(params->z) },
        { 0, 0, 0, 1 }
    };

    float tile_translation_scale[4][4];
    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            float v = 0;
            for (int k = 0; k < 4; ++k)
            {
                v += tile_translation[i][k] * tile_scale[k][j];
            }

            tile_translation_scale[i][j] = v;
        }
    }

    //cml::matrix_scale(tile_scale,
    //				  static_cast<float>(params->xrepeat),
    //				  static_cast<float>(params->yrepeat),
    //				  1.0f);

    float projection_x_lookat[4][4];
    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            float v = 0;
            for (int k = 0; k < 4; ++k)
            {
                v += projection(i, k) * lookat(k, j);
            }

            projection_x_lookat[i][j] = v;
        }
    }

    float mvp[4][4];
    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            float v = 0;
            for (int k = 0; k < 4; ++k)
            {
                v += projection_x_lookat[i][k] * tile_translation_scale[k][j];
            }

            mvp[i][j] = v;
        }
    }

    //matrix mvp = mat_mult(mat_mult(projection, lookat), tile_scale);

    GLuint glname = texture_manager->get_glname_from_mname(params->resource_name);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, glname);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, 0);

    glBindVertexArray(vb_list["vb_tile"].vert_array_name);

    //set_mvp(mvp);
    glUniformMatrix4fv(shader_manager->u_mvp, 1, GL_TRUE, reinterpret_cast<GLfloat*>(mvp));

    static const int num_tri = 12;
    glDrawArrays(GL_TRIANGLES, 0, 3*num_tri);
}

void rendering_thread::clear_screen()
{
    glClearColor(0, 0, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
}

void rendering_thread::render_image(double x, double y, double z, const char* vb_name, int resource_name)
{
    glUniform1i(shader_manager->u_diffuse, 0);
    glUniform1f(shader_manager->u_is_outline_fs, 0.0f);
    set_font(false);
    set_alpha_mult(1.0f);
    set_alpha_add(1.0f);
    matrix model_trans;
    cml::matrix_translation(model_trans,
        static_cast<float>(x),
        static_cast<float>(y),
        static_cast<float>(z));

    const Texture* tex_0 = get_texture(resource_name);
    assert(tex_0);
    if (!tex_0)
        return;

    matrix mvp = projection * lookat * model_trans;
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex_0->name);

    set_tile_count(1, 1);
    set_tile_index(0, 0);

    // 버텍스 버퍼 바인드
    glBindVertexArray(vb_list[vb_name].vert_array_name);

    // Qt 쪽에서 알파 블렌딩 파라미터를 바꿔버리는 듯.
    // 여기서 재설정하는 것이 안전함.
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // 캐릭터 외곽선 렌더링
    /*
    glUniform1f(shader_manager->u_is_outline_fs, 1.0f);
    {
    matrix outline_scale;
    matrix mvp_scaled = mvp;
    set_mvp(mvp_scaled);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    }
    glUniform1f(shader_manager->u_is_outline_fs, 0.0f);
    */

    // 실제 캐릭터 렌더링
    set_mvp(mvp);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    // Collision proxy

    //  glUniform4f(shader_manager.uniform_vsflags, 1, 0, 0, 0);
    //  glUniform4f(shader_manager.uniform_fsflags, 1, 0, 1, 0);
    //
    //  glBindVertexArray(vb_circle.vert_array_name);
    //  glDrawArrays(GL_TRIANGLES, 0, 32*3);
    //
}

void rendering_thread::render_image(double x, double y, double z, const char* vb_name, int resource_name, int anim_uindex, int anim_vindex)
{
    glUniform1i(shader_manager->u_diffuse, 0);
    glUniform1f(shader_manager->u_is_outline_fs, 0.0f);
    set_font(false);
    set_alpha_mult(1.0f);
    set_alpha_add(1.0f);
    matrix model_trans;
    cml::matrix_translation(model_trans,
        static_cast<float>(x),
        static_cast<float>(y),
        static_cast<float>(z));

    const Texture* tex_0 = get_texture(resource_name);
    assert(tex_0);
    if (!tex_0)
        return;

    matrix mvp = projection * lookat * model_trans;
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex_0->name);

    set_tile_count(4, 4);
    set_tile_index(anim_uindex, anim_vindex);

    // 버텍스 버퍼 바인드
    glBindVertexArray(vb_list[vb_name].vert_array_name);

    // Qt 쪽에서 알파 블렌딩 파라미터를 바꿔버리는 듯.
    // 여기서 재설정하는 것이 안전함.
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // 캐릭터 외곽선 렌더링
    /*
    glUniform1f(shader_manager->u_is_outline_fs, 1.0f);
    {
    matrix outline_scale;
    matrix mvp_scaled = mvp;
    set_mvp(mvp_scaled);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    }
    glUniform1f(shader_manager->u_is_outline_fs, 0.0f);
    */

    // 실제 캐릭터 렌더링
    set_mvp(mvp);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    // Collision proxy

    //  glUniform4f(shader_manager.uniform_vsflags, 1, 0, 0, 0);
    //  glUniform4f(shader_manager.uniform_fsflags, 1, 0, 1, 0);
    //
    //  glBindVertexArray(vb_circle.vert_array_name);
    //  glDrawArrays(GL_TRIANGLES, 0, 32*3);
    //
}

void rendering_thread::clear_rendering_context()
{
    glDisable(GL_DEPTH_TEST);

    glUseProgram(0);
    glBindVertexArray(0);
}

void rendering_thread::frame_move(double dt)
{
    const Texture* tex_0 = get_texture(0);
    const Texture* tex_1 = get_texture(1);
    const Texture* tex_2 = get_texture(2);
    const Texture* tex_3 = get_texture(3);

    const VertexBuffer&		vb_0	  = vb_list["vb_user"];
    const VertexBuffer&		vb_1	  = vb_list["vb_tile"];
    const VertexBuffer&		vb_2	  = vb_list["vb_2"];
    const VertexBuffer&		vb_3	  = vb_list["vb_3"];
    const VertexBuffer&		vb_4	  = vb_list["vb_4"];
    const VertexBuffer&		vb_circle = vb_list["vb_circle"];

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(shader_manager->program_name);
    glUniform1i(shader_manager->u_diffuse, 0);


    // Default tile map
    glActiveTexture(GL_TEXTURE0);

    size_t tile_count = 5;
    matrix tile_scale;
    set_font(false);
    set_alpha_add(1.0f);
    set_tile_scale(1.0f, 1.0f);
    set_tile_count(1.0f/tile_count, 1.0f/tile_count);
    set_tile_index(0, 0);

    cml::matrix_scale(tile_scale,
        static_cast<float>(tile_count),
        static_cast<float>(tile_count),
        1.0f);

    matrix mvp2 = projection * lookat * tile_scale;

    glBindTexture(GL_TEXTURE_2D, tex_1->name);
    glBindVertexArray(vb_1.vert_array_name);

    set_mvp(mvp2);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    // 1x1 Debris (Test)
    tile_count = 1;
    set_tile_count(8, 18);
    set_tile_index(4, 17);

    cml::matrix_scale(tile_scale,
        static_cast<float>(1),
        static_cast<float>(1),
        1.0f);
    matrix debris_trans;
    cml::matrix_translation(debris_trans,
        0.0f,
        64.0f,
        0.0f);

    mvp2 = projection * lookat * debris_trans;

    glBindTexture(GL_TEXTURE_2D, tex_3->name);
    glBindVertexArray(vb_2.vert_array_name);

    set_mvp(mvp2);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    // 5x5 Debris (Test)
    set_tile_count(8, 18);
    set_tile_index(0, 0);
    set_tile_scale(5, 5);

    cml::matrix_translation(debris_trans,
        -32.0f*4,
        32.0f,
        0.0f);

    mvp2 = projection * lookat * debris_trans;

    glBindTexture(GL_TEXTURE_2D, tex_3->name);
    glBindVertexArray(vb_3.vert_array_name);

    set_mvp(mvp2);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    // 4x5 Debris (Test)
    set_tile_count(8, 18);
    set_tile_index(4, 8);
    set_tile_scale(4, 5);

    cml::matrix_translation(debris_trans,
        32.0f*4,
        32.0f*4,
        0.0f);

    mvp2 = projection * lookat * debris_trans;

    glBindTexture(GL_TEXTURE_2D, tex_3->name);
    glBindVertexArray(vb_4.vert_array_name);

    set_mvp(mvp2);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    set_tile_scale(1, 1);

    // Font rendering on the map tiles

    set_font(true);

    matrix font_trans;

    matrix mvp3;

    cml::matrix_translation(font_trans, -500.0f, 0.0f, 0.0f);
    mvp3 = projection * lookat * font_trans;
    set_mvp(mvp3);
    //font_manager->tex_render(L"이 문자열은 텍스쳐에다가 찍어낸 뒤 렌더링하는 것입니다.");

    // Test particle


    // entitys
    set_alpha_mult(1.0f);

    typedef std::pair<size_t, double> reorder_pair_t;
    std::vector<reorder_pair_t> yreorder;

    /*
    const auto& entities = entity_manager::getSingleton().get_entities();

    BOOST_FOREACH(const entity& e, entities) {

    yreorder.push_back( std::make_pair(yreorder.size(), get_as<component::pos0>(e).y) );
    }
    */

    std::sort(yreorder.begin(), yreorder.end(), yreorder_compare);

    /*
    BOOST_FOREACH(const reorder_pair_t& rp, yreorder) {

    set_font(false);
    set_alpha_mult(1.0f);

    const entity& e = entities[ rp.first ];

    component::pos0& pos0 = get_as<component::pos0>(e);
    component::anim& anim = get_as<component::anim>(e);

    matrix model_trans;
    cml::matrix_translation(model_trans,
    static_cast<float>(pos0.x),
    static_cast<float>(pos0.y),
    0.5f);

    matrix mvp = projection * lookat * model_trans;
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex_0.name);

    set_tile_count(4, 4);
    set_tile_index( static_cast<GLfloat>(anim.anim_uindex),
    static_cast<GLfloat>(anim.anim_vindex));
    set_mvp(mvp);

    glBindVertexArray(vb_0.vert_array_name);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    // Collision proxy

    //  glUniform4f(shader_manager.uniform_vsflags, 1, 0, 0, 0);
    //  glUniform4f(shader_manager.uniform_fsflags, 1, 0, 1, 0);
    //
    //  glBindVertexArray(vb_circle.vert_array_name);
    //  glDrawArrays(GL_TRIANGLES, 0, 32*3);
    //
    }
    */

    // Test texture font
    set_font(true);        
    cml::matrix_translation(font_trans, 0.0f, 0.0f, 0.9f);
    mvp3 = projection * lookat * font_trans;
    set_mvp(mvp3);

    /***********/


    glUseProgram(0);



    //glfwSwapBuffers();
}
