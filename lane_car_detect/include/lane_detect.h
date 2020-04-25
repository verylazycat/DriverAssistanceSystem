/*
 * @Author: verylazycat 
 * @Date: 2020-04-17 18:45:15 
 * @Last Modified by: verylazycat
 * @Last Modified time: 2020-04-17 18:46:14
 */
#include <string>
#include <opencv2/opencv.hpp>
class LaneDetector 
{
private:
	double img_size;
	double img_center;
    // 左边界
	bool left_flag = false;  
    // 右边界
	bool right_flag = false;
    // 车道线方程  
    // y = m*x + b
	cv::Point right_b;  
	double right_m;  
	cv::Point left_b; 
	double left_m;  

public:
    //高斯模糊
	cv::Mat deNoise(cv::Mat inputImage);  
    // 边缘提取
	cv::Mat edgeDetector(cv::Mat img_noise);
    // ROI  
	cv::Mat mask(cv::Mat img_edges);  
    // 霍夫线检测
	std::vector<cv::Vec4i> houghLines(cv::Mat img_mask);  
    //路线提取
	std::vector<std::vector<cv::Vec4i> > lineSeparation(std::vector<cv::Vec4i> lines, cv::Mat img_edges);  
    //车道线获取,一条
	std::vector<cv::Point> regression(std::vector<std::vector<cv::Vec4i> > left_right_lines, cv::Mat inputImage);  
    // 转向决策，消失点决策
	std::string predictTurn();  
    // 渲染绘画
	int plotLane(cv::Mat inputImage, std::vector<cv::Point> lane, std::string turn);  
};

cv::Mat LaneDetector::deNoise(cv::Mat inputImage) {
	cv::Mat output;
	cv::GaussianBlur(inputImage, output, cv::Size(3, 3), 0, 0);
	return output;
}

cv::Mat LaneDetector::edgeDetector(cv::Mat img_noise) {
	cv::Mat output;
	cv::Mat kernel;
	cv::Point anchor;

	//灰度化
	cv::cvtColor(img_noise, output, cv::COLOR_RGB2GRAY);
	// 二值化
	cv::threshold(output, output, 140, 255, cv::THRESH_BINARY);

    // 偏离警告
    // 参考：https://in.mathworks.com/help/vision/examples/lane-departure-warning-system-1.html#d120e8693
	anchor = cv::Point(-1, -1);
	kernel = cv::Mat(1, 3, CV_32F);
	kernel.at<float>(0, 0) = -1;
	kernel.at<float>(0, 1) = 0;
	kernel.at<float>(0, 2) = 1;

	//边缘提取
	cv::filter2D(output, output, -1, kernel, anchor, 0, cv::BORDER_DEFAULT);
	// cv::imshow("output", output);
	return output;
}

cv::Mat LaneDetector::mask(cv::Mat img_edges) {
	cv::Mat output;
    // 创建mask
	cv::Mat mask = cv::Mat::zeros(img_edges.size(), img_edges.type());
	// 定位点
    cv::Point pts[4] = {
		cv::Point(210, 720),
		cv::Point(550, 450),
		cv::Point(717, 450),
		cv::Point(1280, 720)
	};
    // 区域填充
	cv::fillConvexPoly(mask, pts, 4, cv::Scalar(255, 0, 0));
	//图像和mask相乘输出
	cv::bitwise_and(img_edges, mask, output);
	return output;
}

//霍夫直线检测
std::vector<cv::Vec4i> LaneDetector::houghLines(cv::Mat img_mask) {
	std::vector<cv::Vec4i> line;
	HoughLinesP(img_mask, line, 1, CV_PI / 180, 20, 20, 30);

	return line;
}
//路线提取
//过滤line
//左右两条line
std::vector<std::vector<cv::Vec4i> > LaneDetector::lineSeparation(std::vector<cv::Vec4i> lines, cv::Mat img_edges) {
	std::vector<std::vector<cv::Vec4i> > output(2);
	size_t j = 0;
	cv::Point ini;
	cv::Point fini;
    //斜率限制阈值
	double slope_thresh = 0.3;
	std::vector<double> slopes;
	std::vector<cv::Vec4i> selected_lines;
	std::vector<cv::Vec4i> right_lines, left_lines;

	// 斜率计算
	for (auto i : lines) {
		ini = cv::Point(i[0], i[1]);
		fini = cv::Point(i[2], i[3]);

		// m = (y1 - y0)/(x1 - x0)
		double slope = (static_cast<double>(fini.y) - static_cast<double>(ini.y)) / (static_cast<double>(fini.x) - static_cast<double>(ini.x) + 0.00001);

		//如果小于限制阈值，则去除
        //保存大于限制阈值的线
		if (std::abs(slope) > slope_thresh) {
			slopes.push_back(slope);
			selected_lines.push_back(i);
		}
	}

	// 区分左右线
	img_center = static_cast<double>((img_edges.cols / 2));
	while (j < selected_lines.size()) {
		ini = cv::Point(selected_lines[j][0], selected_lines[j][1]);
		fini = cv::Point(selected_lines[j][2], selected_lines[j][3]);

		// 如果斜率大于０且fini.x和fini大于图像中心点,则判断为左侧,反之右侧
		if (slopes[j] > 0 && fini.x > img_center && ini.x > img_center) {
			right_lines.push_back(selected_lines[j]);
			right_flag = true;
		}
		else if (slopes[j] < 0 && fini.x < img_center && ini.x < img_center) {
			left_lines.push_back(selected_lines[j]);
			left_flag = true;
		}
		j++;
	}

	output[0] = right_lines;
	output[1] = left_lines;

	return output;
}

