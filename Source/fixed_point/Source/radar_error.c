#include "radar_error.h"

#include <stdio.h>


void radar_error(const char *reason, const char *file, int line, int error_code)
{
    printf("Radar error: %s, file %s, line %d\n", reason, file, line);
}
