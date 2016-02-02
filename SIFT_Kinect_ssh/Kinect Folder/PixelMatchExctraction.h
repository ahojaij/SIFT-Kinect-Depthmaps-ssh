#include <fstream>
#include <stdio.h>
#include <iostream>
#include "opencv2/core/core.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/nonfree/features2d.hpp"

#include "InteractionMatrix.h"

using namespace cv;


/** Get pixel location of best object feature match in the image**/
float* GetPxPy(SurfFeatureDetector, Mat, Mat, std::vector<cv::KeyPoint>, Mat);
float* GetVxVy(USHORT * , USHORT *);

#define feature_matching 1
#define depthmap_matching 2
