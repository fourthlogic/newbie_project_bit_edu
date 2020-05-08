#include "opencv2/opencv.hpp" 
#include <iostream>  

using namespace cv;
using namespace std;


// 최소제곱법을 통해 교점 표시
void drawXandLine(Mat& src, Mat& dst, vector<Point>& LCirCenters, vector<Point>& BCirCenters);
// 최소제곱법 함수
Vec2f LeastSquared(vector<Point>& pts);
// 최소제곱법 함수 (x, y 좌표 스위칭 후 계산)
Vec2f LeastSquared_Reverse(vector<Point>& pts);


// 최소제곱법 함수
Vec2f LeastSquared(vector<Point>& pts)
{
    double a, b;
    double xx = 0, x = 0, xy = 0, y = 0;
    int n = pts.size();
    Vec2f result;
    for (int i = 0; i < n; i++)
    {
        xx += (pts[i].x) * (pts[i].x);
        x += (pts[i].x);
        xy += (pts[i].x) * (pts[i].y);
        y += (pts[i].y);
        
    }
    a = (n * xy - x * y) / (n * xx - x * x);
    b = (xx * y - x * xy) / (n * xx - x * x);
    result[0] = a;
    result[1] = b;
    
    //cout << "y=" << a << "x+" << b << endl; // y=ax+b
    return result;
}

// 최소제곱법 x, y좌표 스위칭 후 계산한 뒤 나온 식을 다시 y=x 대칭이동
Vec2f LeastSquared_Reverse(vector<Point>& pts)
{
    double a, b;
    double xx = 0, x = 0, xy = 0, y = 0;
    int n = pts.size();
    Vec2f result;
    for (int i = 0; i < n; i++)
    {
        xx += (pts[i].y) * (pts[i].y);
        x += (pts[i].y);
        xy += (pts[i].y) * (pts[i].x);
        y += (pts[i].x);

    }
    a = (n * xy - x * y) / (n * xx - x * x);
    b = (xx * y - x * xy) / (n * xx - x * x);
    // x=ya+b ==> y = (1/a) * x - b/a
    result[0] = 1/a;
    result[1] = -b/a;

    //cout << "y=" << a << "x+" << b << endl; // y=ax+b
    return result;
}

// 직선의 방정식에서 일정 거리 이상 떨어진 원은 제외하고 표시 (교점, 직선, 원 디스플레이)
void drawXandLine(Mat& src, Mat& dst, vector<Point>& LCirCenters, vector<Point>& BCirCenters)
{
    vector<Point> NewLCirCenters, NewBCirCenters;
    if (!src.data)
        return;
    if (!dst.data)
        cvtColor(src, dst, COLOR_GRAY2BGR);

    Vec2f LEquation, BEquation;
    Point target, temp1, temp2;
    LEquation = LeastSquared_Reverse(LCirCenters);
    BEquation = LeastSquared(BCirCenters);

    // 직선의 방정식과 거리가 먼 경우를 제외하고 원 표시
    for (int i = 0; i < LCirCenters.size(); i++)
    {
        Point center(LCirCenters[i].x, LCirCenters[i].y);
        if (4 > abs(LEquation[0] * center.x - center.y + LEquation[1]) / sqrt(pow(LEquation[0], 2) + 1))
        {
			circle(dst, center, 5, Scalar(0, 0, 255), 1, -1);
            NewLCirCenters.push_back(center);
        }
    }
    
    for (int i = 0; i < BCirCenters.size(); i++)
    {
        Point center(BCirCenters[i].x, BCirCenters[i].y);
        if (4 > abs(BEquation[0] * center.x - center.y + BEquation[1]) / sqrt(pow(BEquation[0], 2) + 1))
        {
			circle(dst, center, 5, Scalar(0, 0, 255), 1, -1);
            NewBCirCenters.push_back(center);
        }
    }

    LEquation = LeastSquared_Reverse(NewLCirCenters);
    BEquation = LeastSquared(NewBCirCenters);
    target.x = cvRound((BEquation[1] - LEquation[1]) / (LEquation[0] - BEquation[0]));
    target.y = cvRound((LEquation[0] * (BEquation[1] - LEquation[1])) / (LEquation[0] - BEquation[0]) + LEquation[1]);

    // 주황색 직선의 방정식 그리기
    temp1 = { cvRound(-LEquation[1] / LEquation[0]), 0 }; // y=0 이고, 최소제곱법 직선의 방정식을 지나는 점
    temp2 = { cvRound((target.y + 20 - LEquation[1]) / LEquation[0]), target.y + 20 }; // target에서 좀 더 아래쪽의 점
    line(dst, temp1, temp2, Scalar(0, 127, 255));
    temp1 = { dst.cols, cvRound(BEquation[0] * dst.cols + BEquation[1]) }; // x = src.cols 이고, 최소제곱법 직선의 방정식을 지나는 점
    temp2 = { target.x - 20, cvRound(BEquation[0] * (target.x - 20) + BEquation[1]) }; // target에서 좀 더 왼쪽의 점
    line(dst, temp1, temp2, Scalar(0, 127, 255));

    // 라임색 X 그리기
    temp1 = { target.x - 7, target.y - 7 };
    temp2 = { target.x + 7,target.y + 7 };
    line(dst, temp1, temp2, Scalar(0, 255, 191));
    temp1 = { target.x - 7, target.y + 7 };
    temp2 = { target.x + 7,target.y - 7 };
    line(dst, temp1, temp2, Scalar(0, 255, 191));
}