#include "clientpch.h"
#include "collision.h"
#include "entity.h"
#include "entity_manager.h"
#include "input_manager.h"
#include "msg_writer.h"
#include "msg_session.h"
#include "game_msg.h"
#include "component_pos0.h"
#include "component_anim.h"

using namespace client;

extern bool             g_force_send_predicted_movement;
extern msg_session_ref  g_session;

inline double distance2(const entity& o1, const entity& o2)
{
    const component::pos0& o1_pos = get_as<component::pos0>(o1);
    const component::pos0& o2_pos = get_as<component::pos0>(o2);

	const double dx = (o1_pos.x - o2_pos.x);
	const double dy = (o1_pos.y - o2_pos.y);

	return dx*dx + dy*dy;
}

namespace Quadrant {
	enum Type {
		I,
		II,
		III,
		IV
	};
}

inline Quadrant::Type relative_dir(const entity& ref, const entity& target)
{
    const component::pos0& ref_pos = get_as<component::pos0>(ref);
    const component::pos0& target_pos = get_as<component::pos0>(target);

	const double dx = (target_pos.x - ref_pos.x);
	const double dy = (target_pos.y - ref_pos.y);

	double rad = atan2(dy, dx);

	if (0<=rad && rad < M_PI/2) {
		return Quadrant::I;
	} else if (M_PI/2 <= rad && rad < M_PI) {
		return Quadrant::II;
	} else if (-M_PI <= rad && rad < -M_PI/2) {
		return Quadrant::III;
	}    
	return Quadrant::IV;
}

void client::resolve_collision(double r)
{
    entity player = em().get_player();
    if (!player.is_valid()) return;

    component::pos0& player_pos = get_player_as<component::pos0>();
    
    BOOST_FOREACH(const entity& o, em().get_entities()) {

        component::pos0& o_pos = get_as<component::pos0>(o);

        if (o == player) {
			continue;
		}

		double ax = o_pos.x - player_pos.x;
		double ay = o_pos.y - player_pos.y;
		
		static bool prev_update_player_pos = false;
		bool update_player_pos = false;
		
		for (int i = 0; i < 50; ++i) {
			if (distance2(player, o) < 4*r*r) {
				
				update_player_pos = true;
					
				Quadrant::Type q = relative_dir(player, o);

				if (get_key_state('A')) {
					if (q == Quadrant::II) player_pos.y -= 1e-1;
					else if (q == Quadrant::III) player_pos.y += 1e-1;
				} else if (get_key_state('D')) {
					if (q == Quadrant::I) player_pos.y -= 1e-1;
					else if (q == Quadrant::IV) player_pos.y += 1e-1;
				} else if (get_key_state('S')) {
					if (q == Quadrant::III) player_pos.x += 1e-1;
					else if (q == Quadrant::IV) player_pos.x -= 1e-1;
				} else if (get_key_state('W')) {
					if (q == Quadrant::I) player_pos.x -= 1e-1;
					else if (q == Quadrant::II) player_pos.x += 1e-1;
				} else {
					player_pos.x -= ax/1e1;
					player_pos.y -= ay/1e1;
				}
			}
		}

		if (update_player_pos) {

            const auto& player_anim = get_player_as<component::anim>();

            double dir = direction_from_move(player_anim.cur_dir);
            msg::move_msg msg(player.get_id(), player_pos.x, player_pos.y, dir, 0);

			g_session->write(msg);
		}

		if (prev_update_player_pos && !update_player_pos) {
			g_force_send_predicted_movement = true;
		} else {
			g_force_send_predicted_movement = false;
		}
		

		prev_update_player_pos = update_player_pos;
	}
}
