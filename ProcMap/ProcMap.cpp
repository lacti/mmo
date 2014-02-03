// ProcMap.cpp : Defines the exported functions for the DLL application.
//
#include "ProcMap.h"
#include <math.h>
#include <string.h>
#include <stdlib.h>

#ifndef M_PI
#define M_PI		3.1415926535897932384626433832795
#endif

#pragma warning(disable: 4244)	//Shut up about double to float casting
#pragma warning(disable: 4305)	//'=' : truncation from 'const double' to 'float'

sint GlobalRandom[2];

//PURPOSE:	Linear interpolation
inline sreal LinearInterpolation(sreal a, sreal b, sreal weightOfb)
{
	//return  a*(1.0-weightOfb)+b*weightOfb;
	return a+weightOfb*(b-a);	//Better!
}

//PURPOSE:	Unlike linear interpolation, this gives a smoother interpolation
inline sreal CosineInterpolation(sreal a, sreal b, sreal weightOfb)
{
	//f=(1.0-cosf(weightOfb*M_PI))*0.5;
	//return  a*(1.0-f)+b*f;
	//return a+f*(b-a);	//Better!
	return a+((1.0-cosf(weightOfb*M_PI))*0.5)*(b-a);	//Better!
}


/* coherent noise function over 1, 2 or 3 dimensions */
/* (copyright Ken Perlin) */

//Source : http://mrl.nyu.edu/~perlin/doc/oscar.html


#define B 0x100				//256
#define B2 0x200			//512
#define BM 0xff				//255


#define N 0x1000			//4096
#define Nfloat	4096.0


#define NP 12   /* 2^N */
#define NM 0xfff			//4095

sint p[B+B+2];
sreal g3[B+B+2][3];		//Filled with random values for 3D Perlin Noise
sreal g2[B+B+2][2];		//Filled with random values for 2D Perlin Noise
sreal g1[B+B+2];		//Filled with random values for 1D Perlin Noise

sint AllOnesArray[4]={1, 1, 1, 1};
sreal AllOnesfloatArray[4]={1.0, 1.0, 1.0, 1.0};

sreal AllTwosfloatArray[4]={2.0, 2.0, 2.0, 2.0};
sreal AllThreesfloatArray[4]={3.0, 3.0, 3.0, 3.0};

#define s_curve(t) (t*t*(3.0-2.0*t))


#define setup(i, b0, b1, r0, r1)\
	t = i + N;\
	b0 = ((sint)t) & BM;\
	b1 = (b0+1) & BM;\
	r0 = t - (sint)t;\
	r1 = r0 - 1.;

inline void NormalizeVector2DFLOAT_2(sreal *pvector)
{
	sreal normalizingConstant;
	normalizingConstant=1.0/sqrtf(pvector[0]*pvector[0]+pvector[1]*pvector[1]);
	pvector[0]*=normalizingConstant;
	pvector[1]*=normalizingConstant;
}

inline void NormalizeVectorFLOAT_2(sreal *pvector)
{
	sreal normalizingConstant;
	normalizingConstant=1.0/sqrtf(pvector[0]*pvector[0]+pvector[1]*pvector[1]+pvector[2]*pvector[2]);
	pvector[0]*=normalizingConstant;
	pvector[1]*=normalizingConstant;
	pvector[2]*=normalizingConstant;
}

void InitPerlinNoise()
{
	sint i, j, k;

	for(i=0; i<B; i++)		//B=256
	{
		p[i]=i;

		g1[i]=(sreal)((rand() % B2)-B)/B;

		g2[i][0]=(sreal)((rand() % B2)-B)/B;
		//g2[i][0]=(sreal)((k++ % B2)-B)/B;
		g2[i][1]=(sreal)((rand() % B2)-B)/B;
		//g2[i][1]=(sreal)((k++ % B2)-B)/B;
		NormalizeVector2DFLOAT_2(g2[i]);

		g3[i][0]=(sreal)((rand() % B2)-B)/B;		//B=256, B2=512
		g3[i][1]=(sreal)((rand() % B2)-B)/B;
		g3[i][2]=(sreal)((rand() % B2)-B)/B;
		NormalizeVectorFLOAT_2(g3[i]);
	}

	while(i--)		//Some kind of random swapping in p array
	{				//Perlin is a terrible coder
		k=p[i];
		j=rand() % B;
		p[i]=p[j];
		p[j]=k;
	}

	for(i=0; i<B+2; i++)
	{
		p[B+i]=p[i];		//Making a copy?
		g1[B+i]=g1[i];

		g2[B+i][0]=g2[i][0];
		g2[B+i][1]=g2[i][1];

		g3[B+i][0]=g3[i][0];
		g3[B+i][1]=g3[i][1];
		g3[B+i][2]=g3[i][2];
	}
}

//PURPOSE:	glh stores a table of random number dedicated to the perlin noise
//			generator. If you want new numbers in the table, call this.
void glhPerlinRegenerateTable()
{
	InitPerlinNoise();
}

//PURPOSE:	Used by glhPerlinNoise1D
inline sreal InterpolatedNoise1D_Linear(sreal x)
{
	sint bx0, bx1;
	sreal rx0, rx1, sx, t, u, v;

	//setup(x, bx0, bx1, rx0, rx1);
	t=x+N;
	bx0=((sint)t) & BM;
	bx1=(bx0+1) & BM;
	rx0=t-(sint)t;
	rx1=rx0-1.0;

	sx=s_curve(rx0);

	u=rx0*g1[p[bx0]];
	v=rx1*g1[p[bx1]];

	return LinearInterpolation(u, v, sx);
}

//PURPOSE:	Called from SmoothedNoise1_b
inline sreal Noise1(sint x)
{
    x=(x<<13)^x;
	return (1.0-((x*(x*x*15731+789221)+1376312589) & 0x7FFFFFFF)/1073741824.0);
}

