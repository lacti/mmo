#include <memory.h>
#include <stdlib.h>
#include <math.h>
#include "matrix.h"

#ifndef M_PI
#define M_PI		3.1415926535897932384626433832795
#endif

sreal* FArray128;								//Reserved 128 sreal for SSE calculations
int MemoryAlignmentOnAllocation;				//Default : 16. For SSE (memory align)

static sreal GlobalIdentityMatrixf[]=
	{1.0, 0.0, 0.0, 0.0,
	0.0, 1.0, 0.0, 0.0,
	0.0, 0.0, 1.0, 0.0,
	0.0, 0.0, 0.0, 1.0};

void glhInit()
{
	MemoryAlignmentOnAllocation=16;

	FArray128=(sreal *)_aligned_malloc(sizeof(sreal)*128, MemoryAlignmentOnAllocation);
}

void glhRotatef2_SSE_Aligned(sreal *matrix, sreal angleInRadians, sreal *xyzw)
{
	//See page 191 of Elementary Linear Algebra by Howard Anton
	sreal OneMinusCosAngle, CosAngle, SinAngle;
	sreal A_OneMinusCosAngle, C_OneMinusCosAngle;
	CosAngle=cosf(angleInRadians);			//Some stuff for optimizing code
	OneMinusCosAngle=1.0f-CosAngle;
	SinAngle=sinf(angleInRadians);
	A_OneMinusCosAngle=xyzw[0]*OneMinusCosAngle;
	C_OneMinusCosAngle=xyzw[2]*OneMinusCosAngle;
	//Make a copy
	FArray128[0]=matrix[0];
	FArray128[1]=matrix[1];
	FArray128[2]=matrix[2];
	FArray128[3]=matrix[3];
	FArray128[4]=matrix[4];
	FArray128[5]=matrix[5];
	FArray128[6]=matrix[6];
	FArray128[7]=matrix[7];
	FArray128[8]=matrix[8];
	FArray128[9]=matrix[9];
	FArray128[10]=matrix[10];
	FArray128[11]=matrix[11];
	FArray128[12]=matrix[12];
	FArray128[13]=matrix[13];
	FArray128[14]=matrix[14];
	FArray128[15]=matrix[15];

	//Matrix rotation matrix
	FArray128[16]=xyzw[0]*A_OneMinusCosAngle+CosAngle;
	FArray128[17]=xyzw[1]*A_OneMinusCosAngle+xyzw[2]*SinAngle;
	FArray128[18]=xyzw[2]*A_OneMinusCosAngle-xyzw[1]*SinAngle;
	FArray128[19]=0.0;

	FArray128[20]=xyzw[1]*A_OneMinusCosAngle-xyzw[2]*SinAngle;
	FArray128[21]=xyzw[1]*xyzw[1]*OneMinusCosAngle+CosAngle;
	FArray128[22]=xyzw[1]*C_OneMinusCosAngle+xyzw[0]*SinAngle;
	FArray128[23]=0.0;

	FArray128[24]=xyzw[0]*C_OneMinusCosAngle+xyzw[1]*SinAngle;
	FArray128[25]=xyzw[1]*C_OneMinusCosAngle-xyzw[0]*SinAngle;
	FArray128[26]=xyzw[2]*C_OneMinusCosAngle+CosAngle;
	FArray128[27]=0.0;
	//The last column of rotate[] is {0 0 0 1}
	FArray128[28]=FArray128[29]=FArray128[30]=0.0;
	FArray128[31]=1.0;

	//We can do the matrix multiplication with SSE
	__asm
	{
		mov		eax, FArray128

		//Cache to L1
		prefetchnta	[eax]						; cache the left matrix second column now
		prefetchnta	[eax+16]					; cache the left matrix second column now
		prefetchnta	[eax+32]					; cache the left matrix second column now
		prefetchnta	[eax+48]					; cache the left matrix second column now

		mov		edx, matrix

		//Cache to L1
		prefetchnta [eax+80]					; cache the right matrix second column now

		movaps	xmm0, [eax]						; aligned move, get first column
		movaps	xmm5, [eax+64]					; aligned move, get first column
		movaps	xmm4, xmm5
		shufps	xmm4, xmm4, 0					; first 32 bit copied to all 32 bits
		mulps	xmm0, xmm4
		movaps	xmm1, [eax+16]					; aligned move, get second column
		movaps	xmm4, xmm5
		shufps	xmm4, xmm4, 0x55				; second 32 bit copied to all 32 bits
		mulps	xmm1, xmm4
		movaps	xmm2, [eax+32]					; aligned move, get third column
		movaps	xmm4, xmm5
		shufps	xmm4, xmm4, 0xAA				; third 32 bit copied to all 32 bits
		mulps	xmm2, xmm4
		movaps	xmm3, [eax+48]					; aligned move, get fourth column
		shufps	xmm5, xmm5, 0xFF				; fourth 32 bit copied to all 32 bits
		mulps	xmm3, xmm5
		addps	xmm0, xmm1						; now add them all up to make the result
		addps	xmm0, xmm2
		addps	xmm0, xmm3
		movaps	[edx], xmm0

		//Cache to L1
		prefetchnta [eax+96]					; cache the right matrix second column now

		movaps	xmm0, [eax]						; aligned move, get first column
		movaps	xmm5, [eax+80]					; aligned move, get second column
		movaps	xmm4, xmm5
		shufps	xmm4, xmm4, 0					; first 32 bit copied to all 32 bits
		mulps	xmm0, xmm4
		movaps	xmm1, [eax+16]					; aligned move, get second column
		movaps	xmm4, xmm5
		shufps	xmm4, xmm4, 0x55				; second 32 bit copied to all 32 bits
		mulps	xmm1, xmm4
		movaps	xmm2, [eax+32]					; aligned move, get third column
		movaps	xmm4, xmm5
		shufps	xmm4, xmm4, 0xAA				; third 32 bit copied to all 32 bits
		mulps	xmm2, xmm4
		movaps	xmm3, [eax+48]					; aligned move, get fourth column
		shufps	xmm5, xmm5, 0xFF				; fourth 32 bit copied to all 32 bits
		mulps	xmm3, xmm5
		addps	xmm0, xmm1						; now add them all up to make the result
		addps	xmm0, xmm2
		addps	xmm0, xmm3
		movaps	[edx+16], xmm0

		//Cache to L1
		prefetchnta [eax+112]					; cache the right matrix second column now

		movaps	xmm0, [eax]						; aligned move, get first column
		movaps	xmm5, [eax+96]					; aligned move, get third column
		movaps	xmm4, xmm5
		shufps	xmm4, xmm4, 0					; first 32 bit copied to all 32 bits
		mulps	xmm0, xmm4
		movaps	xmm1, [eax+16]					; aligned move, get second column
		movaps	xmm4, xmm5
		shufps	xmm4, xmm4, 0x55				; second 32 bit copied to all 32 bits
		mulps	xmm1, xmm4
		movaps	xmm2, [eax+32]					; aligned move, get third column
		movaps	xmm4, xmm5
		shufps	xmm4, xmm4, 0xAA				; third 32 bit copied to all 32 bits
		mulps	xmm2, xmm4
		movaps	xmm3, [eax+48]					; aligned move, get fourth column
		shufps	xmm5, xmm5, 0xFF				; fourth 32 bit copied to all 32 bits
		mulps	xmm3, xmm5
		addps	xmm0, xmm1						; now add them all up to make the result
		addps	xmm0, xmm2
		addps	xmm0, xmm3
		movaps	[edx+32], xmm0

		movaps	xmm0, [eax]						; aligned move, get first column
		movaps	xmm5, [eax+112]					; aligned move, get fourth column
		movaps	xmm4, xmm5
		shufps	xmm4, xmm4, 0					; first 32 bit copied to all 32 bits
		mulps	xmm0, xmm4
		movaps	xmm1, [eax+16]					; aligned move, get second column
		movaps	xmm4, xmm5
		shufps	xmm4, xmm4, 0x55				; second 32 bit copied to all 32 bits
		mulps	xmm1, xmm4
		movaps	xmm2, [eax+32]					; aligned move, get third column
		movaps	xmm4, xmm5
		shufps	xmm4, xmm4, 0xAA				; third 32 bit copied to all 32 bits
		mulps	xmm2, xmm4
		movaps	xmm3, [eax+48]					; aligned move, get fourth column
		shufps	xmm5, xmm5, 0xFF				; fourth 32 bit copied to all 32 bits
		mulps	xmm3, xmm5
		addps	xmm0, xmm1						; now add them all up to make the result
		addps	xmm0, xmm2
		addps	xmm0, xmm3
		movaps	[edx+48], xmm0
	}
}


