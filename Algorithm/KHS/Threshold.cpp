#include "opencv2/opencv.hpp"
#include <iostream>

using namespace cv;
using namespace std;


//=========================================================================
// �Ӱ谪 ���� �׽�Ʈ �� ���� ����� threshild ����
//=========================================================================


// threshold call back �Լ� - �⺻ opencv �����Լ� ���
void onChangethresholdValue2(int value, void* userData);
void onChangethresholdValue1(int value, void* userData);

// �⺻��
void binaryThreshold(Mat& src, Mat& dst, int MinValue, int MaxValue, int outValue, int inValue);

// �ݴ���
void rangeThreshold(Mat& src, Mat& dst, int MinValue, int MaxValue, int outValue, int inValue);

Mat img;
Mat test;
Mat bin;

int threshold1 = 160; 
int threshold2 = 100; 

void main()
{
    // 1. ������� �����ؼ� �󺧸� 
    img = imread("image/a1.png", IMREAD_GRAYSCALE);
    if (img.empty()) {
        cerr << "Image load failed!" << endl;
        return;
    }
    test = img.clone();
    bin = img.clone();


    // 1. threshold �Ӱ谪 test Code 
    //namedWindow("threshold1", WINDOW_AUTOSIZE);
    //namedWindow("threshold2", WINDOW_AUTOSIZE);
    //createTrackbar("threshold ����", "threshold1", &threshold1, 255, onChangethresholdValue1);
    //createTrackbar("threshold ����", "threshold2", &threshold2, 255, onChangethresholdValue2);
    //
    //threshold(test, test, threshold1, 255, THRESH_BINARY);
    //
    //Matx <uchar, 3, 3>mask(0, 1, 0, 1, 1, 1, 0, 1, 0);
    //morphologyEx(test, test, MORPH_DILATE, mask); //ħ�� ������
    //imshow("threshold1", test);
    //
    //threshold(bin, bin, threshold2, 255, THRESH_BINARY_INV);
    //imshow("threshold2", bin);
    //waitKey(0);

    // 2. ������ threshold �Լ� �׽�Ʈ
    binaryThreshold(test, test, 38, 200, 0, 255);
    rangeThreshold(bin, bin, 38, 200, 0, 255);
    imshow(" binaryT", test);
    imshow("rangeT", bin);
    waitKey();

}


// BINARY threshold call back �Լ�
void onChangethresholdValue1(int value, void* userData)
{
    cvtColor(img, test, COLOR_BGR2GRAY);
    threshold(test, test, value, 255, THRESH_BINARY);

    Matx <uchar, 3, 3>mask(0, 1, 0, 1, 1, 1, 0, 1, 0);
    morphologyEx(test, test, MORPH_DILATE, mask); //ħ��
    imshow("threshold1", test);
    imshow("sum", test | bin);
}

// BINARY INV threshold call back �Լ�
void onChangethresholdValue2(int value, void* userData)
{
    cvtColor(img, bin, COLOR_BGR2GRAY);
    threshold(bin, bin, value, 255, THRESH_BINARY_INV);
    Matx <uchar, 3, 3>mask(0, 1, 0, 1, 1, 1, 0, 1, 0);
    morphologyEx(bin, bin, MORPH_DILATE, mask); //ħ��
    imshow("threshold2", bin);
    imshow("sum", test | bin);
}


// ���� ���� ����ȭ
void binaryThreshold(Mat& src, Mat& dst, int MinValue, int MaxValue, int outValue, int inValue)
{
    if (!dst.data)
        dst = Mat(src.size(), CV_8UC1, Scalar(0));

    for (int i = 0; i < src.rows; i++)
    {
        uchar* srcPtr = src.ptr<uchar>(i);
        uchar* dstptr = dst.ptr<uchar>(i);
        for (int j = 0; j < src.cols; j++)
        {
            if (srcPtr[j] < MaxValue && srcPtr[j] > MinValue)
                dstptr[j] = inValue;
            else
                dstptr[j] = outValue;
        }
    }
}

// ���� �ۿ� �� value�� ����ȭ 
void rangeThreshold(Mat& src, Mat& dst, int MinValue, int MaxValue, int outValue, int inValue)
{
    if (!dst.data)
        dst = Mat(src.size(), CV_8UC1, Scalar(0));

    for (int i = 0; i < src.rows; i++)
    {
        uchar* srcPtr = src.ptr<uchar>(i);
        uchar* dstptr = dst.ptr<uchar>(i);
        for (int j = 0; j < src.cols; j++)
        {
            // max ������ ũ�ų� min�� ���� ũ�� value ġȯ
            if (srcPtr[j] > MaxValue || srcPtr[j] < MinValue)
                dstptr[j] = outValue;
            else
                dstptr[j] = inValue;
        }
    }
}

