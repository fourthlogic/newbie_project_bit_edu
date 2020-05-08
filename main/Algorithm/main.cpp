#include "opencv2/opencv.hpp" 
#include <iostream>  
#include <time.h>  

using namespace cv;
using namespace std;


// 최소제곱법을 통해 교점 표시
void drawXandLine(Mat& src, Mat& dst, vector<Point>& LCirCenters, vector<Point>& BCirCenters);

// 최소제곱법 함수
Vec2f LeastSquared(vector<Point>& pts);

// 최소제곱법 함수 (x, y 좌표 스위칭 후 계산)
Vec2f LeastSquared_Reverse(vector<Point>& pts);

// 3점을 통해 외곽 좌표 ROI 선택
void OutermostROI(Mat& src, Mat& Ldst, Mat& Bdst, vector<Point>& pts, int range);

// 3점 좌표 추출 수정
void CornerCoordinate(Mat& src, Mat& Ldst, Mat& Bdst, vector<Point>& dst);

// 변환된 이미지를 통해 원 줌심 추출
void CircleCenterDetection(Mat& Ldst, Mat& Bdst, vector<Vec3f>& LdstCircles, vector<Vec3f>& BdstCircles);

// 원 중심을 통해 새로운 이미지에 서클 그리기
void DrawCircleOnImage(Mat& src, Mat& dst, vector<Point>& LCirCenters, vector<Point>& BCirCenters);

// 수식값 적용
void OutermostROI2(Mat& src, Mat& Ldst, Mat& Bdst, vector<Point>& pts, Vec3f& LEquation, Vec3f& BEquation, int range);

void pointsRollBack(Vec3f& LEquation, Vec3f& BEquation, vector<Vec3f>& LCircles, vector<Vec3f>& BCircles, vector<Point>& LCirCenters, vector<Point>& BCirCenters);

void main()
{
    time_t start;
    time_t end;
    start = clock();
    Mat src = imread("images/a1.png", IMREAD_GRAYSCALE);
    namedWindow("src Image", WINDOW_AUTOSIZE);
    imshow("src Image", src);

    vector<Point> threePoints; // 외곽 3점 좌표 값
    vector<Vec3f> LCircles; // L허프서클 좌표 값
    vector<Vec3f> BCircles; // B허프서클 좌표 값
    Vec3f LEquation; // L 방정식 값
    Vec3f BEquation; // B 방정식 값
    vector<Point> LCirCenters; // L 중심 좌표들의 값 
    vector<Point> BCirCenters; // B 중심 좌표들의 값 

    Mat Ldst;
    Mat Bdst;
    Mat newImage;
    CornerCoordinate(src, Ldst, Bdst, threePoints); // 3점 좌표 추출
    OutermostROI2(src, Ldst, Bdst, threePoints, LEquation, BEquation, 20); // 3점이용 외곽 수식 계산
    CircleCenterDetection(Ldst, Bdst, LCircles, BCircles); // ROI 이미지를 통해 원의 중심좌표 추출 (허프서클)
    pointsRollBack(LEquation, BEquation, LCircles, BCircles, LCirCenters, BCirCenters);
    //DrawCircleOnImage(src, newImage, LCirCenters, BCirCenters); // 추출된 중심좌표를 통해 원 그리기
    drawXandLine(src, newImage, LCirCenters, BCirCenters);
    imshow("Ldst", Ldst);
    imshow("Bdst", Bdst);
    imshow("circleImage", newImage);

    end = clock();
    cout << end - start << endl;
    waitKey();
}

void FindingEquations(vector<Point>& threePoints, vector<Vec2f>& Equations)
{
    if (Equations.size() < 2)
        Equations.resize(2);

    if (threePoints[1].x == threePoints[0].x)
    {
        Equations[0][0] = 0;
        Equations[0][1] = threePoints[0].x;
    }
    else
    {
        Equations[0][0] = ((double)threePoints[1].y - threePoints[0].y) / ((double)threePoints[1].x - threePoints[0].x);
        Equations[0][1] = threePoints[0].y - (Equations[0][0] * threePoints[0].x);
    }

    if (threePoints[2].x == threePoints[1].x) {

        Equations[1][0] = 0;
        Equations[1][1] = threePoints[1].x;
    }
    else {
        Equations[1][0] = ((double)threePoints[2].y - threePoints[1].y) / ((double)threePoints[2].x - threePoints[1].x);
        Equations[1][1] = threePoints[1].y - (Equations[1][0] * threePoints[1].x);
    }
}

