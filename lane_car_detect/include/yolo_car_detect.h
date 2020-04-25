/*
 * @Author: verylazycat 
 * @Date: 2020-04-17 14:28:03 
 * @Last Modified by: verylazycat
 * @Last Modified time: 2020-04-18 12:12:02
 */
#include <opencv2/dnn.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <fstream>
#include <sstream>
#include <vector>
using namespace cv;
using namespace dnn;
using namespace std;

//置信度
float confThreshold = 0.5;
float nmsThreshold = 0.4;
//图片输入尺寸
//参看cfg文件
int inpWidth = 416;  
int inpHeight = 416; 
//配置文件
const string classesFile = "../config/coco.names";
const string modelConfiguration = "../config/yolov3-tiny.cfg";
const string modelWeights = "../config/yolov3-tiny.weights";
//测试视频
cv::Mat img_denoise;
cv::Mat img_edges;
cv::Mat img_mask;
std::vector<cv::Vec4i> lines;
std::vector<std::vector<cv::Vec4i> > left_right_lines;
std::vector<cv::Point> lane;
std::string turn;
int flag_plot = -1;
int i = 0;
vector<string> classes;
// 删除置信度低的图片
void postprocess(Mat& frame, const vector<Mat>& out);

//画预测框
void drawPred(int classId, float conf, int left, int top, int right, int bottom, Mat& frame);

//获取分类
vector<String> getOutputsNames(const Net& net);

//config检测
size_t configFileDect()
{
	if(classesFile == NULL)
	{
		fprintf(stdout,"classesFile is NULL\n");
		return 1;
	}
	if(modelConfiguration == NULL)
	{
		perror("modelConfiguration");
		return 1;
	}
	if(modelWeights == NULL)
	{
		perror("modelWeights");
		return 1;
	}
}


//抑制低置信度
void postprocess(Mat& frame, const vector<Mat>& outs)
{
    vector<int> classIds;
    vector<float> confidences;
    vector<Rect> boxes;
    // 遍历每一个box
    for (size_t i = 0; i < outs.size(); ++i)
    {
        float* data = (float*)outs[i].data;
        for (int j = 0; j < outs[i].rows; ++j, data += outs[i].cols)
        {
            Mat scores = outs[i].row(j).colRange(5, outs[i].cols);
            Point classIdPoint;
            double confidence;
            //获取score最大和最小值
            minMaxLoc(scores, 0, &confidence, 0, &classIdPoint);
            if (confidence > confThreshold)
            {
                int centerX = (int)(data[0] * frame.cols);
                int centerY = (int)(data[1] * frame.rows);
                int width = (int)(data[2] * frame.cols);
                int height = (int)(data[3] * frame.rows);
                int left = centerX - width / 2;
                int top = centerY - height / 2;
                
                classIds.push_back(classIdPoint.x);
                confidences.push_back((float)confidence);
                boxes.push_back(Rect(left, top, width, height));
            }
        }
    }
    
    vector<int> indices;
    NMSBoxes(boxes, confidences, confThreshold, nmsThreshold, indices);
    for (size_t i = 0; i < indices.size(); ++i)
    {
        int idx = indices[i];
        Rect box = boxes[idx];
        drawPred(classIds[idx], confidences[idx], box.x, box.y,
                 box.x + box.width, box.y + box.height, frame);
    }
}

//绘画渲染
void drawPred(int classId, float conf, int left, int top, int right, int bottom, Mat& frame)
{
    //矩形框
    rectangle(frame, Point(left, top), Point(right, bottom), Scalar(0, 255, 0), 3);
    
    //获取name和置信度
    string label = format("%.2f", conf);
    if (!classes.empty())
    {
        CV_Assert(classId < (int)classes.size());
        label = classes[classId] + ":" + label;
    }
    //说明
    int baseLine;
    Size labelSize = getTextSize(label, FONT_HERSHEY_SIMPLEX, 0.5, 1, &baseLine);
    top = max(top, labelSize.height);
    rectangle(frame, Point(left, top - round(1.5*labelSize.height)), Point(left + round(1.5*labelSize.width), top + baseLine), Scalar(255, 255, 255), FILLED);
    putText(frame, label, Point(left, top), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(0,0,0),1);
}
//获取name
vector<String> getOutputsNames(const Net& net)
{
    static vector<String> names;
    if (names.empty())
    {
        vector<int> outLayers = net.getUnconnectedOutLayers();
        //获取所有name
        vector<String> layersNames = net.getLayerNames();
        //获取当前的name
        names.resize(outLayers.size());
        for (size_t i = 0; i < outLayers.size(); ++i)
        names[i] = layersNames[outLayers[i] - 1];
    }
    return names;
}
