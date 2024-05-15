#include <librealsense2/rs.hpp>
#include <opencv2/opencv.hpp>
#include <fstream>
#include <chrono>
#include <thread>
#include <iostream>

using namespace std;

class Data
{
	public:
		double distance;
		int x;
		int y;
		friend ostream& operator<<(ostream& output, Data &d)
		{
			output << d.distance << " " << d.x << " " << d.y << std::endl;
			return output;
		}
};



int main() 
{
    // Declare RealSense pipeline, context, and depth sensor
    rs2::pipeline pipe;
    rs2::config cfg;
    cv::Mat hsv, blur, gray, thresholded, result;
    cv::Mat gray_show, color_show, depth_show;
    cv::Mat yellowMask, redMask;
    cv::Scalar c(0,255,0);
    rs2::colorizer color_map;
    bool found = false;
    Data d;
    
		int count = 0;
		
		// Output File
		std::ofstream file("crater.txt");

    // Enable depth stream and color stream and configure pipeline
    cfg.enable_stream(RS2_STREAM_DEPTH, 640, 480, rs2_format::RS2_FORMAT_Z16, 30);
    cfg.enable_stream(RS2_STREAM_COLOR, 640, 480, rs2_format::RS2_FORMAT_BGR8, 30);
    pipe.start(cfg);
 
    
    while (cv::waitKey(1) < 0) 
    {
	
    	rs2::frameset frames = pipe.wait_for_frames();
    
	   	auto depth_frame = frames.get_depth_frame();
	   	auto color_frame = frames.get_color_frame();
		

	    auto colorized_depth = color_map.process(depth_frame);
	    
			cv::Mat depth_map(cv::Size(640, 480), CV_8UC3, (void*)colorized_depth.get_data(), cv::Mat::AUTO_STEP);
			cv::Mat color(cv::Size(640, 480), CV_8UC3, (void*)color_frame.get_data(), cv::Mat::AUTO_STEP);
	    
	    //cv::imwrite("images/depth_map.jpg", depth_map); 
	    cv::medianBlur(depth_map, blur, 9);
	    //cv::imwrite("images/blur.jpg", blur);
	    cv::cvtColor(blur, hsv, cv::COLOR_BGR2HSV);
	    //cv::imwrite("images/hsv_map.jpg", hsv);
	    
	    cv::Scalar lowerYellow(20, 100, 100);  // Lower bound for yellow in HSV
	    cv::Scalar upperYellow(30, 255, 255);  // Upper bound for yellow in HSV

	    cv::Scalar lowerRed1(0, 100, 100);     // Lower bound for red in HSV (hue < 10)
	    cv::Scalar upperRed1(10, 255, 255);    // Upper bound for red in HSV (hue < 10)

	    cv::Scalar lowerRed2(160, 100, 100);   // Lower bound for red in HSV (hue > 160)
	    cv::Scalar upperRed2(179, 255, 255);   // Upper bound for red in HSV (hue > 160)
	    
	    cv::inRange(hsv, lowerYellow, upperYellow, yellowMask);
	    cv::inRange(hsv, lowerRed1, upperRed1, redMask);
	    cv::inRange(hsv, lowerRed2, upperRed2, redMask);

	    // Combine the masks to get regions with yellow and red colors
	    cv::Mat yellowRedMask = yellowMask | redMask;

	    
	   // cv::imwrite("images/yello_red.jpg", yellowRedMask);
	    // Apply the mask to the original image
	    
	    
	    cv::Mat segmentedImage;
	    blur.copyTo(segmentedImage, yellowRedMask);
	   // cv::imwrite("Segmented.jpg", segmentedImage);
			cv::cvtColor(segmentedImage, gray, cv::COLOR_BGR2GRAY);
	    
	    //cv::imwrite("images/gray.jpg", gray);
	    cv::Rect roi(0,0,gray.cols, gray.rows/2);
	    gray(roi)=cv::Scalar(0);
			
			cv::threshold(gray, thresholded, 1, 255, cv::THRESH_BINARY);
			
			//cv::imwrite("images/thresholded.jpg", thresholded);
			std::vector<std::vector<cv::Point>> contours;
			
			cv::findContours(thresholded, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

			// Filter contours based on area
			double maxArea = 0;
			double maxAreaIdx = -1; 
			for (size_t i=0; i< contours.size(); i++) {
				  double area = cv::contourArea(contours[i]);
				  if (area > maxArea) {
				      maxArea = area;
				      maxAreaIdx = static_cast<int>(i);
				  }
			}
			
			cv::Mat mask = cv::Mat::zeros(thresholded.size(), CV_8UC1);
			
			cv::drawContours(mask, contours, maxAreaIdx, cv::Scalar(255), cv::FILLED);
			

			cv::bitwise_and(thresholded, mask, result);
			//cv::imwrite("images/thresh.jpg", thresholded);
					
			cv::Moments mu = cv::moments(contours[maxAreaIdx]);
			cv::Point centroid(mu.m10/mu.m00, mu.m01/mu.m00);
			//cv::imwrite("images/colorin.jpg", color);
			
		
			found = false;
			count = 0;
			
			for(int i = centroid.y; count < 10; i--, count++) 
			{	
				int point = result.at<uchar>(i, centroid.x);
				if(point == 0)
				{
					found = true;
				}
				else if(count > 5 && point == 1)
				{
					found = false;
					break;
				}
				
			}
			
			
			if(found)
			{
				cv::circle(color, centroid, 10, c, 2);
				d.x = centroid.x;
				d.y = centroid.y;
				d.distance = depth_frame.get_distance(centroid.x, centroid.y);
			}
			
			else
			{
				d.x = 0;
				d.y = 0;
				d.distance = 0;
			}
			
			file.seekp(0);
			
			if(d.x == 0)
			{
				file << "S";
			}
			else if(d.x > 320)
			{
				file << "L";
			}
			else
			{
				file << "R";
			}
			
			file.flush();
			
			
			
			
			
			cv::resize(color, color_show, cv::Size(320, 240));
			cv::resize(result, gray_show, cv::Size(320, 240));
			
			
			//cv::imwrite("images/color.jpg", color);
			//cv::imwrite("images/result.jpg", result);
			//cv::imshow("color",color_show);
			cv::imshow("gray", gray_show);

		
		}
		file.close();
		  
    


    // Stop the pipeline and release resources
    pipe.stop();

    return 0;
}
