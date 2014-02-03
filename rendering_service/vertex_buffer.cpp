#include "stdafx.h"
#include <iostream>
#include "vertex_buffer.h"

static char* POINTER_OFFSET(size_t i)
{
    return (char*)NULL + i;
}

struct VertexData
{
    VertexData(double ix, double iy, double iu, double iv)
        : x(ix), y(iy), u(iu), v(iv) {}

    double x;
    double y;
    double u;
    double v;
};

struct VertexData3D
{
    VertexData3D(double ix, double iy, double iz, double iu, double iv)
        : x(ix), y(iy), z(iz), u(iu), v(iv) {}

    double x;
    double y;
    double z;
    double u;
    double v;
};

static void create_vertex_buffer_from_array(GLuint& buffer_name, GLuint& vert_array_name, VertexData* vertex_data, size_t vertex_count)
{
    const static int vertex_size = sizeof(double)*4; // x, y, u, v
    assert(glGetError() == GL_NO_ERROR);
    glGenBuffers(1, &buffer_name);
    assert(glGetError() == GL_NO_ERROR);
    glBindBuffer(GL_ARRAY_BUFFER, buffer_name);
    assert(glGetError() == GL_NO_ERROR);
    glBufferData(GL_ARRAY_BUFFER, vertex_size * vertex_count, vertex_data,
        GL_STATIC_DRAW);
    assert(glGetError() == GL_NO_ERROR);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    assert(glGetError() == GL_NO_ERROR);
    std::cout << "buffer_name = " << buffer_name << std::endl;

    glGenVertexArrays(1, &vert_array_name);
    assert(glGetError() == GL_NO_ERROR);
    glBindVertexArray(vert_array_name);
    assert(glGetError() == GL_NO_ERROR);
    {    
        glBindBuffer(GL_ARRAY_BUFFER, buffer_name);
        assert(glGetError() == GL_NO_ERROR);
        glVertexAttribPointer(semantic::attr::POSITION, 2, GL_DOUBLE,
            GL_FALSE, vertex_size,
            POINTER_OFFSET(0));
        assert(glGetError() == GL_NO_ERROR);
        glVertexAttribPointer(semantic::attr::TEXCOORD, 2, GL_DOUBLE,
            GL_FALSE, vertex_size,
            POINTER_OFFSET(sizeof(double)*2));
        assert(glGetError() == GL_NO_ERROR);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        assert(glGetError() == GL_NO_ERROR);
        glEnableVertexAttribArray(semantic::attr::POSITION);
        assert(glGetError() == GL_NO_ERROR);
        glEnableVertexAttribArray(semantic::attr::TEXCOORD);
        assert(glGetError() == GL_NO_ERROR);
    }
    glBindVertexArray(0);
    assert(glGetError() == GL_NO_ERROR);

    RsDebugOutput("vert_array_name = %d\n", vert_array_name);

    int error = 0;
    while(glGetError() != GL_NO_ERROR)	{		++error;	}

    assert(error == 0);

    return;
}

