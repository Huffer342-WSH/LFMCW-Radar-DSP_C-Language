#include "radar_mag.h"
#include "radar_assert.h"

/**
 * @brief
 *
 * @param mag
 * @param rdm
 * @param numChannel
 * @param rdmOffset
 * @return int
 */
/**
 * @brief 幅度谱计算，累加数个RDM的幅度谱
 *
 * @param[out] mag  输出矩阵，幅度谱
 * @param[in] rdm  输入矩阵，RDM
 * @param numChannel  numChannel
 * @param rdmOffset  rdmOffset
 * @return int  0 - success
 *
 * @details 输入
 *
 */
int radar_clac_magSpec2D(matrix2d_int32_t *mag, matrix2d_complex_int16_t *rdm, uint16_t numChannel, uint16_t rdmOffset)
{
    RADAR_ASSERT(mag != NULL && rdm != NULL && mag->size0 == rdm->size0 && mag->size1 == rdm->size1);

    return 0;
}
