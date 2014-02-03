#include "stdafx.h"
#include "basicmath.h"

// x보다 크거나 같은 수 중 가장 가까운 2의 제곱수 구하기
unsigned int nlpo2(unsigned int x)
{
	x |= (x >> 1);
	x |= (x >> 2);
	x |= (x >> 4);
	x |= (x >> 8);
	x |= (x >> 16);
	return x+1;
}

float clampf(float v, float lo, float hi)
{
	return v < lo ? lo : (v > hi ? hi : v);
}
