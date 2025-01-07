#pragma once

#include "track_target.hh"
#include "track_kalman.hh"


class Deleter
{
private:
public:
    Deleter() { };

    ~Deleter() { };

    void delete_tracks(TrackedTargets &tracked_targets, std::vector<Hypothesis> &hypotheses);
};
