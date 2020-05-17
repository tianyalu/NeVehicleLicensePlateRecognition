#include "plateLocate.h"

PlateLocate::PlateLocate()
{
}

PlateLocate::~PlateLocate()
{
}

/**
* 尺寸校验(宽高比&面积)
*/
int PlateLocate::verifySizes(RotatedRect rotatedRect)
{
    //容错率
    float error = 0.75f;
    //理想宽高比
    float aspect = float(136) / float(36);
    //真实宽高比
    float realAspect = float(rotatedRect.size.width) / float(rotatedRect.size.height);
    if (realAspect < 1) realAspect = (float)rotatedRect.size.height / (float)rotatedRect.size.width;
    //真实面积
    float area = rotatedRect.size.height * rotatedRect.size.width;
    //最小 最大面积 不符合的丢弃
    //给个大概就行 随时调整
    //尽量给大一些没关系， 这还是初步筛选。
    int areaMin = 44 * aspect * 14;
    int areaMax = 440 * aspect * 140;

    //比例浮动 error认为也满足
    //最小宽高比
    float aspectMin = aspect - aspect * error;
    //最大宽高比
    float aspectMax = aspect + aspect * error;

    if ((area < areaMin || area > areaMax) || (realAspect < aspectMin || realAspect > aspectMax))
        return 0;
    return 1;
}

/**
* 转换安全矩形，防止矩形框超出图像边界
*/
void PlateLocate::safeRect(Mat src, RotatedRect rect, Rect2f& safa_rect)
{
    //RotatedRect 没有坐标
    //转为正常的带坐标的边框
    Rect2f boudRect = rect.boundingRect2f();
    //左上角 x,y
    float tl_x = boudRect.x > 0 ? boudRect.x : 0;
    float tl_y = boudRect.y > 0 ? boudRect.y : 0;
    //这里是拿 坐标 x，y 从0开始的 所以-1
    //比如宽长度是10，x坐标最大是9， 所以src.clos-1 
    //右下角
    float br_x = boudRect.x + boudRect.width < src.cols
        ? boudRect.x + boudRect.width - 1
        : src.cols - 1;
    float br_y = boudRect.y + boudRect.height < src.rows
        ? boudRect.y + boudRect.height - 1
        : src.rows - 1;
    float  w = br_x - tl_x;
    float h = br_y - tl_y;
    if (w <= 0 || h <= 0) return;
    safa_rect = Rect2f(tl_x, tl_y, w, h);
}
/**
* 旋转
*/
void PlateLocate::rotation(Mat src, Mat& dst, Size rect_size, Point2f center, double angle)
{
    //获得旋转矩阵
    Mat rot_mat = getRotationMatrix2D(center, angle, 1);
    //运用仿射变换
    Mat mat_rotated;
    //矫正后 大小会不一样，但是对角线肯定能容纳
    int max = sqrt(pow(src.rows, 2) + pow(src.cols, 2));
    warpAffine(src, mat_rotated, rot_mat, Size(max, max),
        INTER_CUBIC);
    //imshow("旋转前", src);
    //imshow("旋转后", mat_rotated);
    //截取 尽量把车牌多余的区域截取掉
    getRectSubPix(mat_rotated, Size(rect_size.width, rect_size.height), center, dst);
    //imshow("截取后", dst);
    //waitKey();
    mat_rotated.release();
    rot_mat.release();
}
/**
* 矩形矫正
*/
void PlateLocate::tortuosity(Mat src, vector<RotatedRect>& rects, vector<Mat>& dst_plates)
{
    //循环要处理的矩形
    for (RotatedRect roi_rect : rects) {
        //矩形角度
        float roi_angle = roi_rect.angle;
        float r = (float)roi_rect.size.width / (float)roi_rect.size.height;
        if (r < 1) {
            roi_angle = 90 + roi_angle;
        }
        //矩形大小
        Size roi_rect_size = roi_rect.size;
        //让rect在一个安全的范围(不能超过src)
        Rect2f  safa_rect;
        safeRect(src, roi_rect, safa_rect);
        //候选车牌
        //抠图  这里不是产生一张新图片 而是在src身上定位到一个Mat 让我们处理
        //数据和src是同一份
        Mat src_rect = src(safa_rect);
        //真正的候选车牌
        Mat dst;
        //不需要旋转的 旋转角度小没必要旋转了
        if (roi_angle - 5 < 0 && roi_angle + 5 > 0) {
            dst = src_rect.clone();
        }
        else {
            //相对于roi的中心点 不减去左上角坐标是相对于整个图的
            //减去左上角则是相对于候选车牌的中心点 坐标
            Point2f roi_ref_center = roi_rect.center - safa_rect.tl();
            Mat rotated_mat;
            //矫正 rotated_mat: 矫正后的图片
            rotation(src_rect, rotated_mat, roi_rect_size, roi_ref_center, roi_angle);
            dst = rotated_mat;
        }
        //调整大小
        Mat plate_mat;
        //高+宽
        plate_mat.create(36, 136, CV_8UC3); //跟样本尺寸有关
        resize(dst, plate_mat, plate_mat.size());
        dst_plates.push_back(plate_mat);
        dst.release();
    }
}