void glhMultMatrixf2_SSE_Aligned(sreal *result_leftMatrix, sreal *rightMatrix)
{
	//result_leftMatrix = result_leftMatrix * rightMatrix
	memcpy(FArray128, result_leftMatrix, 16*sizeof(sreal));

	//We can do the matrix multiplication with SSE
	__asm
	{
		mov		eax, FArray128

		//Cache to L1
		prefetchnta	[eax]						; cache the left matrix second column now
		prefetchnta	[eax+16]					; cache the left matrix second column now
		prefetchnta	[eax+32]					; cache the left matrix second column now
		prefetchnta	[eax+48]					; cache the left matrix second column now

		mov		edx, result_leftMatrix
		mov		ecx, rightMatrix

		//Cache to L1
		prefetchnta [ecx+16]					; cache the right matrix second column now

		movaps	xmm0, [eax]						; aligned move, get first column
		movaps	xmm5, [ecx]						; aligned move, get first column
		movaps	xmm4, xmm5
		shufps	xmm4, xmm4, 0					; first 32 bit copied to all 32 bits
		mulps	xmm0, xmm4
		movaps	xmm1, [eax+16]					; aligned move, get second column
		movaps	xmm4, xmm5
		shufps	xmm4, xmm4, 0x55				; second 32 bit copied to all 32 bits
		mulps	xmm1, xmm4
		movaps	xmm2, [eax+32]					; aligned move, get third column
		movaps	xmm4, xmm5
		shufps	xmm4, xmm4, 0xAA				; third 32 bit copied to all 32 bits
		mulps	xmm2, xmm4
		movaps	xmm3, [eax+48]					; aligned move, get fourth column
		shufps	xmm5, xmm5, 0xFF				; fourth 32 bit copied to all 32 bits
		mulps	xmm3, xmm5
		addps	xmm0, xmm1						; now add them all up to make the result
		addps	xmm0, xmm2
		addps	xmm0, xmm3
		movaps	[edx], xmm0

		//Cache to L1
		prefetchnta [ecx+32]					; cache the right matrix second column now

		movaps	xmm0, [eax]						; aligned move, get first column
		movaps	xmm5, [ecx+16]					; aligned move, get second column
		movaps	xmm4, xmm5
		shufps	xmm4, xmm4, 0					; first 32 bit copied to all 32 bits
		mulps	xmm0, xmm4
		movaps	xmm1, [eax+16]					; aligned move, get second column
		movaps	xmm4, xmm5
		shufps	xmm4, xmm4, 0x55				; second 32 bit copied to all 32 bits
		mulps	xmm1, xmm4
		movaps	xmm2, [eax+32]					; aligned move, get third column
		movaps	xmm4, xmm5
		shufps	xmm4, xmm4, 0xAA				; third 32 bit copied to all 32 bits
		mulps	xmm2, xmm4
		movaps	xmm3, [eax+48]					; aligned move, get fourth column
		shufps	xmm5, xmm5, 0xFF				; fourth 32 bit copied to all 32 bits
		mulps	xmm3, xmm5
		addps	xmm0, xmm1						; now add them all up to make the result
		addps	xmm0, xmm2
		addps	xmm0, xmm3
		movaps	[edx+16], xmm0

		//Cache to L1
		prefetchnta [ecx+48]					; cache the right matrix second column now

		movaps	xmm0, [eax]						; aligned move, get first column
		movaps	xmm5, [ecx+32]					; aligned move, get third column
		movaps	xmm4, xmm5
		shufps	xmm4, xmm4, 0					; first 32 bit copied to all 32 bits
		mulps	xmm0, xmm4
		movaps	xmm1, [eax+16]					; aligned move, get second column
		movaps	xmm4, xmm5
		shufps	xmm4, xmm4, 0x55				; second 32 bit copied to all 32 bits
		mulps	xmm1, xmm4
		movaps	xmm2, [eax+32]					; aligned move, get third column
		movaps	xmm4, xmm5
		shufps	xmm4, xmm4, 0xAA				; third 32 bit copied to all 32 bits
		mulps	xmm2, xmm4
		movaps	xmm3, [eax+48]					; aligned move, get fourth column
		shufps	xmm5, xmm5, 0xFF				; fourth 32 bit copied to all 32 bits
		mulps	xmm3, xmm5
		addps	xmm0, xmm1						; now add them all up to make the result
		addps	xmm0, xmm2
		addps	xmm0, xmm3
		movaps	[edx+32], xmm0

		movaps	xmm0, [eax]						; aligned move, get first column
		movaps	xmm5, [ecx+48]					; aligned move, get fourth column
		movaps	xmm4, xmm5
		shufps	xmm4, xmm4, 0					; first 32 bit copied to all 32 bits
		mulps	xmm0, xmm4
		movaps	xmm1, [eax+16]					; aligned move, get second column
		movaps	xmm4, xmm5
		shufps	xmm4, xmm4, 0x55				; second 32 bit copied to all 32 bits
		mulps	xmm1, xmm4
		movaps	xmm2, [eax+32]					; aligned move, get third column
		movaps	xmm4, xmm5
		shufps	xmm4, xmm4, 0xAA				; third 32 bit copied to all 32 bits
		mulps	xmm2, xmm4
		movaps	xmm3, [eax+48]					; aligned move, get fourth column
		shufps	xmm5, xmm5, 0xFF				; fourth 32 bit copied to all 32 bits
		mulps	xmm3, xmm5
		addps	xmm0, xmm1						; now add them all up to make the result
		addps	xmm0, xmm2
		addps	xmm0, xmm3
		movaps	[edx+48], xmm0
	}
}


