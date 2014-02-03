#include "stdafx.h"
#include "rendering_service.h"
#include "rendering_thread.h"
#include "vertex_buffer.h"
#include "pngloader.h"
#include "texture_manager.h"

using namespace client;

static void null_debug_output_handler(const char* line)
{
}

static rendering_thread* g_rendering_thread;
static std::wstring g_work_path;
rs_fn_debug_output_handler g_debug_output_handler = null_debug_output_handler;
static TextureManager g_tex_man;

void RsDebugOutput(const char* format, ...)
{
    static const size_t max_length = 4096;
    char buffer[max_length];
    va_list vaList;
    va_start(vaList, format);
    _vsnprintf(buffer, max_length, format, vaList);
    va_end(vaList);

    g_debug_output_handler(buffer);
}

RENDERING_SERVICE_API int rs_is_initialized(void)
{
    return g_rendering_thread ? 1 : 0;
}

RENDERING_SERVICE_API int rs_initialize(void)
{
	g_rendering_thread = new rendering_thread();
    g_rendering_thread->initialize();

    return 0;
}

RENDERING_SERVICE_API int rs_set_work_path(const wchar_t* work_path)
{
    g_work_path = work_path;

    return 0;
}

RENDERING_SERVICE_API int rs_register_shader_program(const char* vert, const char* frag)
{
    g_rendering_thread->register_shader_program(vert, frag);

    return 0;
}

RENDERING_SERVICE_API int rs_register_texture(int resource_name, const char* tex)
{
    g_rendering_thread->register_texture(resource_name, tex);

    return 0;
}

RENDERING_SERVICE_API int rs_destroy(void)
{
	
		rs_delete_buffer(1);		rs_delete_vertex_array(1);
		rs_delete_buffer(2);		rs_delete_vertex_array(2);
		rs_delete_buffer(3);		rs_delete_vertex_array(3);
		rs_delete_buffer(4);		rs_delete_vertex_array(4);
		rs_delete_buffer(5);		rs_delete_vertex_array(5);
		rs_delete_buffer(6);		rs_delete_vertex_array(6);
		rs_delete_buffer(7);		rs_delete_vertex_array(7);

    delete g_rendering_thread;
    g_rendering_thread = nullptr;

    return 0;
}

RENDERING_SERVICE_API int rs_set_debug_output_handler(rs_fn_debug_output_handler handler)
{
    g_debug_output_handler = handler;

    return 0;
}

RENDERING_SERVICE_API int rs_delete_buffer(GLuint buf)
{
	GLuint bufArray[1] = { buf };
	glDeleteBuffers(1, bufArray);
	return 0;
}

RENDERING_SERVICE_API int rs_delete_vertex_array(GLuint buf)
{
	GLuint bufArray[1] = { buf };
	glDeleteVertexArrays(1, bufArray);
	return 0;
}

RENDERING_SERVICE_API int rs_register_vertex_buffer_rectangle(const char* name, double width, double height)
{
    g_rendering_thread->register_vertex_buffer_rectangle(name, width, height);

    return 0;
}

RENDERING_SERVICE_API int rs_register_vertex_buffer_cube(const char* name, double width, double height, double depth)
{
    g_rendering_thread->register_vertex_buffer_cube(name, width, height, depth);

    return 0;
}

RENDERING_SERVICE_API int rs_projection(int width, int height)
{
    g_rendering_thread->setup_projection_matrix(width, height);
    return 0;
}

RENDERING_SERVICE_API int rs_lookat(double x, double y)
{
    g_rendering_thread->setup_lookat_matrix();
    return 0;
}

RENDERING_SERVICE_API int rs_clear_screen()
{
    g_rendering_thread->clear_screen();

    return 0;
}

RENDERING_SERVICE_API int rs_render_tile(const rs_render_tile_params* params)
{
    g_rendering_thread->render_tile(params);

    return 0;
}

RENDERING_SERVICE_API int rs_render_simple_image(double x, double y, double z, const char* vb_name, int resource_name)
{
    g_rendering_thread->render_image(x, y, z, vb_name, resource_name);

    return 0;
}

RENDERING_SERVICE_API int rs_render_image(double x, double y, double z, const char* vb_name, int resource_name, int anim_uindex, int anim_vindex)
{
    g_rendering_thread->render_image(x, y, z, vb_name, resource_name, anim_uindex, anim_vindex);

    return 0;
}

RENDERING_SERVICE_API int rs_clear_context()
{
    g_rendering_thread->clear_rendering_context();

    return 0;
}

RENDERING_SERVICE_API GLuint rs_create_texture_from_png_file(const char* filename, int* img_width, int* img_height)
{
	if (!filename || !img_width || !img_height)
		return 0;

	std::ifstream png_stream(filename, std::ios::binary);
	if (png_stream.is_open())
	{
		return create_texture_from_png_stream(png_stream, *img_width, *img_height);
	}

	return 0;
}

RENDERING_SERVICE_API GLuint rs_get_texture_from_png_file(int resource_name, const char* filename)
{
	g_tex_man.add(resource_name, filename);
	
	return g_tex_man.get_glname_from_mname(resource_name);
}

RENDERING_SERVICE_API void rs_get_texture_size(int resource_name, int* w, int* h)
{
	const Texture* t = g_tex_man.get_tex_from_mname(resource_name);

	if (t)
	{
		*w = t->width;
		*h = t->height;
	}
}

// This is the constructor of a class that has been exported.
// see rendering_service.h for the class definition
Crendering_service::Crendering_service()
{
	return;
}
