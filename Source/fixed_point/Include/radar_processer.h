#ifndef _RADAR_PROCESSER_H_
#define _RADAR_PROCESSER_H_

#include "radar_config.h"
#include "radar_types.h"

#ifdef __cplusplus
extern "C" {
#endif


int radardsp_input_new_frame(radar_handle_t *radar, int16_t *data, size_t size);


#ifdef __cplusplus
}
#endif
#endif /* _RADAR_PROCESSER_H_ */
