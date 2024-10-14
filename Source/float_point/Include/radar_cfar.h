#ifndef _RADAR_CFAR_H_
#define _RADAR_CFAR_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint32_t idx0;
    uint32_t idx1;
    double amp;
    double snr;
} cfar2d_point_t;

/**
 * @brief 2D-CFAR检测结果
 */
typedef struct {
    cfar2d_point_t *point; /* 需要外部分配内存然后赋值 */
    uint32_t numMax;
    uint32_t numPoint;
} cfar2d_result_t;


int radardsp_cfar2d(const double *amp, cfar2d_result_t *res, uint32_t num0, uint32_t num1,
                    uint32_t train0, uint32_t train1, uint32_t guard0[][32], uint32_t guard1,
                    double thSNR, double thAmp);

#ifdef __cplusplus
}
#endif
#endif /* _RADAR_CFAR_H_ */
