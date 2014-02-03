#include "gtest/gtest.h"
#include "../mmobase/mmobase.h"
#include "../rendering_service/rpublicinterface.h"

void RenderingServiceTest() {}

TEST(RenderingServiceTest, LoadSingleTriangleMesh)
{
    char resPath[1024] = {0};
    GetMmoResourcePath(resPath, 1024, "triangle.fbx");
    ASSERT_GT(strlen(resPath), size_t(0));
    ASSERT_NE(RHANDLE_INVALID, r_load_fbx_scene(resPath));
    ASSERT_EQ(1, r_get_object_count(RSCENEROOTTYPE));
    ASSERT_EQ(1, r_get_object_count(RMESHDATATYPE));
    ASSERT_EQ(1, r_get_object_count(RSIMPLEMESHTYPE));
    RHANDLE mesh = r_find_handle("Triangle");
    ASSERT_NE(RHANDLE_INVALID, mesh);
    ASSERT_EQ(3*1, r_meshdata_vertex_count(mesh));
    RHANDLE ins = r_find_handle("Triangle.Instance");
    ASSERT_NE(RHANDLE_INVALID, ins);
    const float* translation = r_simplemesh_lcltranslation(ins);
    EXPECT_FLOAT_EQ(0.1f, translation[0]);
    EXPECT_FLOAT_EQ(0.2f, translation[1]);
    EXPECT_FLOAT_EQ(0.3f, translation[2]);
    const float* rotation = r_simplemesh_lclrotation(ins);
    EXPECT_FLOAT_EQ(20.0f, rotation[0]);
    EXPECT_FLOAT_EQ(40.0f, rotation[1]);
    EXPECT_FLOAT_EQ(60.0f, rotation[2]);
    const float* scaling = r_simplemesh_lclscaling(ins);
    EXPECT_FLOAT_EQ(1.5f, scaling[0]);
    EXPECT_FLOAT_EQ(2.0f, scaling[1]);
    EXPECT_FLOAT_EQ(2.5f, scaling[2]);
    ASSERT_EQ(1+1+1, r_unload_all());
}

TEST(RenderingServiceTest, LoadSinglePlaneMesh)
{
    char resPath[1024] = {0};
    GetMmoResourcePath(resPath, 1024, "plane.fbx");
    ASSERT_GT(strlen(resPath), size_t(0));
    ASSERT_NE(RHANDLE_INVALID, r_load_fbx_scene(resPath));
    ASSERT_EQ(1, r_get_object_count(RSCENEROOTTYPE));
    ASSERT_EQ(1, r_get_object_count(RMESHDATATYPE));
    ASSERT_EQ(1, r_get_object_count(RSIMPLEMESHTYPE));
    RHANDLE mesh = r_find_handle("Plane");
    ASSERT_NE(RHANDLE_INVALID, mesh);
    ASSERT_EQ(3*2, r_meshdata_vertex_count(mesh));
    RHANDLE ins = r_find_handle("Plane.Instance");
    ASSERT_NE(RHANDLE_INVALID, ins);
    const float* translation = r_simplemesh_lcltranslation(ins);
    EXPECT_FLOAT_EQ(1.0f, translation[0]);
    EXPECT_FLOAT_EQ(2.0f, translation[1]);
    EXPECT_FLOAT_EQ(3.0f, translation[2]);
    const float* rotation = r_simplemesh_lclrotation(ins);
    EXPECT_FLOAT_EQ(0, rotation[0]);
    EXPECT_FLOAT_EQ(0, rotation[1]);
    EXPECT_FLOAT_EQ(0, rotation[2]);
    const float* scaling = r_simplemesh_lclscaling(ins);
    EXPECT_FLOAT_EQ(1.0f, scaling[0]);
    EXPECT_FLOAT_EQ(1.0f, scaling[1]);
    EXPECT_FLOAT_EQ(1.0f, scaling[2]);
    ASSERT_EQ(1+1+1, r_unload_all());
}

