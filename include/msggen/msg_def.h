#ifndef __MSG_DEF_H__
#define __MSG_DEF_H__

typedef int    msg_size_t;
typedef int    msg_type_t;

// message constants
namespace msg {
    const int MAX_MSG_SIZE = 256 * 1024;
    const int DEFAULT_BUFFER_SIZE = 4096;

}

// message default field types
#include "msg_bin.h"

#endif
