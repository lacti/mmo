#pragma once

#ifdef RENDERING_SERVICE_EXPORTS
#define RENDERING_SERVICE_API __declspec(dllexport)
#else
#define RENDERING_SERVICE_API __declspec(dllimport)
#endif

struct BalloonConfig
{
    float ox;
    float oy;
    float oz;
    float bwidth;
    float bheight;
    float r;
    float* v;
    float* p;
    int alignZ;
    const wchar_t* text;
    float* vInv;
    GLint gUniMvp;
    VertexBuffer vbpie;
    VertexBuffer vbui;
    GLuint gTexBalloon;
};

RENDERING_SERVICE_API
void RenderBalloon(const BalloonConfig* bc);
