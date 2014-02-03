#include "stdafx.h"
#include "../mmobase/matrix.h"
#include "../mmobase/basicmath.h"
#include "../rendering_service/vertex.h"
#include "../rendering_service/simplestaticobject.h"
#include "balloon.h"
#include "font.h"
#include "texture.h"

std::map<std::pair<std::wstring, int>, GLuint> gBalloonTextTextureCache;

GLuint CreateBalloonText(
    _In_ const wchar_t* text,
    _In_ int textSize,
    _Out_ int *out_sw,
    _Out_ int *out_sh
    )
{
    auto resourceKey = std::pair<std::wstring, int>(std::wstring(text), textSize);

    auto it = gBalloonTextTextureCache.find(resourceKey);
    if (it != gBalloonTextTextureCache.end())
    {
        return it->second;
    }
    
    int textWidth, textMaxBearing, textMaxHeight, sw, sh;
    unsigned char* surface;

    textWidth = GetTextWidth(textSize, text, &textMaxBearing, &textMaxHeight);

    sw = nlpo2(textWidth);
    sh = nlpo2(textMaxHeight);

    surface = (unsigned char*)malloc(4 * sw * sh);
    for (int i = 0; i < 4 * sw * sh; i += 4)
    {
        surface[i + 0] = 0x2f;
        surface[i + 1] = 0x2f;
        surface[i + 2] = 0x2f;
        surface[i + 3] = 0x2f;
    }

    float tint[3] = { 1.0, 1.0, 1.0 };
    DrawTextToMemory(surface, sw, sh, text, (sw-textWidth)/2, (sh+textMaxBearing)/2, textSize, tint);

    *out_sw = sw;
    *out_sh = sh;

    GLuint texId = CreateTextureFromMemory(sw, sh, surface);

    free(surface);

    gBalloonTextTextureCache[resourceKey] = texId;

    return texId;
}

