#include "stdafx.h"
#include "winsys.h"
#include "../rendering_service/texture.h"
#include "../rendering_service/font.h"
#include "pushbutton.h"
#include "basicdraw.h"

unsigned int
    CreateSigninWindowBackgroundTexture(
    _Inout_ _Notnull_ unsigned char** ppSurface
    )
{
    int i, j;
    int fontsize = 32;
    unsigned char* texdata;
    int texsize = 512;
    const wchar_t* wintitle = L"시작하기";
    int mainborderthickness = texsize / 100;
    int subborderthickness = mainborderthickness / 2;
    int titlemargin = mainborderthickness;
    unsigned int t;

    texdata = (unsigned char*)malloc(4 * texsize * texsize);

    for (i = 0; i < texsize; ++i)
    {
        for (j = 0; j < texsize; ++j)
        {
            unsigned char c[4];

            if (j < mainborderthickness)
            {
                c[0] = c[1] = c[2] = 0x27; c[3] = 0xff;
            }
            else if (j > texsize - mainborderthickness)
            {
                c[0] = c[1] = c[2] = 0x27; c[3] = 0xff;
            }
            else
            {

                if (i > texsize - mainborderthickness)
                {
                    c[0] = c[1] = c[2] = 0x27; c[3] = 0xff;
                }
                else if (i > texsize - mainborderthickness - mainborderthickness)
                {
                    c[0] = c[1] = c[2] = 0x58; c[3] = 0xff;
                }
                else if (i > texsize - mainborderthickness - mainborderthickness - (fontsize + titlemargin*2))
                {	
                    c[0] = c[1] = c[2] = 0x33; c[3] = 0xff;
                }
                else if (i > texsize - mainborderthickness - mainborderthickness - (fontsize + titlemargin*2) - subborderthickness)
                {
                    c[0] = c[1] = c[2] = 0x27; c[3] = 0xff;
                }
                else if (i > texsize - mainborderthickness - mainborderthickness - (fontsize + titlemargin*2) - subborderthickness - subborderthickness)
                {
                    c[0] = c[1] = c[2] = 0x58; c[3] = 0xff;
                }
                else if (i > mainborderthickness + mainborderthickness)
                {	
                    c[0] = c[1] = c[2] = 0x24; c[3] = 0xff;
                }
                else if (i > mainborderthickness)
                {
                    c[0] = c[1] = c[2] = 0x27; c[3] = 0xff;
                }
                else
                {
                    c[0] = c[1] = c[2] = 0x58; c[3] = 0xff;
                }
            }

            texdata[4*(texsize * i + j) + 0] = c[0];
            texdata[4*(texsize * i + j) + 1] = c[1];
            texdata[4*(texsize * i + j) + 2] = c[2];
            texdata[4*(texsize * i + j) + 3] = c[3];
        }
    }

    DrawTextToMemory(
        texdata,
        texsize,
        texsize,
        L"Start 시작하기",
        titlemargin*2,
        mainborderthickness + titlemargin + fontsize,
        fontsize,
        NULL
        );

    DrawTextToMemory(
        texdata,
        texsize,
        texsize,
        L"User Name 이름",
        titlemargin*2 + titlemargin*2,
        mainborderthickness + titlemargin + fontsize*3,
        fontsize,
        NULL
        );

    DrawTextToMemory(
        texdata,
        texsize,
        texsize,
        L"World 세계",
        titlemargin*2 + titlemargin*2,
        mainborderthickness + titlemargin + fontsize*6,
        fontsize,
        NULL
        );

    DrawPushButtonToMemory(
        texdata,
        texsize,
        texsize,
        texsize - 200 - 20,
        texsize - 50 - 20,
        200,
        50,
        2,
        L"Start 시작",
        fontsize
        );

    t = CreateTextureFromMemory(texsize, texsize, texdata);

    *ppSurface = texdata;

    return t;
}

