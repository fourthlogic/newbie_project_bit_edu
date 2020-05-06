#include "opencv2/opencv.hpp" 
#include <iostream>  

using namespace cv;
using namespace std;


// 최소제곱법을 통해 교점 표시
void drawXandLine(Mat& src, vector<Point>& cols, vector<Point>& rows);
// 최소제곱법 함수
Point2f LeastSquared(vector<Point>& pts);
// 최소제곱법 함수 (x, y 좌표 스위칭 후 계산)
Point2f LeastSquared_Reverse(vector<Point>& pts);


// 최소제곱법 함수
Point2f LeastSquared(vector<Point>& pts)
{
    double a, b;
    double xx = 0, x = 0, xy = 0, y = 0;
    int n = pts.size();
    Point2f result;
    for (int i = 0; i < n; i++)
    {
        xx += (pts[i].x) * (pts[i].x);
        x += (pts[i].x);
        xy += (pts[i].x) * (pts[i].y);
        y += (pts[i].y);

    }
    a = (n * xy - x * y) / (n * xx - x * x);
    b = (xx * y - x * xy) / (n * xx - x * x);
    result.x = a;
    result.y = b;

    cout << "y=" << a << "x+" << b << endl; // y=ax+b
    return result;
}
// 최소제곱법 x, y좌표 스위칭 후 계산한 뒤 나온 식을 다시 y=x 대칭이동
Point2f LeastSquared_Reverse(vector<Point>& pts)
{
    double a, b;
    double xx = 0, x = 0, xy = 0, y = 0;
    int n = pts.size();
    Point2f result;
    for (int i = 0; i < n; i++)
    {
        xx += (pts[i].y) * (pts[i].y);
        x += (pts[i].y);
        xy += (pts[i].y) * (pts[i].x);
        y += (pts[i].x);

    }
    a = (n * xy - x * y) / (n * xx - x * x);
    b = (xx * y - x * xy) / (n * xx - x * x);
    result.x = 1 / a;
    result.y = -b / a;

    return result;
}

// 최소제곱법을 통해 교점 표시
void drawXandLine(Mat& src, vector<Point>& cols, vector<Point>& rows)
{
    Point2f const_cols, const_rows;
    Point target, temp1, temp2;
    const_cols = LeastSquared_Reverse(cols);
    const_rows = LeastSquared(rows);

    target.x = cvRound((const_rows.y - const_cols.y) / (const_cols.x - const_rows.x));
    target.y = cvRound((const_cols.x * (const_rows.y - const_cols.y)) / (const_cols.x - const_rows.x) + const_cols.y);

    // 주황색 직선의 방정식 그리기
    temp1 = { cvRound(-const_cols.y / const_cols.x), 0 }; // y=0 이고, 최소제곱법 직선의 방정식을 지나는 점
    temp2 = { cvRound((target.y + 20 - const_cols.y) / const_cols.x), target.y + 20 }; // target에서 좀 더 아래쪽의 점
    line(src, temp1, temp2, Scalar(0, 127, 255));
    temp1 = { src.cols, cvRound(const_rows.x * src.cols + const_rows.y) }; // x = src.cols 이고, 최소제곱법 직선의 방정식을 지나는 점
    temp2 = { target.x - 20, cvRound(const_rows.x * (target.x - 20) + const_rows.y) }; // target에서 좀 더 왼쪽의 점
    line(src, temp1, temp2, Scalar(0, 127, 255));

    // 라임색 X 그리기
    temp1 = { target.x - 7, target.y - 7 };
    temp2 = { target.x + 7,target.y + 7 };
    line(src, temp1, temp2, Scalar(0, 255, 191));
    temp1 = { target.x - 7, target.y + 7 };
    temp2 = { target.x + 7,target.y - 7 };
    line(src, temp1, temp2, Scalar(0, 255, 191));

    cout << "교점의 좌표 = " << target << endl;
}