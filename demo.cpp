
#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include <Windows.h>


#include <string.h>
#include "CameraLink.h"

using namespace std;
using namespace cv;

int main(int, char**) {

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
		cout << "Logged in" << "\n";
	}
	Mat frame = Mat();

	namedWindow("BGR", 1);

	for (;;)
	{
		if (camlink.ReadFrame(frame))
		{

			imshow("BGR", frame);

			if (cv::waitKey(30) > 0)
			{
				break;
			}

			std::cout << camlink.fps.get() << "\n";
		}
	}


	camlink.Close();




}