//PURPOSE:	Called from InterpolatedNoise1D_Linear_b, InterpolatedNoise1D_Cos_b
inline sreal SmoothNoise1_b(sreal x)
{
	return Noise1((sint)x)/2.0+Noise1((sint)x-1)/4.0+Noise1((sint)x+1)/4.0;
}

//PURPOSE:	Used by glhPerlinNoise1D when mode==GLH_MODE_ALTERNATE2
inline sreal InterpolatedNoise1D_Linear_b(sreal x)
{
	sint integer_X=(sint)x;
	sreal fractional_X=x-(sreal)integer_X;

	sreal v1=SmoothNoise1_b(integer_X);
	sreal v2=SmoothNoise1_b(integer_X+1);

	return LinearInterpolation(v1, v2, fractional_X);
}

//PURPOSE:	Used by glhPerlinNoise1D when mode==GLH_MODE_ALTERNATE2
inline sreal InterpolatedNoise1D_Cos_b(sreal x)
{
	sint integer_X=(sint)x;
	sreal fractional_X=x-(sreal)integer_X;

	sreal v1=SmoothNoise1_b(integer_X);
	sreal v2=SmoothNoise1_b(integer_X+1);

	return CosineInterpolation(v1, v2, fractional_X);
}

//PURPOSE:	Used by glhPerlinNoise1D
inline sreal InterpolatedNoise1D_Cos(sreal x)
{
	sint bx0, bx1;
	sreal rx0, rx1, sx, t, u, v;

	//setup(x, bx0, bx1, rx0, rx1);
	t=x+N;
	bx0=((sint)t) & BM;
	bx1=(bx0+1) & BM;
	rx0=t-(sint)t;
	rx1=rx0-1.0;

	sx=s_curve(rx0);

	u=rx0*g1[p[bx0]];
	v=rx1*g1[p[bx1]];

	return CosineInterpolation(u, v, sx);
}

//PURPOSE:	Used by PerlinNoise2D
inline sreal InterpolatedNoise2D_Linear(sreal x, sreal y)
{
	/*//OLD CODE - I will rearrange some of the lines so that
	//it will be easier to make MMX, SSE version
	sint bx0, bx1, by0, by1, b00, b10, b01, b11;
	sreal rx0, rx1, ry0, ry1, *q, sx, sy, a, b, t, u, v;
	sint i, j;

	//setup(x, bx0, bx1, rx0, rx1);
	t=x+N;
	bx0=((sint)t) & BM;
	bx1=(bx0+1) & BM;
	rx0=t-(sint)t;
	rx1=rx0-1.0;
	//setup(y, by0, by1, ry0, ry1);
	t=y+N;
	by0=((sint)t) & BM;
	by1=(by0+1) & BM;
	ry0=t-(sint)t;
	ry1=ry0-1.0;

	i=p[bx0];
	j=p[bx1];

	b00=p[i+by0];
	b10=p[j+by0];
	b01=p[i+by1];
	b11=p[j+by1];

	sx=s_curve(rx0);
	sy=s_curve(ry0);

#define at2(rx, ry) (rx*q[0]+ry*q[1])

	q=g2[b00];
	u=at2(rx0, ry0);
	q=g2[b10];
	v=at2(rx1, ry0);
	a=LinearInterpolation(u, v, sx);

	q=g2[b01];
	u=at2(rx0, ry1);
	q=g2[b11];
	v=at2(rx1, ry1);
	b=LinearInterpolation(u, v, sx);

	return LinearInterpolation(a, b, sy);*/

	sint bx0, bx1, by0, by1, b00, b10, b01, b11;
	sreal rx0, rx1, ry0, ry1, *q, sx, sy, a, b, t1, t2, u, v;
	sint i, j;
	sint it1, it2;
	sreal fit1, fit2;

	t1=x+N;
	t2=y+N;
	it1=(sint)t1;
	it2=(sint)t2;
	fit1=(sreal)it1;
	fit2=(sreal)it2;
	
	bx0=it1 & BM;
	bx1=(it1+1) & BM;
	by0=it2 & BM;
	by1=(it2+1) & BM;

	rx0=t1-fit1;
	rx1=rx0-1.0;
		
	ry0=t2-fit2;
	ry1=ry0-1.0;

	sx=rx0*rx0*(3.0-2.0*rx0);
	sy=ry0*ry0*(3.0-2.0*ry0);

#define at2(rx, ry) (rx*q[0]+ry*q[1])

	i=p[bx0];
	j=p[bx1];

	b00=p[i+by0];
	b10=p[j+by0];
	b01=p[i+by1];
	b11=p[j+by1];

	q=g2[b00];
	u=at2(rx0, ry0);
	q=g2[b10];
	v=at2(rx1, ry0);
	a=LinearInterpolation(u, v, sx);

	q=g2[b01];
	u=at2(rx0, ry1);
	q=g2[b11];
	v=at2(rx1, ry1);
	b=LinearInterpolation(u, v, sx);

	sreal z=LinearInterpolation(a, b, sy);
	return z;
}


//Make 16 byte (128 bit) aligned for SSE, requires processor pack for VC++

#ifdef WIN32
#define PM_ALIGN16 __declspec(align(16))
#else
#define PM_ALIGN16
#endif

PM_ALIGN16	sint bx0[4], bx1[4], by0[4], by1[4], b00[4], b10[4], b01[4], b11[4];
PM_ALIGN16	sreal rx0[4], rx1[4], ry0[4], ry1[4], sx[4], sy[4], a[4], b[4], t1[4], t2[4], u[4], v[4];
PM_ALIGN16	sint i[4], j[4];

PM_ALIGN16	sint it1[4], it2[4];
PM_ALIGN16	sreal fit1[4], fit2[4];

PM_ALIGN16	sint temp[36];		//4*4
PM_ALIGN16	sreal ftemp[8];		//4*2


PM_ALIGN16	sreal NfloatArray[4]={4096.0, 4096.0, 4096.0, 4096.0};		//For MMX, SSE
PM_ALIGN16	sint BMArray[4]={BM, BM, BM, BM};

