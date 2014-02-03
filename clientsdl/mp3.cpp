#include "stdafx.h"
#include "mp3.h"

lame_t gFp;

void InitializeLame()
{
	gFp = lame_init();
}

void DecodeMp3()
{
	char source[1024];
	GetMmoResourcePath(source, 1024, "whi.mp3");

	char *argv[4] = {
		"lame",
		"--decode",
		source,
		"whi.wav"
	};

	lame_main_ex(gFp, 4, argv);
}

void DecodeMp3(const char* mp3FileName, char* wavFileName, size_t wavFileNameLen)
{
	StringCbCopyA(wavFileName, wavFileNameLen, mp3FileName);
	StringCbCatA(wavFileName, wavFileNameLen, ".wav");

	char source[1024];
	StringCbCopyA(source, sizeof(source), mp3FileName);

	char *argv[4] = {
		"lame",
		"--decode",
		source,
		wavFileName
	};

	lame_main_ex(gFp, 4, argv);
}

void ShutdownLame()
{
	lame_close(gFp);
}
