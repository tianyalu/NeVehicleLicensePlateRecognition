#include "plateRecognize.h"


PlateRecognize::PlateRecognize()
{
	sobelLocate = new SobelLocate();
}

PlateRecognize::~PlateRecognize()
{
	if (sobelLocate) {
		delete sobelLocate;
		sobelLocate = 0;
	}
}

/**
* 车牌识别（定位+检测+识别）
*/
String PlateRecognize::plateRecognize(Mat src)
{
	//1.车牌定位
	//定位结果：初步筛选的候选车牌向量集
	vector<Mat> dst_plates;
	sobelLocate->locate(src, dst_plates);

	return String("7777777");
}
