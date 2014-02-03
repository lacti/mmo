#pragma once

#ifdef RENDERING_SERVICE_EXPORTS
#define RENDERING_SERVICE_API __declspec(dllexport)
#else
#define RENDERING_SERVICE_API __declspec(dllimport)
#endif

struct VertexBuffer;

RENDERING_SERVICE_API void load_obj(VertexBuffer* vb, const char* filename);
//RENDERING_SERVICE_API void rs_load_fbx(VertexBuffer** pVbArray, int* pVbArrayNum, const char* filename);
