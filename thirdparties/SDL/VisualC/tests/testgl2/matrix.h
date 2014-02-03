#pragma once

typedef float sreal;

void glhInit();
void glhLoadIdentityf2(sreal *matrix);
extern void glhTranslatef2_SSE_Aligned(sreal *matrix, sreal *xyzw);
extern void glhScalef2_SSE_Aligned(sreal *matrix, sreal *xyzw);
void glhRotatef2_SSE_Aligned(sreal *matrix, sreal angleInRadians, sreal *xyzw);
void glhMultMatrixf2_SSE_Aligned(sreal *result_leftMatrix, sreal *rightMatrix);
void glhMultMatrixSwappedf2_SSE_Aligned(sreal *result_rightMatrix, sreal *leftMatrix);
void glhLookAtf2(sreal *matrix, sreal *eyePosition3D, sreal *center3D, sreal *upVector3D);
void glhPerspectivef2(sreal *matrix, sreal fovyInDegrees, sreal aspectRatio, sreal znear, sreal zfar);
void glhOrthof2(sreal *matrix, sreal left, sreal right, sreal bottom, sreal top, sreal znear, sreal zfar);
