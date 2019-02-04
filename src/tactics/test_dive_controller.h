// Copyright 2017 - 2018 jaholtz@cs.umass.edu
// College of Information and Computer Sciences,
// University of Massachusetts Amherst
//
//
// This software is free: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License Version 3,
// as published by the Free Software Foundation.
//
// This software is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// Version 3 in the file COPYING that came with this distribution.
// If not, see <http://www.gnu.org/licenses/>.
// ========================================================================

#include <memory>
#include <vector>

#include "math/poses_2d.h"
#include "motion_control/ntoc_2d.h"
#include "motion_control/optimal_control_1d.h"
#include "state/shared_state.h"
#include "state/world_robot.h"
#include "state/world_state.h"
#include "tactics/tactic.h"

#ifndef SRC_TACTICS_TEST_DIVE_CONTROLLER_H_
#define SRC_TACTICS_TEST_DIVE_CONTROLLER_H_

namespace tactics {
// 1-dimensional controller that will try to reach a given point at
// a given time without regard for the velocity at the time when it reaches
// that point.
class TestDiveController : public Tactic {
 public:
  TestDiveController(const state::WorldState& world_state,
                TacticArray* tactic_list,
                state::SharedState* shared_state,
                OurRobotIndex our_robot_index,
                state::SoccerState* soccer_state);

  ~TestDiveController();

  const char* Name() const override { return "test_dive_controller"; }
  void Init() override;
  void Run() override;
  void Reset() override;
  void SetGoal(const pose_2d::Pose2Df& pose) override;
  void SetTime(const double time);

 private:
  pose_2d::Pose2Df robot_goal_pose;
  double_t goal_time_;
  Vector2f previous_intercept_point_;
  float time_to_goal_;
  int interception_count_ = 0;
  bool is_intercepting_ = false;
  const int kInterceptHysterisis  = 25;
};
}  // namespace tactics

#endif  // SRC_TACTICS_TEST_DIVE_CONTROLLER_H_
