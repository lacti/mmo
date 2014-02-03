#pragma once

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

typedef float sreal;

#ifdef MMOBASE_EXPORTS
#define MMOBASE_API __declspec(dllexport)
#else
#define MMOBASE_API __declspec(dllimport)
#endif

MMOBASE_API void glhInit(void);
MMOBASE_API void glhLoadIdentityf2(sreal *matrix);
MMOBASE_API void glhLoadIdentity3f2(sreal *matrix);
MMOBASE_API void glhTranslatef2_SSE_Aligned(sreal *matrix, const sreal *xyzw);
MMOBASE_API void glhScalef2_SSE_Aligned(sreal *matrix, const sreal *xyzw);
MMOBASE_API void glhRotatef2_SSE_Aligned(sreal *matrix, sreal angleInRadians, sreal *xyzw);
MMOBASE_API void glhMultMatrixf2_SSE_Aligned(sreal *result_leftMatrix, sreal *rightMatrix);
MMOBASE_API void glhMultMatrixSwappedf2_SSE_Aligned(sreal *result_rightMatrix, sreal *leftMatrix);
MMOBASE_API void glhLookAtf2(sreal *matrix, sreal *eyePosition3D, sreal *center3D, sreal *upVector3D);
MMOBASE_API void glhPerspectivef2(sreal *matrix, sreal fovyInDegrees, sreal aspectRatio, sreal znear, sreal zfar);
MMOBASE_API void glhOrthof2(sreal *matrix, sreal left, sreal right, sreal bottom, sreal top, sreal znear, sreal zfar);

MMOBASE_API void calculate_view_matrix(float* v, float* vInv, float camyaw, float campitch, float camdist, const float* camtrackplayertrans);
