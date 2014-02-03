#pragma once
// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the RENDERING_SERVICE_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// RENDERING_SERVICE_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.

#ifdef RENDERING_SERVICE_EXPORTS
#define RENDERING_SERVICE_API __declspec(dllexport)
#else
#define RENDERING_SERVICE_API __declspec(dllimport)
#endif

// This class is exported from the rendering_service.dll
class RENDERING_SERVICE_API Crendering_service {
public:
    Crendering_service(void);
    // TODO: add your methods here.
};

extern RENDERING_SERVICE_API int nrendering_service;

struct rs_render_tile_params
{
    int resource_name;
    int uindex, vindex;
    double x;
    double y;
    double z;
    int xrepeat, yrepeat;
};

typedef void (*rs_fn_debug_output_handler)(const char* line);

RENDERING_SERVICE_API int fnrendering_service(void);
RENDERING_SERVICE_API int rs_is_initialized(void);
RENDERING_SERVICE_API int rs_initialize(void);
RENDERING_SERVICE_API int rs_destroy(void);
RENDERING_SERVICE_API int rs_set_work_path(const wchar_t* work_path);
RENDERING_SERVICE_API int rs_register_shader_program(const char* vert, const char* frag);
RENDERING_SERVICE_API int rs_register_texture(int resource_name, const char* tex);
RENDERING_SERVICE_API int rs_set_debug_output_handler(rs_fn_debug_output_handler handler);
RENDERING_SERVICE_API int rs_register_vertex_buffer_rectangle(const char* name, double width, double height);
RENDERING_SERVICE_API int rs_register_vertex_buffer_cube(const char* name, double width, double height, double depth);
RENDERING_SERVICE_API int rs_delete_buffer(GLuint buf);
RENDERING_SERVICE_API int rs_delete_vertex_array(GLuint buf);
RENDERING_SERVICE_API int rs_projection(int width, int height);
RENDERING_SERVICE_API int rs_lookat(double x, double y);
RENDERING_SERVICE_API int rs_clear_screen();
RENDERING_SERVICE_API int rs_render_tile(const rs_render_tile_params* params);
RENDERING_SERVICE_API int rs_render_simple_image(double x, double y, double z, const char* vb_name, int resource_name);
RENDERING_SERVICE_API int rs_render_image(double x, double y, double z, const char* vb_name, int resource_name, int anim_uindex, int anim_vindex);
RENDERING_SERVICE_API int rs_clear_context();
RENDERING_SERVICE_API GLuint rs_create_texture_from_png_file(const char* filename, int* img_width, int* img_height);
RENDERING_SERVICE_API GLuint rs_get_texture_from_png_file(int resource_name, const char* filename);
RENDERING_SERVICE_API void rs_get_texture_size(int resource_name, int* w, int* h);
RENDERING_SERVICE_API void rs_create_backbuffer(int screen_width, int screen_height);
RENDERING_SERVICE_API void rs_free_backbuffer();
RENDERING_SERVICE_API void rs_create_pp_program();
RENDERING_SERVICE_API void rs_start_backbuffer_rendering();
RENDERING_SERVICE_API void rs_render_fullscreen_backbuffer();
RENDERING_SERVICE_API float rs_get_fullscreen_backbuffer_scale();
RENDERING_SERVICE_API void rs_set_fullscreen_backbuffer_scale(float s);
RENDERING_SERVICE_API void rs_generateShadowFBO(struct shadowmap_conf* conf);
RENDERING_SERVICE_API void rs_build_shadowmap_shader_program(shadowmap_conf* conf);
//RENDERING_SERVICE_API void rs_test_load_fbx_file();

