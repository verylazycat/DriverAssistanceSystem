/*
 * @Author: verylazycat 
 * @Date: 2020-02-05 14:56:20 
 * @Last Modified by: verylazycat
 * @Last Modified time: 2020-02-09 13:39:19
 */
#include <main.h>
#include <render_face.h>
#include <Compute.h>
#include <BuzzerControl.h>
#include <fatigueDetection.h>
#include <getTime.h>
int main()
{
	//判断的基础信息
	double LeftEyeBase,RightEyeBase,InnerLipBase;
	//标志位,用于第一次录入基础信息
	int Base = 1;
	//记录哈欠次数
	int innerLipCounts = 0;
	//记录眯眼次数
	int squintingCounts = 0;
	try
	{
		//读取摄像头
		cv::VideoCapture cap(0);
		// 窗体大小设置,看需求
		//cap.set(CV_CAP_PROP_FRAME_WIDTH, 640);  
		//cap.set(CV_CAP_PROP_FRAME_HEIGHT, 480);  
		//模型加载 
		frontal_face_detector detector = get_frontal_face_detector();
		shape_predictor pose_model;
		deserialize(datPath) >> pose_model;
 
		int count = 0;
		std::vector<dlib::rectangle> faces;

		while (1)
		{
			cv::Mat img, img_small;
			cap >> img;
			//文字
			cv::putText(img,"s:save,q:quit",cv::Point(img.rows/2,img.cols/2),FONT_HERSHEY_SIMPLEX,2,Scalar(0,0,255),1,8,false);
			cv::resize(img, img_small, cv::Size(), 1.0 / RATIO, 1.0 / RATIO);
 
			cv_image<bgr_pixel> cimg(img);
			cv_image<bgr_pixel> cimg_small(img_small);
 
			//人脸检测   
			if (count++ % SKIP_FRAMES == 0) {
				faces = detector(cimg_small);
			}
			cout << "检测人数:" << faces.size() << endl;
			//无人状态
			if(faces.empty())
			{
				cout  << "\033[31m警告:无人状态\033[0m"<< endl;
				//蜂鸣器功能,后续添加
				// BuzzerControl();
				getCurrentTime();
			}

			// 关键点检测  
			std::vector<full_object_detection> shapes;
			for (unsigned long i = 0; i < faces.size(); ++i) 
			{
				dlib::rectangle r(
					(long)(faces[i].left() * RATIO),
					(long)(faces[i].top() * RATIO),
					(long)(faces[i].right() * RATIO),
					(long)(faces[i].bottom() * RATIO)
				);
				// 关键点保存
				full_object_detection shape = pose_model(cimg, r);
				shapes.push_back(shape);
 
				//渲染画线
				render_face(img, shape);
				double rightEye = RightEyeCompute(img,shape);
				double leftEye = LeftEyeCompute(img,shape);
				double innerLip = InnerLipCompute(img,shape);
				cout<<"-------------"<<endl;

				//第一次识别信息导入,以此为后续判断的标注
				//RightEyeBase,LeftEyeBase,innerLipBase
				if(Base)
				{
					RightEyeBase = rightEye;
					LeftEyeBase = leftEye;
					InnerLipBase = innerLip;
					//退出信息录入,避免后续覆盖
					Base--;
					cout<<"用户数据录入成功>>>初始化成功!"<<endl;
				}
				int temp = fatigueDetection(LeftEyeBase,leftEye,RightEyeBase,rightEye,InnerLipBase,innerLip,innerLipCounts,squintingCounts);
				innerLipCounts += temp;
				squintingCounts += temp;
				//疲劳驾驶
				// innerLipCounts 哈欠次数 需要实际调试确定临界点
				if(innerLipCounts >= 50)
				{
					//红色打印
					cout  << "\033[31m警告:疲劳驾驶\033[0m"<< endl;	
					getCurrentTime();	
				}
				// squintingCounts 眯眼次数
				if(squintingCounts >= 50)
				{
					//红色打印
					cout  << "\033[31m警告:疲劳驾驶\033[0m"<< endl;	
					getCurrentTime();
				}
				//----------------------------------------------------
			}
			// std::cout << "count:" << count << std::endl;
			if (img.empty())
			{
				cerr<<"img is mepty!"<<endl;
				getCurrentTime();
				break;
			}
			imshow("Frame", img);
			// Q q退出
			char c = (char)waitKey(25);
			if (c == 'Q' || c == 'q')
			{
				cout<<"退出成功"<<endl;
				getCurrentTime();
				break;
			}
			if(c == 's'||c == 'S')
			{
				cv::imwrite("../img/img.png",img);
				cout<<"保存成功"<<endl;
				getCurrentTime();
			}
		}
	}
	catch (serialization_error& e)
	{
		cout << "模型下载链接 " << endl;
		cout << "http://dlib.net/files/shape_predictor_68_face_landmarks.dat.bz2" << endl;
		cout << endl << e.what() << endl;
		getCurrentTime();
	}
	catch (exception& e)
	{
		cout << e.what() << endl;
		getCurrentTime();
	}
	return 0;
}