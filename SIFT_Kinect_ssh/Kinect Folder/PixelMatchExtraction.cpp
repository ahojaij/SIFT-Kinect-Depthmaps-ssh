#include "PixelMatchExctraction.h"

float* GetPxPy(SurfFeatureDetector detector, Mat img_scene, Mat img_object, std::vector<cv::KeyPoint> keypoints_object, Mat descriptors_object)
  {
	  // Mat img_scene = cv::imread( argv[2], CV_LOAD_IMAGE_GRAYSCALE );
	  //-- Step 1: Detect the keypoints using SURF Detector

	  std::vector<cv::KeyPoint> keypoints_scene;
	  detector.detect( img_scene, keypoints_scene );

	  //-- Step 2: Calculate descriptors (feature vectors)
	  cv::SurfDescriptorExtractor extractor;
	  cv::Mat descriptors_scene;
	  extractor.compute( img_scene, keypoints_scene, descriptors_scene );

	  //-- Step 3: Matching descriptor vectors using FLANN matcher
	  cv::FlannBasedMatcher matcher;
	  std::vector< cv::DMatch > matches;
	  matcher.match( descriptors_object, descriptors_scene, matches );

	  double max_dist = 0; double min_dist = 100;

	  //-- Quick calculation of max and min distances between keypoints
	  for( int i = 0; i < descriptors_object.rows; i++ )
	  { double dist = matches[i].distance;
		if( dist < min_dist ) min_dist = dist;
		if( dist > max_dist ) max_dist = dist;
	  }

	  printf("-- Max dist : %f \n", max_dist );
	  printf("-- Min dist : %f \n", min_dist );

	  //-- Draw only "good" matches (i.e. whose distance is less than 3*min_dist )

	  cv::Mat img_matches;
	  std::vector< cv::DMatch > good_matches;
	  for( int i = 0; i < descriptors_object.rows; i++ )
	  {
		  if( matches[i].distance < 2*min_dist )
		  { good_matches.push_back( matches[i]); }
	  }

	  drawMatches( img_object, keypoints_object, img_scene, keypoints_scene,
				   good_matches, img_matches, cv::Scalar::all(-1), cv::Scalar::all(-1),
				   cv::vector<char>(), cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS );
/*
	  //-- Localize the object
	  std::vector<cv::Point2f> obj;
	  std::vector<cv::Point2f> scene;

	  for( int i = 0; i < good_matches.size(); i++ )
	  {
		//-- Get the keypoints from the good matches
		obj.push_back( keypoints_object[ good_matches[i].queryIdx ].pt );
		scene.push_back( keypoints_scene[ good_matches[i].trainIdx ].pt );
	  }

	  cv::Mat H = cv::findHomography( obj, scene, CV_RANSAC );

	  //-- Get the corners from the image_1 ( the object to be "detected" )
	  std::vector<cv::Point2f> obj_corners(4);
	  obj_corners[0] = cvPoint(0,0); obj_corners[1] = cvPoint( img_object.cols, 0 );
	  obj_corners[2] = cvPoint( img_object.cols, img_object.rows ); obj_corners[3] = cvPoint( 0, img_object.rows );
	  std::vector<cv::Point2f> scene_corners(4);

	  perspectiveTransform( obj_corners, scene_corners, H);
  
	  //-- Draw lines between the corners (the mapped object in the scene - image_2 )
	  line( img_matches, scene_corners[0] + cv::Point2f( img_object.cols, 0), scene_corners[1] + cv::Point2f( img_object.cols, 0), cv::Scalar(0, 255, 0), 4 );
	  line( img_matches, scene_corners[1] + cv::Point2f( img_object.cols, 0), scene_corners[2] + cv::Point2f( img_object.cols, 0), cv::Scalar( 0, 255, 0), 4 );
	  line( img_matches, scene_corners[2] + cv::Point2f( img_object.cols, 0), scene_corners[3] + cv::Point2f( img_object.cols, 0), cv::Scalar( 0, 255, 0), 4 );
	  line( img_matches, scene_corners[3] + cv::Point2f( img_object.cols, 0), scene_corners[0] + cv::Point2f( img_object.cols, 0), cv::Scalar( 0, 255, 0), 4 );
*/  
  
  
	  //-- Show detected matches
	  cv::imshow( "Good Matches & Object detection", img_matches );


	  //-- Controller condition for image matching

	  //-- Match detection condition
	  double sumtest = 0, Averagesumtest = 0;
	  int j = 0;
	  int MinIndex = 0;
	  float px_obj = 0, py_obj = 0, px_scene = 0, py_scene = 0;
	  if (min_dist <= 1.2e-1)
	  {
		  for( int i = 0; i < descriptors_object.rows; i++ )
		  {
			  if( matches[i].distance < 2*min_dist)
			  {
				 sumtest = sumtest + matches[i].distance ;
				 j++;
			  }
		  }
		  Averagesumtest = sumtest/j;
		  printf("-- SumTest : %f \n", sumtest );
		  printf("-- Average SumTest : %f \n", sumtest/j );

		  //-- Position of minumum distance
//		  if (Averagesumtest <= 1e-1)
		  {
			  for( int i = 0; i < descriptors_object.rows; i++ )
			  {
				  if( matches[i].distance == min_dist)
				  {
					  px_obj = keypoints_object[i].pt.x;
					  py_obj = keypoints_object[i].pt.y;
					  std::vector<cv::Point2f> scene2;
					  scene2.push_back( keypoints_scene[ matches[i].trainIdx ].pt );
					  px_scene = scene2[0].x;
					  py_scene = scene2[0].y;
					  std::cout<<"x_scene: "<<px_scene<<"y_scene: "<<py_scene<<std::endl;
					  std::cout<<"x_object: "<<px_obj<<"y_object: "<<py_obj<<std::endl;
				  }
			  }
		  }
	  }
	  float* info = new float[4];
	  info[0] = px_scene;
	  info[1] = py_scene;
	  info[2] = px_obj;
	  info[3] = py_obj;
	  return info;
  }



