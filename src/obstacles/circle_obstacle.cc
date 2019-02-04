// Copyright 2016 - 2018 slane@cs.umass.edu, kvedder@umass.edu
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

#include "obstacles/circle_obstacle.h"

#include <glog/logging.h>
#include <cmath>
#include <limits>

#include "constants/constants.h"
#include "eigen3/Eigen/Core"
#include "logging/logger.h"
#include "math/geometry.h"
#include "math/math_util.h"
#include "math/poses_2d.h"
#include "obstacles/obstacle.h"

using Eigen::Vector2f;
using pose_2d::Pose2Df;
using math_util::Sq;
using geometry::SafeVectorNorm;
using geometry::ProjectPointOntoLineSegment;
using std::ceil;
using std::floor;

namespace obstacle {

CircleObstacle::CircleObstacle(const pose_2d::Pose2Df& pose, ObstacleType type,
                               float radius)
    : Obstacle(type), center_(pose.translation), radius_(radius) {
  bounding_box_.x_ = ceil(center_.x());
  bounding_box_.y_ = floor(center_.y());
  bounding_box_.width_ = ceil(2.0f * radius_) + 2;
  bounding_box_.half_width_ = ceil(radius_) + 2;
  bounding_box_.height_ = ceil(2.0f * radius_) + 2;
  bounding_box_.half_height_ = ceil(radius_) + 2;
}

bool CircleObstacle::PointCollision(const Eigen::Vector2f& point,
                                    float margin) const {
  // If the distance from the point to the center of the circle is less than the
  // radius, there is a collision. Otherwise there is no collision. Obstacles
  // shall be defined such that the robot can safely sit on their edge.
  if (bounding_box_.PointCollision(point, margin)) {
    float distance = (point - center_).squaredNorm();
    return distance < Sq(radius_ + margin);
  } else {
    return false;
  }
}

bool CircleObstacle::LineCollision(const Eigen::Vector2f& line_start,
                                   const Eigen::Vector2f& line_end,
                                   float margin) const {
  // If the distance from the closest point on the line to the center of
  // the circle is less than the radius, there is a collision. Otherwise there
  // is no collision. Obstacles shall be defined such that the robot can safely
  // sit on their edge.
  float squared_distance;
  Eigen::Vector2f projection;

  ProjectPointOntoLineSegment(center_, line_start, line_end, &projection,
                              &squared_distance);

  return squared_distance < Sq(radius_ + margin);
}

float CircleObstacle::PointDistance(const Eigen::Vector2f& point,
                                    float margin) const {
  // The distance between the two points is the magnitude of the difference
  // vector.
  float distance = (point - center_).norm();
  return fmax(distance - radius_ - margin, 0);
}

float CircleObstacle::PointDistanceToCenter(const Eigen::Vector2f& point,
                                            float margin) const {
  return (point - center_).norm() - margin;
}

float CircleObstacle::LineDistance(const Eigen::Vector2f& line_start,
                                   const Eigen::Vector2f& line_end,
                                   float margin) const {
  float squared_distance;
  Eigen::Vector2f projection;

  ProjectPointOntoLineSegment(center_, line_start, line_end, &projection,
                              &squared_distance);

  return fmax(std::sqrt(squared_distance) - radius_ - margin, 0);
}

Eigen::Vector2f CircleObstacle::NearestFreePoint(const Eigen::Vector2f& point,
                                                 float margin) const {
  // Get a vector from the center to the point
  Eigen::Vector2f nearest_point = point - center_;

  if (fabs(nearest_point.x()) <= kEpsilon &&
      fabs(nearest_point.y()) <= kEpsilon) {
    nearest_point.x() += -radius_ + margin + kEpsilonPad;
  } else if (SafeVectorNorm(nearest_point) < radius_ + margin + kEpsilonPad) {
    // Normalize that vector
    nearest_point.normalize();

    // Multiply by the radius
    nearest_point *= radius_ + margin + kEpsilonPad;
  }

  // Translate by the center
  nearest_point += center_;

  return nearest_point;
}

bool CircleObstacle::FurthestFreePointOnLine(const Eigen::Vector2f& line_start,
                                             const Eigen::Vector2f& line_end,
                                             Eigen::Vector2f* free_point,
                                             float* distance_from_start,
                                             float margin) const {
  // Handle case where line starts inside of the obstacle.
  if (PointCollision(line_start, margin)) {
    *free_point = line_start;
    *distance_from_start = 0;
    return true;
  }
  if (!LineCollision(line_start, line_end, margin)) {
    *free_point = line_end;
    *distance_from_start = (line_end - line_start).norm();
    NP_CHECK_MSG(!PointCollision((*free_point), margin), "Margin: " << margin);
    return false;
  }
  Vector2f projected_point(0, 0);
  float current_distance = std::numeric_limits<float>::max();
  ProjectPointOntoLineSegment(center_, line_start, line_end, &projected_point,
                              &current_distance);

  Vector2f translation_vector = projected_point - line_start;
  constexpr float kNumericInstabilityThreshold = 0.001f;
  const float slide_back_distance =
      sqrt(Sq(radius_ + margin) - (projected_point - center_).squaredNorm()) +
      // Additional padding is to deal with the fact that the computation above
      // is not numerically stable.
      kEpsilon + kNumericInstabilityThreshold;
  translation_vector -=
      (geometry::GetNormalizedOrZero(translation_vector) * slide_back_distance);

  *free_point = line_start + translation_vector;
  *distance_from_start = translation_vector.norm();

  NP_CHECK_MSG(!PointCollision(line_start, margin), "Margin: " << margin);
  return true;
}

// Gets the center point of the obstacle.
Pose2Df CircleObstacle::GetPose() const { return Pose2Df(0, center_); }

void CircleObstacle::UpdatePose(const pose_2d::Pose2Df& pose) {
  // Update local x and y to match the x and y of the supplied pose
  center_ = pose.translation;

  bounding_box_.x_ = ceil(center_.x());
  bounding_box_.y_ = floor(center_.y());
  bounding_box_.width_ = ceil(2.0f * radius_) + 2;
  bounding_box_.half_width_ = ceil(radius_) + 2;
  bounding_box_.height_ = ceil(2.0f * radius_) + 2;
  bounding_box_.half_height_ = ceil(radius_) + 2;
}

bool CircleObstacle::GetTangents(const Vector2f& desired_point,
                                 const float margin, Vector2f* right_tangent,
                                 Vector2f* left_tangent) const {
  if (PointCollision(desired_point, margin)) {
    return false;
  }
  geometry::GetTangentPoints(desired_point, center_, radius_ + margin,
                             right_tangent, left_tangent);
  return true;
}

IntegerBoundingBox CircleObstacle::GetInternalBoundingBox() const {
  const int axis_distance_from_center =
      static_cast<int>(std::floor(radius_ * kSqrtTwo / 2.0f)) - 1;
  const int x = static_cast<int>(center_.x());
  const int y = static_cast<int>(center_.y());
  return IntegerBoundingBox(x, y, axis_distance_from_center * 2,
                            axis_distance_from_center * 2);
}

IntegerBoundingBox CircleObstacle::GetExternalBoundingBox() const {
  return bounding_box_;
}

const float CircleObstacle::GetRadius() const { return radius_; }

void CircleObstacle::DrawObstacle(logger::Logger* local_logger) const {
  local_logger->AddCircle(center_, radius_, 0, 0, 0, 1);
}

}  // namespace obstacle
