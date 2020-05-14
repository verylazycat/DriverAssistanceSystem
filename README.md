# `DriverAssistanceSystem`

系统分为如下几个部分：

- 驾驶员危险行为检测（睡觉，疲劳，特殊时期未带口罩等等）
- 道路识别，航向辅助判断

- 辅助泊车（正在开发）

----

#### lane_car_detect

1. `yolo_car_detect`

```
使用开源yolo-tiny模型
具体流程如下：
1.config文件检测
2.导入网络:readNetFromDarknet
3.读取frame
4.初始化net参数:blobFromImage
5.前向传播:net.forward
6.confidence抑制
7.图像处理:降噪，边缘检测，ROI,霍夫线获取
8.渲染
```

