#pragma once

#define POSITION 0
#define NORMAL 1
#define COLOR 3
#define TEXCOORD 4

typedef struct VertexData3D
{
	float x;
	float y;
    float z;
	float u;
	float v;
	float nx;
	float ny;
	float nz;
} VertexData3D;

typedef struct VertexBuffer
{
	unsigned int vbuffer;
	unsigned int varray;
	VertexData3D* data;
	size_t len;
} VertexBuffer;

void create_vertex_buffer_from_array_3d(VertexBuffer* vb);