//PURPOSE:	Called from SmoothNoise2_b
inline sreal Noise2(sint x, sint y)
{
    sint n=x+y*57;
	n=(n<<13)^n;
	return (1.0-((n*(n*n*15731+789221)+1376312589) & 0x7FFFFFFF)/1073741824.0);
}

//PURPOSE:	Called from InterpolatedNoise2D_Linear_b, InterpolatedNoise2D_Cos_b
inline sreal SmoothNoise2_b(sreal x, sreal y)
{
	//Do a 3x3 filter kernel
	// 4 corners = 1/16
	// 4 side = 1/8
	// 1 center = 1/4
	// total = 4/16 + 8/16 + 4/16 = 16/16 = 1
	sreal corners=(Noise2(x-1, y-1)+Noise2(x+1, y-1)+Noise2(x-1, y+1)+Noise2(x+1, y+1))/16.0;
	sreal sides=(Noise2(x-1, y)+Noise2(x+1, y)+Noise2(x, y-1)+Noise2(x, y+1))/8.0;
	sreal center=Noise2(x, y)/4.0;
	return corners+sides+center;
}

//PURPOSE:	Used by PerlinNoise2D
inline sreal InterpolatedNoise2D_Linear_b(sreal x, sreal y)
{
	sint integer_X=(sint)x;
	sreal fractional_X=x-(sreal)integer_X;

	sint integer_Y=(sint)y;
	sreal fractional_Y=y-(sreal)integer_Y;

	sreal v1=SmoothNoise2_b(integer_X, integer_Y);
	sreal v2=SmoothNoise2_b(integer_X+1, integer_Y);
	sreal v3=SmoothNoise2_b(integer_X, integer_Y+1);
	sreal v4=SmoothNoise2_b(integer_X+1, integer_Y+1);

	sreal i1=LinearInterpolation(v1, v2, fractional_X);
	sreal i2=LinearInterpolation(v3, v4, fractional_X);

	return LinearInterpolation(i1, i2, fractional_Y);
}

//PURPOSE:	Used by PerlinNoise2D
inline sreal InterpolatedNoise2D_Cos(sreal x, sreal y)
{
	sint bx0, bx1, by0, by1, b00, b10, b01, b11;
	sreal rx0, rx1, ry0, ry1, *q, sx, sy, a, b, t, u, v;
	sint i, j;

	//setup(x, bx0, bx1, rx0, rx1);
	t=x+N;
	bx0=((sint)t) & BM;
	bx1=(bx0+1) & BM;
	rx0=t-(sint)t;
	rx1=rx0-1.0;
	//setup(y, by0, by1, ry0, ry1);
	t=y+N;
	by0=((sint)t) & BM;
	by1=(by0+1) & BM;
	ry0=t-(sint)t;
	ry1=ry0-1.0;

	i=p[bx0];
	j=p[bx1];

	b00=p[i+by0];
	b10=p[j+by0];
	b01=p[i+by1];
	b11=p[j+by1];

	sx=s_curve(rx0);
	sy=s_curve(ry0);

#define at2(rx, ry) (rx*q[0]+ry*q[1])

	q=g2[b00];
	u=at2(rx0, ry0);
	q=g2[b10];
	v=at2(rx1, ry0);
	a=CosineInterpolation(u, v, sx);

	q=g2[b01];
	u=at2(rx0, ry1);
	q=g2[b11];
	v=at2(rx1, ry1);
	b=CosineInterpolation(u, v, sx);

	return CosineInterpolation(a, b, sy);
}

//PURPOSE:	Used by PerlinNoise2D
inline sreal InterpolatedNoise2D_Cos_b(sreal x, sreal y)
{
	sint integer_X=(sint)x;
	sreal fractional_X=x-(sreal)integer_X;

	sint integer_Y=(sint)y;
	sreal fractional_Y=y-(sreal)integer_Y;

	sreal v1=SmoothNoise2_b(integer_X, integer_Y);
	sreal v2=SmoothNoise2_b(integer_X+1, integer_Y);
	sreal v3=SmoothNoise2_b(integer_X, integer_Y+1);
	sreal v4=SmoothNoise2_b(integer_X+1, integer_Y+1);

	sreal i1=CosineInterpolation(v1, v2, fractional_X);
	sreal i2=CosineInterpolation(v3, v4, fractional_X);

	return CosineInterpolation(i1, i2, fractional_Y);
}


//PURPOSE:	Used by PerlinNoise3D
inline sreal InterpolatedNoise3D_Linear(sreal x, sreal y, sreal z)
{
	sint bx0, bx1, by0, by1, bz0, bz1, b00, b10, b01, b11;
	sreal rx0, rx1, ry0, ry1, rz0, rz1, *q, sy, sz, a, b, c, d, t, u, v;
	sint i, j;

	//setup(x, bx0, bx1, rx0, rx1);
	t=x+N;
	bx0=((sint)t) & BM;
	bx1=(bx0+1) & BM;
	rx0=t-(sint)t;
	rx1=rx0-1.0;
	//setup(y, by0, by1, ry0, ry1);
	t=y+N;
	by0=((sint)t) & BM;
	by1=(by0+1) & BM;
	ry0=t-(sint)t;
	ry1=ry0-1.0;
	//setup(z, bz0, bz1, rz0, rz1);
	t=z+N;
	bz0=((sint)t) & BM;
	bz1=(bz0+1) & BM;
	rz0=t-(sint)t;
	rz1=rz0-1.0;

	i=p[bx0];
	j=p[bx1];

	b00=p[i+by0];
	b10=p[j+by0];
	b01=p[i+by1];
	b11=p[j+by1];

	t=s_curve(rx0);
	sy=s_curve(ry0);
	sz=s_curve(rz0);

#define at3(rx, ry, rz) (rx*q[0]+ry*q[1]+rz*q[2])

	q=g3[b00+bz0];
	u=at3(rx0, ry0, rz0);
	q=g3[b10+bz0];
	v=at3(rx1, ry0, rz0);
	a=LinearInterpolation(u, v, t);

	q=g3[b01+bz0];
	u=at3(rx0,ry1,rz0);
	q=g3[b11+bz0];
	v=at3(rx1, ry1, rz0);
	b=LinearInterpolation(u, v, t);

	c=LinearInterpolation(a, b, sy);

	q=g3[b00+bz1];
	u=at3(rx0, ry0, rz1);
	q=g3[b10+bz1];
	v=at3(rx1, ry0, rz1);
	a=LinearInterpolation(u, v, t);

	q=g3[b01+bz1];
	u=at3(rx0, ry1, rz1);
	q=g3[b11+bz1];
	v=at3(rx1, ry1, rz1);
	b=LinearInterpolation(u, v, t);

	d=LinearInterpolation(a, b, sy);

	return LinearInterpolation(c, d, sz);
}

