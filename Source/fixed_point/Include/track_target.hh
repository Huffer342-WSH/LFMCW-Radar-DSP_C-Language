#pragma once

#include "track_kalman.hh"
#include "track_target.h"
#include "radar_measurement.h"

#include <cstdint>
#include <list>

#define CXX_LIST_NODE_TTPE std::__detail::_List_node_base
/**
 * @brief 生命周期类
 * 
 * @note 定点数版本，所以定义不用浮点数（❗❗❗❗不确定是否用浮点数来保证精度）
 * 
 */
class LifeCycle
{

private:
public:
    int32_t score;                            //当前分数
    rd_float_t unassociated_time;                //关联失败时间
    rd_float_t deducted_score;                   //未关联扣除分数
    //动态列表，误差计算后面再看
    // measurements_t measurements;              //量测值
    // measurements_t post_mrasurements;        //预测的测量值
    // int32_t mese;                             //量测误差

    LifeCycle(int32_t score)
    {
        this->score = score;
        this->unassociated_time = 0;
        this->deducted_score = 0;
        // this->mese = 0;
        // this->measurements = *radar_measurements_alloc(10);
        // this->post_mrasurements = *radar_measurements_alloc(10);
        return;
    };

    ~LifeCycle() { };

};

class TrackedTarget
{
public:
    uint32_t uuid;
    GaussianState state;
    LifeCycle life_cycle;
    TrackedTarget(uint32_t uuid, GaussianState &state)
        : uuid(uuid)
        , state(state)
        , life_cycle(0)
    {
    }
    ~TrackedTarget() = default;

    static TrackedTarget *cast_from_c(tracked_targets_list_node_t target);
};


class TrackedTargets : public std::list<TrackedTarget>
{
public:
    TrackedTargets() = default;
    ~TrackedTargets() = default;

    static TrackedTargets *cast_from_c(tracked_targets_list_t *targets)
    {
        return (reinterpret_cast<TrackedTargets *>(targets));
    }

    tracked_targets_list_t *cast_to_c()
    {
        return reinterpret_cast<tracked_targets_list_t *>(this);
    }
};
