#pragma once

#ifdef RENDERING_SERVICE_EXPORTS
#define RENDERING_SERVICE_API __declspec(dllexport)
#else
#define RENDERING_SERVICE_API __declspec(dllimport)
#endif

enum VertexSemantic
{
	POSITION = 0,
	TEXCOORD,
	NORMAL,
	COLOR,
};

typedef struct VertexData3D
{
	float x, y, z;
	float u, v;
	float nx, ny, nz;
	float r, g, b, a;
} VertexData3D;

typedef struct VertexBuffer
{
	unsigned int vbuffer;
	unsigned int varray;
	VertexData3D* data;
	size_t len;
} VertexBuffer;

RENDERING_SERVICE_API void create_vertex_buffer_from_array_3d(VertexBuffer* vb);
RENDERING_SERVICE_API void create_vertex_buffer_from_heightmap_3d(VertexBuffer* vb, float cellw, float cellh, int pcx, int pcy, float* pz);
RENDERING_SERVICE_API void create_vertex_buffer_pie(VertexBuffer* vertbuf, float r, int count);
