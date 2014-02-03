#pragma once

// x, y 지점이 막혀있는 칸인지를 반환한다.
// 막혀있으면 true, 아니면 false를 반환한다.
bool PfQueryBlock(int x, int y);

// 주인공의 위치를 설정한다.
void PfSetLocation(int x, int y);

// 목표 위치를 설정한다.
void PfSetTargetLocation(int x, int y)
{
}

// 주인공의 현재 위치를 반환한다.
void PfGetLocation(int& x, int& y)
{
}

// 목표 위치를 향해 주인공을 한칸 이동시킨다.
// 이동이 일어났으면 true, 목표 위치에 이미 도달했거나 더이상 움직일 수 없으면 false 반환
bool PfProceed()
{
}
