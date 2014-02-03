#pragma once

#define DATA_RELOADABLE 1

namespace data { ;

typedef int id_t;

template <typename _Ty>
struct data_type_t {
    typedef _Ty type;
};


struct xyz_t {
    double x, y, z;

    xyz_t(): x(0), y(0), z(0) {}
};

inline bool parse_data_xyz(const char* str, xyz_t* pos)
{
    const char* first = str;
    const char* second = strchr(str, ',');
    if (second == NULL)
        return false;
    ++second;

    pos->x = atof(first);
    pos->y = atof(second);
    
    const char* third = strchr(second, ',');
    if (third != NULL)
        pos->z = atof(++third);
    return true;
}

}

#define CLEAR_POINTER_LIST(key, list_name) \
    { \
        std::for_each(list_name.begin(), list_name.end(), [=] (key* ptr) { delete ptr; }); \
        list_name.clear(); \
    }

#define CLEAR_POINTER_MAP(key, map_name) \
    { \
        std::for_each(map_name.begin(), map_name.end(), [=] (key::value_type pair) { delete pair.second; }); \
        map_name.clear(); \
    }