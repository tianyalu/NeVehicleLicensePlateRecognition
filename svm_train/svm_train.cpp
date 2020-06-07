#include "svm_train.h"


int main()
{
	train();
	return 0;
}

/**--------------SVM(支持向量机)分类器训练与测试--------------------
 *  SVM:通常用来进行模式识别、分类以及回归分析
 *  使用SVM对于查找到的车牌号区域进行判断,进一步确定定位车牌号区域正确
 *
 *  流程:
 *   1、加载全部样本(训练数据) 训练数据有正确的,有错误的。也就是分为车牌区域与非车牌区域
 *   2、创建样本数据标签,标记对应样本分类(正确/错误)
 *   3、训练、保存
 */

void getSvmHOGFeatures(const Mat &image, Mat &features) {
	//
	HOGDescriptor hog(Size(128, 64), Size(16, 16), Size(8, 8), Size(8, 8), 3);
	std::vector<float> descriptor;

	//    Size dsize = Size(128,64);
	Mat trainImg = Mat(hog.winSize, CV_32S);
	resize(image, trainImg, hog.winSize);

	//计算
	hog.compute(trainImg, descriptor, Size(8, 8));

	Mat mat_featrue(descriptor);
	mat_featrue.copyTo(features);
}

void train()
{
	printf("准备训练SVM车牌识别分类模型...\n");
	//创建SVM
	Ptr<SVM> classifier = SVM::create();
	//深度学习就是调整参数的过程 ... ...
	//这些参数肯定不是最好的,但是基本稳定.
	//不同参数效果不同、训练检测速度不同,同时也影响着模型文件的大小.

	//大致参数调整可以从下面的地址 svm_test.png
	//http://www.csie.ntu.edu.tw/~cjlin/libsvm/

	//=====================================================================
	//特征空间的隐式映射-核函数:通过将数据映射到高维空间，来解决在原始空间中线性不可分的问题
	//svmxxx.png
	//http://blog.csdn.net/liukun321/article/details/41574617
	//内核函数，默认就是径向基函数 对于大多数情况都是一个较好的选择
	//classifier->setKernel(SVM::RBF);
	 //内核函数的参数 默认就是1  rbf只会需要这一个参数
	//classifier->setGamma(1);
	//svm的类型 默认就是这个 允许用异常值惩罚因子（C）进行不完全分类
	//classifier->setType(SVM::C_SVC);


	//c越大错误率较小，间隔也较小 间隔指 对应样本离同类区域的距离
	//classifier->setC(100);

	//迭代算法的终止条件 迭达到20000次终止  
	//SVM训练的过程就是一个通过 迭代 解决约束条件下的二次优化问题，允许算法在适当的条件下停止计算
	//classifier->setTermCriteria(cvTermCriteria(CV_TERMCRIT_ITER, 20000, 0.0001));
	auto time_start = cv::getTickCount();
	vector<TrainStruct>svm_data;
	Mat src_mat;
	vector<string> pos_files;
	getFiles(SVM_POS, pos_files);
	//正样本 标签是：1
	for (auto file : pos_files) {
		svm_data.push_back({ file,1 });
	}
	//负样本 标签为：-1
	vector<string> neg_files;
	getFiles(SVM_NEG, neg_files);
	for (auto file : neg_files) {
		svm_data.push_back({ file,-1 });
	}

	Mat samples;
	vector<int> responses;
	for (auto data : svm_data) {
		auto image = imread(data.fileName, IMREAD_GRAYSCALE);
		if (image.empty()) {
			printf("加载样本失败 image: %s.\n", data.fileName.c_str());
			continue;
		}
		//二值
		threshold(image, image, 0, 255, THRESH_BINARY + THRESH_OTSU);
		Mat feature;
		//获得hog特征
		getSvmHOGFeatures(image, feature);
		//调整为一行
		feature = feature.reshape(1, 1);
		// 图片数据:x x x
		// samples:x x x
		//         x x x
		samples.push_back(feature);
		responses.push_back(data.label);
	}

	//训练数据必须是 CV_32FC1 数据为32位浮点类型，单通道
	samples.convertTo(samples, CV_32FC1);
	//训练数据  行
	Ptr <TrainData> trainData = TrainData::create(samples, SampleTypes::ROW_SAMPLE, responses);
	printf("训练数据准备完成,开始训练！\n");

	//其他参数都是默认值 为了把
	//最后一个参数设置true 则会创建更平衡的验证子集 
	//也就是如果是2类分类的话能得到更准确的结果
	classifier->trainAuto(trainData, 10, SVM::getDefaultGrid(SVM::C),
		SVM::getDefaultGrid(SVM::GAMMA), SVM::getDefaultGrid(SVM::P),
		SVM::getDefaultGrid(SVM::NU), SVM::getDefaultGrid(SVM::COEF),
		SVM::getDefaultGrid(SVM::DEGREE), true);

	classifier->save(SVM_XML);

	printf("训练完成 ,模型保存: %s  \n", SVM_XML);
}
