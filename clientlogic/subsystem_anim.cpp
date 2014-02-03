#include "clientpch.h"
#include "subsystem_anim.h"
#include "animation.inl"
#include "entity_manager.h"
#include "component_anim.h"

using namespace client;

void subsystem_anim::frame_move(component::anim& c, double dt)
{
    assert(c.cur_dir != EMove::STOP);

    c.anim_uindex = move_anim_seq[ c.cur_dir ][ c.anim_frame ][ 0 ];
    c.anim_vindex = move_anim_seq[ c.cur_dir ][ c.anim_frame ][ 1 ];

    c.anim_frame = static_cast<int>(c.last_move_duration / c.move_frame_duration) % 4;

    if (c.cur_move != EMove::STOP) {
        if (c.cur_dir != c.cur_move) {
            c.cur_dir = c.cur_move;
            //last_move_duration = 0;
        } else {
            c.last_move_duration += dt;
        }
    } else {
        //anim_frame = static_cast<int>(last_move_duration / anim_frame_duration) % 4;

        if (c.anim_frame != 0 && c.anim_frame != 2) {
            c.last_move_duration -= dt;
            c.anim_frame = static_cast<int>(c.last_move_duration / c.move_frame_duration) % 4;
        }
    }

}
