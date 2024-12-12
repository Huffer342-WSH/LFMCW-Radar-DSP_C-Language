#include "radar_error.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>


void radar_error(const char *reason, const char *file, int line, int error_code)
{
    RADAR_LOG_PRINTF("Radar error: %s, file %s, line %d\n", reason, file, line);
}
