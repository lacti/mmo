#pragma once

#define THREAD_SAFE_RELOADER    1

namespace data { ;

class data_reloader {
public:
    virtual bool is_reloadable() = 0;
    virtual void reload() = 0;
};

template <typename _Ty>
class data_referer {
public:
    void add();
    void release();
};

template <typename _Ty>
class data_reloader_impl : public data_reloader {
public:
    virtual bool is_reloadable();
    virtual void reload();
};

class data_depend_map {
public:
    static data_depend_map& instance();

public:
    void add(int index);
    void release(int index);
    bool is_depend(int index);

    void clear();

    data_depend_map();
    ~data_depend_map();

#if THREAD_SAFE_RELOADER
    typedef boost::interprocess::interprocess_recursive_mutex mutex_type;
    typedef boost::interprocess::scoped_lock<mutex_type> lock_type;

    mutex_type mutex;
#endif

private:
    void do_reload();

public:
    typedef std::deque<data_reloader*> reloader_queue_t;
    reloader_queue_t reloader_queue;
    
private:
    static const int max_size = 1024;
    int depend_map[max_size];
};

inline data_depend_map& data_depend_map::instance()
{
    static data_depend_map _instance;
    return _instance;
}

inline void data_depend_map::add(int index)
{
#if THREAD_SAFE_RELOADER
    lock_type lock(mutex);
#endif
    ++depend_map[index];
}

inline void data_depend_map::release(int index)
{
#if THREAD_SAFE_RELOADER
    lock_type lock(mutex);
#endif
    --depend_map[index];
    do_reload();
}

inline void data_depend_map::do_reload()
{
    static bool reloading = false;
    if (reloading)
        return;

    reloading = true;
    while (!reloader_queue.empty()) {
        data_reloader* reloader = reloader_queue.front();
        if (reloader->is_reloadable()) {
            reloader->reload();
            reloader_queue.pop_front();
            delete reloader;
        } else break;
    }
    reloading = false;
}

inline bool data_depend_map::is_depend(int index)
{
    return depend_map[index] != 0;
}

inline void data_depend_map::clear()
{
    std::for_each(reloader_queue.begin(), reloader_queue.end(), [=] (data_reloader* reloader) {
        delete reloader;
    });
    reloader_queue.clear();
}

inline data_depend_map::data_depend_map()
{
    memset(depend_map, 0, sizeof(depend_map));
}

inline data_depend_map::~data_depend_map()
{
    clear();
}

}