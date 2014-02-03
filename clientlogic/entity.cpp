#include "stdafx.h"
#include "entity.h"
#include "entity_manager.h"
#include "component_garbage.h"

using namespace client;

inline client::entity_manager* ClCtxEm();

bool entity::is_valid() const
{
    if (id == INVALID_ENTITY_ID)
        return false;

    const entity_manager::entity_vector& ev = ClCtxEm()->get_entities();

    return ev.find(id) != ev.end();
}

void entity::invalidate()
{
    // 여기서 바로 컴포넌트를 삭제하면 다른 스레드에서 슬퍼한다.
    // em().remove_all_components(*this);

    id = INVALID_ENTITY_ID;
}

/*
void entity::frame_move(double dt)
{
if (interp) {
double dx = x1 - x;
double dy = y1 - y;

anim.dx = dt * ((dx > 1) ? anim.move_speed : (dx < -1) ? -anim.move_speed : 0);
anim.dy = dt * ((dy > 1) ? anim.move_speed : (dy < -1) ? -anim.move_speed : 0);

if (anim.dx == 0 && anim.dy == 0) {
anim.cur_move = EMove::STOP;
}
}

x += anim.dx;
y += anim.dy;

anim.dx = 0;
anim.dy = 0;

if (id == get_player_id()) {
anim.prev_move = anim.cur_move;
anim.cur_move = EMove::STOP;
}
}
*/
