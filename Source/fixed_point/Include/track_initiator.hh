#pragma once

#include "track_target.hh"
#include "track_associator.hh"
#include "track_kalman.hh"

#include <vector>
#include <Eigen/Dense>

class Initiator
{
private:
public:
    Associator &associator;
    Initiator(Associator &associator)
        : associator(associator)
    {
        return;
    };

    ~Initiator() { };

    void initiate(TrackedTargets &tracked_targets, TrackedTargets &unconfirmed_targets, std::vector<Eigen::Vector3d> &measurements, uint32_t timestamp_ms);

    void update_lifecycle(TrackedTargets &targets, std::vector<Hypothesis> hypotheses);


    void move_confirmed_targets(TrackedTargets &tracked_targets, TrackedTargets &unconfirmed_targets);


    void creat_targets(TrackedTargets &unconfirmed_targets, std::vector<Eigen::Vector3d> &measurements, uint32_t timestamp_ms);
};
