
struct Wave
{
	void* mp3Buffer;
	int mp3BufLen;

	Uint32 length;
	Uint8* buffer;
	Uint32 cursor;
};

void InitializeLame();
void DecodeMp3();
void DecodeMp3(const char* mp3FileName, char* wavFileName, size_t wavFileNameLen);
void ShutdownLame();
