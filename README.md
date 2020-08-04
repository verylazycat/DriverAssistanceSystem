[toc]
[中文](https://github.com/verylazycat/DriverAssistanceSystem/blob/master/README_CN.md)

# DriverAssistanceSystem

> Only for learning and testing

## FatigueDrivingDetection

### Subfunction

- Track line detection (support for keeping track while driving, and use a camera to identify the marking line of the driving track);
- Offset warning;
- Road condition and vehicle condition detection;

## lane_car_detect

### Subfunction

- Unmanned early warning;
- Fatigue driving warning;

# Application scenario

It is applied to the vehicles that are driven in time on the highway, suitable for long-distance passenger rental, etc.;
## the reason

1.Road complexity is low, reducing the difficulty of keeping track (less interference with recognition);
2. The road is longer and the driver is prone to fatigue;
# Technical solutions

## Current development progress

1.Each module is independent of each other (the subsequent integration is required);
2. `The parking module has not been developed yet (for economic and technical reasons, this module needs to use slam technology);`
3. Track maintenance and behavior detection development have been completed;
4. Meet low computing requirements;
5. Simulation test (not completed);

## System integration scheme

Use ROS (Robot Operating System) to integrate all functional modules, based on the Linux platform, across multiple languages (c/c++/python), to solve communication problems between different languages, and to introduce the concept of computational graphs, when all processes and their data Processing will be expressed through a point-to-point network. Each functional module provides a service, and users subscribe to these services through ROS Master, and the communication between services can be coordinated, managed and analyzed by ROS Master.

> Still learning, I will integrate again later...

[Official website](https://www.ros.org)

# Overall flow chart of track line & condition recognition
![http://www.imod.top/img/detect.png](http://www.imod.top/img/detect.png)

The main processing for each frame of input image is as follows:

1.Lane line recognition:： 

- Noise reduction: 
- Edge detection
- ROI processing 
- Hough line acquisition 
- Route acquisition 
- Rendering

2.Vehicle condition recognition： 

- Load yolo network 
- Forward propagation 
- Rendering

# The overall process of driver behavior and state detection

![http://www.imod.top/img/%E9%A9%BE%E9%A9%B6%E5%91%98%E8%A1%8C%E4%B8%BA%E5%8F%8A%E7%8A%B6%E6%80%81%E6%A3%80%E6%B5%8B.png](http://www.imod.top/img/%E9%A9%BE%E9%A9%B6%E5%91%98%E8%A1%8C%E4%B8%BA%E5%8F%8A%E7%8A%B6%E6%80%81%E6%A3%80%E6%B5%8B.png)

The key operations are as follows：

1.Save the status data when the driver starts the program

2.Judge whether driverless

3.Determine whether the current technician status is abnormal based on the recorded status data

# Main operations of lane line recognition

![http://www.imod.top/img/%E8%BD%A6%E9%81%93%E7%BA%BF%E5%A4%84%E7%90%86.png](http://www.imod.top/img/%E8%BD%A6%E9%81%93%E7%BA%BF%E5%A4%84%E7%90%86.png)

# Direction judgment logic

![Direction judgment](http://www.imod.top/img/方向判断.png)

After obtaining the road line, we have obtained two equations:

$$
y_{1}=a_{1}x_{1}+b_{1}\\
y_{2}=a_{2}x_{2}+b_{2}
$$


Calculate the intersection point (x0, y0) of the two lines, which is the vanishing point mentioned in the figure above; In addition, we know the horizontal center line of the image: x;

Therefore, we have the following judgment basis:

- x0 > x + thr_vp :Turn right
- x0 < x - thr_vp:Turn left
- x0 >= (x - thr_vp) && x0 <= (x + thr_vp)：straight line

> Note: thr_vp is an adjustment parameter, debug according to actual situation

![test](http://www.imod.top/img/测试.png)

# Vehicle condition recognition

Target detection and recognition algorithms based on deep learning are roughly divided into the following three categories:

1. Target detection and recognition algorithms based on region recommendations, such as R-CNN, Fast-R-CNN, Faster-R-CNN;

  2. Target detection and recognition algorithms based on regression, such as YOLO, SSD;

  3. Search-based target detection and recognition algorithms, such as AttentionNet based on visual attention;

The YOLO algorithm is used for identification here, and its advantages are as follows:

1. Very fast. The speed on Titan X GPU is 45 fps, and the accelerated version of YOLO is almost 150 fps;

2. YOLO predicts based on the global information of the image;

3. Strong generalization ability;
4.High accuracy；

- Specific algorithm reference yolo paper
- Training reference[DarkNet](https://pjreddie.com/darknet/yolo/)

> Note: Due to the limited computing power of the notebook, the yolo model trained on the open source coco data set is used here to support 80 classifications and identifications. Refer to the configuration file for specific classifications.

![preview](https://pic3.zhimg.com/v2-ee4db90336d60d251d7254f9918c3a48_r.jpg)

# Judgment basis for state characteristics

You need to know the following:

1. "Eye Aspect Ratio" (EAR) We can apply facial landmark detection to locate important areas of the face, including eyes, eyebrows, nose, ears and mouth

2.EAR calculation formula:


$$
EAR=(∣∣p2−p6∣∣+∣∣p3−p5∣∣)/(2∣∣p1−p4∣∣)
$$


![eyeEar](http://www.imod.top/img/eyeEar.png)

> For details, please refer to Soukupová and Čech's 2016 paper Real-Time Eye Blink Detection using Facial Landmarks

## Landmark algorithm

Landmark is a technology for extracting facial feature points. The Dlib library has 68 points for the face mark. In the article "Calling Dlib Library for Face Key Point Marking", there is a schematic diagram of the effect and the calibration point number. In the future, the points in the landmark can be used to extract the eye area and the mouth area for fatigue detection, and the nose and other parts can be used for 3D pose estimation. The Dlib library uses the algorithm mentioned in "One Millisecond Face Alignment with an Ensemble of Regression Trees" CVPR2014: ERT (ensemble of regression trees) cascade regression, which is a regression tree method based on gradient enhancement learning. The algorithm uses cascading regression factors. First, it needs to use a series of calibrated face images as the training set, and then it will generate a model. Use the correlation method based on feature selection to project the target output into a random direction w, and select a pair of features (u, v) so that Ii(u)-Ii(v) and the projected target wTri are on the training data Has the highest sample correlation. When a picture is obtained, the algorithm will generate an initial shape, which is to first estimate a rough feature point position, and then use the gradient boosting algorithm to reduce the sum of the square error of the initial shape and ground truth. Use the least square method to minimize the error, and get the cascaded regression factor for each level. The core formula is as follows:
$$
\hat{S}^{t+1}=\hat{S}+r_{t}(I,\hat{t}^{t})
$$

Shows the current level of regressor. The input parameters of the regressor are image I and the updated shape of the previous regressor, and the features used can be gray values or other. Each regressor is composed of many trees, and the parameters of each tree are trained according to the coordinate difference between current shape and ground truth and randomly selected pixel pairs.

Unlike LBF, ERT directly stores the updated value ΔS of shape in the leaf node leaf node during the process of learning Tree. The initial position S is after passing all the learned trees, and the meanshape adds all the leaf nodes that have passed. ΔS, the final position of the key points of the face can be obtained.

# EAR reference meaning
"Eye Aspect Ratio" (EAR): We can apply facial landmark detection to locate important areas of the face, including eyes, eyebrows, nose, ears and mouth.

1. Where p1...p6 are the 2D facial landmark positions; 2. The numerator of the equation is to calculate the distance between vertical eye signs, and the denominator is to calculate the distance between horizontal eye signs, because there is only one set of horizontal points, but there are two sets Vertical point, so weighted denominator
![EAR](http://www.imod.top/img/EAR.png)

> When the human eyes are closed, the EAR decreases sharply. We use this to detect the state of human eyes closed.
![EARTEST](http://www.imod.top/img/EARTEST.png)

# Distortion correction

After turning on the camera, the first thing to do beforehand distortion correction (not added yet).
Use the method provided by opencv to calculate the camera calibration matrix and distortion coefficient through the checkerboard picture group
(distortion coefficients).
Correspondingly, please refer to this article [blog](https://blog.csdn.net/qq_36342854/article/details/88933308)

# Improve the accuracy of road line recognition

Currently, the traditional digital image processing technology is used to identify the road. Although the amount of calculation is small, its generalization is very poor and its anti-interference ability is poor.
The defects are as follows:
First, the visual system is very sensitive to background light, such as a tree-lined road with strong sunlight. The line of the road is divided into pieces by the light, making it impossible to
Extract the track line.
Second, the visual system requires complete road markings. For some roads that have been in disrepair for a long time, the road markings are not obvious and incomplete, and some
The same is true for the road that has just been opened for a few years.
Third, the vision system needs to have a unified format of track lines, which is especially important for systems that identify track lines according to the model library. Some track lines
The format is very peculiar, such as blue track lines and very narrow track lines. The model library must travel all over the country to integrate these peculiar tracks one by one.
Included can ensure smooth detection.
Fourth, the vision system cannot correspond to low-light environments, especially in the dark night without street lights. Generally LKW (Lane keep assistance) requires
It is necessary to start at a speed above 72 kilometers per hour. One reason is that people will not easily change lanes when the speed is relatively high. Another reason is that it is relatively low.
The high speed means that the sampling points of the vision system are insufficient, and the accuracy of the fitted track is low.
Fifth, if the surface of the track is covered by water, the visual system will be completely ineffective.
The specific solutions are as follows:
1. Based on the laser radar echo width;
2. A grayscale image formed based on the reflected intensity information of the lidar, or the combination of intensity information and elevation information to filter out invalid information;
3. Lidar SLAM works with high-precision maps to not only detect track lines but also perform self-positioning;
4. Use lidar to obtain different characteristics of road edge height information or physical reflection information, and detect the road edge first, because the road width is
It is known that the position of the track line is calculated based on the distance. For some roads with a difference of less than 3 cm between the road edge and the road, this method
Not available.
The latter three methods require multi-line lidar, and at least 16-line lidar. The former can use 4-line or single-line lidar;

# Performance improvement

-Improve hardware performance, expand memory, and use high-performance GPU;
-Use TensorRt framework

>TensorRT is a high-performance deep learning inference optimizer that can provide low
>Delay and high throughput deployment reasoning. TensorRT can be used for super large-scale data centers, embedded platforms or autonomous driving
>The platform performs inference acceleration. TensorRT can now support almost all TensorFlow, Caffe, Mxnet, Pytorch, etc.
> Some deep learning frameworks combine TensorRT and NVIDIA GPUs, which can perform fast in almost all frameworks.
>Quick and efficient deployment reasoning
# Import map concept

> Not implemented

Under the condition of lidar, apply slam technology to build a space map

# Installation configuration

-ROS

> Currently not used, do not install
- opencv

> version:3.4.5

- Dlib

> Used to detect driver fatigue

-Compile the program

```bash
cd FatigueDrivingDetection
mkdir build
cd build/
cmake .. -DCMAKE_BUILD_TYPE=Release | tee ../log/cmake_log
make -j |tee ../log/make_log
chmod +x FatigueDrivingDetection
./FatigueDrivingDetection
```

```bash
cd lane_car_detect/
mkdir build
cd build
cmake ..
make
chmod +x lane_car_detect
./lane_car_detect
```

Reference [blog](http://www.imod.top/_posts/2020-07-01-%E8%BD%A6%E9%81%93%E4%BF%9D%E6%8C%81&%E7%96%B2%E5%8A%B3%E9%A9%BE%E9%A9%B6%E6%A3%80%E6%B5%8B/)