void glhMultMatrixSwappedf2_SSE_Aligned(sreal *result_rightMatrix, sreal *leftMatrix)
{
	//result_rightMatrix = leftMatrix * result_rightMatrix
	memcpy(FArray128, result_rightMatrix, 16*sizeof(sreal));

	//We can do the matrix multiplication with SSE
	__asm
	{
		mov		eax, leftMatrix

		//Cache to L1
		prefetchnta	[eax]						; cache the left matrix second column now
		prefetchnta	[eax+16]					; cache the left matrix second column now
		prefetchnta	[eax+32]					; cache the left matrix second column now
		prefetchnta	[eax+48]					; cache the left matrix second column now

		mov		edx, result_rightMatrix
		mov		ecx, FArray128

		//Cache to L1
		prefetchnta [ecx+16]					; cache the right matrix second column now

		movaps	xmm0, [eax]						; aligned move, get first column
		movaps	xmm5, [ecx]						; aligned move, get first column
		movaps	xmm4, xmm5
		shufps	xmm4, xmm4, 0					; first 32 bit copied to all 32 bits
		mulps	xmm0, xmm4
		movaps	xmm1, [eax+16]					; aligned move, get second column
		movaps	xmm4, xmm5
		shufps	xmm4, xmm4, 0x55				; second 32 bit copied to all 32 bits
		mulps	xmm1, xmm4
		movaps	xmm2, [eax+32]					; aligned move, get third column
		movaps	xmm4, xmm5
		shufps	xmm4, xmm4, 0xAA				; third 32 bit copied to all 32 bits
		mulps	xmm2, xmm4
		movaps	xmm3, [eax+48]					; aligned move, get fourth column
		shufps	xmm5, xmm5, 0xFF				; fourth 32 bit copied to all 32 bits
		mulps	xmm3, xmm5
		addps	xmm0, xmm1						; now add them all up to make the result
		addps	xmm0, xmm2
		addps	xmm0, xmm3
		movaps	[edx], xmm0

		//Cache to L1
		prefetchnta [ecx+32]					; cache the right matrix second column now

		movaps	xmm0, [eax]						; aligned move, get first column
		movaps	xmm5, [ecx+16]					; aligned move, get second column
		movaps	xmm4, xmm5
		shufps	xmm4, xmm4, 0					; first 32 bit copied to all 32 bits
		mulps	xmm0, xmm4
		movaps	xmm1, [eax+16]					; aligned move, get second column
		movaps	xmm4, xmm5
		shufps	xmm4, xmm4, 0x55				; second 32 bit copied to all 32 bits
		mulps	xmm1, xmm4
		movaps	xmm2, [eax+32]					; aligned move, get third column
		movaps	xmm4, xmm5
		shufps	xmm4, xmm4, 0xAA				; third 32 bit copied to all 32 bits
		mulps	xmm2, xmm4
		movaps	xmm3, [eax+48]					; aligned move, get fourth column
		shufps	xmm5, xmm5, 0xFF				; fourth 32 bit copied to all 32 bits
		mulps	xmm3, xmm5
		addps	xmm0, xmm1						; now add them all up to make the result
		addps	xmm0, xmm2
		addps	xmm0, xmm3
		movaps	[edx+16], xmm0

		//Cache to L1
		prefetchnta [ecx+48]					; cache the right matrix second column now

		movaps	xmm0, [eax]						; aligned move, get first column
		movaps	xmm5, [ecx+32]					; aligned move, get third column
		movaps	xmm4, xmm5
		shufps	xmm4, xmm4, 0					; first 32 bit copied to all 32 bits
		mulps	xmm0, xmm4
		movaps	xmm1, [eax+16]					; aligned move, get second column
		movaps	xmm4, xmm5
		shufps	xmm4, xmm4, 0x55				; second 32 bit copied to all 32 bits
		mulps	xmm1, xmm4
		movaps	xmm2, [eax+32]					; aligned move, get third column
		movaps	xmm4, xmm5
		shufps	xmm4, xmm4, 0xAA				; third 32 bit copied to all 32 bits
		mulps	xmm2, xmm4
		movaps	xmm3, [eax+48]					; aligned move, get fourth column
		shufps	xmm5, xmm5, 0xFF				; fourth 32 bit copied to all 32 bits
		mulps	xmm3, xmm5
		addps	xmm0, xmm1						; now add them all up to make the result
		addps	xmm0, xmm2
		addps	xmm0, xmm3
		movaps	[edx+32], xmm0

		movaps	xmm0, [eax]						; aligned move, get first column
		movaps	xmm5, [ecx+48]					; aligned move, get fourth column
		movaps	xmm4, xmm5
		shufps	xmm4, xmm4, 0					; first 32 bit copied to all 32 bits
		mulps	xmm0, xmm4
		movaps	xmm1, [eax+16]					; aligned move, get second column
		movaps	xmm4, xmm5
		shufps	xmm4, xmm4, 0x55				; second 32 bit copied to all 32 bits
		mulps	xmm1, xmm4
		movaps	xmm2, [eax+32]					; aligned move, get third column
		movaps	xmm4, xmm5
		shufps	xmm4, xmm4, 0xAA				; third 32 bit copied to all 32 bits
		mulps	xmm2, xmm4
		movaps	xmm3, [eax+48]					; aligned move, get fourth column
		shufps	xmm5, xmm5, 0xFF				; fourth 32 bit copied to all 32 bits
		mulps	xmm3, xmm5
		addps	xmm0, xmm1						; now add them all up to make the result
		addps	xmm0, xmm2
		addps	xmm0, xmm3
		movaps	[edx+48], xmm0
	}
}

