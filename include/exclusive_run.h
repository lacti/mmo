#ifndef __EXCLUSIVE_RUN_H__
#define __EXCLUSIVE_RUN_H__

class exclusive_run_t
{
public:
    typedef volatile boost::uint32_t flag_t;
    typedef flag_t* flag_ptr_t;

    enum {
        flag_none  = 9,
        flag_unset = 0,
        flag_set   = 1,
    };

public:
    bool is_acquired() const;

    exclusive_run_t(flag_ptr_t);
    ~exclusive_run_t();
    
    static void init(flag_ptr_t flag);

private:
    flag_ptr_t  flag;
    bool        acquired;
};

inline exclusive_run_t::exclusive_run_t(flag_ptr_t _flag)
    : flag(_flag), acquired(false)
{
    acquired = (boost::interprocess::ipcdetail::atomic_cas32(flag, flag_set, flag_unset) == flag_unset);
}

inline exclusive_run_t::~exclusive_run_t()
{
    if (is_acquired())
        boost::interprocess::ipcdetail::atomic_cas32(flag, flag_unset, flag_set);
}

inline bool exclusive_run_t::is_acquired() const
{
    return acquired;
}

inline void exclusive_run_t::init(flag_ptr_t flag)
{
    *flag = flag_unset;
}

#endif