void
    RedrawSigninWindow(
    _In_ _Notnull_ const unsigned char *const bgSurface,
    _In_ unsigned int texId,
    _In_ _Notnull_ const wchar_t* userName,
    _In_ _Notnull_ const wchar_t* worldName
    )
{
    const int dynfontsize = 32;
    const int dynfonttexsize = 512;
    unsigned char* textrenderdata = (unsigned char*)malloc(4*dynfonttexsize*dynfonttexsize);
    wchar_t renderText[4096] = {0};

    memcpy(textrenderdata, bgSurface, 4*dynfonttexsize*dynfonttexsize);

    DrawTextToMemory(textrenderdata, dynfonttexsize, dynfonttexsize, userName, 20, dynfontsize*5, dynfontsize, NULL);
    DrawTextToMemory(textrenderdata, dynfonttexsize, dynfonttexsize, worldName, 20, dynfontsize*10, dynfontsize, NULL);

    //glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texId);
    //glActiveTexture(GL_TEXTURE0);

    glTexSubImage2D(
        GL_TEXTURE_2D,
        0,
        0,
        0,
        dynfonttexsize,
        dynfonttexsize,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        textrenderdata
        );
    glGenerateMipmap(GL_TEXTURE_2D);

    free(textrenderdata);
}

unsigned int
    CreateChatWindowBackgroundTexture(
    _Inout_ _Notnull_ unsigned char** ppSurface,
    _In_ int texsize_x,
    _In_ int texsize_y
    )
{
    int i, j;
    int fontsize = 32;
    unsigned char* texdata;
    const wchar_t* wintitle = L"시작하기";
    int mainborderthickness = texsize_x / 100;
    int subborderthickness = mainborderthickness / 2;
    int titlemargin = mainborderthickness;
    unsigned int t;
    unsigned int alpha = 0xcf;

    texdata = (unsigned char*)malloc(4 * texsize_x * texsize_y);

    for (i = 0; i < texsize_y; ++i)
    {
        for (j = 0; j < texsize_x; ++j)
        {
            unsigned char c[4];

            c[0] = c[1] = c[2] = 0x27; c[3] = alpha;

            texdata[4*(texsize_x * i + j) + 0] = c[0];
            texdata[4*(texsize_x * i + j) + 1] = c[1];
            texdata[4*(texsize_x * i + j) + 2] = c[2];
            texdata[4*(texsize_x * i + j) + 3] = c[3];
        }
    }

    t = CreateTextureFromMemory(texsize_x, texsize_y, texdata);

    *ppSurface = texdata;

    return t;
}

void
    RedrawChatWindow(
    _In_ _Notnull_ const unsigned char *const bgSurface,
    _In_ unsigned int texId,
    _In_ _Notnull_ const wchar_t* chatLog,
    _In_ _Notnull_ const wchar_t* currentLine,
    _In_ int inputFocus
    )
{
    extern CLSALIGN16 float chatuiscale[4];

    const int dynfontsize = 13;
    const int dynfonttexsize_x = (int)chatuiscale[0];
    const int dynfonttexsize_y = (int)chatuiscale[1];
    unsigned char* textrenderdata = (unsigned char*)malloc(4*dynfonttexsize_x*dynfonttexsize_y);
    wchar_t renderText[4096] = {0};

    memcpy(textrenderdata, bgSurface, 4*dynfonttexsize_x*dynfonttexsize_y);

    DrawTextToMemory(textrenderdata, dynfonttexsize_x, dynfonttexsize_y, chatLog, 0, dynfontsize, dynfontsize, NULL);

    int maxBearingY, maxHeight;
    int textWidth = GetTextWidth(dynfontsize, currentLine, &maxBearingY, &maxHeight);

    extern int CHAT_LOG_LINES;

    if (inputFocus)
    {
        DrawInvertedColorBoxToMemory(dynfonttexsize_x, dynfonttexsize_y, textrenderdata, textWidth, dynfontsize*CHAT_LOG_LINES, 4, dynfontsize);
    }
    DrawTextToMemory(textrenderdata, dynfonttexsize_x, dynfonttexsize_y, currentLine, 0, dynfontsize*(CHAT_LOG_LINES+1), dynfontsize, NULL);


    glBindTexture(GL_TEXTURE_2D, texId);
    //glActiveTexture(GL_TEXTURE0);

    glTexSubImage2D(
        GL_TEXTURE_2D,
        0,
        0,
        0,
        dynfonttexsize_x,
        dynfonttexsize_y,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        textrenderdata
        );
    glGenerateMipmap(GL_TEXTURE_2D);

    free(textrenderdata);
}
