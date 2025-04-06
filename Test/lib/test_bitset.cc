#include <radar/lib/bitset.h>
#include <gtest/gtest.h>
#include <cstdlib>
#include <vector>

TEST(RadarDP_Lib, bitset)
{
    const size_t num_bits = 1024; // 测试 bitset 的位数
    const int iterations = 100;   // 重复测试次数

    // 重复多次测试，不同种子产生不同随机序列，但测试结果可复现
    for (int iter = 0; iter < iterations; ++iter) {
        // 使用 iter 作为随机数种子，确保每次测试都有不同的随机序列
        srand(iter);
        std::vector<bool> expected(num_bits, false);

        // 创建一个初始值全部为 false 的 bitset
        bitset_t *bs = bitset_new(num_bits, false);
        ASSERT_NE(bs, nullptr);

        // 根据随机序列设置每一位
        for (size_t pos = 0; pos < num_bits; ++pos) {
            int bit = rand() % 2; // 随机生成 0 或 1
            expected[pos] = (bit == 1);
            if (expected[pos]) {
                bitset_set(bs, pos);
            } else {
                // 即使初始为 false，也调用一次 reset 来验证函数
                bitset_reset(bs, pos);
            }
        }

        // 验证每一位的状态是否与预期一致
        for (size_t pos = 0; pos < num_bits; ++pos) {
            bool actual = bitset_test(bs, pos);
            EXPECT_EQ(actual, expected[pos]) << "Iteration " << iter << ", bit position " << pos << " incorrect.";
        }

        bitset_delete(bs);
    }
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
