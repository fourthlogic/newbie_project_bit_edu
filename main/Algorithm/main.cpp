#include "opencv2/opencv.hpp" 
#include <iostream>  
#include <time.h>  

using namespace cv;
using namespace std;


// 최소제곱법을 통해 교점 표시
void Drawing(Mat& src, Mat& dst, vector<Point>& HCirCenters, vector<Point>& VCirCenters);

// 최소제곱법 함수
Vec2f LSM_Horizontal(vector<Point>& pts);

// 최소제곱법 함수 (x, y 좌표 스위칭 후 계산)
Vec2f LSM_Vertical(vector<Point>& pts);


// 3점 좌표 추출 수정
void CornerCoordinate(Mat& src, vector<Point>& conerPts);
//미분연산 함수
void Differential(Mat& src, Mat& dst);
//원 검출
void CircleDetection(Mat& src, vector<Point>& circles, vector<Point>& conerPts, int radMin, int radMax, int value);
//ROI 좌표 검출
void ROI_Points(Mat& src, vector<Point>& conerPts, vector<Point>& vConerPts, vector<Point>& hConerPts, int range);

void main()
{
    time_t start;
    time_t end;
    Mat src = imread("image/a (8).png", IMREAD_GRAYSCALE);
    namedWindow("src Image", WINDOW_AUTOSIZE);
    //imshow("src Image", src);
    start = clock();

    vector<Point> conerpts; // 외곽 3점 좌표 값
  
    vector<Point> vConerPts; // L 중심 좌표들의 값 
    vector<Point> hConerPts; // B 중심 좌표들의 값 

    vector<Point> VCirCenters;
    vector<Point> HCirCenters;

    Mat newImage;
 
    CornerCoordinate(src,conerpts); // 3점 좌표 추출
    
    ROI_Points(src, conerpts, VCirCenters, HCirCenters, 20);
    time_t circle_start, circle_end;
    circle_start = clock();
    CircleDetection(src, vConerPts, VCirCenters, 2, 4, 130);
    CircleDetection(src, hConerPts, HCirCenters, 2, 4, 130);
 
    
    Drawing(src, newImage, vConerPts, hConerPts);
    circle_end = clock();
    cout << "circle: " << circle_end - circle_start << endl;
    
    imshow("circleImage", newImage);

    end = clock();
    cout << end - start << endl;
    waitKey();
}




