#pragma once
#ifndef ANNPREDICT_H
#define ANNPREDICT_H

#include <opencv2/opencv.hpp>
using namespace std;
using namespace cv;
using namespace ml;

class AnnPredict {
public:
	AnnPredict(const char* ann_model, const char* ann_zh_model);
	~AnnPredict();

	/**
	* plate: svm测评得到的最终车牌
	* 返回：车牌字符串
	*/
	string doPredict(Mat final_plate);

	bool clearMaoDing(Mat& src);

	bool verifyCharSize(Mat src);

	int getCityIndex(vector<Rect> rects);

	void getChineseRect(Rect cityRect, Rect& chineseRect);

	void predict(vector<Mat> plateCharMats, string& str_plate);

private:
	Ptr<ANN_MLP> ann; //字母&数字
	Ptr<ANN_MLP> ann_zh; //中文字符
	HOGDescriptor* annHog = 0;
	void getHogFeatures(HOGDescriptor* annHog, Mat src, Mat& dst);
	static string ZHCHARS[];
	static char CHARS[];
};

#endif //!ANNPREDICT_H
