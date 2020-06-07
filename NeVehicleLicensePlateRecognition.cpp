// NeVehicleLicensePlateRecognition.cpp: 定义应用程序的入口点。
//

#include "NeVehicleLicensePlateRecognition.h"
#include "plateRecognize.h"
#include "SvmPredict.h"

int main()
{
	//imread(车牌图像文件的路径)
	const char* srcPath = "E:/AndroidWangYiCloud/CPlusWorkspace/NeVehicleLicensePlateRecognition/NeVehicleLicensePlateRecognition/resources/test2.jpg";
	const char* svmXmlPath = "E:/AndroidWangYiCloud/CPlusWorkspace/NeVehicleLicensePlateRecognition/NeVehicleLicensePlateRecognition/resources/svm.xml";
	const char* annXmlPath = "E:/AndroidWangYiCloud/CPlusWorkspace/NeVehicleLicensePlateRecognition/NeVehicleLicensePlateRecognition/resources/ann.xml";
	const char* annZhXmlPath = "E:/AndroidWangYiCloud/CPlusWorkspace/NeVehicleLicensePlateRecognition/NeVehicleLicensePlateRecognition/resources/ann_zh.xml";
	Mat src = imread(srcPath);
	PlateRecognize pr(svmXmlPath, annXmlPath, annZhXmlPath);
	string str_plate = pr.plateRecognize(src);

	cout << "车牌：" << str_plate << endl;
	waitKey();
	 
	return 0;
}
