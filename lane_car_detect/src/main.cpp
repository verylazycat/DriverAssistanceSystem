/*
 * @Author: verylazycat 
 * @Date: 2020-04-17 12:39:24 
 * @Last Modified by: verylazycat
 * @Last Modified time: 2020-04-18 13:12:06
 */

#include "yolo_car_detect.h"
#include "lane_detect.h"
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <error.h>

// 摄像头调用
// #define __USE__CAP_0__

//视频测试
#define __USE__VIDEO__

int main(int argc ,char *argv[]) 
{
	
	#ifdef  __USE__CAP_0__
	//摄像头测试
	cv::VideoCapture cap(0);
	#endif

	#ifdef  __USE__VIDEO__
	cv::VideoCapture cap("../video/test.mp4");
	#endif
	
	if (!cap.isOpened())
		return -1;
	//config检测
	configFileDect;
	//导入网络
    Net net = readNetFromDarknet(modelConfiguration, modelWeights);
    net.setPreferableBackend(DNN_BACKEND_OPENCV);
    net.setPreferableTarget(DNN_TARGET_CPU);
    Mat frame, blob;
	//lane检测
	LaneDetector lanedetector; 
	// frame处理
	while (i < 540) {
		//frame为空
		if (!cap.read(frame))
			break;
		//按照预先设定的参数初始化frame
        blobFromImage(frame, blob, 1/255.0, cvSize(inpWidth, inpHeight), Scalar(0,0,0), true, false);
        //将预处理后的frame输入网络
        net.setInput(blob);
        // 前向传播获取outs
        vector<Mat> outs;
        net.forward(outs, getOutputsNames(net));
        // 值信度抑制
		postprocess(frame, outs);
		//time
        vector<double> layersTimes;
        double freq = getTickFrequency() / 1000;
        double t = net.getPerfProfile(layersTimes) / freq;
        string label = format("Inference time for a frame : %.2f ms", t);
        putText(frame, label, Point(0, 15), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 255));
		// 降噪
		img_denoise = lanedetector.deNoise(frame);
		//边缘检测
		img_edges = lanedetector.edgeDetector(img_denoise);
		// ROI
		img_mask = lanedetector.mask(img_edges);
		//霍夫线获取
		lines = lanedetector.houghLines(img_mask);
		if (!lines.empty())
		{
			//区分左右
			left_right_lines = lanedetector.lineSeparation(lines, img_edges);
			//过滤
			lane = lanedetector.regression(left_right_lines, frame);
			//消失点预测方向
			turn = lanedetector.predictTurn();
			//航向输出
			cout<<turn<<endl;
			//渲染
			flag_plot = lanedetector.plotLane(frame, lane, turn);
			cv::waitKey(25);
		}
		else {
			flag_plot = -1;
		}
	}
	return flag_plot;
}