float* GetVxVy(USHORT * object_depth, USHORT * scene_depth)
{
	
//	std::ofstream output_depth_error("output_depth_error.txt");
	USHORT * depth_error = new USHORT[640*480];

    for(int i=0;i<640*480;i++)
    {
		depth_error[i] = scene_depth[i]-object_depth[i];
//       output_depth_error<<depth_error[i]<<std::endl;
    }

//	std::cout<<"Done Saving ";
	double L_x[6], L_y[6], L_P_Z[6], dZ_dx = 0, dZ_dy = 0;
	double ** L_Z;
	L_Z = new double*[640*480];
	for(int i=0; i<640*480; ++i)
	{
		L_Z[i] = new double[6];
	}

	for(int i=0; i<480*640; ++i)
	{
		if (scene_depth[i] == 0)
			scene_depth[i] = 1;
	}
	std::cout<<"Done Checking for Zero ";

	double ** L_Z_trans;
	L_Z_trans = new double*[6];
	for(int i=0; i<6; ++i)
		L_Z_trans[i] = new double[640*480];

	double ** L_Z_plus;
	L_Z_plus = new double*[6];
	for(int i=0; i<6; ++i)
		L_Z_plus[i] = new double[640*480];

	double ** L_Z_trans_L_Z;
	L_Z_trans_L_Z = new double*[6];
	for(int i=0; i<6; ++i)
		L_Z_trans_L_Z[i] = new double[6];

	Eigen::MatrixXd L_Z_trans_Eigen(6,640*480);

	Eigen::MatrixXd L_Z_trans_L_Z_Eigen(6,6);

	Eigen::MatrixXd L_Z_plus_Eigen(6,480*640);

	L_Z_plus_Eigen = getInteractionMatrix(scene_depth, L_x, L_y, L_P_Z, dZ_dx, dZ_dy, L_Z, L_Z_trans, L_Z_trans_Eigen, L_Z_trans_L_Z, L_Z_trans_L_Z_Eigen, L_Z_plus_Eigen);
	std::cout<<"Done L_Z_plus_Eigen ";

	
	Eigen::VectorXd depth_error_Eigen(480*640);
	for (int i = 0; i<480*640; ++i)
	{
		depth_error_Eigen(i) = depth_error[i];
	}

	double lambda = 2.5;

	Eigen::VectorXd v(6);
	v = -lambda*L_Z_plus_Eigen*depth_error_Eigen;

//	double x;
//	std::cin>>x;
//	delete depth, depth2, color, color2, depth_error;
//	for(int i=0; i<640*480; ++i)
//		delete L_Z[i];
	
	std::cout<<"All Done ";
	float* info = new float[6];
	info[0] = v[0];
	info[1] = v[1];
	info[2] = v[2];
	info[3] = v[3];
	info[4] = v[4];
	info[5] = v[5];

	std::cout<<"Velocity Values: "<<v[0]<<" "<<v[1]<<" "<<v[2]<<" "<<v[3]<<" "<<v[4]<<" "<<v[5]<<" "<<std::endl;

	std::cout<<"Velocity Values: "<<info[0]<<" "<<info[1]<<" "<<info[2]<<" "<<info[3]<<" "<<info[4]<<" "<<info[5]<<" "<<std::endl;
	return info;

}