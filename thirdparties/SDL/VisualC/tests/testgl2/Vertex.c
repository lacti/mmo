#include <stdio.h>
#include "SDL_opengl.h"
#include "Vertex.h"

extern PFNGLGENBUFFERSPROC glGenBuffers;
extern PFNGLBUFFERDATAPROC glBufferData;
extern PFNGLBINDBUFFERPROC glBindBuffer;
extern PFNGLGENVERTEXARRAYSPROC glGenVertexArrays;
extern PFNGLBINDVERTEXARRAYPROC glBindVertexArray;
extern PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer;
extern PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;

static char* POINTER_OFFSET(size_t i)
{
    return (char*)NULL + i;
}

void create_vertex_buffer_from_array_3d(VertexBuffer* vb)
{
    const static int vertex_size = sizeof(VertexData3D); // sizeof(float)*8; // x, y, z, u, v, nx, ny, nz

    glGenBuffers(1, &vb->vbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vb->vbuffer);
    glBufferData(GL_ARRAY_BUFFER, vertex_size * vb->len, vb->data, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
 
	printf("buffer_name = %d\n", vb->vbuffer);
     
	glGenVertexArrays(1, &vb->varray);
	glBindVertexArray(vb->varray);
    {    
		glBindBuffer(GL_ARRAY_BUFFER, vb->vbuffer);
		{
			glVertexAttribPointer(POSITION, 3, GL_FLOAT,
								  GL_FALSE, vertex_size,
								  POINTER_OFFSET(0));
			glVertexAttribPointer(TEXCOORD, 2, GL_FLOAT,
								  GL_FALSE, vertex_size,
								  POINTER_OFFSET(sizeof(float)*3));
			glVertexAttribPointer(NORMAL, 3, GL_FLOAT,
								  GL_FALSE, vertex_size,
								  POINTER_OFFSET(sizeof(float)*5));
		}
        glBindBuffer(GL_ARRAY_BUFFER, 0);
 
        glEnableVertexAttribArray(POSITION);
        glEnableVertexAttribArray(TEXCOORD);
		glEnableVertexAttribArray(NORMAL);
    }
    glBindVertexArray(0);
 
	printf("vert_array_name = %d\n", vb->varray);
    
    return;
}
