#include "stdafx.h"
#include "Vertex.h"

static char* POINTER_OFFSET(size_t i)
{
    return (char*)NULL + i;
}

void create_vertex_buffer_from_array_3d(VertexBuffer* vb)
{
    const static int vertex_size = sizeof(VertexData3D); // sizeof(float)*8; // x, y, z, u, v, nx, ny, nz

    glActiveTexture(GL_TEXTURE0);
    glGenBuffers(1, &vb->vbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vb->vbuffer);
    glBufferData(GL_ARRAY_BUFFER, vertex_size * vb->len, vb->data, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    //printf("buffer_name = %d\n", vb->vbuffer);

    glGenVertexArrays(1, &vb->varray);
    glBindVertexArray(vb->varray);
    {    
        static const VertexSemantic semantics[] = { POSITION, TEXCOORD, NORMAL, COLOR };
        static const int offsets[] = { 3, 2, 3, 4 };

        glBindBuffer(GL_ARRAY_BUFFER, vb->vbuffer);
        {
            int acc = 0;

            for (size_t i = 0; i < _countof(semantics); ++i)
            {
                glVertexAttribPointer(semantics[i], offsets[i], GL_FLOAT, GL_FALSE, vertex_size, POINTER_OFFSET(sizeof(float)*acc));

                acc += offsets[i];
            }

            assert(sizeof(float)*acc == sizeof(VertexData3D));
        }
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        for (VertexSemantic vs : semantics)
        {
            glEnableVertexAttribArray(vs);
        }
    }
    glBindVertexArray(0);

    //printf("vert_array_name = %d\n", vb->varray);

    return;
}

void create_vertex_buffer_from_heightmap_3d(VertexBuffer* vertbuf, float cellw, float cellh, int pcx, int pcy, float* pz)
{
    if (!vertbuf || pcx < 2 || pcy < 2)
        return;
    if (vertbuf->data || vertbuf->len != 0 || vertbuf->vbuffer != 0 || vertbuf->varray != 0)
        return;

    int totalVertCount = (pcx - 1) * (pcy - 1) * 3 * 2;

    vertbuf->data = (VertexData3D*)calloc(totalVertCount, sizeof(VertexData3D));
    vertbuf->len = totalVertCount;

    int vi = 0;

    for (int i = 0; i < pcy-1; ++i)
    {
        for (int j = 0; j < pcx-1; ++j)
        {
            float za = *(pz + ( i    * pcx) + j    );
            float zb = *(pz + ( i    * pcx) + j + 1);
            float zc = *(pz + ((i+1) * pcx) + j    );
            float zd = *(pz + ((i+1) * pcx) + j + 1);

            float xa = cellw * j;
            float xb = cellw * (j+1);
            float xc = xa;
            float xd = xb;

            float ya = cellh * i;
            float yb = ya;
            float yc = cellh * (i+1);
            float yd = yc;

            float ua = (float) j   /(pcx-1);
            float ub = (float)(j+1)/(pcx-1);
            float uc = ua;
            float ud = ub;

            float va = (float) i   /(pcy-1);
            float vb = va;
            float vc = (float)(i+1)/(pcy-1);
            float vd = vc;

            // 
            // C +---+ D
            //   |  /|
            //   | / |
            //   |/  |
            // A +---+ B
            //

            // Normal for A
            float ab[3] = { xb - xa, yb - ya, zb - za };
            float ac[3] = { xc - xa, yc - ya, zc - za };
            float abcN[3] = {
                ab[1]*ac[2] - ab[2]*ac[1],
                ab[2]*ac[0] - ab[0]*ac[2],
                ab[0]*ac[1] - ab[1]*ac[0]
            };
            float abcNLen = sqrtf(abcN[0]*abcN[0] + abcN[1]*abcN[1] + abcN[2]*abcN[2]);
            abcN[0] /= abcNLen;
            abcN[1] /= abcNLen;
            abcN[2] /= abcNLen;

            // Normal for B			
            float bd[3] = { xd - xb, yd - yb, zd - zb };
            float ba[3] = { xa - xb, ya - yb, za - zb };
            float bdaN[3] = {
                bd[1]*ba[2] - bd[2]*ba[1],
                bd[2]*ba[0] - bd[0]*ba[2],
                bd[0]*ba[1] - bd[1]*ba[0]
            };
            float bdaNLen = sqrtf(bdaN[0]*bdaN[0] + bdaN[1]*bdaN[1] + bdaN[2]*bdaN[2]);
            bdaN[0] /= bdaNLen;
            bdaN[1] /= bdaNLen;
            bdaN[2] /= bdaNLen;

            // Normal for C
            float ca[3] = { xa - xc, ya - yc, za - zc };
            float cd[3] = { xd - xc, yd - yc, zd - zc };
            float cadN[3] = {
                ca[1]*cd[2] - ca[2]*cd[1],
                ca[2]*cd[0] - ca[0]*cd[2],
                ca[0]*cd[1] - ca[1]*cd[0]
            };
            float cadNLen = sqrtf(cadN[0]*cadN[0] + cadN[1]*cadN[1] + cadN[2]*cadN[2]);
            cadN[0] /= cadNLen;
            cadN[1] /= cadNLen;
            cadN[2] /= cadNLen;

            // Normal for D
            float dc[3] = { xc - xd, yc - yd, zc - zd };
            float db[3] = { xb - xd, yb - yd, zb - zd };
            float dcbN[3] = {
                dc[1]*db[2] - dc[2]*db[1],
                dc[2]*db[0] - dc[0]*db[2],
                dc[0]*db[1] - dc[1]*db[0]
            };
            float dcbNLen = sqrtf(dcbN[0]*dcbN[0] + dcbN[1]*dcbN[1] + dcbN[2]*dcbN[2]);
            dcbN[0] /= dcbNLen;
            dcbN[1] /= dcbNLen;
            dcbN[2] /= dcbNLen;

            float points[][8] = {
                { xa, ya, za, ua, va, abcN[0], abcN[1], abcN[2] }, // A
                { xb, yb, zb, ub, vb, bdaN[0], bdaN[1], bdaN[2] }, // B
                { xd, yd, zd, ud, vd, dcbN[0], dcbN[1], dcbN[2] }, // D

                { xa, ya, za, ua, va, abcN[0], abcN[1], abcN[2] }, // A
                { xd, yd, zd, ud, vd, dcbN[0], dcbN[1], dcbN[2] }, // D
                { xc, yc, zc, uc, vc, cadN[0], cadN[1], cadN[2] }, // C
            };

            for (int k = 0; k < _countof(points); ++k)
            {
                VertexData3D& d = vertbuf->data[vi];

                d.x = points[k][0];
                d.y = points[k][1];
                d.z = points[k][2];
                d.u = points[k][3];
                d.v = points[k][4];
                d.nx = points[k][5];
                d.ny = points[k][6];
                d.nz = points[k][7];
                d.r = 1.0f;
                d.g = 1.0f;
                d.b = 1.0f;
                d.a = 1.0f;

                ++vi;
            }
        }
    }

    create_vertex_buffer_from_array_3d(vertbuf);
}

void create_vertex_buffer_pie(VertexBuffer* vertbuf, float r, int count)
{
    std::vector<VertexData3D> v(count+1);

    VertexData3D origin;
    memset(&origin, 0, sizeof(VertexData3D));
    origin.r = 1.0f;
    origin.g = 1.0f;
    origin.b = 1.0f;
    origin.a = 1.0f;
    origin.nz = 1.0f;

    v[0] = origin;

    for (int i = 0; i < count; ++i)
    {
        float rad = (float)M_PI/2/(count-1)*i;

        origin.x = r * cosf(rad);
        origin.y = r * sinf(rad);

        /*origin.u = cosf(rad);
        origin.v = sinf(rad);*/

        v[i + 1] = origin;
    }

    vertbuf->data = &v[0];
    vertbuf->len = v.size();

    create_vertex_buffer_from_array_3d(vertbuf);

    vertbuf->data = nullptr;
}
