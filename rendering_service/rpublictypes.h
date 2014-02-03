#pragma once

#include <stdint.h>

typedef uint32_t RHANDLE;
static const RHANDLE RHANDLE_INVALID = 0;

// RObject type hierarchy
enum RObjectType
{
    RSCENENODETYPE,
        RSCENEROOTTYPE,
        RSIMPLEMESHTYPE,
        RCAMERATYPE,
        RLIGHTTYPE,
    RMATERIALDATATYPE,
    RMESHDATATYPE,
    RTEXTUREDATATYPE,
        RRENDERTEXTUREDATATYPE,
    RSHADERDATATYPE
};
