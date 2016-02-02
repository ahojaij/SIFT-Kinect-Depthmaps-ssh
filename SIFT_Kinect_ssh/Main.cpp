#include <stdio.h>
#include <iostream>
#include <fstream>
#include "opencv2/core/core.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/nonfree/features2d.hpp"
#include "opencv2/imgproc/imgproc.hpp"
//#include <windows.h> //For the Sleep(in ms) command

//#include "Kinect Folder/PixelMatchExctraction.h"
//#include "Kinect Folder/3DServoing.h"
#include "ssh Folder/ssh_Send.h"
//#include "Kinect Folder/KinectSensor.h"
//#include <Eigen/LU>
//#include <Eigen/Core>
//#include "Kinect Folder/InteractionMatrix.h"
//#include <fstream>
#include "Kinect Folder\KinectWrapper.h"

#define feature_matching 1
#define depthmap_matching 2

void readme();

float* SIFT_Kinect(KinectWrapper, cv::SurfFeatureDetector, cv::Mat, USHORT *, cv::Mat, USHORT*, std::vector<cv::KeyPoint>, cv::Mat, int type);




/** @function main */
int main( int argc, char** argv )
{
/*	
	if( argc != 3 )
	{
		readme(); return -1;
	}

	cv::Mat img_object = cv::imread( argv[1], CV_LOAD_IMAGE_GRAYSCALE );
	cv::Mat img_scene = cv::imread( argv[2], CV_LOAD_IMAGE_GRAYSCALE );

//	cv::Mat img_object = cv::imread( "10.ppm", CV_LOAD_IMAGE_GRAYSCALE );
//	cv::Mat img_scene = cv::imread( "11.ppm", CV_LOAD_IMAGE_GRAYSCALE );

	if( !img_object.data || !img_scene.data )
	{
		std::cout<< " --(!) Error reading images " << std::endl; //return -1;
	}
*/
	//////////////////////////
	// Grabbing OBJECT image:
	//////////////////////////
	KinectWrapper myKinect;

	/* Assuming a 480x640 Image*/
	int rows = 480;
	int cols = 640;

	unsigned int* color;
	color = myKinect.GetImage_color(); // Reading for the first time just because it was randomly reading far/near images
	color = myKinect.GetImage_color();
	std::cout<<"done reading image";

	cv::Mat img_object;
	cv::Mat img_object_Gray;
	cv::Mat img_scene_temp;
	img_scene_temp = cv::Mat(rows, cols, CV_8UC4, (uchar*)color);
	cv::cvtColor( img_scene_temp, img_object_Gray, CV_RGB2GRAY );
	img_object = img_object_Gray;
//	cv::imshow ("image1", img_object);
//	cv::waitKey(0);
	USHORT* object_depth = myKinect.GetImage_depth();
//	USHORT* object_depth;
//	color = myKinect.GetImage_color();
//	std::cout<<"done reading image2";



	//-- Step 1: Detect the keypoints using SURF Detector
	int minHessian = 400;

	cv::SurfFeatureDetector detector( minHessian );

	std::vector<cv::KeyPoint> keypoints_object;

	detector.detect( img_object, keypoints_object );

	//-- Step 2: Calculate descriptors (feature vectors)
	cv::SurfDescriptorExtractor extractor;

	cv::Mat descriptors_object;

	extractor.compute( img_object, keypoints_object, descriptors_object );


	/*************************************
	Initializing ssh session:
	*************************************/
	std::string Host;
	std::cout<<"Who is your Host: ";
	std::getline(std::cin, Host);

	sshStruct sshStruct_session;
	sshStruct_session = open_session (Host);

	sshStruct_session.rc = ssh_channel_open_session(sshStruct_session.channel);
	if (sshStruct_session.rc < 0)
	{
		close_session (sshStruct_session);
		return 1;
	}

	sshStruct_session.rc = open_interactive_shell_session(sshStruct_session.channel);
	if (sshStruct_session.rc < 0) 
	{
		printf("error : %s\n",ssh_get_error(sshStruct_session.session));
		close_session (sshStruct_session);
		return 1;
	}
	/*************************************
	Calling the interactive shell:
	*************************************/
//	sshStruct_session.rc = interactive_shell_session(sshStruct_session.channel);

	if (sshStruct_session.rc < 0) 
	{
		printf("error : %s\n",ssh_get_error(sshStruct_session.session));
		close_session (sshStruct_session);
		return 1;
	}
	
	/*************************************
	Initializing Control Parameters:
	*************************************/
	int Pixel_x = 600/2;//center of image in x
	int Pixel_y = 363/2;//center of image in y
	int tolerance = 20;
	double vx = 0, vy = 0;//endpoint velocities
	float px_scene = 0, py_scene = 0, px_obj = 0, py_obj = 0;
	float* px_py_scene = new float[2];
	float* vx_vy_scene;// = new float[6];
	cv::Mat img_scene_Gray;
	USHORT * scene_depth;
	scene_depth = object_depth;
	int type = 0;
//	cv::Mat img_scene_temp2;

	std::string cmd;
	int z = 0;

//	cmd=NULL;
	sshStruct_session.rc = interactive_shell_session_send(sshStruct_session.channel,cmd);
	if (sshStruct_session.rc < 0) 
	{
		printf("error : %s\n",ssh_get_error(sshStruct_session.session));
		close_session (sshStruct_session);
		return 1;
	}
//	sshStruct_session.rc = interactive_shell_session_receive(sshStruct_session.channel);
	if (sshStruct_session.rc < 0) 
	{
		printf("error : %s\n",ssh_get_error(sshStruct_session.session));
		close_session (sshStruct_session);
		return 1;
	}

	type = depthmap_matching; // "feature_matching" for feature matching and "depthmap_matching" for depthmap matching
	while(z<3)
	{
		z++;
		//--Start Velocity Controller:

		/*************************************
		Getting Object Loacation:
		*************************************/
		

		
		if (type == feature_matching)
		{
			color = myKinect.GetImage_color();

			std::cout<<"done reading image";
			img_scene_temp = cv::Mat(rows, cols, CV_8UC4, (uchar*)color);
			cv::cvtColor( img_scene_temp, img_scene_Gray, CV_RGB2GRAY );
//			cv::imshow ("image_color", img_scene_temp);
//			cv::imshow ("image", img_scene_Gray);
//			cv::waitKey(0);
	
			px_py_scene = SIFT_Kinect(myKinect, detector, img_scene_Gray, scene_depth, img_object, object_depth, keypoints_object, descriptors_object, type);
			cv::waitKey(0); // To view the image match
//			std::cout<<"x: "<<px_py_scene[0]<<"y: "<<px_py_scene[1]<<std::endl;
//			px_py_scene[0] = 100;
//			px_py_scene[1] = 150;
			px_scene = px_py_scene[0];
			py_scene = px_py_scene[1];
			px_obj = px_py_scene[2];
			py_obj = px_py_scene[3];

		
		
			/*************************************
			Executing Simple Controller:
			*************************************/
			printf("-- Min Index x : %f \n", px_scene);
			printf("-- Min Index y : %f \n", py_scene);

			if (px_scene == 0 || py_scene == 0)
			{
				std::cout<<"No match detected; random search mode: \n";
				vx = 0.5;
				vy = 0.5;
			}
			else
			{
				std::cout<<"Centering object: \n";
				// Image xy-coordinate system (0,0) starts at top left corner
				if (px_scene <=px_obj - tolerance)
				{
					vx = 0.2;
				}
				else if (px_scene >=px_obj + tolerance)
				{
					vx = -0.2;
				}
				else
				{
					vx = 0;
				}
				if (py_scene <=py_obj - tolerance)
				{
					vy = 0.2;
				}
				else if (py_scene >=py_obj + tolerance)
				{
					vy = -0.2;
				}
				else
				{
					vy = 0;
				}
			}
		}
		else if (type == depthmap_matching)
		{
			cv::Mat scene_depth2;
			float factor;
			factor = 255.0/3000.0;
			scene_depth = myKinect.GetImage_depth();
			uchar * scene_depth_temp = new uchar[480*640];
//			float * scene_depth_temp = new float[480*640];


			for (int i = 0; i<480*640; i++)
			{
				scene_depth_temp[i] = scene_depth[i];//*factor;
			}
			
			img_scene_temp = cv::Mat(rows, cols, CV_8UC1, scene_depth_temp);//CV_32F
			scene_depth2 = img_scene_temp;

			std::ofstream output_depth("output_depth_ref.txt");
			for (int i = 0; i<480; i++)
			{
				unsigned char* Mi = scene_depth2.ptr<unsigned char>(i);
				for (int j = 0; j<640; j++)
				{
					output_depth<<(float)Mi[j]<<std::endl;
				}
			}
			std::cout<<"Done Saving ";

			cv::imshow("depth",scene_depth2);
			cv::waitKey(0);

			vx_vy_scene = SIFT_Kinect(myKinect, detector, img_scene_Gray, scene_depth, img_object, object_depth, keypoints_object, descriptors_object, type);
			vx = vx_vy_scene[0];
			vy = vx_vy_scene[1];
		}
		if (vx == 0 && vy == 0)
		{
			type == depthmap_matching;
		}


		printf("-- Velocity in the x : %f \n", vx);
		printf("-- Velocity in the y : %f \n", vy);
		/*************************************
		Calling the interactive shell For Executing Controller: (Needs Modification to send vx and vy)
		*************************************/
//		cmd = "ls";
		std::cout << "Give characters: ";
		std::getline(std::cin, cmd);
		cmd+="\r";
		sshStruct_session.rc = interactive_shell_session_send(sshStruct_session.channel,cmd);
		if (sshStruct_session.rc < 0) 
		{
			printf("error : %s\n",ssh_get_error(sshStruct_session.session));
			close_session (sshStruct_session);
			return 1;
		}
		if (z%5==0)
		{
			sshStruct_session.rc = interactive_shell_session_receive(sshStruct_session.channel);
			if (sshStruct_session.rc < 0) 
			{
				printf("error : %s\n",ssh_get_error(sshStruct_session.session));
				close_session (sshStruct_session);
				return 1;
			}
		}

		
	}
	/*************************************
	Closing ssh session:
	*************************************/
	close_session (sshStruct_session);

	cv::waitKey(0);

	return 0;
}

  /** @function readme */
  void readme()
  { std::cout << " Usage: ./SURF_descriptor <img1> <img2>" << std::endl; }
