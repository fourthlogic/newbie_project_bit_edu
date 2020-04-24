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
    createTrackbar("Max 변경", "img", &MaxValue, 200, onChangeValue);
    createTrackbar("Min 변경", "img", &MinValue, 200, onChangeValue);

    imageShow();
    waitKey(0);
    return 0;
}

// 트랙바
void onChangeValue(int value, void* userData)
{
    imageShow();
}

// 원
void setLabel(Mat& img, const vector<Point>& pts, const String& label)
{
    Rect rc = boundingRect(pts);// 컨투어의 박스 
    Point pt = Point((rc.x + rc.width / 2), rc.y + rc.height / 2);
    circle(img, pt, 3, Scalar(0, 0, 255), 1); // 중심좌표로 서클변환
   
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
    findContours(canny, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE); // 심플모드 백터수를 줄이기위해
    for (vector<Point> pts : contours) {
        double areaValue = contourArea(pts); // 면적값 계산

        vector<Point> approx;
        approxPolyDP(pts, approx, arcLength(pts, true) * 0.02, true);

        int vtc = (int)approx.size();
        
        if (vtc == 3 || vtc == 4)
            continue;
        Mat mat;
        
        // max값과 min값을 주어 원의 면적으로 필요없는 연산을 제외
        if (areaValue > MaxValue || areaValue < MinValue)
            continue;
        setLabel(dst, pts, ""); // 점을 찍기
    }
    imshow("dst", dst);
}



