#include <librealsense2/rs.hpp>
#include <opencv2/opencv.hpp>
#include <fstream>

cv::Scalar redColor(0, 0, 255);
cv::Scalar green(0,255,0);


int count = 0;
int main(int argc, char **argv)
{
	rs2::pipeline pipe;
  rs2::config cfg;
  cfg.enable_stream(RS2_STREAM_COLOR, 640, 480, rs2_format::RS2_FORMAT_BGR8, 30);
  cfg.enable_stream(RS2_STREAM_DEPTH, 640, 480, rs2_format::RS2_FORMAT_Z16, 30);
  pipe.start(cfg);
  
  std::ofstream file("sample.txt");
  
  cv::Scalar lowerRed1(0, 100, 100);     // Lower bound for red in HSV (hue < 10)
	cv::Scalar upperRed1(10, 255, 255);    // Upper bound for red in HSV (hue < 10)

	cv::Scalar lowerRed2(160, 100, 100);   // Lower bound for red in HSV (hue > 160)
	cv::Scalar upperRed2(179, 255, 255);   // Upper bound for red in HSV (hue > 160)

  cv::Mat result;
  
  while(cv::waitKey(1) < 0)
  {
  	rs2::frameset frames = pipe.wait_for_frames();
	  auto color_frame = frames.get_color_frame();
	  cv::Mat color(cv::Size(640, 480), CV_8UC3, (void*)color_frame.get_data(), cv::Mat::AUTO_STEP);
	  
	  
	  
	  cv::Mat hsv, mask1;
    cv::cvtColor(color, hsv, cv::COLOR_BGR2HSV);
    
    std::vector<cv::Mat> channels;
    cv::split(hsv, channels);
    
    channels[2] += 50;
    
    cv::merge(channels, hsv);
    
   	cv::Mat enhanced;
   	cv::cvtColor(hsv, enhanced, cv::COLOR_HSV2BGR);
    
	  cv::inRange(hsv, lowerRed1, upperRed1, mask1);
	  cv::inRange(hsv, lowerRed2, upperRed2, mask1);
	  
	 
	  std::vector<std::vector<cv::Point>> contours;
		cv::findContours(mask1, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

		double maxArea = 0;
		double maxAreaIdx = -1; 
		for (size_t i=0; i< contours.size(); i++) {
			  double area = cv::contourArea(contours[i]);
			  if (area > maxArea) {
			      maxArea = area;
			      maxAreaIdx = static_cast<int>(i);
			  }
		}
		
		
		cv::Mat mask2 = cv::Mat::zeros(mask1.size(), CV_8UC1);
		
		cv::drawContours(mask2, contours, maxAreaIdx, cv::Scalar(255), cv::FILLED);
		
		cv::bitwise_and(mask1, mask2, result);
	  file.seekp(0);
		if(maxAreaIdx != -1)
		{
			cv::Moments mu = cv::moments(contours[maxAreaIdx]);
			cv::Point centroid(mu.m10/mu.m00, mu.m01/mu.m00);
			cv::circle(color, centroid, 10, cv::Scalar(0,255,0), 2);
			
			int point = centroid.x;
		
			
			if(point > 270 && point < 370)
			{
				
				auto depth_frame = frames.get_depth_frame();
				double distance = depth_frame.get_distance(centroid.x, centroid.y)*100;
				std::cout << distance << std::endl;
				if(distance*100 < 50)
				{
					file << "STOP";
					count++;
				}
				file << "S";
				
				
			}
			else if(point > 370)
			{
				file << "R";
			}
			else
			{
				file << "L";
			}
		}	
		
		cv::line(color, cv::Point(270,0), cv::Point(270, 480), green, 2);
		cv::line(color, cv::Point(370,0), cv::Point(370, 480), green, 2);
		
		
		
		file.flush();
 
    cv::imshow("color", color);

  }
  pipe.stop();
  return 0;
 }
  
  
  
    
	   	








	



    
   
    

