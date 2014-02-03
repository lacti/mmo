#include "stdafx.h"
#include "tilemap.h"
#include "ClientPathFinder.h"
#include <map>

static ClientPathFinder* PathFinder;
static std::map<PosNode, bool> Obstacles;

ClFinderId cltm_new_finder();
void cltm_free_finder(ClFinderId finder);

void cltm_init_obstacles()
{
    PathFinder = cltm_new_finder();

    for (int i = -20; i < 20; ++i)
        Obstacles[PosNode(0,i)] = true;

    for (int i = -20; i < 20; ++i)
        Obstacles[PosNode(i,0)] = true;
}

int cltm_query_obstacle(int x, int y)
{
    return Obstacles.find(PosNode(x, y)) == Obstacles.end() ? 0 : 1;
}

ClFinderId cltm_new_finder()
{
    return new ClientPathFinder;
}

void cltm_free_finder(ClFinderId finder)
{
    delete finder;
}

void cltm_proceed(int x0, int y0, int xt, int yt, int* x1, int* y1)
{
    if (PathFinder == nullptr)
        return;

    PosNode current(x0, y0);
    PosNode dest(xt, yt);
    PosNode next;

    PathFinder->NextStep(current, dest, next);

    if (x1) *x1 = next.first;
    if (y1) *y1 = next.second;
}
