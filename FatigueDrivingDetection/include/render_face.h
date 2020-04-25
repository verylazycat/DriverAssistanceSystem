/*
 * @Author: verylazycat 
 * @Date: 2020-02-06 11:57:30 
 * @Last Modified by: verylazycat
 * @Last Modified time: 2020-02-08 13:19:01
 */
#include <main.h>
// 画线 
void draw_polyline(cv::Mat &img, const dlib::full_object_detection& d, const int start, const int end, bool isClosed = false)
{
	std::vector <cv::Point> points;
	for (int i = start; i <= end; ++i)
	{
		points.push_back(cv::Point(d.part(i).x(), d.part(i).y()));
	}
	cv::polylines(img, points, isClosed, cv::Scalar(255, 0, 0), 2, 16);
}
// 渲染 
void render_face(cv::Mat &img, const dlib::full_object_detection& d)
{
	DLIB_CASSERT
	(
		d.num_parts() == 68,
		"\n\t Invalid inputs were given to this function. "
		<< "\n\t d.num_parts():  " << d.num_parts()
	);
 
	draw_polyline(img, d, 0, 16);           // Jaw line
	draw_polyline(img, d, 17, 21);          // Left eyebrow
	draw_polyline(img, d, 22, 26);          // Right eyebrow
	draw_polyline(img, d, 27, 30);          // Nose bridge
	draw_polyline(img, d, 30, 35, true);    // Lower nose
	draw_polyline(img, d, 36, 41, true);    // Left eye
	draw_polyline(img, d, 42, 47, true);    // Right Eye
	draw_polyline(img, d, 48, 59, true);    // Outer lip
	draw_polyline(img, d, 60, 67, true);    // Inner lip
 
}
