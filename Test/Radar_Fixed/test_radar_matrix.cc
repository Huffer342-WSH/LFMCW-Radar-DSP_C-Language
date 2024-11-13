#include <gtest/gtest.h>

#include "radar_matrix.h"

#include <iostream>
#include <iomanip>
#include <cstdio>

TEST(RadarFixedTest, radar_matrix_static_alloc)
{
    alignas(MATRIX_STATIC_BUFFER_ALIGN(matrix3d_complex_int16_t,
                                       int16_t)) static uint8_t buffer[MATRIX_STATIC_BUFFER_SIZE(matrix3d_complex_int16_t, int16_t, 12000)];


    printf("%-50s \t 0x%016jx\n", "buffer addr:", (uintptr_t)buffer);
    printf("%-50s \t %-10zu\n", "alignof( matrix3d_complex_int16_t ):", alignof(matrix3d_complex_int16_t));
    printf("%-50s \t %-10zu\n", "alignof( int16_t ):", alignof(int16_t));

    printf("%-50s \t %-10zu\n", "sizeof ( buffer ):", sizeof(buffer));
    printf("%-50s \t %-10zu\n", "sizeof ( matrix3d_complex_int16_t ):", sizeof(matrix3d_complex_int16_t));
    printf("%-50s \t %-10zu\n", "sizeof ( int16_t ):", sizeof(int16_t));

    ASSERT_EQ((uintptr_t)buffer % alignof(matrix3d_complex_int16_t), 0);
    ASSERT_EQ((uintptr_t)buffer % alignof(int16_t), 0);

    matrix3d_complex_int16_t *m = radar_matrix3d_complex_int16_alloc_static(buffer, sizeof(buffer), 10, 20, 30);

    ASSERT_NE((uintptr_t)m, 0);
    printf("%-50s \t 0x%016jx\n", "matrix addr:", (uintptr_t)m);
    printf("%-50s \t 0x%016jx\n", "matirx.data addr:", (uintptr_t)m->data);


    memset(m->data, 0, sizeof(int16_t) * m->size0 * m->size1 * m->size2 * 2);
}

// 运行所有测试
int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