// 기존 좌표로 변환
void pointsRollBack(Vec3f& LEquation, Vec3f& BEquation, vector<Vec3f>& LCircles, vector<Vec3f>& BCircles, vector<Point>& LCirCenters, vector<Point>& BCirCenters)
{
    double rotateX;
    double rotateY;
    if (LEquation[0] == 0)
    {
        LCirCenters.resize(LCircles.size());
        for (int i = 0; i < LCircles.size(); i++)
        {
            rotateY = LCircles[i][1] + LEquation[2];
            rotateX = LCircles[i][0] + LEquation[1];
            LCirCenters[i].x = rotateX;
            LCirCenters[i].y = rotateY;
        }
    }
    else
    {
        LCirCenters.resize(LCircles.size());
        for (int i = 0; i < LCircles.size(); i++)
        {
            rotateY = LCircles[i][1] + LEquation[2];
            rotateX = (rotateY - LEquation[1]) / LEquation[0] + LCircles[i][0];
            LCirCenters[i].x = rotateX;
            LCirCenters[i].y = rotateY;
        }
    }

    if (BEquation[0] == 0)
    {

        BCirCenters.resize(BCircles.size());
        for (int i = 0; i < BCircles.size(); i++)
        {
            rotateX = BCircles[i][0] + BEquation[2]; // x + st.x
            rotateY = -20 + (BCircles[i][1] + 1) + BEquation[1]; // hb - y -1 + b
            BCirCenters[i].x = rotateX;
            BCirCenters[i].y = rotateY;
        }
        //rotateY = y + BetaB;
        //rotateY = hB - BCircles[i][1] - 1 + BEquation[1];
    }
    else
    {
        BCirCenters.resize(BCircles.size());
        for (int i = 0; i < BCircles.size(); i++)
        {
            rotateX = BCircles[i][0] + BEquation[2]; // x + st.x
            rotateY = BEquation[0] * rotateX + BEquation[1] - (20 - BCircles[i][1] - 1); // alpha * x + beta - y
            BCirCenters[i].x = rotateX;
            BCirCenters[i].y = rotateY;
        }


        //rotateY = AlphaB * x + BetaB - (hB - y - 1);
        //Bdst.ptr<uchar>(hB - y - 1)[x - pts[1].x] = src.ptr<uchar>((int)rotateY)[x];
    }
}

// 수식값 적용
void OutermostROI2(Mat& src, Mat& Ldst, Mat& Bdst, vector<Point>& pts, Vec3f& LEquation, Vec3f& BEquation, int range = 20)
{
    int hL = abs(pts[0].y - pts[1].y);
    int wL = range;

    int hB = range;
    int wB = abs(pts[1].x - pts[2].x);

    Ldst = Mat(hL, wL, CV_8UC1, Scalar(0));
    Bdst = Mat(hB, wB, CV_8UC1, Scalar(0));

    double AlphaL;
    double BetaL;

    double AlphaB;
    double BetaB;

    double rotateX;
    double rotateY;

    if (pts[1].x == pts[0].x) {
        LEquation[0] = AlphaL = 0;
        LEquation[1] = BetaL = pts[0].x;
        LEquation[2] = pts[0].y;

        for (int y = pts[0].y; y < pts[0].y + hL; y++)
        {
            for (int x = 0; x < wL; x++)
            {
                rotateX = x + BetaL;
                Ldst.ptr<uchar>(y - pts[0].y)[x] = src.ptr<uchar>(y)[(int)rotateX];
            }
        }
    }
    else {
        LEquation[0] = AlphaL = ((double)pts[1].y - pts[0].y) / ((double)pts[1].x - pts[0].x);
        LEquation[1] = BetaL = pts[0].y - (AlphaL * pts[0].x);
        LEquation[2] = pts[0].y;

        for (int y = pts[0].y; y < pts[0].y + hL; y++)
        {
            for (int x = 0; x < wL; x++)
            {
                rotateX = (y - BetaL) / AlphaL + x;
                Ldst.ptr<uchar>(y - pts[0].y)[x] = src.ptr<uchar>(y)[(int)rotateX];
            }
        }
    }

    if (pts[2].x == pts[1].x) {

        BEquation[0] = AlphaB = 0;
        BEquation[1] = BetaB = pts[1].x;
        BEquation[2] = pts[1].x;

        for (int x = pts[1].x; x < pts[1].x + wB; x++)
        {
            for (int y = 0; y < hB; y++)
            {
                rotateY = y + BetaB;
                Bdst.ptr<uchar>(hB - y - 1)[x - pts[1].x] = src.ptr<uchar>((int)rotateY)[x];
            }
        }
    }
    else {
        BEquation[0] = AlphaB = ((double)pts[2].y - pts[1].y) / ((double)pts[2].x - pts[1].x);
        BEquation[1] = BetaB = pts[1].y - (AlphaB * pts[1].x);
        BEquation[2] = pts[1].x;

        for (int x = pts[1].x; x < pts[1].x + wB; x++)
        {
            for (int y = 0; y < hB; y++)
            {
                rotateY = AlphaB * x + BetaB - y;
                Bdst.ptr<uchar>(hB - y - 1)[x - pts[1].x] = src.ptr<uchar>((int)rotateY)[x];
            }
        }
    }

}