//PURPOSE:	Used by PerlinNoise3D
inline sreal InterpolatedNoise3D_Cos(sreal x, sreal y, sreal z)
{
	sint bx0, bx1, by0, by1, bz0, bz1, b00, b10, b01, b11;
	sreal rx0, rx1, ry0, ry1, rz0, rz1, *q, sy, sz, a, b, c, d, t, u, v;
	sint i, j;

	//setup(x, bx0, bx1, rx0, rx1);
	t=x+N;
	bx0=((sint)t) & BM;
	bx1=(bx0+1) & BM;
	rx0=t-(sint)t;
	rx1=rx0-1.0;
	//setup(y, by0, by1, ry0, ry1);
	t=y+N;
	by0=((sint)t) & BM;
	by1=(by0+1) & BM;
	ry0=t-(sint)t;
	ry1=ry0-1.0;
	//setup(z, bz0, bz1, rz0, rz1);
	t=z+N;
	bz0=((sint)t) & BM;
	bz1=(bz0+1) & BM;
	rz0=t-(sint)t;
	rz1=rz0-1.0;

	i=p[bx0];
	j=p[bx1];

	b00=p[i+by0];
	b10=p[j+by0];
	b01=p[i+by1];
	b11=p[j+by1];

	t=s_curve(rx0);
	sy=s_curve(ry0);
	sz=s_curve(rz0);

#define at3(rx, ry, rz) (rx*q[0]+ry*q[1]+rz*q[2])

	q=g3[b00+bz0];
	u=at3(rx0, ry0, rz0);
	q=g3[b10+bz0];
	v=at3(rx1, ry0, rz0);
	a=CosineInterpolation(u, v, t);

	q=g3[b01+bz0];
	u=at3(rx0,ry1,rz0);
	q=g3[b11+bz0];
	v=at3(rx1, ry1, rz0);
	b=CosineInterpolation(u, v, t);

	c=CosineInterpolation(a, b, sy);

	q=g3[b00+bz1];
	u=at3(rx0, ry0, rz1);
	q=g3[b10+bz1];
	v=at3(rx1, ry0, rz1);
	a=CosineInterpolation(u, v, t);

	q=g3[b01+bz1];
	u=at3(rx0, ry1, rz1);
	q=g3[b11+bz1];
	v=at3(rx1, ry1, rz1);
	b=CosineInterpolation(u, v, t);

	d=CosineInterpolation(a, b, sy);

	return CosineInterpolation(c, d, sz);
}

//PURPOSE:	Called from SmoothNoise3_b
inline sreal Noise3(sint x, sint y, sint z)
{
    sint n=x+y*57+z*57;
	n=(n<<13)^n;
	return (1.0-((n*(n*n*15731+789221)+1376312589) & 0x7FFFFFFF)/1073741824.0);
}

//PURPOSE:	Called from InterpolatedNoise3D_Linear_b, InterpolatedNoise3D_Cos_b
inline sreal SmoothNoise3_b(sreal x, sreal y, sreal z)
{
	//Do a 3x3x3 filter kernel
	// 8 corners = 1/43 = 0.023255....
	// 12 far sides = 1/64 = 0.1875
	// 6 center sides = 1/16 = 0.375
	// 1 center = 1/4 = 0.25
	// total = 
	sreal corners=(Noise3(x-1, y-1, z-1)+Noise3(x+1, y-1, z-1)+Noise3(x-1, y+1, z-1)+Noise3(x+1, y+1, z-1)+
		Noise3(x-1, y-1, z+1)+Noise3(x+1, y-1, z+1)+Noise3(x-1, y+1, z+1)+Noise3(x+1, y+1, z+1))/43.0;
	
	sreal farsides=(Noise3(x-1, y, z-1)+Noise3(x+1, y, z-1)+Noise3(x, y-1, z-1)+Noise3(x, y+1, z-1)+
		Noise3(x-1, y, z+1)+Noise3(x+1, y, z+1)+Noise3(x, y-1, z+1)+Noise3(x, y+1, z+1)+
		Noise3(x-1, y, z)+Noise3(x+1, y, z)+Noise3(x, y-1, z)+Noise3(x, y+1, z))/64.0;

	sreal centersides=(Noise3(x, y, z-1)+Noise3(x, y, z+1)+Noise3(x, y-1, z)+Noise3(x, y+1, z)+
		Noise3(x-1, y, z)+Noise3(x+1, y, z))/16.0;
	sreal center=Noise3(x, y, z)/4.0;
	return corners+farsides+centersides+center;
}

