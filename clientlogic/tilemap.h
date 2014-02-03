#pragma once

class ClientPathFinder;
typedef ClientPathFinder* ClFinderId;

void cltm_init_obstacles();
void cltm_proceed(int x0, int y0, int xt, int yt, int* x1, int* y1);
int cltm_query_obstacle(int x, int y);