// 3점을 통해 기울기를 이용하여 추출된 부분의 ROI Image 생성
void OutermostROI(Mat& src, Mat& Ldst, Mat& Bdst, vector<Point>& pts, int range = 20)
{
    int hL = abs(pts[0].y - pts[1].y);
    int wL = range;

    int hB = range;
    int wB = abs(pts[1].x - pts[2].x);

    Ldst = Mat(hL, wL, CV_8UC1, Scalar(0));
    Bdst = Mat(hB, wB, CV_8UC1, Scalar(0));

    double AlphaL;
    double BetaL;

    double AlphaB;
    double BetaB;

    double rotateX;
    double rotateY;

    if (pts[1].x == pts[0].x) {
        AlphaL = 0;
        BetaL = pts[0].x;

        for (int y = pts[0].y; y < pts[0].y + hL; y++)
        {
            for (int x = 0; x < wL; x++)
            {
                rotateX = x + BetaL;
                Ldst.ptr<uchar>(y - pts[0].y)[x] = src.ptr<uchar>(y)[(int)rotateX];
            }
        }
    }
    else {
        AlphaL = ((double)pts[1].y - pts[0].y) / ((double)pts[1].x - pts[0].x);
        BetaL = pts[0].y - (AlphaL * pts[0].x);

        for (int y = pts[0].y; y < pts[0].y + hL; y++)
        {
            for (int x = 0; x < wL; x++)
            {
                rotateX = (y - BetaL) / AlphaL + x;
                Ldst.ptr<uchar>(y - pts[0].y)[x] = src.ptr<uchar>(y)[(int)rotateX];
            }
        }
    }

    if (pts[2].x == pts[1].x) {

        AlphaB = 0;
        BetaB = pts[1].x;
        for (int x = pts[1].x; x < pts[1].x + wB; x++)
        {
            for (int y = 0; y < hB; y++)
            {
                rotateY = y + BetaB;
                Bdst.ptr<uchar>(hB - y - 1)[x - pts[1].x] = src.ptr<uchar>((int)rotateY)[x];
            }
        }
    }
    else {
        AlphaB = ((double)pts[2].y - pts[1].y) / ((double)pts[2].x - pts[1].x);
        BetaB = pts[1].y - (AlphaB * pts[1].x);

        for (int x = pts[1].x; x < pts[1].x + wB; x++)
        {
            for (int y = 0; y < hB; y++)
            {
                rotateY = AlphaB * x + BetaB - y;
                Bdst.ptr<uchar>(hB - y - 1)[x - pts[1].x] = src.ptr<uchar>((int)rotateY)[x];
            }
        }
    }

}

// 변환된 이미지를 통해 원 줌심 추출
void CircleCenterDetection(Mat& Ldst, Mat& Bdst, vector<Vec3f>& LdstCircles, vector<Vec3f>& BdstCircles)
{
    Mat Ledge;
    Mat Bedge;
    Canny(Ldst, Ledge, 100, 150);
    Canny(Bdst, Bedge, 100, 150);
    //imshow("Ledge", Ledge);
    //imshow("Bedge", Bedge);
    HoughCircles(Ledge, LdstCircles, HOUGH_GRADIENT, 1, 3, 100, 5, 3, 4);
    HoughCircles(Bedge, BdstCircles, HOUGH_GRADIENT, 1, 3, 100, 5, 3, 4);
}

