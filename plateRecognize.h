#pragma once
#ifndef PLATERECOGNIZE_H
#define PLATERECOGNIZE_H

#include "SobelLocate.h"

class PlateRecognize
{
public:
	PlateRecognize();
	~PlateRecognize();

	/**
	* src 待识别车牌原图
	* return 车牌字符串
	*/
	String plateRecognize(Mat src);

private:
	SobelLocate* sobelLocate = 0;
};


#endif