//PURPOSE:	Used by PerlinNoise3D
inline sreal InterpolatedNoise3D_Linear_b(sreal x, sreal y, sreal z)
{
	sint integer_X=(sint)x;
	sreal fractional_X=x-(sreal)integer_X;

	sint integer_Y=(sint)y;
	sreal fractional_Y=y-(sreal)integer_Y;

	sint integer_Z=(sint)z;
	sreal fractional_Z=z-(sreal)integer_Z;

	sreal v1=SmoothNoise3_b(integer_X, integer_Y, integer_Z);
	sreal v2=SmoothNoise3_b(integer_X+1, integer_Y, integer_Z);
	sreal v3=SmoothNoise3_b(integer_X, integer_Y+1, integer_Z);
	sreal v4=SmoothNoise3_b(integer_X+1, integer_Y+1, integer_Z);
	sreal v5=SmoothNoise3_b(integer_X, integer_Y, integer_Z+1);
	sreal v6=SmoothNoise3_b(integer_X+1, integer_Y, integer_Z+1);
	sreal v7=SmoothNoise3_b(integer_X, integer_Y+1, integer_Z+1);
	sreal v8=SmoothNoise3_b(integer_X+1, integer_Y+1, integer_Z+1);

	sreal i1=LinearInterpolation(v1, v2, fractional_X);
	sreal i2=LinearInterpolation(v3, v4, fractional_X);
	sreal i3=LinearInterpolation(v5, v6, fractional_X);
	sreal i4=LinearInterpolation(v7, v8, fractional_X);
	sreal i5=LinearInterpolation(i1, i2, fractional_Y);
	sreal i6=LinearInterpolation(i3, i4, fractional_Y);

	return LinearInterpolation(i5, i6, fractional_Z);
}

//PURPOSE:	Used by PerlinNoise3D
inline sreal InterpolatedNoise3D_Cos_b(sreal x, sreal y, sreal z)
{
	sint integer_X=(sint)x;
	sreal fractional_X=x-(sreal)integer_X;

	sint integer_Y=(sint)y;
	sreal fractional_Y=y-(sreal)integer_Y;

	sint integer_Z=(sint)z;
	sreal fractional_Z=z-(sreal)integer_Z;

	sreal v1=SmoothNoise3_b(integer_X, integer_Y, integer_Z);
	sreal v2=SmoothNoise3_b(integer_X+1, integer_Y, integer_Z);
	sreal v3=SmoothNoise3_b(integer_X, integer_Y+1, integer_Z);
	sreal v4=SmoothNoise3_b(integer_X+1, integer_Y+1, integer_Z);
	sreal v5=SmoothNoise3_b(integer_X, integer_Y, integer_Z+1);
	sreal v6=SmoothNoise3_b(integer_X+1, integer_Y, integer_Z+1);
	sreal v7=SmoothNoise3_b(integer_X, integer_Y+1, integer_Z+1);
	sreal v8=SmoothNoise3_b(integer_X+1, integer_Y+1, integer_Z+1);

	sreal i1=CosineInterpolation(v1, v2, fractional_X);
	sreal i2=CosineInterpolation(v3, v4, fractional_X);
	sreal i3=CosineInterpolation(v5, v6, fractional_X);
	sreal i4=CosineInterpolation(v7, v8, fractional_X);
	sreal i5=CosineInterpolation(i1, i2, fractional_Y);
	sreal i6=CosineInterpolation(i3, i4, fractional_Y);

	return CosineInterpolation(i5, i6, fractional_Z);
}


//PURPOSE:	Generate a 1D noise function, based on persistence which
//			is defined as
//			frequency = 2^i
//			amplitude = persistence^i
//			in other words ...
//			ln(amplitude) = i*ln(persistence)
//			ln(amplitude)/ln(persistence) = i
//			frequency = 2^ln(amplitude)/ln(persistence)  <----
//
//			The numberOfOctaves which controls how many frequencies to sum up
//			boolOctave is an array the size of numberOfOctaves, with GL_TRUE and GL_FALSE
//			to control which ones to sum up.
//			interpolationQuality should be GLH_INTERPOLATIONQUALITY_LINEAR for low quality
//				GLH_INTERPOLATIONQUALITY_COS for high
//			mode can be GLH_MODE_STANDARD, GLH_MODE_ALTERNATE, GLH_MODE_ALTERNATE2, GLH_MODE_ALTERNATE3, GLH_MODE_ALTERNATE4
//
//			posx is 1 sreal that describes a position in 1D
//
//			The result is a single sreal and is written to *result.
//
//			Returns 1 for sucess, -1 if bad parameter passed		
sint glhPerlinNoise1D(sreal *result, sint interpolationQuality, sint mode, sint optimize, sreal posx, sreal persistence, sint numberOfOctaves, sint *boolOctave)
{
	sreal total=0.0, frequency, amplitude;
	sint i;

	if(interpolationQuality==GLH_INTERPOLATIONQUALITY_LINEAR)
	{
		if(mode==GLH_MODE_STANDARD)
		{
			//if(optimize==GLH_OPTIMIZE_NONE)
			{
				for(i=0; i<numberOfOctaves; i++)
				{	//Sum up the signals, based on sin function
					if(boolOctave[i])
					{
						frequency=powf(2.0, (sreal)i);
						amplitude=powf(persistence, (sreal)i);
					
						total+=amplitude*InterpolatedNoise1D_Linear(posx*frequency);
					}
				}
			}
			//else
			//	return -1;
		}
		else if(mode==GLH_MODE_ALTERNATE)
		{
			for(i=0; i<numberOfOctaves; i++)
			{	//Sum up the signals, based on sin function
					if(boolOctave[i])
				{
					frequency=powf(2.0, (sreal)i);

					total+=InterpolatedNoise1D_Linear(posx*frequency)/frequency;
				}
			}
		}
		else if(mode==GLH_MODE_ALTERNATE2)
		{
			for(i=0; i<numberOfOctaves; i++)
			{	//Sum up the signals, based on sin function
				if(boolOctave[i])
				{
					frequency=powf(2.0, (sreal)i);
					amplitude=powf(persistence, (sreal)i);
										

					total+=amplitude*InterpolatedNoise1D_Linear_b(posx*frequency);
				}
			}
		}
		else if(mode==GLH_MODE_ALTERNATE3)
		{
			for(i=0; i<numberOfOctaves; i++)
			{	//Sum up the signals, based on sin function
				if(boolOctave[i])
				{
					frequency=powf(2.0, (sreal)i);
					
					total+=fabsf(InterpolatedNoise1D_Linear(posx*frequency)/frequency);
				}
			}
		}
		else if(mode==GLH_MODE_ALTERNATE4)
		{
			for(i=0; i<numberOfOctaves; i++)
			{	//Sum up the signals, based on sin function
				if(boolOctave[i])
				{
					frequency=powf(2.0, (sreal)i);

					total+=fabsf(InterpolatedNoise1D_Linear_b(posx*frequency)/frequency);
				}
			}
		}
		else
			return -1;

		/*
		while(total<0.0)
		{
			total+=1.0;
		}
		while(total>1.0)
		{
			total-=1.0;
		}
		*/

		*result=total;
		return 1;
	}
	else if(interpolationQuality==GLH_INTERPOLATIONQUALITY_COS)
	{
		if(mode==GLH_MODE_STANDARD)
		{
			for(i=0; i<numberOfOctaves; i++)
			{	//Sum up the signals, based on sin function
				if(boolOctave[i])
				{
					frequency=powf(2.0, (sreal)i);
					amplitude=powf(persistence, (sreal)i);
					
					

					total+=amplitude*InterpolatedNoise1D_Cos(posx*frequency);
				}
			}
		}
		else if(mode==GLH_MODE_ALTERNATE)
		{
			for(i=0; i<numberOfOctaves; i++)
			{	//Sum up the signals, based on sin function
				if(boolOctave[i])
				{
					frequency=powf(2.0, (sreal)i);

					total+=InterpolatedNoise1D_Cos(posx*frequency)/frequency;
				}
			}
		}
		else if(mode==GLH_MODE_ALTERNATE2)
		{
			for(i=0; i<numberOfOctaves; i++)
			{	//Sum up the signals, based on sin function
				if(boolOctave[i])
				{
					frequency=powf(2.0, (sreal)i);
					amplitude=powf(persistence, (sreal)i);
										

					total+=amplitude*InterpolatedNoise1D_Cos_b(posx*frequency);
				}
			}
		}
		else if(mode==GLH_MODE_ALTERNATE3)
		{
			for(i=0; i<numberOfOctaves; i++)
			{	//Sum up the signals, based on sin function
				if(boolOctave[i])
				{
					frequency=powf(2.0, (sreal)i);
					
					total+=fabsf(InterpolatedNoise1D_Cos(posx*frequency)/frequency);
				}
			}
		}
		else if(mode==GLH_MODE_ALTERNATE4)
		{
			for(i=0; i<numberOfOctaves; i++)
			{	//Sum up the signals, based on sin function
				if(boolOctave[i])
				{
					frequency=powf(2.0, (sreal)i);

					total+=fabsf(InterpolatedNoise1D_Cos_b(posx*frequency)/frequency);
				}
			}
		}
		else
			return -1;

		while(total<0.0)
		{
			total+=1.0;
		}
		while(total>1.0)
		{
			total-=1.0;
		}
		*result=total;
		return 1;
	}
	else
		return -1;
}


