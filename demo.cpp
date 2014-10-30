#include "stdafx.h"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include <Windows.h>

#include "stdafx.h"
#include <string.h>
#include "CameraLink.h"

using namespace std;
using namespace cv;


int main(int, char**) {

	namedWindow("Frame", 1);
	CameraLink camlink = CameraLink("192.168.0.103", "admin", "123456", 8091);

	if (0 == camlink.CameraLinkInit())
	{
		 camlink.CameraLinkLogin(&camlink); 
		 
		 while (camlink.loggedin != 1)
		 {
			 if (camlink.loggedin == -1)
			 {
				 camlink.CameraLinkLogin(&camlink);
			 }
			 cv::waitKey(30);
		 }
		 cout << "Logged in" <<"\n";
	}
	
	Mat frame = Mat();

	for (;;)
	{
		if (camlink.ReadFrame(frame))
		{
			imshow("Frame", frame);
			cv::waitKey(30);
			
			std::cout << camlink.fps.get() << "\n";
		}
	}

	




}