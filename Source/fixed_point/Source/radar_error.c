#include "radar_error.h"

#include <stdio.h>


void radar_error(const char *reason, const char *file, int line, int error_code)
{

    while (1) {
        printf("Radar error: %s, file %s, line %d\n", reason, file, line);
        for (volatile int i = 0; i < 10000; i++);
    }
}
