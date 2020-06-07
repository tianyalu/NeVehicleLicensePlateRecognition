#ifndef ANNTRAIN_H
#define ANNTRAIN_H

#include <opencv2/opencv.hpp>
#include "utils.h"
#include <string>

using namespace std;
using namespace cv;
using namespace ml;

#define ANN_ZH_XML "C:/Users/Administrator/Desktop/vlpr/train/ann/ann_zh.xml"
#define ANN_XML "C:/Users/Administrator/Desktop/vlpr/train/ann/ann.xml"
#define ANN_ZH_SAMPLE "C:/Users/Administrator/Desktop/vlpr/train/ann/ann_zh"
#define ANN_SAMPLE "C:/Users/Administrator/Desktop/vlpr/train/ann/ann"

struct TrainStruct
{
	string fileName;
	int label;
};

void train();
void train_zh();//训练中文字符

#endif // !ANNTRAIN_H

