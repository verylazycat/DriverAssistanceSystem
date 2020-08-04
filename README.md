[toc]

# DriverAssistanceSystem

> 仅用于学习,测试使用.

## FatigueDrivingDetection

### 子功能

- ⻋道线检测(对行驶时保持⻋道提供支持,借助一个摄像头识别行驶⻋道的标志线);
- 偏移警告;
- 路况⻋辆状况检测;

## lane_car_detect

### 子功能

- 无人驾驶预警;
- 疲劳驾驶预警;

# 应用场景

应用于 `高速路 `时间驾驶的⻋辆,适用于⻓途客出租⻋等等;

## 原因

1. 道路复杂度低,降低⻋道保持难度(对识别的干扰较少);
2. 路途较⻓,驾驶人员容易疲劳;

# 技术方案

## 当前开发进度状况

1. 各模块相互独立(后续需要整合);
2. `泊⻋模块暂未开发(涉及经济和技术原因,此模块需要使用 slam 技术);`
3. ⻋道保持和行为检测开发已完成;
4. 满足低运算要求;
5. 仿真测试(未完成);

## 系统整合方案

使用 ROS (机器人操作系统)去整合所有功能模块,基于 Linux 平台,跨多语言(c/c++/python)实现,解决不同语言间通讯问题,引入 计算图 概念,当所有进程以及他们所进行的数据处理,将会通过一种点对点的网络形式表现出来。各个功能模块提供一个服务,用戶通过 ROS Master 去 订阅 这些服务,且服务之间的通讯可由 ROS Master协调管理解析。

> 还在学习中,以后有空再整合...

