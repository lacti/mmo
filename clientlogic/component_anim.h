#pragma once

#include "component.h"

namespace EMove {
    enum Type {
        LEFT = 0, RIGHT, DOWN, UP, STOP = 100
    };
}

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

inline double direction_from_move(EMove::Type t)
{
    double dir = 0;
    switch (t) {
    case EMove::LEFT:   dir = M_PI;         break;
    case EMove::RIGHT:  dir = 0;            break;
    case EMove::DOWN:   dir = 3.0/2*M_PI;   break;
    case EMove::UP:     dir = M_PI/2;       break;
    default:            assert(0);          break;
    }
    return dir;
}

inline EMove::Type move_from_direction(double dir)
{
    if (-1e-2 < dir && dir < 1e-2) return EMove::RIGHT;
    else if (M_PI-1e-2 < dir && dir < M_PI+1e-2) return EMove::LEFT;
    else if (3.0/2*M_PI-1e-2 < dir && dir < 3.0/2*M_PI+1e-2) return EMove::DOWN;
    else if (M_PI/2-1e-2 < dir && dir < M_PI/2+1e-2) return EMove::UP;
    else { assert(0); }

    return EMove::STOP;
}

namespace component
{
    class anim : public client::component_base
    {
    public:
        static const component_type::type component_index = component_type::ANIM;
        static tbb::concurrent_vector<anim> data_array;

        static const double DEFAULT_MOVE_SPEED;
        static const double DEFAULT_MOVE_FRAME_DURATION;

        anim()
            : prev_move(EMove::STOP)
            , cur_move(EMove::STOP)
            , cur_dir(EMove::UP)
            , anim_uindex(0)
            , anim_vindex(0)
            , anim_frame(0)
            , last_move_duration(0)
            , dx(0)
            , dy(0)
            , move_speed(DEFAULT_MOVE_SPEED)
            , move_frame_duration(DEFAULT_MOVE_FRAME_DURATION)
        {}

        EMove::Type prev_move;
        EMove::Type cur_move;
        double dx;
        double dy;
        double move_speed;
        size_t anim_uindex;
        size_t anim_vindex;
        EMove::Type cur_dir;
        size_t anim_frame;
        double last_move_duration;
        double move_frame_duration;
        bool interp;
    };
}
