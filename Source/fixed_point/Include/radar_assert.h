#ifndef _RADAR_ASSERT_H_
#define _RADAR_ASSERT_H_

#include <stdio.h>
#include <assert.h>


#define RADAR_ASSERT(expression)                                                             \
    if (!(expression)) {                                                                     \
        printf("Assertion failed: %s, file %s, line %d\n", #expression, __FILE__, __LINE__); \
    }

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif
#endif /* _RADAR_ASSERT_H_ */
