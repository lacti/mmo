#include "stdafx.h"
#include "audio.h"
#include "mp3.h"

Uint32 wav_length;
Uint8* wav_buffer;
Uint32 wav_cursor = 0;

static tbb::concurrent_queue<Wave> gWaveQueue;
HANDLE gQueuePushedEvent;

void SDLCALL WavCallback(void *userdata, Uint8 * stream, int len)
{
    int validLen = len;

    if (wav_cursor + len > wav_length )
    {
        validLen = wav_length - wav_cursor;
    }

    if (validLen > 0)
    {
        memcpy(stream, wav_buffer + wav_cursor, validLen);

        wav_cursor += validLen;

        if (len - validLen > 0)
        {
            memset(stream + validLen, 0, len - validLen);
        }
    }
    else if (validLen == 0)
    {   
        memset(stream, 0, len);
    }
}

void PlayMp3(void* buffer, int buflen)
{
    void* bufferCopy = (void*)malloc(buflen);
    memcpy(bufferCopy, buffer, buflen);

    Wave w;
    w.mp3Buffer = bufferCopy;
    w.mp3BufLen = buflen;

    gWaveQueue.push(w);
    SetEvent(gQueuePushedEvent);
}

void PlaySampleWavFile()
{
    char resPath[1024];
    GetMmoResourcePath(resPath, 1024, "smiling.wav");

    SDL_AudioSpec wav_spec, wav_spec_obtained;
    if (SDL_LoadWAV(resPath, &wav_spec, &wav_buffer, &wav_length) == NULL)
    {
        cl_debug_output("wav file open\n");
    }

    wav_spec.callback = WavCallback;

    if (SDL_OpenAudio(&wav_spec, &wav_spec_obtained) < 0)
    {
    	cl_debug_output("open audio error\n");
    }

    SDL_PauseAudio(0);
}

int SDLCALL AudioThread(void *data)
{	
    InitializeLame();

    //PlaySampleWavFile();

    gQueuePushedEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

    while (true)
    {
        if (WaitForSingleObject(gQueuePushedEvent, INFINITE) == WAIT_OBJECT_0)
        {
            Wave w;
            if (gWaveQueue.try_pop(w))
            {
                char filename[1024], outFileName[1024];
                static int seq;
                StringCbPrintfA(filename, sizeof(filename), "temp%d.mp3", seq++);
                FILE* fw = fopen(filename, "wb");
                fwrite(w.mp3Buffer, w.mp3BufLen, 1, fw);

                DecodeMp3(filename, outFileName, sizeof(outFileName));

                SDL_PauseAudio(1);

                SDL_AudioSpec wav_spec, wav_spec_obtained;
                if (SDL_LoadWAV(outFileName, &wav_spec, &wav_buffer, &wav_length) == NULL)
                {
                    cl_debug_output("wav file open\n");
                }

                wav_spec.callback = WavCallback;

                wav_cursor = 0;

                SDL_CloseAudio();

                if (SDL_OpenAudio(&wav_spec, &wav_spec_obtained) < 0)
                {
                    cl_debug_output("open audio error\n");
                }

                SDL_PauseAudio(0);

                free(w.mp3Buffer);
            }
        }
    }

    SDL_FreeWAV(wav_buffer);

    ShutdownLame();

    return 0;
}
