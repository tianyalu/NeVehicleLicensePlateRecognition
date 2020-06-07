#include "SobelLocate.h"

SobelLocate::SobelLocate()
{
}

SobelLocate::~SobelLocate()
{
}

/**
* sobel 定位
*/
void SobelLocate::locate(Mat src, vector<Mat>& dst_plates)
{
	//imshow("src", src);
	//1.高斯模糊
	Mat blur;
	//在opencv中，只支持奇数半径
	GaussianBlur(src, blur, Size(5, 5), 0);
	//imshow("blur", blur);

	//2.灰度化
	Mat gray;
	cvtColor(blur, gray, COLOR_BGR2GRAY);
	//imshow("gray", gray);

	//3.Sobel运算
	Mat sobel_16;
	//Sobel求导后的导数值可能小于0或者大于255,8位存不下
	Sobel(gray, sobel_16, CV_16S, 1, 0);
	//imshow("sobel_16", sobel_16);
	//16位转回8位
	Mat sobel;
	convertScaleAbs(sobel_16, sobel);
	//imshow("sobel", sobel);

	//4.二值化(正/反)
	//正二值化 适合：蓝牌（深底浅字）
	//反二值化 适合：浅底深字
	Mat shold;
	threshold(sobel, shold, 0, 255, THRESH_OTSU + THRESH_BINARY);
	//imshow("shold", shold);

	//5.形态学操作（闭操作）
	Mat element = getStructuringElement(MORPH_RECT, Size(17, 3)); //经验值，可调
	Mat close;
	morphologyEx(shold, close, MORPH_CLOSE, element);
	//imshow("close", close);

	//6.求轮廓
	vector<vector<Point>> contours;
	findContours(
		close, //输入图像
		contours, //输出轮廓
		RETR_EXTERNAL, //取外轮廓
		CHAIN_APPROX_NONE //取轮廓上所有像素点
	);
	RotatedRect rotatedRect;
	vector<RotatedRect> vec_sobel_rects;
	for each (vector<Point> points in contours)
	{
		rotatedRect = minAreaRect(points); //取最小外接矩形（可旋转/带角度）
		//rectangle(src, rotatedRect.boundingRect(), Scalar(0, 0, 255)); //画红色矩形

		//7.尺寸判断
		//初步过滤不符合要求的矩形
		if (verifySizes(rotatedRect)) {
			vec_sobel_rects.push_back(rotatedRect);
		}
	}

	//for each (RotatedRect rect in vec_sobel_rects)
	//{
	//	rectangle(src, rect.boundingRect(), Scalar(0, 255, 0)); //画绿色矩形
	//}

	//矩形矫正
	tortuosity(src, vec_sobel_rects, dst_plates);
	//for each (Mat m in dst_plates)
	//{
	//	/*imshow("sobel 候选车牌", m);
	//	waitKey();*/
	//}
	//imshow("sobel取轮廓", src);

	blur.release();
	gray.release();
	sobel_16.release();
	sobel.release();
	shold.release();
	element.release();
	close.release();
}
