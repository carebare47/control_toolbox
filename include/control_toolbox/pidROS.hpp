/*********************************************************************
 * Software License Agreement (BSD License)
 *
 *  Copyright (c) 2008, Willow Garage, Inc.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *   * Neither the name of the Willow Garage nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 *  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 *  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 *  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 *  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *********************************************************************/
#ifndef CONTROL_TOOLBOX__PIDROS_HPP_
#define CONTROL_TOOLBOX__PIDROS_HPP_

#include <memory>
#include <string>

#include "control_msgs/msg/pid_state.hpp"

#include "rclcpp/clock.hpp"
#include "rclcpp/duration.hpp"
#include "rclcpp/node.hpp"

#include "realtime_tools/realtime_buffer.h"
#include "realtime_tools/realtime_publisher.h"

#include "control_toolbox/pid.hpp"


namespace control_toolbox
{

class PidROS
{
public:
  /*!
   * \brief Destructor of PidROS class.
   *
   * The node is passed to this class to handler the ROS parameters, this class allows
   * to add a prefix to the pid parameters
   *
   * \param node ROS node
   * \param topic_prefix prefix to add to the pid parameters.
   */
  explicit PidROS(rclcpp::Node::SharedPtr & node, std::string topic_prefix = std::string(""));

  /*!
   * \brief Destructor of PidROS class.
   */
  ~PidROS();

  /*!
   * \brief Initialize the PID controller and set the paramaters
   * \param p The proportional gain.
   * \param i The integral gain.
   * \param d The derivative gain.
   * \param i_max The max integral windup.
   * \param i_min The min integral windup.
   */
  void initPid(double p, double i, double d, double i_max, double i_min, bool antiwindup);

  /*!
   * \brief Initialize the PID controller based on already setted parameters
   * \return True is all parameters are set (p, i, d, i_min and i_max), False otherwise
   */
  bool initPid();

  /*!
   * \brief Reset the state of this PID controller
   */
  void reset();

  /*!
   * \brief Set the PID error and compute the PID command with nonuniform time
   * step size. The derivative error is computed from the change in the error
   * and the timestep \c dt.
   *
   * \param error  Error since last call (error = target - state)
   * \param dt Change in time since last call in seconds
   *
   * \returns PID command
   */
  double computeCommand(double error, rclcpp::Duration dt);

  /*!
   * \brief Get PID gains for the controller.
   * \return gains A struct of the PID gain values
   */
  Pid::Gains getGains();

  /*!
   * \brief Set PID gains for the controller.
   * \param p The proportional gain.
   * \param i The integral gain.
   * \param d The derivative gain.
   * \param i_max The max integral windup.
   * \param i_min The min integral windup.
   */
  void setGains(double p, double i, double d, double i_max, double i_min, bool antiwindup = false);

  /*!
   * \brief Set PID gains for the controller.
   * \param gains A struct of the PID gain values
   */
  void setGains(const Pid::Gains & gains);

  /*!
   * \brief Set current command for this PID controller
   */
  void setCurrentCmd(double cmd);

  /*!
   * \brief Return current command for this PID controller
   */
  double getCurrentCmd();

  /*!
   * \brief Return PID error terms for the controller.
   * \param pe[out] The proportional error.
   * \param ie[out] The integral error.
   * \param de[out] The derivative error.
   */
  void getCurrentPIDErrors(double & pe, double & ie, double & de);

  /*!
   * \brief Print to console the current parameters
   */
  void printValues();

private:
  void setParameterEventCallback();

  void publishPIDState(double cmd, double error, rclcpp::Duration dt);

  void declare_param(const std::string & param_name, rclcpp::ParameterValue param_value);

  bool getDoubleParam(const std::string & param_name, double & value);

  bool getBooleanParam(const std::string & param_name, bool & value);

  rclcpp::node_interfaces::OnSetParametersCallbackHandle::SharedPtr parameter_callback_;

  rclcpp::Node::SharedPtr node_;

  std::shared_ptr<realtime_tools::RealtimePublisher<control_msgs::msg::PidState>> rt_state_pub_;
  std::shared_ptr<rclcpp::Publisher<control_msgs::msg::PidState>> state_pub_;

  Pid pid_;
  std::string topic_prefix_;
};

}  // namespace control_toolbox

#endif  // CONTROL_TOOLBOX__PIDROS_HPP_