TEST(RenderingServiceTest, LoadTriangleAndPlane)
{
    char resPath[1024] = {0};
    GetMmoResourcePath(resPath, 1024, "triangleandplane.fbx");
    ASSERT_GT(strlen(resPath), size_t(0));
    ASSERT_NE(RHANDLE_INVALID, r_load_fbx_scene(resPath));
    ASSERT_EQ(1, r_get_object_count(RSCENEROOTTYPE));
    ASSERT_EQ(2, r_get_object_count(RMESHDATATYPE));
    ASSERT_EQ(2, r_get_object_count(RSIMPLEMESHTYPE));
    {
        RHANDLE mesh = r_find_handle("Triangle");
        ASSERT_NE(RHANDLE_INVALID, mesh);
        ASSERT_EQ(3*1, r_meshdata_vertex_count(mesh));
        RHANDLE ins = r_find_handle("Triangle.Instance");
        ASSERT_NE(RHANDLE_INVALID, ins);
        const float* translation = r_simplemesh_lcltranslation(ins);
        EXPECT_FLOAT_EQ(0.1f, translation[0]);
        EXPECT_FLOAT_EQ(0.2f, translation[1]);
        EXPECT_FLOAT_EQ(0.3f, translation[2]);
        const float* rotation = r_simplemesh_lclrotation(ins);
        EXPECT_FLOAT_EQ(20.0f, rotation[0]);
        EXPECT_FLOAT_EQ(40.0f, rotation[1]);
        EXPECT_FLOAT_EQ(60.0f, rotation[2]);
        const float* scaling = r_simplemesh_lclscaling(ins);
        EXPECT_FLOAT_EQ(1.5f, scaling[0]);
        EXPECT_FLOAT_EQ(2.0f, scaling[1]);
        EXPECT_FLOAT_EQ(2.5f, scaling[2]);
    }
    {
        RHANDLE mesh = r_find_handle("Plane");
        ASSERT_NE(RHANDLE_INVALID, mesh);
        ASSERT_EQ(3*2, r_meshdata_vertex_count(mesh));
        RHANDLE ins = r_find_handle("Plane.Instance");
        ASSERT_NE(RHANDLE_INVALID, ins);
        const float* translation = r_simplemesh_lcltranslation(ins);
        EXPECT_FLOAT_EQ(1.0f, translation[0]);
        EXPECT_FLOAT_EQ(2.0f, translation[1]);
        EXPECT_FLOAT_EQ(3.0f, translation[2]);
        const float* rotation = r_simplemesh_lclrotation(ins);
        EXPECT_FLOAT_EQ(0, rotation[0]);
        EXPECT_FLOAT_EQ(0, rotation[1]);
        EXPECT_FLOAT_EQ(0, rotation[2]);
        const float* scaling = r_simplemesh_lclscaling(ins);
        EXPECT_FLOAT_EQ(1.0f, scaling[0]);
        EXPECT_FLOAT_EQ(1.0f, scaling[1]);
        EXPECT_FLOAT_EQ(1.0f, scaling[2]);
    }
    ASSERT_EQ(1+2+2, r_unload_all());
}

TEST(RenderingServiceTest, LoadColoredTriangle)
{
    char resPath[1024] = {0};
    GetMmoResourcePath(resPath, 1024, "coloredtriangle.fbx");
    ASSERT_GT(strlen(resPath), size_t(0));
    ASSERT_NE(RHANDLE_INVALID, r_load_fbx_scene(resPath));
    ASSERT_EQ(1, r_get_object_count(RSCENEROOTTYPE));
    ASSERT_EQ(1, r_get_object_count(RMESHDATATYPE));
    ASSERT_EQ(1, r_get_object_count(RSIMPLEMESHTYPE));
    RHANDLE mesh = r_find_handle("ColoredTriangle");
    ASSERT_NE(RHANDLE_INVALID, mesh);
    ASSERT_EQ(3*1, r_meshdata_vertex_count(mesh));
    RHANDLE ins = r_find_handle("ColoredTriangle.Instance");
    ASSERT_NE(RHANDLE_INVALID, ins);
    const float* translation = r_simplemesh_lcltranslation(ins);
    EXPECT_FLOAT_EQ(0, translation[0]);
    EXPECT_FLOAT_EQ(0, translation[1]);
    EXPECT_FLOAT_EQ(0, translation[2]);
    const float* rotation = r_simplemesh_lclrotation(ins);
    EXPECT_FLOAT_EQ(0, rotation[0]);
    EXPECT_FLOAT_EQ(0, rotation[1]);
    EXPECT_FLOAT_EQ(0, rotation[2]);
    const float* scaling = r_simplemesh_lclscaling(ins);
    EXPECT_FLOAT_EQ(1.0f, scaling[0]);
    EXPECT_FLOAT_EQ(1.0f, scaling[1]);
    EXPECT_FLOAT_EQ(1.0f, scaling[2]);
    ASSERT_EQ(1+1+1, r_unload_all());
}
