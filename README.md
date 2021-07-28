#### PlanarSlam_ROS

在ros环境下运行Planar SLAM

为了在实验室平台上运行Planar SLAM，相机使用的是D435,故对源作者的SLAM进行稍作修改，使得在ROS环境下运行。

大家将可将main_ros.cc拷贝至源作者Examples/RGB-D 

1. 在工作空间下创建功能包

     ```c++
     ~/catkin_ws/src
     catkin_create_pkg PlanarSLAM roscpp std_msgs cv_bridge camera_model message_filters
     ```

2. 将我写的main_ros.cc拷贝至源作者Examples/RGB-D 下,并统一拷贝至PlanarSLAM功能包下

3. 运行作者的编译文件

     ```
     .build.sh
     ```

4. 将我上传的CMakeLists.txt替换掉源作者的或者在源作者的CMakeLists.txt 增加如下

     ```
     find_package(catkin REQUIRED COMPONENTS
             roscpp
             std_msgs
             sensor_msgs
             cv_bridge
             camera_model
             message_filters
             )
     include_directories(
             ${catkin_INCLUDE_DIRS}
     )
     
     add_executable(Planar_SLAM_ros Examples/RGB-D/main_ros.cc)
     target_link_libraries(Planar_SLAM_ros ${PROJECT_NAME} ${catkin_LIBRARIES})
     ```

5. 编译

     ```
     cd ~/catkin_ws
     catkin_make 
     ```

6. 运行

     ```c++
     rosrun PlanarSLAM Planar_SLAM_ros /home/wangwen/catkin_ws/src/PlanarSLAM/Vocabulary/ORBvoc.txt /home/wangwen/catkin_ws/src/PlanarSLAM/Examples/RGB-D/TUM1.yaml
     //新建终端
     roslaunch realsense2_camera rs_camera.launch
     ```





