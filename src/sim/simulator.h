// Copyright 2018 jaholtz@cs.umass.edu
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

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include "messages_robocup_ssl_wrapper.pb.h"
#include "net/netraw.h"
#include "radio_protocol_wrapper.pb.h"
#include "sim/worldstate.h"
#include "state/world_state.h"
#include "state/position_velocity_state.h"
#include "util/random.h"
#include "thread_safe/thread_safe_queue.h"

#ifndef SRC_SIM_SIMULATOR_H_
#define SRC_SIM_SIMULATOR_H_

namespace simulator {
class Simulator {
 public:
    Simulator(double step_time, WorldState* world_state);

    // Returns the current simulated world state as a set of SSL Packets
    std::vector<SSLVisionProto::SSL_WrapperPacket> GetSSLWrapperPackets();

    // Returns the current simulated world state as a soccer world state
    state::PositionVelocityState GetWorldState(const team::Team team);

    // Simulates a single time step with the given robot commands.
    void SimulateStep(const RadioProtocolWrapper& command);

 private:
    // Returns true if the given velocity command is legal
    bool ValidateVelocityCommandIntegrity(
        const RadioProtocolWrapper& velocity_command);
    // The amount of time to simulate in an update.
    const double step_time_;
    // The current time in the simulation
    double current_time_;
    // Frame number generated by this simulator
    int frame_index_;

    // A random generator used by the simulator.
    util_random::Random random_;

    // Contains the simulated world_state_
    WorldState* world_state_;
};
}  // namespace simulator

#endif  // SRC_SIM_SIMULATOR_H_
