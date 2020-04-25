/*
 * @Author: verylazycat 
 * @Date: 2020-02-09 12:41:21 
 * @Last Modified by: verylazycat
 * @Last Modified time: 2020-02-09 13:41:51
 */
//警告状态
#include <main.h>
int fatigueDetection(double leftEyeBase,double leftEye,double rightEyeBase,double rightEye,double innerLipBase,double innerLip,int innerLipCounts,int squintingCounts)
{
    //当内嘴唇高宽比例大于基础高宽比0.,定义位打哈欠
    if(innerLip - innerLipBase >= 0.3)
    {
        //绿色打印
        cout  << "\033[32m警告:哈欠:\033[0m"<<innerLipCounts<< endl;
        //计数
        return 1;
    }
    if(leftEyeBase - leftEye >=0.1 || rightEyeBase - rightEye >= 0.1)
    {
        //绿色打印
        cout  << "\033[32m警告:眯眼:\033[0m"<<squintingCounts<< endl;
        //计数
        return 1;
    }
    if((leftEye + rightEye)/2 < 0.2)
    {
        // 绿色打印
        cout  << "\033[32m警告:闭眼\033[0m"<< endl;	
    }
    return 0;
}