//PURPOSE:	Generate a 2D noise function, based on persistence which
//			is defined as
//			frequency = 2^i
//			amplitude = persistence^i
//			in other words ...
//			ln(amplitude) = i*ln(persistence)
//			ln(amplitude)/ln(persistence) = i
//			frequency = 2^ln(amplitude)/ln(persistence)  <----
//
//			The numberOfOctaves which controls how many frequencies to sum up
//			boolOctave is an array the size of numberOfOctaves, with GL_TRUE and GL_FALSE
//			to control which ones to sum up.
//			interpolationQuality should be GLH_INTERPOLATIONQUALITY_LINEAR for low quality
//				GLH_INTERPOLATIONQUALITY_COS for high
//			mode can be GLH_MODE_STANDARD, GLH_MODE_ALTERNATE, GLH_MODE_ALTERNATE2
//
//			pos is 2 floats that describe a position in 2D
//
//			The result is a single sreal and is written to *result.
//
//			Returns 1 for sucess, -1 if bad parameter passed
sint glhPerlinNoise2D(sreal *result, sint interpolationQuality, sint mode, sint optimize, sreal *pos, sreal persistence, sint numberOfOctaves, sint *boolOctave)
{
	sreal total[4], frequency, amplitude;
	sint i;

	if(interpolationQuality==GLH_INTERPOLATIONQUALITY_LINEAR)
	{
		if(mode==GLH_MODE_STANDARD)
		{
			if(optimize==GLH_OPTIMIZE_NONE)
			{
				total[0]=0.0;
				for(i=0; i<numberOfOctaves; i++)
				{	//Sum up the signals, based on sin function
					if(boolOctave[i])
					{
						frequency=powf(2.0, (sreal)i);
						amplitude=powf(persistence, (sreal)i);
					
						total[0]+=amplitude*InterpolatedNoise2D_Linear(pos[0]*frequency, pos[1]*frequency);
					}
				}

				*result=total[0];
			}
			else
				return -1;
		}
		else if(mode==GLH_MODE_ALTERNATE)
		{
			total[0]=0.0;
			for(i=0; i<numberOfOctaves; i++)
			{	//Sum up the signals, based on sin function
				if(boolOctave[i])
				{
					frequency=powf(2.0, (sreal)i);
				
					total[0]+=InterpolatedNoise2D_Linear(pos[0]*frequency, pos[1]*frequency)/frequency;
				}
			}

			*result=total[0];
		}
		else if(mode==GLH_MODE_ALTERNATE2)
		{
			total[0]=0.0;
			for(i=0; i<numberOfOctaves; i++)
			{	//Sum up the signals, based on sin function
				if(boolOctave[i])
				{
					frequency=powf(2.0, (sreal)i);
					amplitude=powf(persistence, (sreal)i);
								
					total[0]+=amplitude*InterpolatedNoise2D_Linear_b(pos[0]*frequency, pos[1]*frequency);
				}
			}

			*result=total[0];
		}
		else if(mode==GLH_MODE_ALTERNATE3)
		{
			total[0]=0.0;
			for(i=0; i<numberOfOctaves; i++)
			{	//Sum up the signals, based on sin function
				if(boolOctave[i])
				{
					frequency=powf(2.0, (sreal)i);
					
					total[0]+=fabsf(InterpolatedNoise2D_Linear(pos[0]*frequency, pos[1]*frequency)/frequency);
				}
			}

			*result=total[0];
		}
		else if(mode==GLH_MODE_ALTERNATE4)
		{
			total[0]=0.0;
			for(i=0; i<numberOfOctaves; i++)
			{	//Sum up the signals, based on sin function
				if(boolOctave[i])
				{
					frequency=powf(2.0, (sreal)i);
				
					total[0]+=fabsf(InterpolatedNoise2D_Linear_b(pos[0]*frequency, pos[1]*frequency)/frequency);
				}
			}

			*result=total[0];
		}
		else
			return -1;

		return 1;
	}
	else if(interpolationQuality==GLH_INTERPOLATIONQUALITY_COS)
	{
		if(mode==GLH_MODE_STANDARD)
		{
			total[0]=0.0;
			for(i=0; i<numberOfOctaves; i++)
			{	//Sum up the signals, based on sin function
				if(boolOctave[i])
				{
					frequency=powf(2.0, (sreal)i);
					amplitude=powf(persistence, (sreal)i);
										

					total[0]+=amplitude*InterpolatedNoise2D_Cos(pos[0]*frequency, pos[1]*frequency);
				}
			}

			*result=total[0];
		}
		else if(mode==GLH_MODE_ALTERNATE)
		{
			total[0]=0.0;
			for(i=0; i<numberOfOctaves; i++)
			{	//Sum up the signals, based on sin function
				if(boolOctave[i])
				{
					frequency=powf(2.0, (sreal)i);
			
					total[0]+=InterpolatedNoise2D_Cos(pos[0]*frequency, pos[1]*frequency)/frequency;
				}
			}

			*result=total[0];
		}
		else if(mode==GLH_MODE_ALTERNATE2)
		{
			total[0]=0.0;
			for(i=0; i<numberOfOctaves; i++)
			{	//Sum up the signals, based on sin function
				if(boolOctave[i])
				{
					frequency=powf(2.0, (sreal)i);
					amplitude=powf(persistence, (sreal)i);
									
					total[0]+=amplitude*InterpolatedNoise2D_Cos_b(pos[0]*frequency, pos[1]*frequency);
				}
			}

			*result=total[0];
		}
		else if(mode==GLH_MODE_ALTERNATE3)
		{
			total[0]=0.0;
			for(i=0; i<numberOfOctaves; i++)
			{	//Sum up the signals, based on sin function
				if(boolOctave[i])
				{
					frequency=powf(2.0, (sreal)i);
					
					total[0]+=fabsf(InterpolatedNoise2D_Cos(pos[0]*frequency, pos[1]*frequency)/frequency);
				}
			}

			*result=total[0];
		}
		else if(mode==GLH_MODE_ALTERNATE4)
		{
			total[0]=0.0;
			for(i=0; i<numberOfOctaves; i++)
			{	//Sum up the signals, based on sin function
				if(boolOctave[i])
				{
					frequency=powf(2.0, (sreal)i);
				
					total[0]+=fabsf(InterpolatedNoise2D_Cos_b(pos[0]*frequency, pos[1]*frequency)/frequency);
				}
			}

			*result=total[0];
		}
		else
			return -1;

		return 1;
	}
	else
		return -1;
}