//最终车道线选择
std::vector<cv::Point> LaneDetector::regression(std::vector<std::vector<cv::Vec4i> > left_right_lines, cv::Mat inputImage) {
	std::vector<cv::Point> output(4);
	cv::Point ini;
	cv::Point fini;
	cv::Point ini2;
	cv::Point fini2;
	cv::Vec4d right_line;
	cv::Vec4d left_line;
	std::vector<cv::Point> right_pts;
	std::vector<cv::Point> left_pts;

	// If right lines are being detected, fit a line using all the init and final points of the lines
	if (right_flag == true) {
		for (auto i : left_right_lines[0]) {
			ini = cv::Point(i[0], i[1]);
			fini = cv::Point(i[2], i[3]);

			right_pts.push_back(ini);
			right_pts.push_back(fini);
		}

		if (right_pts.size() > 0) {
			// The right line is formed here
			cv::fitLine(right_pts, right_line, CV_DIST_L2, 0, 0.01, 0.01);
			right_m = right_line[1] / right_line[0];
			right_b = cv::Point(right_line[2], right_line[3]);
		}
	}

	// If left lines are being detected, fit a line using all the init and final points of the lines
	if (left_flag == true) {
		for (auto j : left_right_lines[1]) {
			ini2 = cv::Point(j[0], j[1]);
			fini2 = cv::Point(j[2], j[3]);

			left_pts.push_back(ini2);
			left_pts.push_back(fini2);
		}

		if (left_pts.size() > 0) {
			// The left line is formed here
			cv::fitLine(left_pts, left_line, CV_DIST_L2, 0, 0.01, 0.01);
			left_m = left_line[1] / left_line[0];
			left_b = cv::Point(left_line[2], left_line[3]);
		}
	}

	// One the slope and offset points have been obtained, apply the line equation to obtain the line points
	int ini_y = inputImage.rows;
	int fin_y = 470;

	double right_ini_x = ((ini_y - right_b.y) / right_m) + right_b.x;
	double right_fin_x = ((fin_y - right_b.y) / right_m) + right_b.x;

	double left_ini_x = ((ini_y - left_b.y) / left_m) + left_b.x;
	double left_fin_x = ((fin_y - left_b.y) / left_m) + left_b.x;

	output[0] = cv::Point(right_ini_x, ini_y);
	output[1] = cv::Point(right_fin_x, fin_y);
	output[2] = cv::Point(left_ini_x, ini_y);
	output[3] = cv::Point(left_fin_x, fin_y);

	return output;
}


std::string LaneDetector::predictTurn() {
	std::string output;
	double vanish_x;
	double thr_vp = 10;

	// The vanishing point is the point where both lane boundary lines intersect
	vanish_x = static_cast<double>(((right_m*right_b.x) - (left_m*left_b.x) - right_b.y + left_b.y) / (right_m - left_m));

	// The vanishing points location determines where is the road turning
	if (vanish_x < (img_center - thr_vp))
		output = "Left Turn";
	else if (vanish_x >(img_center + thr_vp))
		output = "Right Turn";
	else if (vanish_x >= (img_center - thr_vp) && vanish_x <= (img_center + thr_vp))
		output = "Straight";

	return output;
}

int LaneDetector::plotLane(cv::Mat inputImage, std::vector<cv::Point> lane, std::string turn) {
	std::vector<cv::Point> poly_points;
	cv::Mat output;

	// Create the transparent polygon for a better visualization of the lane
	inputImage.copyTo(output);
	poly_points.push_back(lane[2]);
	poly_points.push_back(lane[0]);
	poly_points.push_back(lane[1]);
	poly_points.push_back(lane[3]);
	cv::fillConvexPoly(output, poly_points, cv::Scalar(0, 0, 255), CV_AA, 0);
	cv::addWeighted(output, 0.3, inputImage, 1.0 - 0.3, 0, inputImage);

	// Plot both lines of the lane boundary
	cv::line(inputImage, lane[0], lane[1], cv::Scalar(0, 255, 255), 5, CV_AA);
	cv::line(inputImage, lane[2], lane[3], cv::Scalar(0, 255, 255), 5, CV_AA);

	// Plot the turn message
	cv::putText(inputImage, turn, cv::Point(50, 90), cv::FONT_HERSHEY_COMPLEX_SMALL, 3, cvScalar(0, 255, 0), 1, CV_AA);

	// Show the final output image
	cv::namedWindow("Lane", CV_WINDOW_AUTOSIZE);
	cv::imshow("Lane", inputImage);
	return 0;
}
