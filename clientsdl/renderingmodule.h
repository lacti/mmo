#pragma once

typedef struct RenderingData
{
    struct CommonState* state;
} RenderingData;

int SDLCALL
    RenderingThread(
    void *data
    );

class RenderingModule
{
public:
    explicit RenderingModule(void* data);
    ~RenderingModule();
    void Tick(float dt);

private:
    Uint64 perf_count_prev;
    CommonState* state;
};