//PURPOSE:	Generate a 3D noise function, based on persistence which
//			is defined as
//			frequency = 2^i
//			amplitude = persistence^i
//			in other words ...
//			ln(amplitude) = i*ln(persistence)
//			ln(amplitude)/ln(persistence) = i
//			frequency = 2^ln(amplitude)/ln(persistence)  <----
//
//			The numberOfOctaves which controls how many frequencies to sum up
//			boolOctave is an array the size of numberOfOctaves, with GL_TRUE and GL_FALSE
//			to control which ones to sum up.
//			interpolationQuality should be GLH_INTERPOLATIONQUALITY_LINEAR for low quality
//				GLH_INTERPOLATIONQUALITY_COS for high
//			mode can be GLH_MODE_STANDARD, GLH_MODE_ALTERNATE, GLH_MODE_ALTERNATE2
//
//			pos is 3 floats that describe a position in 3D
//
//			The result is a single sreal and is written to *result.
//
//			Returns 1 for sucess, -1 if bad parameter passed
sint glhPerlinNoise3D(sreal *result, sint interpolationQuality, sint mode, sint optimize, sreal *pos, sreal persistence, sint numberOfOctaves, sint *boolOctave)
{
	sreal total=0.0, frequency, amplitude;
	sint i;

	if(interpolationQuality==GLH_INTERPOLATIONQUALITY_LINEAR)
	{
		if(mode==GLH_MODE_STANDARD)
		{
			for(i=0; i<numberOfOctaves; i++)
			{	//Sum up the signals, based on sin function
				if(boolOctave[i])
				{
					frequency=powf(2.0, (sreal)i);
					amplitude=powf(persistence, (sreal)i);
										

					total+=amplitude*InterpolatedNoise3D_Linear(pos[0]*frequency, pos[1]*frequency, pos[2]*frequency);
				}
			}
		}
		else if(mode==GLH_MODE_ALTERNATE)
		{
			for(i=0; i<numberOfOctaves; i++)
			{	//Sum up the signals, based on sin function
				if(boolOctave[i])
				{
					frequency=powf(2.0, (sreal)i);
				
					total+=InterpolatedNoise3D_Linear(pos[0]*frequency, pos[1]*frequency, pos[2]*frequency)/frequency;
				}
			}
		}
		else if(mode==GLH_MODE_ALTERNATE2)
		{
			for(i=0; i<numberOfOctaves; i++)
			{	//Sum up the signals, based on sin function
				if(boolOctave[i])
				{
					frequency=powf(2.0, (sreal)i);
					amplitude=powf(persistence, (sreal)i);
									
					total+=amplitude*InterpolatedNoise3D_Linear_b(pos[0]*frequency, pos[1]*frequency, pos[2]*frequency);
				}
			}
		}
		else if(mode==GLH_MODE_ALTERNATE3)
		{
			for(i=0; i<numberOfOctaves; i++)
			{	//Sum up the signals, based on sin function
				if(boolOctave[i])
				{
					frequency=powf(2.0, (sreal)i);
					
					total+=fabsf(InterpolatedNoise3D_Linear(pos[0]*frequency, pos[1]*frequency, pos[2]*frequency)/frequency);
				}
			}
		}
		else if(mode==GLH_MODE_ALTERNATE4)
		{
			for(i=0; i<numberOfOctaves; i++)
			{	//Sum up the signals, based on sin function
				if(boolOctave[i])
				{
					frequency=powf(2.0, (sreal)i);
				
					total+=fabsf(InterpolatedNoise3D_Linear_b(pos[0]*frequency, pos[1]*frequency, pos[2]*frequency)/frequency);
				}
			}
		}
		else
			return -1;

		while(total<0.0)
		{
			total+=1.0;
		}
		while(total>1.0)
		{
			total-=1.0;
		}
		*result=total;
		return 1;
	}
	else if(interpolationQuality==GLH_INTERPOLATIONQUALITY_COS)
	{
		if(mode==GLH_MODE_STANDARD)
		{
			for(i=0; i<numberOfOctaves; i++)
			{	//Sum up the signals, based on sin function
				if(boolOctave[i])
				{
					frequency=powf(2.0, (sreal)i);
					amplitude=powf(persistence, (sreal)i);
										

					total+=amplitude*InterpolatedNoise3D_Cos(pos[0]*frequency, pos[1]*frequency, pos[2]*frequency);
				}
			}
		}
		else if(mode==GLH_MODE_ALTERNATE)
		{
			for(i=0; i<numberOfOctaves; i++)
			{	//Sum up the signals, based on sin function
				if(boolOctave[i])
				{
					frequency=powf(2.0, (sreal)i);
			
					total+=InterpolatedNoise3D_Cos(pos[0]*frequency, pos[1]*frequency, pos[2]*frequency)/frequency;
				}
			}
		}
		else if(mode==GLH_MODE_ALTERNATE2)
		{
			for(i=0; i<numberOfOctaves; i++)
			{	//Sum up the signals, based on sin function
				if(boolOctave[i])
				{
					frequency=powf(2.0, (sreal)i);
					amplitude=powf(persistence, (sreal)i);
									
					total+=amplitude*InterpolatedNoise3D_Cos_b(pos[0]*frequency, pos[1]*frequency, pos[2]*frequency);
				}
			}
		}
		else if(mode==GLH_MODE_ALTERNATE3)
		{
			for(i=0; i<numberOfOctaves; i++)
			{	//Sum up the signals, based on sin function
				if(boolOctave[i])
				{
					frequency=powf(2.0, (sreal)i);
					
					total+=fabsf(InterpolatedNoise3D_Cos(pos[0]*frequency, pos[1]*frequency, pos[2]*frequency)/frequency);
				}
			}
		}
		else if(mode==GLH_MODE_ALTERNATE4)
		{
			for(i=0; i<numberOfOctaves; i++)
			{	//Sum up the signals, based on sin function
				if(boolOctave[i])
				{
					frequency=powf(2.0, (sreal)i);
				
					total+=fabsf(InterpolatedNoise3D_Cos_b(pos[0]*frequency, pos[1]*frequency, pos[2]*frequency)/frequency);
				}
			}
		}
		else
			return -1;

		while(total<0.0)
		{
			total+=1.0;
		}
		while(total>1.0)
		{
			total-=1.0;
		}
		*result=total;
		return 1;
	}
	else
		return -1;
}

