#include "stdafx.h"
#include "screentext.h"
#include "../rendering_service/font.h"
#include "../rendering_service/texture.h"
#include "../mmobase/matrix.h"
#include "../rendering_service/Vertex.h"
#include "../rendering_service/balloon.h"

unsigned int nlpo2(unsigned int x);
float clampf(float v, float lo, float hi);

struct ScreenText
{
    unsigned char * surface;
    int sw, sh;
    GLuint texId;
    float pos[3];
    int durationMs;
    int ageMs;
    bool valid;
    int textWidth;
    int textMaxBearing;
};

tbb::concurrent_vector<ScreenText> gScreenTexts;

void
    RegisterScreenText(
    _In_ unsigned char* surface,
    _In_ const int sw,
    _In_ const int sh,
    _In_ const int textWidth,
    _In_ const int textMaxBearing,
    _In_ const int durationMs
    )
{
    ScreenText st;
    st.texId = 0;
    st.surface = surface;
    st.sw = sw;
    st.sh = sh;
    st.textWidth = textWidth;
    st.textMaxBearing = textMaxBearing;		
    st.pos[0] = 0;
    st.pos[1] = 100;
    st.pos[2] = 0;
    st.durationMs = durationMs;
    st.ageMs = 0;
    st.valid = true;

    gScreenTexts.push_back(st);
}

void
    ShowSingleLineScreenText(
    _In_ const wchar_t* text,
    _In_ int textSize,
    _In_ int durationMs
    )
{
    int textWidth, textMaxBearing, textMaxHeight, sw, sh;
    unsigned char* surface;

    textWidth = GetTextWidth(textSize, text, &textMaxBearing, &textMaxHeight);

    sw = nlpo2(textWidth);
    sh = nlpo2(textMaxHeight);

    surface = (unsigned char*)calloc(4, sw * sh);

    float tint[3] = { 1.0, 1.0, 1.0 };
    DrawTextToMemory(surface, sw, sh, text, (sw-textWidth)/2, (sh+textMaxBearing)/2, textSize, tint);

    RegisterScreenText(surface, sw, sh, textWidth, textMaxBearing, durationMs);
}

void RenderScreenTexts(int w, int h, float dt)
{
    float yOffset = 0;

    //glPushAttrib(GL_DEPTH_BUFFER_BIT);
    //glDisable(GL_DEPTH_TEST);
    glDepthFunc(GL_ALWAYS);

    for (auto&& st : boost::adaptors::reverse(gScreenTexts))
    {
        if (!st.valid)
            return;

        st.ageMs += static_cast<int>(dt * 1000);

        if (st.ageMs >= st.durationMs)
            return;

        if (st.texId == 0)
        {
            st.texId = CreateTextureFromMemory(st.sw, st.sh, st.surface);

            if (st.texId == 0)
            {
                st.valid = false;
                return;
            }
        }

        CLSALIGN16 float m[16], v[16], p[16], mvpdata[16], uvxdata[9];
        CLSALIGN16 float uiviewscale[4] = { 2.0f / w, 2.0f / h, 1.0f, 1.0f };
        CLSALIGN16 float uiviewtrans[4] = { -w/2.0f, -h/2.0f, 0, 0 };
        CLSALIGN16 float uipos[4] = {
            (float)(w/2),
            h - st.pos[1] + yOffset,
            st.pos[2], 1.0f
        };
        CLSALIGN16 float uiscale[4] = { (float)st.sw, (float)st.sh, 1, 1 };

        glhLoadIdentityf2(m);
        glhLoadIdentityf2(v);
        glhLoadIdentityf2(p);

        glhTranslatef2_SSE_Aligned(m, uipos);
        glhScalef2_SSE_Aligned(m, uiscale);

        glhScalef2_SSE_Aligned(v, uiviewscale);
        glhTranslatef2_SSE_Aligned(v, uiviewtrans);

        extern GLuint gTexBalloon;
        extern GLint gUniMvp;
        extern VertexBuffer vbui, vbpie;

        BalloonConfig bc;
        bc.ox = uipos[0];
        bc.oy = uipos[1];
        bc.oz = 0;
        bc.bwidth = uiscale[0];
        bc.bheight = uiscale[1];
        bc.r = 5;
        bc.v = v;
        bc.p = p;
        bc.alignZ = 0;
        bc.text =L"";
        bc.vInv = 0;
        bc.gUniMvp = gUniMvp;
        bc.vbpie = vbpie;
        bc.vbui = vbui;
        bc.gTexBalloon = gTexBalloon;

        //RenderBalloon(&bc);

        glhLoadIdentityf2(mvpdata);
        glhMultMatrixSwappedf2_SSE_Aligned(mvpdata, m);
        glhMultMatrixSwappedf2_SSE_Aligned(mvpdata, v);
        glhMultMatrixSwappedf2_SSE_Aligned(mvpdata, p);

        glhLoadIdentity3f2(uvxdata);

        extern VertexBuffer vbui;
        extern GLint gUniMvp, gUniUvx, gUniAlpha;

        /*
        glUniformMatrix3fv(gUniUvx, 1, GL_FALSE, (GLfloat*)uvxdata);
        glUniformMatrix4fv(gUniMvp, 1, GL_FALSE, (GLfloat*)mvpdata);
        glUniform1f(gUniAlpha, clampf((float)st.ageMs*16 / st.durationMs, 0.0f, 1.0f) );

        glBindTexture(GL_TEXTURE_2D, st.texId);
        glBindVertexArray(vbui.varray);
        glDrawArrays(GL_TRIANGLES, 0, vbui.len);
        glBindVertexArray(0);

        glUniform1f(gUniAlpha, 1.0f);
        */

        yOffset += st.sh;
    }

    //glPopAttrib();
}
