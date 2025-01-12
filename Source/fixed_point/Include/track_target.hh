#pragma once

#include "track_kalman.hh"
#include "track_target.h"

#include <cstdint>
#include <list>


class LifeCycle
{
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
    {
    }
    ~TrackedTarget() = default;
};


class TrackedTargets : public std::list<TrackedTarget>
{
public:
    TrackedTargets() = default;
    ~TrackedTargets() = default;
};


/**
 * @brief  强制转化  'tracked_targets_list_t *'  -> 'TrackedTargets *'
 *
 * @param list 被跟踪目标链表
 * @return TrackedTargets*
 */
static inline TrackedTargets *cast_from_tracked_targets_list(tracked_targets_list_t *list)
{
    return reinterpret_cast<TrackedTargets *>(list);
}


/**
 * @brief 强制转化  'tracked_target_t *'  -> 'TrackedTarget *'
 *
 * @param target 被跟踪目标
 * @return TrackedTarget*
 */
static inline TrackedTarget *cast_from_tracked_target(tracked_target_t *target)
{
    return &(*std::list<TrackedTarget>::iterator(reinterpret_cast<std::__detail::_List_node_base *>(target)));
}
