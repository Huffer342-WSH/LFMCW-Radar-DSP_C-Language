#include <gtest/gtest.h>
#include <stc/cbits.h>

#include <random>

TEST(STCTest, cbits)
{
    const size_t N = 20;
    size_t pos[N];
    cbits bset = cbits_with_size(23, true);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<size_t> dis(0, N - 1);

    for (size_t i = 0; i < N; ++i) {
        pos[i] = dis(gen);
        cbits_set(&bset, pos[i]);
    }

    for (size_t i = 0; i < N; ++i) {
        ASSERT_TRUE(cbits_test(&bset, pos[i]));
    }

    cbits_drop(&bset);
}

// 运行所有测试
int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
