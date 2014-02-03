#include "stdafx.h"
#include "component_anim.h"
#include "component_sid.h"
#include "component_name.h"
#include "component_owner.h"
#include "component_pos0.h"
#include "component_pos1.h"
#include "component_sync.h"
#include "component_garbage.h"
#include "component_entity_template.h"
#include "component_hp.h"
#include "component_seqanim.h"
#include "component_collision.h"
#include "component_static_object_template.h"

using namespace component;

tbb::concurrent_vector<anim>                    anim::data_array;
tbb::concurrent_vector<sid>                     sid::data_array;
tbb::concurrent_vector<name>                    name::data_array;
tbb::concurrent_vector<owner>                   owner::data_array;
tbb::concurrent_vector<pos0>                    pos0::data_array;
tbb::concurrent_vector<pos1>                    pos1::data_array;
tbb::concurrent_vector<sync>                    sync::data_array;
tbb::concurrent_vector<garbage>                 garbage::data_array;
tbb::concurrent_vector<entity_template>         entity_template::data_array;
tbb::concurrent_vector<hp>                      hp::data_array;
tbb::concurrent_vector<seqanim>                 seqanim::data_array;
tbb::concurrent_vector<collision>               collision::data_array;
tbb::concurrent_vector<static_object_template>  static_object_template::data_array;
