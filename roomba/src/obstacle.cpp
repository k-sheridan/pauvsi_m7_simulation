#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include <geometry_msgs/PoseStamped.h>
#include <gazebo_msgs/GetModelState.h>
#include <time.h>
#include <math.h>
#include <visualization_msgs/Marker.h>
#include <sensor_msgs/LaserScan.h>

//the looprate for the loop in main
const int looprate = 20;

//the radius that the copter has to be in to touch the roomba
const double Radius = 0.15;

//The maximum angle to be turned on the 5 second interval
const double ang_5 = (M_PI) / 9.0f;
//Angle to be turned on the 20 second interval
const double ang_20 = 0 - M_PI;
//Angle to be turned on touch
const double ang_touch = 0 - M_PI/4;

//the distance that the copter will have to be to block the roomba
const int distance = Radius;
//The width of the hitbox for the copter to block the roomba
const int blockRadius = 500;


std::string modelName;

geometry_msgs::PoseStamped poseMsg;


//Simplifies a radian
double reduceAngle(double ang){
	while(ang > (M_PI/2)){
		ang -= (M_PI/2);
	}
	while (ang < (M_PI/2)){
		ang += (M_PI/2);
	}
	return ang;

}


int main(int argc, char **argv)
{


 	ros::init(argc, argv, "roomba");

 	ros::NodeHandle n;


 	ros::param::param<std::string>("~model_name", modelName, "roomba1");




	//Published movement messages
 	ros::Publisher pubMov = n.advertise<geometry_msgs::Twist>("roomba/cmd_vel", 1);
	//Published roomba coordinates
 	ros::Publisher pubPos = n.advertise<geometry_msgs::PoseStamped>("roomba/pose", 1);
 	//Publishes the Rviz visualization marker




	//GetModelState Client. Used to gett roomba coordinates from Gazebo
 	ros::ServiceClient gms_c =
 		n.serviceClient<gazebo_msgs::GetModelState>("/gazebo/get_model_state");

 	gazebo_msgs::GetModelState model;
 	model.request.model_name = modelName;

 	ros::Rate loop_rate(looprate);
	//twist object to p}ublish messages
 	geometry_msgs::Twist mov;
 	geometry_msgs::PoseStamped pos;

 	pos.header.stamp = ros::Time::now();
 	pos.header.frame_id = "roomba_odom";

	//The time of the current simulation in seconds
 	double sim_time;

 	double speed = 0.33;

	//coordinates of the roomba
 	double x = 0;
 	double y = 0;
 	double z = 0;

 	double last_x;
 	double last_y;
 	double velocity_x;
 	double velocity_y;

	//coordinates of the copter
 	double copter_x;
 	double copter_y;
 	double copter_z;

 	double ang_reduced;




 	double rand_num = 0;

 	while (ros::ok())
 	{
		pos.header.stamp = ros::Time::now();

		//update the coordinates in the model
 		gms_c.call(model);



		//Set all of the coordinate variables
 		x = model.response.pose.position.x;
		y = model.response.pose.position.y;
		z = model.response.pose.position.z;

		velocity_x = (x - last_x) / looprate;
		velocity_y = (y - last_y) / looprate;

		last_x = x;
		last_y = y;

		//Update the time of the simulation
		sim_time = ros::Time::now().toSec();

		//std::cout << sim_time << std::endl;

		//Move according the the speed
		mov.linear.x = speed;

		/*Move the roomba as it normally should on the 5 and 20 second intervals*/
		mov.angular.z = -0.066;





		pos.pose.position.x = x;
		pos.pose.position.y = y;


		pos.pose.orientation.x = model.response.pose.orientation.x;
		pos.pose.orientation.y = model.response.pose.orientation.y;
		pos.pose.orientation.z = model.response.pose.orientation.z;
		pos.pose.orientation.w = model.response.pose.orientation.w;





//		if(mov.angular.z != 0)
//		{
//			mov.linear.x = 0;
//		}

		//std::cout<<"Last Touch:"<<last_touch<<std::endl;

		pubMov.publish(mov);
		pubPos.publish(pos);


		ros::spinOnce();
		loop_rate.sleep();
	}

	return 0;
}
