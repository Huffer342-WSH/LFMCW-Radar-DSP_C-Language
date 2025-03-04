#pragma once

#include "radar_measurement.h"
#include "track_kalman.hh"
#include "track_target.h"
#include <Eigen/Dense>
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
    int32_t score;            // 当前评分
    double unassociated_time; // 关联失败时间
    int deducted_score;       // 未关联扣除分数

    // 动态列表
    LifeCycle(int32_t score) // 构造函数
    {
        this->score = score; // 当前评分
        this->unassociated_time = 0;
        this->deducted_score = 0;
    }
    ~LifeCycle() { }; // 析构函数
};

class TrackedTarget
{
public:
    static uint32_t next_uuid;
    uint32_t uuid;
    GaussianState state;
    LifeCycle life_cycle;
    TrackedTarget(uint32_t uuid, GaussianState &state)
        : uuid(uuid)
        , state(state)
        , life_cycle(0)
    {
    }


    TrackedTarget(const Vector3r &measurement, const Matrix44r &init_covar, uint32_t timestamp)
        : uuid(this->next_uuid++)
        , life_cycle(0)
    {
        rd_float_t phi = measurement(0);
        rd_float_t rho = measurement(1);
        rd_float_t rho_rate = measurement(2);

        rd_float_t cos_phi = std::cos(phi);
        rd_float_t sin_phi = std::sin(phi);

        rd_float_t x = rho * cos_phi;
        rd_float_t y = rho * sin_phi;
        rd_float_t vx = rho_rate * cos_phi;
        rd_float_t vy = rho_rate * sin_phi;

        state.state_vector << x, vx, y, vy;
        state.covar = init_covar;
        state.timestamp_ms = timestamp;
    }


    ~TrackedTarget() = default;

    static TrackedTarget *cast_from_c(tracked_targets_list_node_t target);
};


class TrackedTargets : public std::list<TrackedTarget>
{
public:
    TrackedTargets() = default;
    ~TrackedTargets() = default;

    void delete_invalid_targets();


    static TrackedTargets *cast_from_c(const tracked_targets_list_t *targets)
    {
        return (reinterpret_cast<TrackedTargets *>((tracked_targets_list_t *)targets));
    }

    tracked_targets_list_t *cast_to_c()
    {
        return reinterpret_cast<tracked_targets_list_t *>(this);
    }
};
