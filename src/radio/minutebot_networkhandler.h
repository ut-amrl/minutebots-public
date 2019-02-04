// Copyright 2017 - 2018 kvedder@umass.edu, jaholtz@umass.edu
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

#ifndef SRC_RADIO_MINUTEBOT_NETWORKHANDLER_H_
#define SRC_RADIO_MINUTEBOT_NETWORKHANDLER_H_

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include "messages_robocup_ssl_wrapper.pb.h"
#include "net/netraw.h"
#include "radio/minutebot_radio.h"
#include "radio_protocol_wrapper.pb.h"
#include "sim/worldstate.h"

namespace radio {
class NetworkHandler {
 public:
  NetworkHandler(const std::string& input_udp_address,
                 const int input_udp_port);
  ~NetworkHandler();
  void Start();

 private:
  void HandleInput(const std::string udp_address, const int udp_port);

  const std::string input_udp_address;
  const int input_udp_port;

  std::atomic_bool is_running;

  std::thread input_thread;
  MinutebotRadio radio;
};
}  // namespace radio

#endif  // SRC_RADIO_MINUTEBOT_NETWORKHANDLER_H__
