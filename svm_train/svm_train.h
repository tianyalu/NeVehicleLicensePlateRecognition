#ifndef SVMTRAIN_H
#define SVMTRAIN_H

#include <opencv2/opencv.hpp>
#include "utils.h"
#include <string>

using namespace std;
using namespace cv;
using namespace ml;

#define SVM_XML "C:/Users/Administrator/Desktop/vlpr/train/svm/svm.xml"
#define SVM_POS "C:/Users/Administrator/Desktop/vlpr/train/svm/posPlate"
#define SVM_NEG "C:/Users/Administrator/Desktop/vlpr/train/svm/negPlate"

struct TrainStruct
{
	string fileName;
	int label;
};

void train();

#endif // SVMTRAIN_H

