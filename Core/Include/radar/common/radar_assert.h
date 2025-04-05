#pragma once

#ifndef __RADAR_LOG_H__
#error "Just include radar_log.h"
#endif

#include <stdlib.h>


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
#undef eigen_assert
#define eigen_assert(X) RADAR_ASSERT(X)
extern "C" {
#endif

#ifdef __cplusplus
}
#endif
