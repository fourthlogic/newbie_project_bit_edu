#include "opencv2/opencv.hpp"
#include <iostream>

using namespace cv;
using namespace std;
void onChangeValue(int value, void* userData);
void setLabel(Mat& img, const vector<Point>& pts, const String& label);
void imageShow();

Mat img;
Mat dst;
int MaxValue = 98;
int MinValue = 2;

int main(int argc, char* argv[])
{
    Mat ROI;
    img = imread("aa.png", IMREAD_COLOR);

    //Canny(img,)
    if (img.empty()) {
        cerr << "Image load failed!" << endl;
        return -1;
    }
    namedWindow("img", WINDOW_AUTOSIZE);
    createTrackbar("Max ����", "img", &MaxValue, 200, onChangeValue);
    createTrackbar("Min ����", "img", &MinValue, 200, onChangeValue);

    imageShow();
    waitKey(0);
    return 0;
}

// Ʈ����
void onChangeValue(int value, void* userData)
{
    imageShow();
}

// ��
void setLabel(Mat& img, const vector<Point>& pts, const String& label)
{
    Rect rc = boundingRect(pts);// �������� �ڽ� 
    Point pt = Point((rc.x + rc.width / 2), rc.y + rc.height / 2);
    circle(img, pt, 3, Scalar(0, 0, 255), 1); // �߽���ǥ�� ��Ŭ��ȯ
   
}

void imageShow()
{
    dst = img.clone();
    cvtColor(img, dst, COLOR_BGR2GRAY);
    Mat canny;


    imshow("img", img);
    Canny(dst, canny, 100, 150);
    threshold(dst, dst, 150, 250, THRESH_BINARY_INV);
    imshow("canny", canny);
    
   
    

    vector<vector<Point>> contours;
    findContours(canny, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE); // ���ø�� ���ͼ��� ���̱�����
    for (vector<Point> pts : contours) {
        double areaValue = contourArea(pts); // ������ ���

        vector<Point> approx;
        approxPolyDP(pts, approx, arcLength(pts, true) * 0.02, true);

        int vtc = (int)approx.size();
        
        if (vtc == 3 || vtc == 4)
            continue;
        Mat mat;
        
        // max���� min���� �־� ���� �������� �ʿ���� ������ ����
        if (areaValue > MaxValue || areaValue < MinValue)
            continue;
        setLabel(dst, pts, ""); // ���� ���
    }
    imshow("dst", dst);
}



