#include "ColorLocate.h"

ColorLocate::ColorLocate()
{
}

ColorLocate::~ColorLocate()
{
}

/**
* color 定位
*/
void ColorLocate::locate(Mat src, vector<Mat>& dst_plates)
{
	//imshow("src", src);
	//1. 颜色空间转化为HSV
	Mat hsv;
	cvtColor(src, hsv, COLOR_BGR2HSV);
	//imshow("hsv", hsv);

	//找蓝色像素，参考 https://www.jianshu.com/p/8ed28846ac9a  hsv范围.png
	//h: 100-124
	//s: 43-255
	//v: 46-255
	int channels = hsv.channels(); //通道数 = 3
	int width = hsv.cols * channels;
	int height = hsv.rows;
	uchar* p;

	if (hsv.isContinuous()) { //判断在内存中是否连续存储
		width *= height;
		height = 1;
	}
	for (int i = 0; i < height; i++)
	{
		p = hsv.ptr<uchar>(i); //第i行的数据
		for (int j = 0; j < width; j+= channels) //注意步进
		{
			int h = p[j]; //H分量
			int s = p[j + 1]; //S分量
			int v = p[j + 2]; //V分量

			bool isBlue = false;
			if (h >= 100 && h <= 124 &&
				s >= 43 && s <= 255 &&
				v >= 46 && v <= 255) {
				isBlue = true;
			}

			if (isBlue) { //把蓝色部分抽离（凸显）高亮
				p[j] = 0;
				p[j + 1] = 0;
				p[j + 2] = 255;
			} else { //非蓝色像素点涂黑
				p[j] = 0;
				p[j + 1] = 0;
				p[j + 2] = 0;
			}
		}
	}

	//imshow("凸显蓝色像素", hsv);

	//分离
	vector<Mat> hsv_split;
	split(hsv, hsv_split); //按通道进行分离 merge
	//imshow("分离V分量", hsv_split[2]);

	//二值化
	Mat shold;
	threshold(hsv_split[2], shold, 0, 255, THRESH_OTSU + THRESH_BINARY);
	//imshow("二值化", shold);
	//闭操作
	Mat element = getStructuringElement(MORPH_RECT, Size(17, 3)); //经验值，可调
	Mat close;
	morphologyEx(shold, close, MORPH_CLOSE, element);
	//imshow("close", close);

	//求轮廓
	vector<vector<Point>> contours;
	findContours(
		close, //输入图像
		contours, //输出图像
		RETR_EXTERNAL, //取外接轮廓
		CHAIN_APPROX_NONE //取轮廓上所有像素点
	);
	RotatedRect rotatedRect;
	vector<RotatedRect> vec_color_rects;
	for each (vector<Point> points in contours)
	{
		rotatedRect = minAreaRect(points); //取最小外接矩形（可旋转/带角度）
		//rectangle(src, rotatedRect.boundingRect(), Scalar(0, 0, 255)); //画红色矩形
		if (verifySizes(rotatedRect)) {
			vec_color_rects.push_back(rotatedRect);
		}
	}
	//for each (RotatedRect rect in vec_color_rects)
	//{
	//	rectangle(src, rect.boundingRect(), Scalar(0, 255, 0)); //画绿色矩形
	//}
	//imshow("color求轮廓", src);

	tortuosity(src, vec_color_rects, dst_plates);
	//for each (Mat m in dst_plates)
	//{
	//	imshow("color 候选车牌", m);
	//	waitKey();
	//}
}
