#ifndef MSTAR_POLICY_H
#define MSTAR_POLICY_H

/****************************************************************************
 * Provides a wrapper for the Boost graphs
 ***************************************************************************/

#include <iostream>
#include <map>
#include <vector>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>

#include "mstar_type_defs.hpp"


namespace mstar{
  /**
   * Generates an individual policy for a robot to reach a specified goal
   */
  class Policy{
  protected:
    Graph g_; // the boost graph this wraps
    std::vector<double> costs_; // holds cost to goal from every configuration
    boost::property_map<Graph, boost::edge_weight_t>::type edge_weight_map_;
    std::vector<int> predecessors_;

  public:
    /**
     * @param g The graph describing the workspace
     * @param goal The goal coordinate of the robot
     */
    Policy(const Graph &g, const RobCoord goal);

    /**
     * Returns the cost-to-go from a vertex
     * @param vert Vertex to query cost from
     *
     * @return the cost to go until the goal is reached
     */
    double get_cost(RobCoord coord);

    /**
     * Returns cost of traversing the edge (u, v)
     *
     * Does not check whether the edge exists
     *
     * @param u Source vertex of the edge
     * @param v Destination vertex of the dge
     *
     * @return the cost of the edge
     */
    double get_edge_cost(RobCoord u, RobCoord v);

    /**
     * Returns the out-neighbors of a given coordinate
     * @param coord Vertex to get out neighbors of
     */
    std::vector<RobCoord> get_out_neighbors(RobCoord coord);

    /**
     * Returns the successor of the specified coordinate
     *
     * @param coord coordinate to compute the successor thereof
     *
     * @return coordinate of next step
     */
    RobCoord get_step(RobCoord coord);
  };


  typedef std::map<double, std::vector<RobCoord>> Offsets;

  /**
   * Generates an individual policy for a robot to reach a specified goal
   *
   * Supports EPEM* with limited offset neighbor generation
   */
  class EPEMstar_Policy: public Policy{
  protected:
    std::vector<Offsets> offset_neighbors_;

  public:
    /**
     * @param g The graph describing the workspace
     * @param goal The goal coordinate of the robot
     */
    EPEMstar_Policy(const Graph &g, const RobCoord goal) :
      Policy(g, goal){
      offset_neighbors_ = std::vector<Offsets>(costs_.size());
    }

    /**
     * Returns the neighbors of coord by offset
     *
     * Offset is the excess cost to reach the goal via a given
     * neighbor compared to the optimal move from a given coord
     *
     * @param coord the coordinate to query from
     */
    Offsets get_offset_neighbors(RobCoord coord);
  };
}

#endif