void glhFastFresnelTerm(sreal *result, sreal *cosAngle, sreal refractionIndex, sint count)
{
	//This approximates the fresnel term with the following equation :
	//R0 = pow(1 - refractionIndex, 2) / pow(1+refractionIndex, 2)
	//
	//and
	//return R0 + (1-R0) * pow(1-dot(light, normal), 5);

	sreal R0=powf(1.0-refractionIndex, 2.0)/powf(1.0+refractionIndex, 2.0);
	sreal R1=1.0-R0;

	sint i;
	for(i=0; i<count; i++)
	{
		if(cosAngle[i]<0.0)
			result[i]=0.0;
		else
		{
			result[i]=R0+R1*powf(1.0-cosAngle[i], 5.0);
		}
	}
}

static float SCALE_FACTOR_X = 100.0f;
static float SCALE_FACTOR_Y = 100.0f;

void WINSTDCALL InitializeProcMap()
{
    glhPerlinRegenerateTable();
}

float WINSTDCALL QueryProcMapHeight(float x, float y)
{
    static sint OCTAVEARRAY[] = { 1, 1, 1, 1, 1 };
    static const float persistence = 0.8f;
    static const float heightScale = 5.0f;

	sreal h;

    float pos[2] = { x / SCALE_FACTOR_X, y / SCALE_FACTOR_Y };

	int r = glhPerlinNoise2D(
		&h,
		GLH_INTERPOLATIONQUALITY_LINEAR,
		GLH_MODE_STANDARD,
		GLH_OPTIMIZE_NONE,
		pos,
		persistence,
		sizeof(OCTAVEARRAY) / sizeof(OCTAVEARRAY[0]),
		OCTAVEARRAY
		);

    h = 0;
    return h * 5.0f;
}

void SetProcMapScaleFactor(float scalex, float scaley)
{
    SCALE_FACTOR_X = scalex;
    SCALE_FACTOR_Y = scaley;
}

float QueryProcMapScaleFactorX()
{
    return SCALE_FACTOR_X;
}

float QueryProcMapScaleFactorY()
{
    return SCALE_FACTOR_Y;
}
