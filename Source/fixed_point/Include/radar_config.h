#ifndef _RADAR_CONFIG_H_
#define _RADAR_CONFIG_H_

/* 选项定义（勿修改） ========================================*/
#define ON 1
#define OFF 0

#define AMP_SPEC_CLAC_METHOD_INSIDE 0
#define AMP_SPEC_CLAC_METHOD_OUTSIDE 1

/* 选项定义 ========================================*/

/* 静态杂波滤除 */
#define ENABLE_STATIC_CLUTTER_FILTERING OFF

/* 内部计算幅度谱还是外部直接提供 */
#define AMPLITUDE_SPECTRUM_CALCULATION_METHOD AMP_SPEC_CLAC_METHOD_INSIDE

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif
#endif /* _RADAR_CONFIG_H_ */
