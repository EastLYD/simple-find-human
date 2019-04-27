/*void kinect_detect_object()

{	ros::NodeHandle nh;
	int rate=100;
	int flag=0;
	ros::Rate loop_rate(rate);
	move_base_msgs::MoveBaseGoal goal;
double goal_angle,angle_duration;
	geometry_msgs::Twist vel;
	ros::Publisher v_pub = nh.advertise<geometry_msgs::Twist>("cmd_vel", 50, true);
	float linear_speed,goal_distance,linear_duration;
	String modelConfiguration = "/home/robot/catkin_ws/src/gpsr/include/MobileNetSSD_deploy.prototxt";
	//String modelBinary = "/home/srobot/catkin_ws/src/voice_pkg/src/maybe_last_train.caffemodel";
	String modelBinary = "/home/robot/catkin_ws/src/gpsr/include/MobileNetSSD_deploy.caffemodel";
	Net net = readNetFromCaffe(modelConfiguration, modelBinary);
	int ticks;

	std::string ns = K2_DEFAULT_NS;
	std::string topicColor = K2_TOPIC_QHD K2_TOPIC_IMAGE_COLOR K2_TOPIC_IMAGE_RECT;
	std::string topicDepth = K2_TOPIC_QHD K2_TOPIC_IMAGE_DEPTH K2_TOPIC_IMAGE_RECT;
	bool useExact = true;
	bool useCompressed = false;
	Receiver::Mode mode = Receiver::IMAGE;
	topicColor = "/" + ns + topicColor;
	topicDepth = "/" + ns + topicDepth;
	Receiver receiver(topicColor, topicDepth, useExact, useCompressed);
	receiver.start(mode);
	Mat color, depth;
        int count=0;


	while(flag==0)
	{
		receiver.imageViewer(color, depth, 1);
		Mat inputBlob = blobFromImage(color, inScaleFactor, Size(inWidth, inHeight), Scalar(meanVal, meanVal, meanVal), false, false);
		net.setInput(inputBlob);
		Mat detection = net.forward("detection_out");
		Mat detectionMat(detection.size[2], detection.size[3], CV_32F, detection.ptr<float>());
		float confidenceThreshold = 0.36;

		for (int i = 0; i < detectionMat.rows; i++)
		{
			float confidence = detectionMat.at<float>(i, 2);
			if (confidence > confidenceThreshold)
			{
				size_t objectClass = (size_t)(detectionMat.at<float>(i, 1));
				int tl_x = static_cast<int>(detectionMat.at<float>(i, 3) * color.cols);
				int tl_y = static_cast<int>(detectionMat.at<float>(i, 4) * color.rows);
				int br_x = static_cast<int>(detectionMat.at<float>(i, 5) * color.cols);
				int br_y = static_cast<int>(detectionMat.at<float>(i, 6) * color.rows);
				if(( objectClass==15 )&&( confidence > 0.5 ))//objectClass==15代表是人类的分类
				{
					rectangle(color, Point(tl_x, tl_y), Point(br_x, br_y), Scalar(0, 255, 0), 3);
					putText(color, to_string(confidence), Point(tl_x, tl_y), FONT_HERSHEY_SIMPLEX, 3, Scalar(0, 20, 255), 3);


					const uint16_t *itD = depth.ptr<uint16_t>((int)( 0.5 * (tl_y + br_y) ));
	   				itD += (int)( 0.5 * (tl_x + br_x) );
	    				register const float depthValue = *itD / 1000.0f;//depthValue单位为米

	    				if((0 < depthValue) && (depthValue < 5)) putText(color, to_string(depthValue), Point((int)( 0.5 * (tl_x + br_x) ),
											(int)( 0.5 * (tl_y + br_y) )), FONT_HERSHEY_SIMPLEX, 3, Scalar(180, 105, 255), 3);
					//if(count%30==0)
						//{

						//拍照保存
						if (find_human==0) { imwrite("detect_human01",color);  find_human++}
						else if(find_human==1) {imwrite ("detect_human02",color); find_human}
						else imwrite ("detect_human03",color);

							printf("%d %d %d %d %f \n",tl_x,tl_y,br_x,br_y,depthValue);
							if(depthValue>0.5&&depthValue<2.0)
							{
							float position=(tl_x+br_x)/9.50/2;
							printf("%f\n",position);
							if(position>0&&position<20)
							{
							  goal_angle=3.14/180*30;
							  vel.angular.z = 0.5;
							}
							else if(position>=20&&position<=40)
							{
							  goal_angle=3.14/180*15;
							  vel.angular.z = 0.5;
							}
							else if(position>=60&&position<=80)
							{
							  goal_angle=3.14/180*15;
							  vel.angular.z = -0.5;
							}
							else
							{
							  goal_angle=3.14/180*30;
							  vel.angular.z = -0.5;
							}   // Set the travel distance to 1.0 meters

      							  vel.linear.x = 0;
        						//How long should it take us to get there?
       							 angle_duration =goal_angle*2;
							  ticks = int(angle_duration * rate);

							for(int j=0;j<ticks;j++)
							{
							v_pub.publish(vel);
							loop_rate.sleep();

							}

							vel.linear.x = 0;
							vel.angular.z = 0;
							v_pub.publish(vel);
   							 ros::spinOnce();



                        // Set the travel distance to 1.0 meters
                            goal_distance = depthValue-0.4;
                            vel.linear.x = 0.2;
                        //How long should it take us to get there?
                            linear_duration = goal_distance /vel.linear.x;
                            ticks = int(linear_duration * rate);
                            vel.angular.z = 0;
                        for(int j=0;j<ticks;j++)
                        {
                            v_pub.publish(vel);
                            loop_rate.sleep();

                        }

							vel.linear.x = 0;
							vel.angular.z = 0;
							v_pub.publish(vel);
                            ros::spinOnce();

                            flag=1;
  			        //拍照保存
							}
                  //}
				}
				//检测不到人，旋转
				//String label = format("%s: %.2f", classNames[objectClass], confidence);
				//putText(color, label, Point(tl_x, tl_y), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 255, 0));
			}

		}
            if(flag==0)//检测不到人，旋转
        {
            goal_angle=3.14/180*120;
            vel.angular.z = 0.5;
            vel.linear.x = 0;
            //How long should it take us to get there?
            angle_duration =goal_angle*2;
            ticks = int(angle_duration * rate);

            for(int j=0;j<ticks;j++)
            {
            v_pub.publish(vel);
            loop_rate.sleep();

            }

            vel.linear.x = 0;
            vel.angular.z = 0;
            v_pub.publish(vel);
            ros::spinOnce();
            sleep(1);

        }

		imshow("color",color);
		if(flag==1) break;
	}//while

}*/
