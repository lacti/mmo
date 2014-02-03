#ifndef __SINGLETON_H__
#define __SINGLETON_H__

template <typename _Target>
class singleton : private _Target {
public:
    static _Target& instance();

private:
    singleton() {}
};

template <class _Target>
inline _Target& singleton<_Target>::instance()
{
    typedef singleton<_Target> target_singleton_t;
    static target_singleton_t singleton_obj;

    return singleton_obj;
}

template <class _Target>
inline _Target& sys()
{
    return singleton<_Target>::instance();
}

#endif