// 3점 좌표 추출 수정판
void CornerCoordinate(Mat& src, vector<Point>& conerPts)
{
    Mat grayImage = src.clone();

    threshold(grayImage, grayImage, 110, 255, THRESH_TOZERO); // min_grayscale이 안되면 0
    threshold(grayImage, grayImage, 158, 255, THRESH_TOZERO_INV); // min_grayscale이 넘어도 0

    Matx <uchar, 3, 3> mask(0, 1, 0, 1, 1, 1, 0, 1, 0);
    morphologyEx(grayImage, grayImage, MORPH_OPEN, mask); // 외곽의 솔트를 제거하기 위해

    vector<vector<Point>> contours;
    findContours(grayImage, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

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
    //0(top) 부터 반시계 방향(꼭짓점) 
    double distance0to1 = sqrt(pow(corners[0].x - corners[1].x, 2) + pow(corners[0].y - corners[1].y, 2));
    double distance0to3 = sqrt(pow(corners[0].x - corners[3].x, 2) + pow(corners[0].y - corners[3].y, 2));
    if (distance0to1 < distance0to3)
    {
        for (int i = 1; i < corners.size(); i++)
            conerPts.push_back(corners[i]);
    }
    else
    {
        for (int i = 0; i < corners.size() - 1; i++)
            conerPts.push_back(corners[i]);
    }

    //cout << "tl = " << dst[0] << endl;
    //cout << "bl = " << dst[1] << endl;
    //cout << "br = " << dst[2] << endl;

}


// 최소제곱법 함수
Vec2f LSM_Horizontal(vector<Point>& pts)
{
    //y=ax+b
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
Vec2f LSM_Vertical(vector<Point>& pts)
{
    //x=ay+b
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
    result[0] = 1 / a;
    result[1] = -b / a;

    return result;
}

// 최소제곱법을 통해 교점 표시
void Drawing(Mat& src, Mat& dst, vector<Point>& vCirCenters, vector<Point>& hCirCenters)
{
    if (!src.data)
        return;
    if (!dst.data)
        cvtColor(src, dst, COLOR_GRAY2BGR);
    Vec2f vEquation, hEquation;
    Point target, temp1, temp2;
    vEquation = LSM_Vertical(vCirCenters);
    hEquation = LSM_Horizontal(hCirCenters);

    target.x = cvRound((hEquation[1] - vEquation[1]) / (vEquation[0] - hEquation[0]));
    target.y = cvRound((vEquation[0] * (hEquation[1] - vEquation[1])) / (vEquation[0] - hEquation[0]) + vEquation[1]);

    // 주황색 직선의 방정식 그리기
    temp1 = { cvRound(-vEquation[1] / vEquation[0]), 0 }; // y=0 이고, 최소제곱법 직선의 방정식을 지나는 점
    temp2 = { cvRound((target.y + 20 - vEquation[1]) / vEquation[0]), target.y + 20 }; // target에서 좀 더 아래쪽의 점
    line(dst, temp1, temp2, Scalar(0, 127, 255));
    temp1 = { dst.cols, cvRound(hEquation[0] * dst.cols + hEquation[1]) }; // x = src.cols 이고, 최소제곱법 직선의 방정식을 지나는 점
    temp2 = { target.x - 20, cvRound(hEquation[0] * (target.x - 20) + hEquation[1]) }; // target에서 좀 더 왼쪽의 점
    line(dst, temp1, temp2, Scalar(0, 127, 255));

    // 라임색 X 그리기
    temp1 = { target.x - 7, target.y - 7 };
    temp2 = { target.x + 7,target.y + 7 };
    line(dst, temp1, temp2, Scalar(0, 255, 191));
    temp1 = { target.x - 7, target.y + 7 };
    temp2 = { target.x + 7,target.y - 7 };
    line(dst, temp1, temp2, Scalar(0, 255, 191));

    //cout << "교점의 좌표 = " << target << endl;

    for (int i = 0; i < vCirCenters.size(); i++)
    {
        Point center(vCirCenters[i].x, vCirCenters[i].y);
        if (4 > abs(vEquation[0] * center.x - center.y + vEquation[1]) / sqrt(pow(vEquation[0], 2) + 1))
            circle(dst, center, 5, Scalar(0, 0, 255), 1, -1);
    }

    for (int i = 0; i < hCirCenters.size(); i++)
    {
        Point center(hCirCenters[i].x, hCirCenters[i].y);
        if (4 > abs(hEquation[0] * center.x - center.y + hEquation[1]) / sqrt(pow(hEquation[0], 2) + 1))
            circle(dst, center, 5, Scalar(0, 0, 255), 1, -1);
    }
}

void NoiseRemove(Mat& src, Mat& dst, int min,int value) {
    uchar* srcPtr;
    uchar* dstPtr;
    for (int x = 0; x < src.rows; x++) {
        srcPtr = src.ptr < uchar >(x);
        dstPtr = dst.ptr < uchar >(x);

        for (int y = 0; y < src.cols; y++) {
            if (srcPtr[y] > min)
                dstPtr[y] = value;
        }
    }
}
void Differential(Mat& src, Mat& dst) {
    Mat dstY, dstX;


    float prewittY[] = {
        -1,0,1,
        -1,0,1,
        -1,0,1
    };
    float prewittX[] = {
        -1,-1,-1,
        0,0,0,
        1,1,1
    };
    Mat maskY(3, 3, CV_32F, prewittY);
    Mat maskX(3, 3, CV_32F, prewittX);

    filter2D(src, dstY, CV_32F, maskY, Point(-1, -1), 3, 1);	
    filter2D(src, dstX, CV_32F, maskX, Point(-1, -1), 3, 1);	

    magnitude(dstY, dstX, dst);	//SQRT(dst1^2, dst2^2)
    
    dst.convertTo(dst, CV_8U);
}

void CircleDetection(Mat& src, vector<Point>& Circles, vector<Point>& conerPts, int radMin, int radMax,int value) {
    vector<vector<Point>> vpts;
    vpts.push_back(conerPts);

    Mat mask = Mat::zeros(src.size(), CV_8UC1);
    fillPoly(mask, vpts, Scalar(255, 255, 255), 8, 0);
    Mat ROI;
    bitwise_and(src, mask, ROI);


    Mat imgThreshold = ROI.clone();
    Mat imgHoughCir = ROI.clone();
    threshold(imgThreshold, imgThreshold, 100, 250, THRESH_BINARY_INV);

    //==================
  /*  imshow("c", imgThreshold);
    waitKey(0);*/
    //==================

    vector<vector<Point>> contours;
    findContours(imgThreshold, contours, RETR_LIST, CHAIN_APPROX_SIMPLE); // 심플모드 백터수를 줄이기위해

    vector<Vec3f> houghCircles;


    for (vector<Point> pts : contours) {
        double areaValue = contourArea(pts); // 면적값 계산
        if (areaValue < 50 && areaValue > 0) {
            Rect rc = boundingRect(pts);

            rectangle(src, rc, Scalar(255, 0, 0));

            if (rc.tl().x - 3 < 0 || rc.tl().y - 3 < 0)
                continue;
            rc -= Point(3, 3);
            if (rc.tl().x + rc.width + 3 > src.cols || rc.tl().y + rc.height + 3 > src.rows)
                continue;
            rc += Size(3, 3);
            Mat cirRect = imgHoughCir(rc);

            //rectangle(image, rc, Scalar(0, 0, 255), 1);
            Differential(cirRect, cirRect);

            /*imshow("cirRect", cirRect);
            waitKey(0);*/

            HoughCircles(cirRect, houghCircles, HOUGH_GRADIENT, 1, 6, 255, 5, radMin, radMax);
            if (houghCircles.size() != 0)
                Circles.push_back(Point(houghCircles[0][0] + rc.tl().x, houghCircles[0][1] + rc.tl().y));
        }
    }
}

void ROI_Points(Mat& src, vector<Point>& conerPts, vector<Point>& vConerPts, vector<Point>& hConerPts, int distance)
{
    int rotateX, rotateY;
    double theta;

    int heightV = sqrt(pow(conerPts[0].y - conerPts[1].y, 2) + pow(conerPts[0].x - conerPts[1].x, 2));
    int widthV = distance;

    int heightH = distance;
    int widthH = sqrt(pow(conerPts[1].y - conerPts[2].y, 2) + pow(conerPts[1].x - conerPts[2].x, 2));


    if (conerPts[1].x == conerPts[0].x) {
        rotateY = conerPts[0].y + heightV;
        rotateX = conerPts[0].x + widthV;
        vConerPts.push_back(conerPts[0]);
        vConerPts.push_back(conerPts[1]);
        vConerPts.push_back(Point(conerPts[1].x + distance, conerPts[1].y));
        vConerPts.push_back(Point(conerPts[0].x + distance, conerPts[0].y));
    }
    else {
        theta = atan((double)((conerPts[0].x - conerPts[1].x)) / (conerPts[0].y - conerPts[1].y));

        vConerPts.push_back(conerPts[0]);

        vConerPts.push_back(conerPts[1]);

        rotateX = widthV * cos(theta) + heightV * sin(theta) + conerPts[0].x;
        rotateY = -widthV * sin(theta) + heightV * cos(theta) + conerPts[0].y;
        vConerPts.push_back(Point(rotateX, rotateY));

        rotateX = (distance)*cos(theta) + conerPts[0].x;
        rotateY = -(distance)*sin(theta) + conerPts[0].y;
        vConerPts.push_back(Point(rotateX, rotateY));

    }

    if (conerPts[2].y == conerPts[1].y) {
        rotateY = conerPts[0].y + heightV;
        rotateX = conerPts[0].x + widthV;
        hConerPts.push_back(Point(conerPts[1].x, conerPts[1].y - distance));
        hConerPts.push_back(conerPts[1]);
        hConerPts.push_back(conerPts[2]);
        hConerPts.push_back(Point(conerPts[2].x, conerPts[2].y - distance));
    }
    else {
        theta = -atan((conerPts[1].y - conerPts[2].y) / (double)((conerPts[1].x - conerPts[2].x)));
        rotateX = -distance * sin(theta) + conerPts[1].x;
        rotateY = -distance * cos(theta) + conerPts[1].y;

        hConerPts.push_back(Point(rotateX, rotateY));
        hConerPts.push_back(conerPts[1]);
        hConerPts.push_back(conerPts[2]);

        rotateX = widthH * cos(theta) - heightH * sin(theta) + conerPts[1].x;
        rotateY = -widthH * sin(theta) - heightH * cos(theta) + conerPts[1].y;
        hConerPts.push_back(Point(rotateX, rotateY));
    }
}
