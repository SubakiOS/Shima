#ifndef KASSERT_H
#define KASSERT_H

#include "panic.h"

#define kassert(condition, reason) \
    do { \
        if (!(condition)) { \
            panic(reason); \
        } \
    } while (0)

#endif