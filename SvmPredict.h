#ifndef SVMPREDICT_H
#define SVMPREDICT_H

#include <opencv2/opencv.hpp>
using namespace std;
using namespace cv;
using namespace ml;


class SvmPredict
{
public:
	SvmPredict(const char* svm_model);
	~SvmPredict();

	//candi_plates : 候选车牌， plate：测评结果（最终车牌）
	void doPredict(vector<Mat> candi_plates, Mat& plate);

private:
	Ptr<SVM> svm;
	HOGDescriptor* svmHog = 0;
	void getHogFeatures(HOGDescriptor* svmHog, Mat src, Mat& dst);
};


#endif // !SVMPREDICT_H
