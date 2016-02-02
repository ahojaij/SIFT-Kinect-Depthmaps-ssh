#include <stdio.h>
#include <iostream>
#include <fstream>
#include "opencv2/core/core.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/nonfree/features2d.hpp"
#include "opencv2/imgproc/imgproc.hpp"


#include "PixelMatchExctraction.h"
//#include "3DServoing.h"
#include "KinectWrapper.h"

using namespace cv;

float* SIFT_Kinect(KinectWrapper myKinect, SurfFeatureDetector detector, Mat img_scene_Gray, USHORT * scene_depth, Mat img_object, USHORT* object_depth, std::vector<cv::KeyPoint> keypoints_object, Mat descriptors_object, int type)
{
	if (type == feature_matching)
	{
		float* info = new float[4];
		info = GetPxPy(detector, img_scene_Gray, img_object, keypoints_object, descriptors_object);
//		return px_py_scene;
//		float* info = new float[2];
//		info[0] = px_py_scene[0];
//		info[1] = px_py_scene[1];
//		info[0] = px_py_scene[2];
//		info[1] = px_py_scene[3];
		
//		std::cout<<"x: "<<info[0]<<"y: "<<info[1]<<std::endl;
		return info;
	}
	else if (type == depthmap_matching)
	{
		float * vx_vy_scene = new float[6];
		vx_vy_scene = GetVxVy(scene_depth, object_depth);
		return vx_vy_scene;
	}






/*
//	Assuming a 480x640 Image
	int rows = 480;
	int cols = 640;
	
	float* px_py_scene = new float[2];
	float * vx_vy_scene = new float[6];
	
	if (type == feature_matching)
	{
		unsigned int* color;
		color = myKinect.GetImage_color();

		std::cout<<"done reading image";

		Mat img_scene_temp = Mat(rows, cols, CV_8UC4, (uchar*)color);
		Mat img_scene_Gray;
		cvtColor( img_scene_temp, img_scene_Gray, CV_RGB2GRAY );
		imshow ("image", img_scene_Gray);
	
		px_py_scene = GetPxPy(detector, img_scene_Gray, img_object, keypoints_object, descriptors_object);
		return px_py_scene;
	}
	else if (type == depthmap_matching)
	{
		USHORT * scene_depth = myKinect.GetImage_depth();
		vx_vy_scene = GetVxVy(scene_depth, object_depth);
		return vx_vy_scene;
	}*/
}
