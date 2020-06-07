#include "SvmPredict.h"

SvmPredict::SvmPredict(const char* svm_model)
{
	svm = SVM::load(svm_model);
	svmHog = new HOGDescriptor(Size(128, 64), Size(16, 16), Size(8, 8), Size(8, 8), 3);
}

SvmPredict::~SvmPredict()
{
	svm->clear();
	svm.release();
	if (svmHog)
	{
		delete svmHog;
		svmHog = 0;
	}
}

void SvmPredict::doPredict(vector<Mat> candi_plates, Mat& final_plate)
{
	Mat plate;
	float score;//svm 评分
	float minScore = FLT_MAX;
	int index = -1;
	for (int i = 0; i < candi_plates.size(); i++)
	{
		plate = candi_plates[i];
		//预处理
		Mat gray;
		cvtColor(plate, gray, COLOR_BGR2GRAY);
		Mat shold;
		threshold(gray, shold, 0, 255, THRESH_OTSU + THRESH_BINARY);

		//提特征
		Mat features;
		getHogFeatures(svmHog, shold, features);
		//
		Mat sample = features.reshape(1, 1);
		//特征交给svm 测评，返回评分
		score = svm->predict(sample, noArray(), StatModel::Flags::RAW_OUTPUT);
		//printf("svm 候选车牌 %d , 评分是：%f\n", i, score);
		if (score < minScore)
		{
			minScore = score;
			index = i;
		}
		gray.release();
		//....

	}
	if (index >= 0)
	{
		final_plate = candi_plates[index].clone();
		/*imshow("svm最终候选车牌", final_plate);
		waitKey();*/
	}
}

void SvmPredict::getHogFeatures(HOGDescriptor* hog, Mat src, Mat& dst)
{
	//归一化
	Mat trainImg = Mat(hog->winSize, CV_32S);
	resize(src, trainImg, hog->winSize);

	//计算特征
	vector<float> descriptor;
	hog->compute(trainImg, descriptor, hog->winSize);

	Mat feature(descriptor);
	feature.copyTo(dst);

	feature.release();
	trainImg.release();

}
