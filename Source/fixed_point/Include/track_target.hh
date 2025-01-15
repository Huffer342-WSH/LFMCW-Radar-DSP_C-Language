#pragma once

#include "track_kalman.hh"
#include "track_target.h"

#include <cstdint>
#include <list>

#define CXX_LIST_NODE_TTPE std::__detail::_List_node_base

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
