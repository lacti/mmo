#pragma once

typedef struct KeyControlVector
{
	unsigned char left:1;
	unsigned char right:1;
	unsigned char up:1;
	unsigned char down:1;
	unsigned char jump:1;
} KeyControlVector;
