// NeVehicleLicensePlateRecognition.cpp: 定义应用程序的入口点。
//

#include "NeVehicleLicensePlateRecognition.h"
#include "plateRecognize.h"

int main()
{
	//imread(车牌图像文件的路径)
	Mat src = imread("E:/AndroidWangYiCloud/CPlusWorkspace/NeVehicleLicensePlateRecognition/NeVehicleLicensePlateRecognition/resources/Test/test1.jpg");
	PlateRecognize pr;
	string str_plate = pr.plateRecognize(src);

	cout << "车牌：" << str_plate << endl;
	waitKey();

	return 0;
}