static void create_vertex_buffer_from_array_3d(GLuint& buffer_name, GLuint& vert_array_name, VertexData3D* vertex_data, size_t vertex_count)
{
    const static int vertex_size = sizeof(double)*5; // x, y, z, u, v

    glGenBuffers(1, &buffer_name);
    glBindBuffer(GL_ARRAY_BUFFER, buffer_name);
    glBufferData(GL_ARRAY_BUFFER, vertex_size * vertex_count, vertex_data,
        GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    std::cout << "buffer_name = " << buffer_name << std::endl;

    glGenVertexArrays(1, &vert_array_name);
    glBindVertexArray(vert_array_name);
    {    
        glBindBuffer(GL_ARRAY_BUFFER, buffer_name);
        glVertexAttribPointer(semantic::attr::POSITION, 3, GL_DOUBLE,
            GL_FALSE, vertex_size,
            POINTER_OFFSET(0));
        glVertexAttribPointer(semantic::attr::TEXCOORD, 2, GL_DOUBLE,
            GL_FALSE, vertex_size,
            POINTER_OFFSET(sizeof(double)*3));
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glEnableVertexAttribArray(semantic::attr::POSITION);
        glEnableVertexAttribArray(semantic::attr::TEXCOORD);
    }
    glBindVertexArray(0);

    RsDebugOutput("vert_array_name = %d\n", vert_array_name);

    return;
}

using namespace client;

VertexBuffer VertexBuffer::create_from_rectangle(double width, double height)
{
    VertexBuffer vb;

    VertexData vertex_data[] = {
        VertexData(-width/2, -height/2, 0, 0),
        VertexData( width/2, -height/2, 1, 0),
        VertexData(-width/2, +height/2, 0, 1),

        VertexData(-width/2, +height/2, 0, 1),
        VertexData( width/2, -height/2, 1, 0),
        VertexData( width/2,  height/2, 1, 1)
    };

    create_vertex_buffer_from_array(vb.buffer_name, vb.vert_array_name, vertex_data, _countof(vertex_data));

    return vb;
}

VertexBuffer VertexBuffer::create_from_cube(double width, double height, double depth)
{
    VertexBuffer vb;

    VertexData3D vertex_data[] = {
        // 하단
        VertexData3D(-width/2, -height/2, -depth/2,  0, 0),
        VertexData3D( width/2, -height/2, -depth/2,  1, 0),
        VertexData3D(-width/2, +height/2, -depth/2,  0, 1),

        VertexData3D(-width/2, +height/2, -depth/2,  0, 1),
        VertexData3D( width/2, -height/2, -depth/2,  1, 0),
        VertexData3D( width/2,  height/2, -depth/2,  1, 1),

        // 상단
        VertexData3D(-width/2, -height/2, +depth/2,  0.0, 0.0),
        VertexData3D( width/2, -height/2, +depth/2,  0.5, 0.0),
        VertexData3D(-width/2, +height/2, +depth/2,  0.0, 0.5),

        VertexData3D(-width/2, +height/2, +depth/2,  0.0, 0.5),
        VertexData3D( width/2, -height/2, +depth/2,  0.5, 0.0),
        VertexData3D( width/2,  height/2, +depth/2,  0.5, 0.5),

        // 전면
        VertexData3D(-width/2, -height/2, -depth/2,  0.5, 0.5),
        VertexData3D( width/2, -height/2, -depth/2,  1.0, 0.5),
        VertexData3D(-width/2, -height/2, +depth/2,  0.5, 1.0),

        VertexData3D(-width/2, -height/2, +depth/2,  0.5, 1.0),
        VertexData3D( width/2, -height/2, -depth/2,  1.0, 0.5),
        VertexData3D( width/2, -height/2, +depth/2,  1.0, 1.0),

        // 후면
        VertexData3D(-width/2, +height/2, -depth/2,  0, 0),
        VertexData3D( width/2, +height/2, -depth/2,  1, 0),
        VertexData3D(-width/2, +height/2, +depth/2,  0, 1),

        VertexData3D(-width/2, +height/2, +depth/2,  0, 1),
        VertexData3D( width/2, +height/2, -depth/2,  1, 0),
        VertexData3D( width/2, +height/2, +depth/2,  1, 1),

        // 좌측
        VertexData3D(-width/2, +height/2, -depth/2,  0, 0),
        VertexData3D(-width/2, -height/2, -depth/2,  1, 0),
        VertexData3D(-width/2, +height/2, +depth/2,  0, 1),

        VertexData3D(-width/2, +height/2, +depth/2,  0, 1),
        VertexData3D(-width/2, -height/2, -depth/2,  1, 0),
        VertexData3D(-width/2, -height/2, +depth/2,  1, 1),

        // 우측
        VertexData3D(+width/2, +height/2, -depth/2,  0, 0),
        VertexData3D(+width/2, -height/2, -depth/2,  1, 0),
        VertexData3D(+width/2, +height/2, +depth/2,  0, 1),

        VertexData3D(+width/2, +height/2, +depth/2,  0, 1),
        VertexData3D(+width/2, -height/2, -depth/2,  1, 0),
        VertexData3D(+width/2, -height/2, +depth/2,  1, 1),
    };

    create_vertex_buffer_from_array_3d(vb.buffer_name, vb.vert_array_name, vertex_data, _countof(vertex_data));

    return vb;
}

VertexBuffer VertexBuffer::create_from_circle(double r, size_t points)
{
    VertexBuffer vb;
    std::vector<VertexData> vertex_data;
    for (size_t i = 0; i < points; ++i) {
        vertex_data.push_back( VertexData(0, 0, 0, 0) );
        vertex_data.push_back( VertexData(r * cos(2*M_PI / points * i),
            r * sin(2*M_PI / points * i),
            0,
            0) );
        vertex_data.push_back( VertexData(r * cos(2*M_PI / points * (i+1)),
            r * sin(2*M_PI / points * (i+1)),
            0,
            0) );
    }

    create_vertex_buffer_from_array(vb.buffer_name, vb.vert_array_name, &vertex_data[0], vertex_data.size());

    return vb;
}
