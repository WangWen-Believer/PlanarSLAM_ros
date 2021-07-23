//
// Created by wangwen on 7/13/21.
//
#include <ros/ros.h>
#include <sensor_msgs/Image.h>
#include <sensor_msgs/image_encodings.h>
#include <sensor_msgs/PointCloud.h>
#include <sensor_msgs/Imu.h>
#include <std_msgs/Bool.h>
#include <cv_bridge/cv_bridge.h>
#include <message_filters/subscriber.h>
#include <message_filters/time_synchronizer.h>

#include <message_filters/synchronizer.h>
#include <message_filters/sync_policies/approximate_time.h>

#include<iostream>
#include<algorithm>
#include<fstream>
#include<chrono>
#include<opencv2/core/core.hpp>
#include<System.h>
#include<Mesh.h>
#include<MapPlane.h>


using namespace std;

class ImageGrabber
{
public:
    ImageGrabber(Planar_SLAM::System* pSLAM):mpSLAM(pSLAM){}

    void GrabRGBD(const sensor_msgs::ImageConstPtr& msgRGB,const sensor_msgs::ImageConstPtr& msgD);

    Planar_SLAM::System* mpSLAM;
};


int main(int argc, char **argv)
{
    ros::init(argc,argv,"Planar_Slam");
    ros::start();

    if(argc != 3)
    {
        cerr << endl << "Usage: rosrun Planar path_to_vocabulary path_to_settings" << endl;
        ros::shutdown();
        return 1;
    }

    ros::NodeHandle n;

    ros::console::set_logger_level(ROSCONSOLE_DEFAULT_NAME, ros::console::levels::Info);

    Planar_SLAM::System SLAM(argv[1], argv[2], Planar_SLAM::System::RGBD, true);
    Planar_SLAM::Config::SetParameterFile(argv[2]);
    ImageGrabber igb(&SLAM);



    message_filters::Subscriber<sensor_msgs::Image> sub_image(n, "/camera/color/image_raw", 1);
    message_filters::Subscriber<sensor_msgs::Image> sub_depth(n, "/camera/aligned_depth_to_color/image_raw", 1);

    typedef message_filters::sync_policies::ApproximateTime<sensor_msgs::Image, sensor_msgs::Image> sync_pol;
    message_filters::Synchronizer<sync_pol> sync(sync_pol(10), sub_image,sub_depth);

    sync.registerCallback(boost::bind(&ImageGrabber::GrabRGBD,&igb,_1,_2));

    ros::spin();
    // Stop all threads
    SLAM.Shutdown();

    // Save camera trajectory
    SLAM.SaveTrajectoryTUM("CameraTrajectory_corridor1.txt");
    SLAM.SaveKeyFrameTrajectoryTUM("KeyFrameTrajectory_corridor1.txt");
    SLAM.SaveMesh("MeshMap_corridor1.ply");

    ros::shutdown();

    return 0;

}
void ImageGrabber::GrabRGBD(const sensor_msgs::ImageConstPtr& msgRGB,const sensor_msgs::ImageConstPtr& msgD)
{
    // Copy the ros image message to cv::Mat.
    cv_bridge::CvImageConstPtr cv_ptrRGB;
    try
    {
        cv_ptrRGB = cv_bridge::toCvShare(msgRGB);
    }
    catch (cv_bridge::Exception& e)
    {
        ROS_ERROR("cv_bridge exception: %s", e.what());
        return;
    }

    cv_bridge::CvImageConstPtr cv_ptrD;
    try
    {
        cv_ptrD = cv_bridge::toCvShare(msgD);
    }
    catch (cv_bridge::Exception& e)
    {
        ROS_ERROR("cv_bridge exception: %s", e.what());
        return;
    }

    mpSLAM->TrackRGBD(cv_ptrRGB->image,cv_ptrD->image,cv_ptrRGB->header.stamp.toSec());
}

