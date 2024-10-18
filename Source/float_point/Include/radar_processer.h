#ifndef _LFMCW_RADAR_PROCESSER_H_
#define _LFMCW_RADAR_PROCESSER_H_

#include "radar_types.h"

#ifdef __cplusplus
extern "C" {
#endif

int radardsp_init(radar_handle_t *radar);

int radardsp_input_new_frame(radar_handle_t *radar, void *data);

#ifdef __cplusplus
}
#endif
#endif /* _LFMCW_RADAR_PROCESSER_H_ */
