#include <gtest/gtest.h>
#include "track_target.hh"
#include <iostream>
using namespace std;


std::vector<int> generateRandomBinaryVector(int N)
{
    // 初始化随机数种子
    std::srand(std::time(nullptr));

    std::vector<int> result;
    result.reserve(N); // 预留空间，提高效率

    for (int i = 0; i < N; ++i) {
        // 随机生成0或1，并添加到vector中
        result.push_back(std::rand() % 2);
    }

    return result;
}

void print_list(const TrackedTargets &targets)
{
    for (auto &elm : targets) {
        cout << elm.uuid << ' ' << ends;
    }
}

TEST(RadarFixedTest, TrackedTargets)
{
    const int N = 10;

    uint32_t uuid = 0;
    GaussianState state;

    tracked_targets_list_t *targets = tracked_targets_list_new();
    TrackedTargets *cxx_targets = reinterpret_cast<TrackedTargets *>(targets);
    TrackedTargets list2;


    for (int i = 0; i < N; i++) {
        cxx_targets->emplace_front(uuid++, state);
        cout << "add target,UUID: " << uuid - 1 << ", target num: " << cxx_targets->size() << endl;
    }


    std::vector<int> uuid_vec;
    FOR_EACH_TARGET(t, targets)
    {
        uint32_t uuid;
        tracked_target_get_uuid(t, &uuid);
        cout << uuid << ' ' << ends;
        uuid_vec.push_back(uuid);
    }
    cout << endl;

    std::vector<int> binaryVector = generateRandomBinaryVector(N);


    int cnt = 0;
    for (auto it = cxx_targets->begin(); it != cxx_targets->end(); cnt++) {
        printf("Cnt:%d, UUId:%d, flag:%d\n", cnt, (*it).uuid, binaryVector[cnt]);
        auto t = it;
        it++;
        if (binaryVector[cnt] == 0) {
            cxx_targets->erase(t);
            cout << "erase target,UUID: " << (*t).uuid << ", target num: " << cxx_targets->size() << " it:" << (*(it)).uuid << endl;
        }
    }


    vector<int> res;
    cout << "Excepted remain targets:" << endl;
    for (size_t i = 0; i < N; i++) {
        if (binaryVector[i] == 1) {
            cout << uuid_vec[i] << ' ' << ends;
            res.push_back(uuid_vec[i]);
        }
    }
    cout << endl;

    cnt = 0;
    printf("Remain targets:\n");
    for (auto &elm : *cxx_targets) {
        printf("%d ", elm.uuid);
        ASSERT_EQ(elm.uuid, res[cnt]);
        cnt++;
    }
    printf("\n");

    list2.splice(list2.begin(), *cxx_targets, cxx_targets->begin());

    printf("list1:");
    print_list(*cxx_targets);
    printf("\n");


    printf("list2:");
    print_list(list2);
    printf("\n");


    tracked_targets_list_delete(targets);
}


int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