void glhLoadIdentityf2(sreal *matrix)
{
	memcpy(matrix, GlobalIdentityMatrixf, sizeof(sreal)*16);
}

void NormalizeVectorFLOAT_2(sreal *pvector)
{
	sreal normalizingConstant;
	normalizingConstant=1.0f/sqrtf(pvector[0]*pvector[0]+pvector[1]*pvector[1]+pvector[2]*pvector[2]);
	pvector[0]*=normalizingConstant;
	pvector[1]*=normalizingConstant;
	pvector[2]*=normalizingConstant;
}

//2 pvectors are passed and normal is computed
void ComputeNormalOfPlaneFLOAT_2(sreal *normal, const sreal *pvector1, const sreal *pvector2)
{
	normal[0]=(pvector1[1]*pvector2[2])-(pvector1[2]*pvector2[1]);
	normal[1]=(pvector1[2]*pvector2[0])-(pvector1[0]*pvector2[2]);
	normal[2]=(pvector1[0]*pvector2[1])-(pvector1[1]*pvector2[0]);
}

void glhLookAtf2(sreal *matrix, sreal *eyePosition3D, sreal *center3D, sreal *upVector3D)
{
	static __declspec(align(32)) sreal matrix2[16], resultMatrix[16];
	__declspec(align(32)) float negPosition3D[] = { -eyePosition3D[0], -eyePosition3D[1], -eyePosition3D[2] };
	sreal forward[3], side[3], up[3];

	forward[0]=center3D[0]-eyePosition3D[0];
	forward[1]=center3D[1]-eyePosition3D[1];
	forward[2]=center3D[2]-eyePosition3D[2];
	NormalizeVectorFLOAT_2(forward);

	//Side = forward x up
	ComputeNormalOfPlaneFLOAT_2(side, forward, upVector3D);
	NormalizeVectorFLOAT_2(side);

	//Recompute up as: up = side x forward
	ComputeNormalOfPlaneFLOAT_2(up, side, forward);

	matrix2[0]=side[0];
	matrix2[4]=side[1];
	matrix2[8]=side[2];
	matrix2[12]=0.0;

	matrix2[1]=up[0];
	matrix2[5]=up[1];
	matrix2[9]=up[2];
	matrix2[13]=0.0;

	matrix2[2]=-forward[0];
	matrix2[6]=-forward[1];
	matrix2[10]=-forward[2];
	matrix2[14]=0.0;

	matrix2[3]=matrix2[7]=matrix2[11]=0.0;
	matrix2[15]=1.0;

	//MultiplyMatrices4by4OpenGL_FLOAT(resultMatrix, matrix, matrix2);
	glhMultMatrixf2_SSE_Aligned(matrix, matrix2);

	
	glhTranslatef2_SSE_Aligned(matrix, negPosition3D);
	
	//memcpy(matrix, resultMatrix, 16*sizeof(sreal));
}

