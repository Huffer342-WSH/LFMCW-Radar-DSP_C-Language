#include <gtest/gtest.h>
#include <stdlib.h>
#include <list>
#include <chrono>
#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <thread>
#include <chrono>

#include <radar/common/mm.h>

// 辅助函数：计时器
class Timer
{
private:
    std::chrono::high_resolution_clock::time_point start_time;
    std::string operation_name;

public:
    Timer(const std::string &name)
        : operation_name(name)
    {
        start_time = std::chrono::high_resolution_clock::now();
    }
    ~Timer()
    {
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count();
        std::cout << operation_name << " 耗时: " << duration << " 微秒" << std::endl;
    }
};
// 测试参数
const int NUM_ITERATIONS = 10; // 循环次数
const int ALLOC_SIZE = 512;    // 每次分配的字节数
// 测试1: malloc/free
TEST(RadarDP_MM, MallocFree)
{
    Timer timer("malloc/free测试");

    for (int i = 0; i < NUM_ITERATIONS; i++) {
        int *p = (int *)rd_malloc(ALLOC_SIZE);
        ASSERT_NE(p, nullptr) << "内存分配失败";

        // 简单使用分配的内存
        p[0] = i;

        rd_free(p);
    }
}
// 测试2: new/delete
TEST(RadarDP_MM, NewDelete)
{
    Timer timer("new/delete测试");

    for (int i = 0; i < NUM_ITERATIONS; i++) {
        int *p = new int[ALLOC_SIZE / sizeof(int)];
        ASSERT_NE(p, nullptr) << "内存分配失败";

        // 简单使用分配的内存
        p[0] = i;

        delete[] p;
    }
}
// 测试3: std::list的emplace和erase
TEST(RadarDP_MM, ListEmplaceErase)
{
    Timer timer("std::list emplace/erase测试");

    std::list<int> myList;

    // 测试emplace操作
    {
        Timer emplace_timer("仅list emplace操作");
        for (int i = 0; i < NUM_ITERATIONS; i++) {
            myList.emplace_back(i);
        }
    }

    // 确认列表大小
    ASSERT_EQ(myList.size(), NUM_ITERATIONS);

    // 测试erase操作
    {
        Timer erase_timer("仅list erase操作");
        while (!myList.empty()) {
            myList.pop_front();
        }
    }

    // 确认列表为空
    ASSERT_TRUE(myList.empty());
}


TEST(RadarDP_MM, malloc_free)
{
    int numTests = 10;
    std::cout << "Starting random malloc/free test with " << numTests << " operations\n";

    // 随机数生成器
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> delayDist(1, 5000);
    std::uniform_int_distribution<> sizeDist(1, 2048);
    std::uniform_int_distribution<> holdDist(1, 1000);

    // 定义测试事件类型
    struct MallocEvent {
        enum Type { MALLOC, FREE } type;
        int id;      // 操作ID
        int time;    // 事件发生时间 (ms)
        size_t size; // 分配大小 (仅对MALLOC有效)
        void *ptr;   // 内存指针 (初始为nullptr)
    };

    // 生成所有malloc事件
    std::vector<MallocEvent> events;
    for (int i = 0; i < numTests; i++) {
        int startTime = delayDist(gen);
        size_t allocSize = sizeDist(gen);
        int holdTime = holdDist(gen);

        // 添加malloc事件
        events.push_back({
            MallocEvent::MALLOC,
            i,         // ID
            startTime, // 开始时间
            allocSize, // 分配大小
            nullptr    // 指针初始为空
        });

        // 添加对应的free事件
        events.push_back({
            MallocEvent::FREE,
            i,                    // ID
            startTime + holdTime, // 结束时间
            0,                    // 大小不适用于free
            nullptr               // 指针将在malloc时更新
        });
    }

    // 按照时间排序所有事件
    std::sort(events.begin(), events.end(), [](const MallocEvent &a, const MallocEvent &b) {
        return a.time < b.time;
    });

    // 执行所有事件
    size_t totalAllocated = 0;
    int activeAllocations = 0;
    size_t peakMemory = 0;
    size_t currentMemory = 0;

    std::cout << "Starting event simulation...\n";
    int currentTime = 0;

    for (auto &event : events) {
        // 更新当前时间
        int timeDelta = event.time - currentTime;
        if (timeDelta > 0) {
            std::cout << "Time: " << event.time << "ms (+" << timeDelta << "ms)\n";
            std::this_thread::sleep_for(std::chrono::microseconds(timeDelta));
            currentTime = event.time;
        }

        if (event.type == MallocEvent::MALLOC) {
            // 执行malloc
            void *ptr = rd_malloc(event.size);
            if (ptr) {
                // 写入一些数据确保内存可用
                memset(ptr, 0xAB, event.size);

                event.ptr = ptr;
                totalAllocated += event.size;
                currentMemory += event.size;
                activeAllocations++;

                // 更新峰值内存使用
                if (currentMemory > peakMemory) {
                    peakMemory = currentMemory;
                }

                std::cout << "  [+] ID " << event.id << ": Allocated " << event.size << " bytes\n";

                // 更新所有对应的free事件，使其指向同一块内存
                for (auto &e : events) {
                    if (e.type == MallocEvent::FREE && e.id == event.id) {
                        e.ptr = ptr;
                        break;
                    }
                }
            } else {
                std::cout << "  [!] ID " << event.id << ": Failed to allocate " << event.size << " bytes\n";
            }
        } else {
            // 执行free
            if (event.ptr) {
                rd_free(event.ptr);
                activeAllocations--;
                std::cout << "  [-] ID " << event.id << ": Freed memory\n";
            } else {
                std::cout << "  [!] ID " << event.id << ": Tried to free NULL pointer\n";
            }
        }
    }

    // 输出统计信息
    std::cout << "\nTest completed!\n";
    std::cout << "Total memory allocated: " << totalAllocated << " bytes\n";
    std::cout << "Peak memory usage: " << peakMemory << " bytes\n";
    std::cout << "Active allocations at end: " << activeAllocations << " (should be 0)\n";

    // 检查是否有内存泄漏
    if (activeAllocations != 0) {
        std::cout << "WARNING: Possible memory leak detected!\n";
    }
}

int main(int argc, char **argv)
{
    // 初始化随机数生成器
    srand(static_cast<unsigned int>(time(nullptr)));

    // 打印测试配置
    std::cout << "===== 内存管理性能测试 =====" << std::endl;
    std::cout << "迭代次数: " << NUM_ITERATIONS << std::endl;
    std::cout << "分配大小: " << ALLOC_SIZE << " 字节" << std::endl;
    std::cout << "===========================" << std::endl;

    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
