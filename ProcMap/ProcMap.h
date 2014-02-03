typedef int sint;
typedef float sreal;

#define GLH_MODE_NONE						0x01000000
#define GLH_MODE_STANDARD					0x0100000F
#define GLH_MODE_ALTERNATE					0x010000F0
#define GLH_MODE_ALTERNATE2					0x010000F1
#define GLH_MODE_ALTERNATE3					0x010000F2
#define GLH_MODE_ALTERNATE4					0x010000F3
#define GLH_INTERPOLATIONQUALITY_LINEAR		0xF0000001
#define GLH_INTERPOLATIONQUALITY_COS		0xF0000002

#define GLH_OPTIMIZE_NONE					0x00100000
#define GLH_OPTIMIZE_MMX_SSE				0x0010000F

void glhPerlinRegenerateTable();
sint glhPerlinNoise1D(sreal *result, sint interpolationQuality, sint mode, sint optimize, sreal posx, sreal persistence, sint numberOfOctaves, sint *boolOctave);
sint glhPerlinNoise2D(sreal *result, sint interpolationQuality, sint mode, sint optimize, sreal *pos, sreal persistence, sint numberOfOctaves, sint *boolOctave);

float QueryProcMapScaleFactorX();
float QueryProcMapScaleFactorY();

#ifdef WIN32
#define WINSTDCALL __stdcall
#else
#define WINSTDCALL
#endif

// Server쪽에서 P/Invoke 이용해서 호출하는 함수
void WINSTDCALL InitializeProcMap();
float WINSTDCALL QueryProcMapHeight(float x, float y);
