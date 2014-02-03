#pragma once

void create_vertex_buffer_from_array(GLuint& buffer_name, GLuint& vert_array_name, double vertex_data[][4], size_t vertex_count);

namespace client {
    class VertexBuffer {
    public:
        static VertexBuffer create_from_rectangle(double width, double height);
        static VertexBuffer create_from_cube(double width, double height, double depth);
        static VertexBuffer create_from_circle(double r, size_t points);

        GLuint buffer_name;
        GLuint vert_array_name;
    };
}
