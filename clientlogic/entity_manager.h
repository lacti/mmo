#pragma once

#include "component.h"
#include "entity.h"
#include <tbb/concurrent_unordered_map.h>

namespace client {
    class entity;

    class entity_manager {
    private:
        struct component_pair {
            component_type::type type;
            size_t data_index;
        };

		mutable LONG ecBusy;

    public:
        
        //typedef tbb::concurrent_unordered_map<int, entity> entity_vector;
        typedef std::unordered_map<int, entity> entity_vector;

        entity_manager();

        entity new_entity(int id);
		entity new_client_entity();

        template <typename T>
        int plug(const entity& e)
        {
            component_pair c_p;
            c_p.type = T::component_index;
            c_p.data_index = T::data_array.size();
            T::data_array.push_back(T());

			while (InterlockedCompareExchange(&ecBusy, 1, 0));
            entity_components.insert( std::pair<entity, component_pair>(e, c_p) );
			InterlockedExchange(&ecBusy, 0);

            return T::data_array.size() - 1;
        }

        int remove_all_components(const entity& e)
        {
			int ret = 0;

			while (InterlockedCompareExchange(&ecBusy, 1, 0));
            ret = entity_components.erase(e);
			InterlockedExchange(&ecBusy, 0);

			return ret;
        }

        entity_vector& get_entities() { return entities; }

        template <typename T>
        T& get_as(const entity& e)
        {
			if (!e.is_valid())
			{
				if (T::data_array.size() == 0)
					T::data_array.push_back(T());
				return T::data_array[0];
			}

			while (InterlockedCompareExchange(&ecBusy, 1, 0));

            auto iter_pair = entity_components.equal_range(e);


            int data_array_index = -1;

            for(auto iter = iter_pair.first; iter != iter_pair.second; ++iter) {

                if (iter->second.type == T::component_index) {

                    data_array_index = iter->second.data_index;
                }
            }

			InterlockedExchange(&ecBusy, 0);

            if (data_array_index < 0)
            {
                data_array_index = plug<T>(e);
            }

            assert(data_array_index >= 0);

			

            return T::data_array[data_array_index];
        }

        template <typename T>
        int has(const entity& e) const
        {
			while (InterlockedCompareExchange(&ecBusy, 1, 0));

			auto iter_pair = entity_components.equal_range(e);
            int num = 0;

            for(auto iter = iter_pair.first; iter != iter_pair.second; ++iter) {

                if (iter->second.type == T::component_index) {

                    ++num;
                }
            }

			InterlockedExchange(&ecBusy, 0);

            return num;
        }

        entity get_player() const
        {
            return player;
        }

        void set_player(const entity& e)
        {
            player = e;

			ClDebugOutput("Player is set to Entity %d.\n", e.get_id());
        }

    private:
        static LONG entity_id_issuer;

        entity_vector entities;
        std::multimap<entity, component_pair> entity_components;
        entity player;
    };

    template <typename T>
    T& get_as(const entity& e)
    {
        return ClCtxEm()->get_as<T>(e);
    }

    template <typename T>
    T& get_player_as()
    {
        return get_as<T>(ClCtxEm()->get_player());
    }
}
