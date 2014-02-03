#pragma once

struct shadowmap_conf
{
    int shadowMapWidth;
    int shadowMapHeight;
    GLuint depthTextureId;
    GLuint fboId;
    GLhandleARB shadowShaderId;
    GLuint shadowMapUniform;
};
