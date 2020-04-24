#include "opencv2/opencv.hpp"
#include <iostream>

using namespace cv;
using namespace std;

// 쓰레숄드 값 설정 
void onChangethresholdValue2(int value, void* userData);
void onChangethresholdValue1(int value, void* userData);

Mat img;
Mat test;
Mat bin;
Mat dst;
int threshold1 = 160; // 160
int threshold2 = 100; // 100


int main(int argc, char* argv[])
{
    Mat ROI;
    img = imread("images/20200115220714674 CAM_3 ULC.png", IMREAD_COLOR);

    if (img.empty()) {
        cerr << "Image load failed!" << endl;
        return -1;
    }

    namedWindow("threshold1", WINDOW_AUTOSIZE);
    namedWindow("threshold2", WINDOW_AUTOSIZE);
    createTrackbar("threshold 변경", "threshold1", &threshold1, 255, onChangethresholdValue1);
    createTrackbar("threshold 변경", "threshold2", &threshold2, 255, onChangethresholdValue2);
    cvtColor(img, test, COLOR_BGR2GRAY);
    threshold(test, test, threshold1, 255, THRESH_BINARY);

    Matx <uchar, 3, 3>mask(0, 1, 0, 1, 1, 1, 0, 1, 0);
    morphologyEx(test, test, MORPH_DILATE, mask); //침식 뭉개기
    imshow("threshold1", test);


    cvtColor(img, bin, COLOR_BGR2GRAY);
    threshold(bin, bin, threshold2, 255, THRESH_BINARY_INV);
    imshow("threshold2", bin);

    waitKey(0);
    return 0;
}


// BINARY threshold 
void onChangethresholdValue1(int value, void* userData)
{
    cvtColor(img, test, COLOR_BGR2GRAY);
    threshold(test, test, value, 255, THRESH_BINARY);

    Matx <uchar, 3, 3>mask(0, 1, 0, 1, 1, 1, 0, 1, 0);
    morphologyEx(test, test, MORPH_DILATE, mask); //침식
    imshow("threshold1", test);
    imshow("sum", test | bin);
}

// BINARY INV threshold 
void onChangethresholdValue2(int value, void* userData)
{
    cvtColor(img, bin, COLOR_BGR2GRAY);
    threshold(bin, bin, value, 255, THRESH_BINARY_INV);
    Matx <uchar, 3, 3>mask(0, 1, 0, 1, 1, 1, 0, 1, 0);
    morphologyEx(bin, bin, MORPH_DILATE, mask); //침식
    imshow("threshold2", bin);
    imshow("sum", test | bin);
}


