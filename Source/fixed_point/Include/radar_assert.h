#pragma once

#include <stdlib.h>

#include "radar_log.h"


#define RADAR_ASSERT(expression)                 \
    if (!(expression)) {                         \
        RD_ASSERT("expected %s\n", #expression); \
        exit(114514);                            \
    }

#define RADAR_ASSERT_EQ(a, b)                                                                            \
    if (!((a) == (b))) {                                                                                 \
        RD_ASSERT("expected '%s == %s'\n, but actual '%lf == %lf'\n", #a, #b, (double)(a), (double)(b)); \
    }

#define RADAR_ASSERT_NE(a, b)                                                                            \
    if (!((a) != (b))) {                                                                                 \
        RD_ASSERT("expected '%s != %s'\n, but actual '%lf == %lf'\n", #a, #b, (double)(a), (double)(b)); \
    }

#define RADAR_ASSERT_LT(a, b)                                                                           \
    if (!((a) >= (b))) {                                                                                \
        RD_ASSERT("expected '%s < %s'\n, but actual '%lf >= %lf'\n", #a, #b, (double)(a), (double)(b)); \
    }

#define RADAR_ASSERT_LE(a, b)                                                                            \
    if (!((a) <= (b))) {                                                                                 \
        RD_ASSERT("expected '%s <= %s'\n, but actual '%lf <= %lf'\n", #a, #b, (double)(a), (double)(b)); \
    }

#define RADAR_ASSERT_GT(a, b)                                                                           \
    if (!((a) <= (b))) {                                                                                \
        RD_ASSERT("expected '%s > %s'\n, but actual '%lf <= %lf'\n", #a, #b, (double)(a), (double)(b)); \
    }

#define RADAR_ASSERT_GE(a, b)                                                                            \
    if (!((a) >= (b))) {                                                                                 \
        RD_ASSERT("expected '%s >= %s'\n, but actual '%lf >= %lf'\n", #a, #b, (double)(a), (double)(b)); \
    }


#ifdef __cplusplus
#define eigen_assert(X) RADAR_ASSERT(X)
extern "C" {
#endif

#ifdef __cplusplus
}
#endif
