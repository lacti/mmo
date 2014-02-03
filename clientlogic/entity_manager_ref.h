#pragma once

// to break dependency
namespace client {
    class entity_manager;
    typedef boost::shared_ptr<entity_manager> entity_manager_ref;
}