void glhFrustumf2(sreal *matrix, sreal left, sreal right, sreal bottom, sreal top, sreal znear, sreal zfar)
{
	static __declspec(align(32)) sreal matrix2[16], temp, temp2, temp3, temp4, resultMatrix[16];
	temp=2.0f*znear;
	temp2=right-left;
	temp3=top-bottom;
	temp4=zfar-znear;
	matrix2[0]=temp/temp2;
	matrix2[1]=0.0;
	matrix2[2]=0.0;
	matrix2[3]=0.0;
	matrix2[4]=0.0;
	matrix2[5]=temp/temp3;
	matrix2[6]=0.0;
	matrix2[7]=0.0;
	matrix2[8]=(right+left)/temp2;
	matrix2[9]=(top+bottom)/temp3;
	matrix2[10]=(-zfar-znear)/temp4;
	matrix2[11]=-1.0;
	matrix2[12]=0.0;
	matrix2[13]=0.0;
	matrix2[14]=(-temp*zfar)/temp4;
	matrix2[15]=0.0;

	glhMultMatrixf2_SSE_Aligned(matrix, matrix2);

	//memcpy(matrix, resultMatrix, 16*sizeof(sreal));
}

void glhPerspectivef2(sreal *matrix, sreal fovyInDegrees, sreal aspectRatio, sreal znear, sreal zfar)
{
	sreal ymax, xmax;
	ymax=znear*tanf(fovyInDegrees*M_PI/360.0);
	//ymin=-ymax;
	//xmin=-ymax*aspectRatio;
	xmax=ymax*aspectRatio;
	glhFrustumf2(matrix, -xmax, xmax, -ymax, ymax, znear, zfar);
}

void glhOrthof2(sreal *matrix, sreal left, sreal right, sreal bottom, sreal top, sreal znear, sreal zfar)
{
	sreal temp2, temp3, temp4;
	temp2=right-left;
	temp3=top-bottom;
	temp4=zfar-znear;
	matrix[0]=2.0f/temp2;
	matrix[1]=0.0;
	matrix[2]=0.0;
	matrix[3]=0.0;
	matrix[4]=0.0;
	matrix[5]=2.0f/temp3;
	matrix[6]=0.0;
	matrix[7]=0.0;
	matrix[8]=0.0;
	matrix[9]=0.0;
	matrix[10]=-2.0f/temp4;
	matrix[11]=0.0;
	matrix[12]=(-right-left)/temp2;
	matrix[13]=(-top-bottom)/temp3;
	matrix[14]=(-zfar-znear)/temp4;
	matrix[15]=1.0f;
}