// 원 중심을 통해 새로운 이미지에 서클 그리기
void DrawCircleOnImage(Mat& src, Mat& dst, vector<Point>& LCirCenters, vector<Point>& BCirCenters)
{
    if (!src.data)
        return;
    if (!dst.data)
        cvtColor(src, dst, COLOR_GRAY2BGR);

    for (int i = 0; i < LCirCenters.size(); i++)
    {
        Point center(cvRound(LCirCenters[i].x), cvRound(LCirCenters[i].y));
        circle(dst, center, 5, Scalar(0, 0, 255), 1, -1);
    }

    for (int i = 0; i < BCirCenters.size(); i++)
    {
        Point center(cvRound(BCirCenters[i].x), cvRound(BCirCenters[i].y));
        circle(dst, center, 5, Scalar(0, 0, 255), 1, -1);
    }
}

// 3점 좌표 추출 수정판
void CornerCoordinate(Mat& src, Mat& Ldst, Mat& Bdst, vector<Point>& dst)
{
    Mat img_gray = src.clone();

    threshold(img_gray, img_gray, 110, 255, THRESH_TOZERO); // min_grayscale이 안되면 0
    threshold(img_gray, img_gray, 160, 255, THRESH_TOZERO_INV); // min_grayscale이 넘어도 0

    Matx <uchar, 3, 3> mask(0, 1, 0, 1, 1, 1, 0, 1, 0);
    morphologyEx(img_gray, img_gray, MORPH_OPEN, mask); // 외곽의 솔트를 제거하기 위해

    vector<vector<Point>> contours;
    findContours(img_gray, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

    vector<Point2f> approx;
    vector<Point2f> corners;

    for (size_t i = 0; i < contours.size(); i++)
    {
        approxPolyDP(Mat(contours[i]), approx, arcLength(Mat(contours[i]), true) * 0.07, true);
        if (fabs(contourArea(Mat(approx))) > 10000)  //면적이 일정크기 이상이어야 한다. 
        {
            int size = approx.size();

            for (int k = 0; k < size; k++)
            {
                corners.push_back(approx[k]);
            }
        }
    }

    double distance0to1 = sqrt(pow(corners[0].x - corners[1].x, 2) + pow(corners[0].y - corners[1].y, 2));
    double distance0to3 = sqrt(pow(corners[0].x - corners[3].x, 2) + pow(corners[0].y - corners[3].y, 2));
    if (distance0to1 < distance0to3)
    {
        for (int i = 1; i < corners.size(); i++)
            dst.push_back(corners[i]);
    }
    else
    {
        for (int i = 0; i < corners.size() - 1; i++)
            dst.push_back(corners[i]);
    }

    //cout << "tl = " << dst[0] << endl;
    //cout << "bl = " << dst[1] << endl;
    //cout << "br = " << dst[2] << endl;

}


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
    
    return result;
}

// 최소제곱법을 통해 교점 표시
void drawXandLine(Mat& src, Mat& dst, vector<Point>& LCirCenters, vector<Point>& BCirCenters)
{
    if (!src.data)
        return;
    if (!dst.data)
        cvtColor(src, dst, COLOR_GRAY2BGR);
    Vec2f LEquation, BEquation;
    Point target, temp1, temp2;
    LEquation = LeastSquared_Reverse(LCirCenters);
    BEquation = LeastSquared(BCirCenters);

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

    //cout << "교점의 좌표 = " << target << endl;

    for (int i = 0; i < LCirCenters.size(); i++)
    {
        Point center(LCirCenters[i].x, LCirCenters[i].y);
        if (4 > abs(LEquation[0] * center.x - center.y + LEquation[1]) / sqrt(pow(LEquation[0], 2) + 1))
			circle(dst, center, 5, Scalar(0, 0, 255), 1, -1);
    }
    
    for (int i = 0; i < BCirCenters.size(); i++)
    {
        Point center(BCirCenters[i].x, BCirCenters[i].y);
        if (4 > abs(BEquation[0] * center.x - center.y + BEquation[1]) / sqrt(pow(BEquation[0], 2) + 1))
			circle(dst, center, 5, Scalar(0, 0, 255), 1, -1);
    }
}