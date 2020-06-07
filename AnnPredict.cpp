#include "AnnPredict.h"

string AnnPredict::ZHCHARS[] = { "川", "鄂", "赣", "甘", "贵", "桂", "黑", "沪", "冀", "津", "京", "吉", "辽", "鲁", "蒙", "闽", "宁", "青", "琼", "陕", "苏", "晋", "皖", "湘", "新", "豫", "渝", "粤", "云", "藏", "浙" };
char AnnPredict::CHARS[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'J', 'K', 'L', 'M', 'N', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z' };

AnnPredict::AnnPredict(const char* ann_model, const char* ann_zh_model)
{
	ann = ANN_MLP::load(ann_model);
	ann_zh = ANN_MLP::load(ann_zh_model);
	annHog = new HOGDescriptor(Size(32, 32), Size(16, 16), Size(8, 8), Size(8, 8), 3);
}

AnnPredict::~AnnPredict()
{
	ann->clear();
	ann.release();
	ann_zh->clear();
	ann_zh.release();
	if (annHog) {
		delete annHog;
		annHog = 0;
	}
}

string AnnPredict::doPredict(Mat final_plate)
{
	//预处理
	//灰度化
	Mat gray;
	cvtColor(final_plate, gray, COLOR_BGR2GRAY);
	//二值化
	Mat shold;
	threshold(gray, shold, 0, 255, THRESH_OTSU + THRESH_BINARY);
	//imshow("二值化车牌", shold);
	
	if (!clearMaoDing(shold)) {
		return string("未识别到车牌");
	}
	//imshow("去铆钉后", shold);

	//求字符轮廓
	vector<vector<Point>> contours;
	findContours(
		shold, //输入图像
		contours, //输出图像
		RETR_EXTERNAL, //取外接轮廓
		CHAIN_APPROX_NONE //取轮廓上所有的像素点
	);
	RotatedRect rotatedRect;
	vector<Rect> vec_ann_rects;
	for each (vector<Point> points in contours)
	{
		Rect rect = boundingRect(points); //取最小外接矩形（可旋转/带角度）
		//rectangle(final_plate, rect, Scalar(0, 0, 255)); //画红色矩形
		Mat m = shold(rect);
		if (verifyCharSize(m)) {
			vec_ann_rects.push_back(rect);
		}
	}
	//cout << "size: " << vec_ann_rects.size() << endl;
	//for each (Rect rect in vec_ann_rects)
	//{
	//	rectangle(final_plate, rect, Scalar(0, 255, 0)); //画绿色矩形
	//}
	//imshow("字符轮廓", final_plate);

	//排序：从左到右
	sort(vec_ann_rects.begin(), vec_ann_rects.end(), [](const Rect& rect1, const Rect& rect2) {
		return rect1.x < rect2.x;
	});

	//获取城市字符框的索引
	int cityIndex = getCityIndex(vec_ann_rects);

	//反推汉字字符框的位置
	Rect chineseRect;
	getChineseRect(vec_ann_rects[cityIndex], chineseRect);

	vector<Mat> plateCharMats; //保存7个字符图块
	plateCharMats.push_back(shold(chineseRect)); //放入中文字符
	if (vec_ann_rects.size() < 6) {
		return string("未识别到车牌");
	}
	int count = 6;
	for (int i = cityIndex; i < vec_ann_rects.size() && count; i++, count--)
	{
		plateCharMats.push_back(shold(vec_ann_rects[i])); //放入城市及之后六个字符
	}

	char winName[100];
	for (int i = 0; i < plateCharMats.size(); i++)
	{
		sprintf(winName, "$d 字符", i);
		imshow(winName, plateCharMats[i]);
	}

	string str_plate;
	predict(plateCharMats, str_plate);

	return str_plate;
}

//每行像素值的变化次数
//清楚铆钉行&判断是否适合后面的分割字符
bool AnnPredict::clearMaoDing(Mat& src) {
	int maxChangeCount = 12;
	vector<int> changes; //统计每行的变化次数
	for (int i = 0; i < src.rows; i++)
	{
		int changeCount = 0; //记录第i行的变化次数
		for (int j = 0; j < src.cols - 1; j++)
		{
			char pixel_1 = src.at<char>(i, j); //前像素
			char pixel_2 = src.at<char>(i, j + 1); //后像素
			if (pixel_1 != pixel_2) {
				//像素发生了改变
				changeCount++;
			}
		}
		changes.push_back(changeCount);
	}

	//车牌字符区域的跳变条件
	//满足条件的行数约等于字符的高度
	int charRows = 0;
	for (int i = 0; i < src.rows; i++)
	{
		if (changes[i] >= 12 && changes[i] <= 60) {
			//当前第i行属于车牌字符区域
			charRows++;
		}
	}

	//判断字符占比
	float heightPercent = charRows * 1.0 / src.rows; //字符高度占比
	if (heightPercent <= 0.4) {
		return false; //车牌字符高度占比不符合要求
	}

	//字符区域 约等于白色像素的个数
	float src_area = src.rows * src.cols;
	float areaPercent = countNonZero(src) * 1.0 / src_area;
	if (areaPercent <= 0.2 || areaPercent >= 0.5) {
		return false; //车牌字符面积占比不合要求
	}

	for (int i = 0; i < changes.size(); i++)
	{
		int changeCount = changes[i];
		if (changeCount < maxChangeCount) {
			//属于铆钉所在像素行，全部 抹黑（修改像素值为0）
			for (int j = 0; j < src.cols; j++)
			{
				src.at<char>(i, j) = 0;
			}
		}
	}
	return true;
}

bool AnnPredict::verifyCharSize(Mat src)
{
	//理想宽高比
	float aspect = 0.5f;
	//真实宽高比
	float realAspect = float(src.cols) / float(src.rows);
	//字符最小高度
	float minHeight = 10.0f;
	float maxHeight = 35.0f;

	float error = 0.7f; //容错率
	float maxAspect = aspect + aspect * error;
	float minAspect = 0.05f;

	float src_area = src.rows * src.cols;
	float areaPercent = countNonZero(src) * 1.0 / src_area;

	if (src.rows >= minHeight && src.rows <= maxHeight //高度
		&& realAspect >= minAspect && realAspect <= maxAspect //宽高比
		&& areaPercent <= 0.8f) {
		return true;
	}

	return false;
}

//获取城市字符索引，参考：show/get_city_character_throry.png
int AnnPredict::getCityIndex(vector<Rect> rects)
{
	int cityIndex = 0;
	for (int i = 0; i < rects.size(); i++)
	{
		Rect rect = rects[i];
		int midX = rect.x + rect.width / 2;
		if (midX < 136 / 7 * 2 && midX > 136 / 7) {
			cityIndex = i;
			break;
		}
	}
	return cityIndex;
}

void AnnPredict::getChineseRect(Rect cityRect, Rect& chineseRect)
{
	//宽度，比城市字符宽一点
	float width = cityRect.width * 1.15f;
	int cityX = cityRect.x; //城市字符框x
	int x = cityX - width;
	chineseRect.x = x >= 0 ? x : 0;
	chineseRect.y = cityRect.y;
	chineseRect.width = width;
	chineseRect.height = cityRect.height;
}

void AnnPredict::predict(vector<Mat> plateCharMats, string& str_plate)
{
	for (int i = 0; i < plateCharMats.size(); i++)
	{
		Mat plate_char = plateCharMats[i];
		Mat features;
		getHogFeatures(annHog, plate_char, features);

		Mat sample = features.reshape(1, 1);
		Mat response;
		Point maxLoc;
		Point minLoc;

		if (i) { //非0即true
			//字母 + 数字
			ann->predict(sample, response);
			minMaxLoc(response, 0, 0, &minLoc, &maxLoc);
			int index = maxLoc.x; //样本索引值(CHAR数组的索引值）
			str_plate += CHARS[index];
		}
		else { //中文
			ann_zh->predict(sample, response);
			minMaxLoc(response, 0, 0, &minLoc, &maxLoc);
			int index = maxLoc.x; //样本索引值(ZHCHAR数组的索引值）
			str_plate += ZHCHARS[index];
		}
		
	}
}

void AnnPredict::getHogFeatures(HOGDescriptor* hog, Mat src, Mat& dst)
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