void RenderBalloon(const BalloonConfig* bc)
{
    if (!bc) return;

    CLSALIGN16 float m[16], mvpdata[16];
    
    static const float corners[][2] = {
        { 1, 1 }, { -1, 1 }, { -1, -1 }, { 1, -1 }
    };

    //glPushAttrib(GL_POLYGON_BIT);
    glDisable(GL_CULL_FACE);

    for (int c = 0; c < _countof(corners); ++c)
    {
        CLSALIGN16 float piepos[4] = {
            bc->ox,
            bc->oy,
            bc->oz,
            1
        };
        CLSALIGN16 float pieposOffset[4] = {
            corners[c][0] * (bc->bwidth/2-bc->r),
            corners[c][1] * (bc->bheight/2-bc->r),
            0,
            0
        };
        CLSALIGN16 float piescale[4] = {
            corners[c][0] * bc->r,
            corners[c][1] * bc->r,
            1,
            0
        };
        
        if (bc->alignZ)
        {
            CLSALIGN16 float vInvNoTrans[16];
            memcpy(vInvNoTrans, bc->vInv, sizeof(vInvNoTrans));
            memcpy(vInvNoTrans + 12, piepos, sizeof(piepos));

            glhLoadIdentityf2(m);
            glhTranslatef2_SSE_Aligned(m, pieposOffset);
            glhScalef2_SSE_Aligned(m, piescale);
            glhMultMatrixSwappedf2_SSE_Aligned(m, vInvNoTrans);
        }
        else
        {
            piepos[0] += pieposOffset[0];
            piepos[1] += pieposOffset[1];
            piepos[2] += pieposOffset[2];
            piepos[3] += pieposOffset[3];

            glhLoadIdentityf2(m);
            glhTranslatef2_SSE_Aligned(m, piepos);
            glhScalef2_SSE_Aligned(m, piescale);
        }

        /*
        glhLoadIdentityf2(mvpdata);
        glhMultMatrixSwappedf2_SSE_Aligned(mvpdata, m);
        glhMultMatrixSwappedf2_SSE_Aligned(mvpdata, bc->v);
        glhMultMatrixSwappedf2_SSE_Aligned(mvpdata, bc->p);
        */

        rs_render_triangle_fan(bc->gTexBalloon, bc->vbpie.varray, bc->vbpie.len, m);

        /*
        glUniformMatrix4fv(bc->gUniMvp, 1, GL_FALSE, (GLfloat*)mvpdata);
        glBindTexture(GL_TEXTURE_2D, bc->gTexBalloon);
        glBindVertexArray(bc->vbpie.varray);
        glDrawArrays(GL_TRIANGLE_FAN, 0, bc->vbpie.len);
        glBindVertexArray(0);
        */
    }

    int sw, sh;
    GLuint texText = bc->gTexBalloon;
    if (bc->text && bc->text[0])
        texText = CreateBalloonText(bc->text, 30, &sw, &sh);

    CLSALIGN16 float piepos[4] = {
        bc->ox,
        bc->oy,
        bc->oz,
        1
    };
    CLSALIGN16 float piescale[4] = {
        bc->bwidth-2*bc->r,
        bc->bheight,
        1,
        0
    };

    if (bc->alignZ)
    {
        CLSALIGN16 float vInvNoTrans[16];
        memcpy(vInvNoTrans, bc->vInv, sizeof(vInvNoTrans));
        memcpy(vInvNoTrans + 12, piepos, sizeof(piepos));

        glhLoadIdentityf2(m);
        glhScalef2_SSE_Aligned(m, piescale);
        glhMultMatrixSwappedf2_SSE_Aligned(m, vInvNoTrans);
    }
    else
    {
        glhLoadIdentityf2(m);
        glhTranslatef2_SSE_Aligned(m, piepos);
        glhScalef2_SSE_Aligned(m, piescale);
    }
            
    glhLoadIdentityf2(mvpdata);
    glhMultMatrixSwappedf2_SSE_Aligned(mvpdata, m);
    glhMultMatrixSwappedf2_SSE_Aligned(mvpdata, bc->v);
    glhMultMatrixSwappedf2_SSE_Aligned(mvpdata, bc->p);

    glUniformMatrix4fv(bc->gUniMvp, 1, GL_FALSE, (GLfloat*)mvpdata);
    glBindTexture(GL_TEXTURE_2D, texText);
    glBindVertexArray(bc->vbui.varray);
    glDrawArrays(GL_TRIANGLES, 0, bc->vbui.len);
    glBindVertexArray(0);

    for (int i = -1; i <= 1; i+=2)
    {
        piepos[0] = bc->ox;
        piepos[1] = bc->oy;
        piescale[0] = bc->r;
        piescale[1] = bc->bheight-2*bc->r;

        CLSALIGN16 float pieposOffset[4] = {
            i * (bc->bwidth-bc->r)/2,
            0,
            0,
            0
        };

        if (bc->alignZ)
        {
            CLSALIGN16 float vInvNoTrans[16];
            memcpy(vInvNoTrans, bc->vInv, sizeof(vInvNoTrans));
            memcpy(vInvNoTrans + 12, piepos, sizeof(piepos));

            glhLoadIdentityf2(m);
            glhTranslatef2_SSE_Aligned(m, pieposOffset);
            glhScalef2_SSE_Aligned(m, piescale);
            glhMultMatrixSwappedf2_SSE_Aligned(m, vInvNoTrans);
        }
        else
        {
            piepos[0] += pieposOffset[0];
            piepos[1] += pieposOffset[1];
            piepos[2] += pieposOffset[2];
            piepos[3] += pieposOffset[3];

            glhLoadIdentityf2(m);
            glhTranslatef2_SSE_Aligned(m, piepos);
            glhScalef2_SSE_Aligned(m, piescale);
        }

            
        glhLoadIdentityf2(mvpdata);
        glhMultMatrixSwappedf2_SSE_Aligned(mvpdata, m);
        glhMultMatrixSwappedf2_SSE_Aligned(mvpdata, bc->v);
        glhMultMatrixSwappedf2_SSE_Aligned(mvpdata, bc->p);

        glUniformMatrix4fv(bc->gUniMvp, 1, GL_FALSE, (GLfloat*)mvpdata);
        glBindTexture(GL_TEXTURE_2D, bc->gTexBalloon);
        glBindVertexArray(bc->vbui.varray);
        glDrawArrays(GL_TRIANGLES, 0, bc->vbui.len);
        glBindVertexArray(0);
    }

    //glPopAttrib();
}
