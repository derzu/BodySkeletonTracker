#ifndef ARMCONTROL_H
#define ARMCONTROL_H

#include <ros/ros.h>
#include <actionlib/client/simple_action_client.h>
#include <control_msgs/FollowJointTrajectoryAction.h>
#include <ros/topic.h>

// Our Action interface type for moving TIAGo's head, provided as a typedef for convenience
typedef actionlib::SimpleActionClient<control_msgs::FollowJointTrajectoryAction> arm_control_client;
typedef boost::shared_ptr<arm_control_client>  arm_control_client_Ptr;

class ArmControl {
	private:
		arm_control_client_Ptr ArmClient;
		// Generates the goal for the TIAGo's arm
		control_msgs::FollowJointTrajectoryGoal arm_goal;

	public:
		ArmControl(int argc, char** argv);
		virtual ~ArmControl();
		void createArmClient(arm_control_client_Ptr& actionClient);
		control_msgs::FollowJointTrajectoryGoal waypoints_arm_goal();
		control_msgs::FollowJointTrajectoryGoal prepareGoal(const char * joint, float value);
		void execute(control_msgs::FollowJointTrajectoryGoal arm_goal);
		

};

#endif

