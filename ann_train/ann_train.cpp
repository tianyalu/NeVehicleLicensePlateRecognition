#include "ann_train.h"


int main()
{
	train();
	train_zh();
	return 0;
}

void getAnnHOGFeatures(const Mat image, Mat& features) {
	HOGDescriptor hog(Size(32, 32), Size(16, 16), Size(8, 8), Size(8, 8), 3);
	std::vector<float> descriptor;

	Mat trainImg = Mat(hog.winSize, CV_32S);
	resize(image, trainImg, hog.winSize);

	//计算
	hog.compute(trainImg, descriptor, Size(8, 8));

	Mat mat_featrue(descriptor);
	mat_featrue.copyTo(features);
}
//Artificial Neural Networks 人工神经网络 多层感知器
//看图 https://docs.opencv.org/4.1.1/dc/dd6/ml_intro.html#ml_intro_ann
//1、把需要识别的物体通过某种途径获得x(输入层个数)个数据
//2、把输入数据分成y(隐藏层神经元个数)个数据(神经元) 根据隐藏层数重复这个步骤
//3、把第2步的输出 输入给输出层 然后输出z(输出层个数)个结果
void train_zh() {
	printf("准备训练ann中文识别模型...\n");
	//图片地址与他对应的标签
	vector<TrainStruct> ann_data;
	Mat train_samples;
	//文件夹名是1-31
	for (int i = 0; i < 31; ++i) {
		char path[100];
		sprintf(path, "%s/%d", ANN_ZH_SAMPLE, i + 1);
		//图片地址
		vector<string> files;
		getFiles(path, files);
		//把文件路径与他的下标标签放入集合
		for (string file : files) {
			ann_data.push_back({ file,i });
		}
	}

	//如 存在3个样本 类别为2 第0，1个样本为第0类，第2个样本是第1类
	//样本数据如下:
	//比如3个样本 向量(数据数)为4(2x2)
	//1,1,1,1
	//1,1,1,1
	//1,1,1,1
	//样本标签则需要为
	//1,0
	//1,0
	//0,1
	Mat samples;
	vector<int> label;
	for (TrainStruct data : ann_data) {
		//直接读出灰度图
		Mat image = imread(data.fileName, IMREAD_GRAYSCALE);
		if (image.empty()) {
			printf("加载样本失败 image: %s.\n", data.fileName.c_str());
			continue;
		}
		//        resize(image, image, Size(ANN_COLS,ANN_ROWS));
		threshold(image, image, 0, 255, THRESH_BINARY + THRESH_OTSU);
		Mat feature;
		getAnnHOGFeatures(image, feature);
		feature = feature.reshape(1, 1);
		//2个集合 一个样本一个标签
		samples.push_back(feature);
		label.push_back(data.label);
	}
	//samples 里面保存了所有的待训练样本的特征数据
	samples.convertTo(samples, CV_32FC1);
	//创建标签
	Mat train_label = Mat::zeros((int)label.size(), 31, CV_32FC1);
	for (int i = 0; i < train_label.rows; ++i) {
		train_label.at<float>(i, label[i]) = 1.f;
	}
	cout << "标签：" << train_label << endl;

	//训练数据
	Ptr<TrainData> train_data = TrainData::create(samples, SampleTypes::ROW_SAMPLE, train_label);

	Ptr<ANN_MLP> classifier = ANN_MLP::create();
	Mat layers;
	layers.create(1, 3, CV_32SC1);
	//输入层
	layers.at<int>(0) = samples.cols;   //要预测的数据(图片)的宽 
	layers.at<int>(1) = 62; //隐藏层神经数 这个是需要调整的 对于我们是不可见的
	//输出层
	layers.at<int>(2) = 31;//类别数 31种 31个车牌省份 也就是train_label的宽
	//cout << "标签：" << layers << endl;
	classifier->setLayerSizes(layers);
	//https://docs.opencv.org/4.1.1/d0/dce/classcv_1_1ml_1_1ANN__MLP.html#a16998f97db903c1c652e68f342240524
	//初始化每个神经元的激活方法,即将结果进过激活函数处理输出到下一行 目前完全支持的只有SIGMOID_SYM 后两个参数算法中的α和β，默认情况下会都被设置为1
	classifier->setActivationFunction(cv::ml::ANN_MLP::SIGMOID_SYM, 1, 1);
	//bp网络 一般用这种算法,加速神经网络训练过程的数值算法，速度快
	classifier->setTrainMethod(ANN_MLP::TrainingMethods::BACKPROP);
	//迭代算法的终止条件 迭代次数(CV_TERMCRIT_ITER)和误差最小值(CV_TERMCRIT_EPS)，一旦有一个达到条件就终止训练
	//classifier->setTermCriteria(cvTermCriteria(CV_TERMCRIT_ITER, 30000, 0.0001));//3.3.0 api
	classifier->setTermCriteria(TermCriteria(TermCriteria::MAX_ITER, 30000, 0.0001));


	printf("准备训练数据完成，开始训练\n");

	classifier->train(train_data);
	classifier->save(ANN_ZH_XML);
	printf("训练ann中文识别模型保存: %s\n", ANN_ZH_XML);
}



void train() {
	printf("准备训练ann字符识别模型...\n");
	vector<TrainStruct> ann_data;
	Mat train_samples;
	for (int i = 0; i < 34; ++i) {
		char path[100];
		sprintf(path, "%s/%d", ANN_SAMPLE, i);
		vector<string> files;
		getFiles(path, files);
		for (string file : files) {
			ann_data.push_back({ file,i });
		}
	}
	Mat samples;
	vector<int> label;
	for (TrainStruct data : ann_data) {
		Mat image = imread(data.fileName, IMREAD_GRAYSCALE);
		if (image.empty()) {
			printf("加载样本失败 image: %s.\n", data.fileName.c_str());
			continue;
		}
		threshold(image, image, 0, 255, THRESH_BINARY + THRESH_OTSU);
		Mat feature;
		getAnnHOGFeatures(image, feature);
		feature = feature.reshape(1, 1);
		samples.push_back(feature);
		label.push_back(data.label);
	}
	samples.convertTo(samples, CV_32FC1);
	Mat train_classes = Mat::zeros((int)label.size(), 34, CV_32FC1);
	for (int i = 0; i < train_classes.rows; ++i) {
		train_classes.at<float>(i, label[i]) = 1.f;
	}

	Ptr<TrainData>  train_data = TrainData::create(samples, SampleTypes::ROW_SAMPLE, train_classes);

	Ptr<ANN_MLP> classifier = ANN_MLP::create();
	Mat layers;
	layers.create(1, 3, CV_32SC1);
	layers.at<int>(0) = samples.cols;
	layers.at<int>(1) = 68;
	layers.at<int>(2) = 34;//类别数
	classifier->setLayerSizes(layers);
	classifier->setActivationFunction(ANN_MLP::SIGMOID_SYM, 1, 1);
	classifier->setTrainMethod(cv::ml::ANN_MLP::TrainingMethods::BACKPROP);
	classifier->setTermCriteria(TermCriteria(TermCriteria::MAX_ITER, 30000, 0.0001));

	printf("准备训练数据完成，开始训练\n");


	classifier->train(train_data);
	classifier->save(ANN_XML);
	printf("训练ann字符识别模型保存: %s\n", ANN_XML);
}