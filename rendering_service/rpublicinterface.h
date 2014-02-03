#pragma once

#ifdef RENDERING_SERVICE_EXPORTS
#define RENDERING_SERVICE_API __declspec(dllexport)
#else
#define RENDERING_SERVICE_API __declspec(dllimport)
#endif

#include "rpublictypes.h"

RENDERING_SERVICE_API RHANDLE r_load_fbx_scene(const char* pszFilePath);
RENDERING_SERVICE_API size_t r_get_object_count(RObjectType type);
RENDERING_SERVICE_API RHANDLE r_find_handle(const char* pszName);
RENDERING_SERVICE_API size_t r_meshdata_vertex_count(RHANDLE r);
RENDERING_SERVICE_API size_t r_unload_all();
RENDERING_SERVICE_API const float* r_simplemesh_lcltranslation(RHANDLE r);
RENDERING_SERVICE_API const float* r_simplemesh_lclrotation(RHANDLE r);
RENDERING_SERVICE_API const float* r_simplemesh_lclscaling(RHANDLE r);
