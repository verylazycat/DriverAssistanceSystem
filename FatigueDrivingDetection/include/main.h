/*
 * @Author: verylazycat 
 * @Date: 2020-02-06 11:26:56 
 * @Last Modified by: verylazycat
 * @Last Modified time: 2020-02-09 13:36:59
 */
#ifndef __BUILD__
#define __BUILD__
#include <dlib/opencv.h>  
#include <opencv2/opencv.hpp>  
#include <dlib/image_processing/frontal_face_detector.h>  
#include <dlib/image_processing/render_face_detections.h>  
#include <dlib/image_processing.h>  
#include <dlib/gui_widgets.h>  
#include "opencv2/opencv.hpp"
#include <iostream>
#include <ctime>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>

using namespace dlib;
using namespace std;
using namespace cv;

#define RATIO 4  
#define SKIP_FRAMES 2 

cv::String datPath = "../config/shape_predictor_68_face_landmarks.dat";

#endif