[官网](https://www.ros.org)

# ⻋道线&⻋况识别总体流程图

![http://www.imod.top/img/detect.png](http://www.imod.top/img/detect.png)

对输入的每一帧图像主要有如下处理：

1.车道线识别： 

- 降噪 
- 边缘检测
- ROI处理 
- 霍夫线获取 
- 路线获取 
- 渲染

2.车况识别： 

- 加载yolo网络 
- 前向传播 
- 渲染

# 驾驶员行为及状态检测总体流程

![http://www.imod.top/img/%E9%A9%BE%E9%A9%B6%E5%91%98%E8%A1%8C%E4%B8%BA%E5%8F%8A%E7%8A%B6%E6%80%81%E6%A3%80%E6%B5%8B.png](http://www.imod.top/img/%E9%A9%BE%E9%A9%B6%E5%91%98%E8%A1%8C%E4%B8%BA%E5%8F%8A%E7%8A%B6%E6%80%81%E6%A3%80%E6%B5%8B.png)

关键操作有如下：

1.保存驾驶人员启动程序时的状态数据

2.判断是否无人驾驶

3.依据记录的状态数据判断当前技术人员状态是否异常

# 车道线识别主要操作

![http://www.imod.top/img/%E8%BD%A6%E9%81%93%E7%BA%BF%E5%A4%84%E7%90%86.png](http://www.imod.top/img/%E8%BD%A6%E9%81%93%E7%BA%BF%E5%A4%84%E7%90%86.png)

# 方向判断逻辑

![方向判断](http://www.imod.top/img/方向判断.png)

获得道路线后，即我们获取了两个方程:


$$
y_{1}=a_{1}x_{1}+b_{1}\\
y_{2}=a_{2}x_{2}+b_{2}
$$


计算两直线交点(x0,y0),即上图所提的消失点； 此外，我们已知图像水平方向中心直线：x；

由此，我们有如下判断依据：

- x0 > x + thr_vp :右转
- x0 < x - thr_vp:左转
- x0 >= (x - thr_vp) && x0 <= (x + thr_vp)：直线

> 注：thr_vp为调整参数，根据实际情况调试

![测试](http://www.imod.top/img/测试.png)

# 车况识别

基于深度学习的目标检测与识别算法大致分为以下三大类:

1.基于区域建议的目标检测与识别算法，如R-CNN, Fast-R-CNN, Faster-R-CNN;

 2.基于回归的目标检测与识别算法，如YOLO, SSD;

 3.基于搜索的目标检测与识别算法，如基于视觉注意的AttentionNet；

此处识别采用YOLO算法,其优点如下：

1.速度非常快。在Titan X GPU上的速度是45 fps，加速版的YOLO差不多是150fps；

2.YOLO是基于图像的全局信息进行预测的； 

3.泛化能力强； 

4.准确率高；

- 具体算法参考yolo论文
- 训练参考[DarkNet](https://pjreddie.com/darknet/yolo/)

> 注：由于笔记本计算能力有限，此处采用开源coco数据集训练的yolo模型，支持80个分类识别，具体分类参考配置文件

![preview](https://pic3.zhimg.com/v2-ee4db90336d60d251d7254f9918c3a48_r.jpg)

# 状态特征判断依据

有如下内容需要了解:

1.“眼睛纵横比”（EAR） 我们可以应用面部标志检测来定位脸部的重要区域，包括眼睛，眉毛，鼻子，耳朵和嘴巴

2.EAR计算公式 :


$$
EAR=(∣∣p2−p6∣∣+∣∣p3−p5∣∣)/(2∣∣p1−p4∣∣)
$$


![eyeEar](http://www.imod.top/img/eyeEar.png)

> 详细内容参考Soukupová和Čech在其2016年的论文Real-Time Eye Blink Detection using Facial Landmarks

## Landmark算法

landmark是一种人脸部特征点提取的技术,Dlib库中为人脸68点标记,在《调用Dlib库进行人脸关键点标记》一文中有效果和标定点序号的示意图。今后可采用landmark中的点提取眼睛区域、嘴巴区 域用于疲劳检测,提取鼻子等部分可用于3D姿态估计。 Dlib库使用《One Millisecond Face Alignment with an Ensemble of Regression Trees》CVPR2014 中提及的算法:ERT(ensemble of regression trees)级联回归,即基于梯度提高学习的回归树方法。 该算法使用级联回归因子,首先需要使用一系列标定好的人脸图片作为训练集,然后会生成一个模型。 使用基于特征选择的相关性方法把目标输出投影到一个随机方向w上,并且选择一对特征(u,v),使 得Ii(u )-Ii(v )与被投影的目标wTri在训练数据上拥有最高的样本相关性。 当获得一张图片后,算法会生成一个initial shape就是首先估计一个大致的特征点位置,然后采用 gradient boosting算法减小initial shape 和 ground truth 的平方误差总和。用最小二乘法来最小化误 差,得到每一级的级联回归因子。核心公式如下:

$$
\hat{S}^{t+1}=\hat{S}+r_{t}(I,\hat{t}^{t})
$$


 示当前级的回归器regressor。回归器的输入参数为图像I和上一级回归器更新后的shape,采用的特征可 以是灰度值或者其它。每个回归器由很多棵树(tree)组成,每棵树参数是根据current shape和ground truth的坐标差和随机挑选的像素对训练得到的。

与LBF不同,ERT是在学习Tree的过程中,直接将shape的更新值ΔS存入叶子结点leaf node.初始位 置S在通过所有学习到的Tree后,meanshape加上所有经过的叶子结点的ΔS,即可得到最终的人脸关 键点位置。

# EAR参考意义

眼睛纵横比”(EAR):我们可以应用面部标志检测来定位脸部的重要区域,包括眼睛,眉毛,鼻子, 耳朵和嘴巴.

1.其中p1…p6是2D面部地标位置; 2.方程的分子是计算垂直眼睛标志之间的距离，而分母是计算水平眼睛标志之间的距离，因为只有一组水平点，但是有两组垂直点，所以进行加权分母

![EAR](http://www.imod.top/img/EAR.png)

> 当人眼闭眼时，EAR急剧减小，我们利用这一点去检测人眼闭眼状态

![EARTEST](http://www.imod.top/img/EARTEST.png)

# 畸变矫正

在开启摄像头后第一件事先做 畸变矫正 (目前还没添加)。
使用 opencv 提供的方法通过 棋盘格图片组 计算相机 校正矩阵 (camera calibration matrix)和 失真系数
(distortion coefficients)。
相应的推到,参考此篇[博客](https://blog.csdn.net/qq_36342854/article/details/88933308)

# 提升道路线识别准确度

当前识别道路下用的是传统数字图像处理技术,虽然计算量较小,不过其 泛化性 很差,抗干扰能力差,具体
缺陷 如下:
第一,视觉系统对背景光线很敏感,诸如阳光强烈的林荫道,⻋道线被光线分割成碎片,致使无法
提取出⻋道线。
第二,视觉系统需要⻋道线的标识完整,有些年久失修的道路,⻋道线标记不明显,不完整,有些
刚开通几年的道路也是如此。
第三,视觉系统需要⻋道线的格式统一,这对按照模型库识别⻋道线的系统尤其重要,有些⻋道线
格式很奇特,比如蓝颜色的⻋道线,很窄的⻋道线,模型库必须走遍全国将这些奇特的⻋道线一一
收录,才能保证顺利检测。
第四,视觉系统无法对应低照度环境,尤其是没有路灯的黑夜。一般LKW(Lane keep assistance)要
求时速在72公里以上才启动,原因之一是速度比较高时人不会轻易换道,另一个原因就是比较低
的⻋速意味着视觉系统的取样点不足,拟合的⻋道线准确度较低。
第五,如果⻋道线表面被水覆盖,视觉系统会完全无效。
具体解决方案如下:
1. 基于激光雷达回波宽度;
2. 基于激光雷达反射强度信息形成的灰度图,或者根据强度信息与高程信息配合,过滤出无效信息;
3. 激光雷达SLAM与高精度地图配合,不仅检测⻋道线还进行自⻋定位;
4. 利用激光雷达能够获取路沿高度信息或物理反射信息不同的特性,先检测出路沿,因为道路宽度是
已知,根据距离再推算出⻋道线位置。对于某些路沿与路面高度相差低于3厘米的道路,这种方法
无法使用。
后三种方法需要多线激光雷达,最少也是16线激光雷达。前者可以使用4线或单线激光雷达;

# 性能提升

- 提升硬件性能,扩大内存,使用高性能GPU;
- 使用TensorRt框架

>TensorRT是一个高性能的深度学习推理(Inference)优化器,可以为深度学习应用提供低
>延迟、高吞吐率的部署推理。TensorRT可用于对超大规模数据中心、嵌入式平台或自动驾驶
>平台进行推理加速。TensorRT现已能支持TensorFlow、Caffe、Mxnet、Pytorch等几乎所
>有的深度学习框架,将TensorRT和NVIDIA的GPU结合起来,能在几乎所有的框架中进行快
>速和高效的部署推理

# 导入地图概念

> 未实现

在有激光雷达的条件下,应用 slam 技术,建立空间地图

# 安装配置

- ROS

> 目前没使用,可不安装

- opencv

> version:3.4.5

- Dlib

> 用于实现检测驾驶员疲劳

- 编译程序

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

参考[博客](http://www.imod.top/_posts/2020-07-01-%E8%BD%A6%E9%81%93%E4%BF%9D%E6%8C%81&%E7%96%B2%E5%8A%B3%E9%A9%BE%E9%A9%B6%E6%A3%80%E6%B5%8B/)
