#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "Vertex.h"

void load_obj(VertexBuffer* vb, const char* filename)
{
	FILE* f = fopen(filename, "r");
	char line [ 512 ]; /* or other suitable maximum line size */
	int nv = 0, nvt = 0, nvn = 0, nf = 0;
	float* varray;
	float* vtarray;
	float* vnarray;
	VertexData3D* farray;
	int i;

	while(fgets(line, sizeof(line), f) != NULL)
	{
		if (line[0] == '#')
			continue;

		if (strncmp(line, "v ", 2) == 0)
		{
			++nv;
		}
		else if (strncmp(line, "vt ", 3) == 0)
		{
			++nvt;
		}
		else if (strncmp(line, "vn ", 3) == 0)
		{
			++nvn;
		}
		else if (strncmp(line, "f ", 2) == 0)
		{
			++nf;
		}

	}

	fseek(f, 0, SEEK_SET);

	varray = (float*)malloc(sizeof(float)*3*nv);
	vtarray = (float*)malloc(sizeof(float)*2*nvt);
	vnarray = (float*)malloc(sizeof(float)*3*nvn);
	farray = (VertexData3D*)malloc(sizeof(VertexData3D)*3*nf);

	nv = 0, nvt = 0, nvn = 0, nf = 0;

	while(fgets(line, sizeof(line), f) != NULL)
	{
		if (line[0] == '#')
			continue;

		if (strncmp(line, "v ", 2) == 0)
		{
			char* endptr;
			varray[nv++] = (float)strtod(line + 2, &endptr);
			varray[nv++] = (float)strtod(endptr + 1, &endptr);
			varray[nv++] = (float)strtod(endptr + 1, &endptr);
		}
		else if (strncmp(line, "vt ", 3) == 0)
		{
			char* endptr;
			vtarray[nvt++] = (float)strtod(line + 3, &endptr);
			vtarray[nvt++] = (float)strtod(endptr + 1, &endptr);
		}
		else if (strncmp(line, "vn ", 3) == 0)
		{
			char* endptr;
			vnarray[nvn++] = (float)strtod(line + 3, &endptr);
			vnarray[nvn++] = (float)strtod(endptr + 1, &endptr);
			vnarray[nvn++] = (float)strtod(endptr + 1, &endptr);
		}
		else if (strncmp(line, "f ", 2) == 0)
		{
			char* endptr = line + 1;

			for (i = 0; i < 3; ++i)
			{	
				int vidx = strtol(endptr + 1, &endptr, 10) - 1;
				int vtidx = strtol(endptr + 1, &endptr, 10) - 1;
				int vnidx = strtol(endptr + 1, &endptr, 10) - 1;

				assert(3*vidx + 2 < nv);
				assert(2*vtidx + 1 < nvt);
				assert(3*vnidx + 2 < nvn);

				farray[nf].x = varray[3*vidx + 0];
				farray[nf].y = varray[3*vidx + 1];
				farray[nf].z = varray[3*vidx + 2];

				farray[nf].u = vtarray[2*vtidx + 0];
				farray[nf].v = vtarray[2*vtidx + 1];

				farray[nf].nx = vnarray[3*vnidx + 0];
				farray[nf].ny = vnarray[3*vnidx + 1];
				farray[nf].nz = vnarray[3*vnidx + 2];

				++nf;
			}
		}
	}
		
	vb->data = farray;
	vb->len = nf;

	create_vertex_buffer_from_array_3d(vb);
	
	free(varray);
	free(vtarray);
	free(vnarray);
	free(farray);

	fclose(f);
}
