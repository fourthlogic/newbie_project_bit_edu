#pragma once
#include "opencv2/opencv.hpp" 
#include <thread>
#include <Windows.h>
#include <iostream>  
#include <time.h>  

using namespace cv;
using namespace std;

typedef struct CirDetectionParam
{
    Mat* src;
    vector<vector<Point>> *contours;
    vector<Point>* cirCenters;
    int radMin;
    int radMax;
    Mutex *mut;
}cirDetectionParam;


typedef struct CirDetectionParam2
{
    Mat & src;
    vector<vector<Point>>& contours;
    vector<Point>& cirCenters;
    int radMin;
    int radMax;
    Mutex mut;
}cirDetectionParam2;

static CirDetectionParam *cir;

// 3점 좌표 추출 수정
void GetCornerPoints(Mat& src, vector<Point>& cornerPts);
//경계 검출
void ContourDetection(Mat& src, vector<Point>& hCirCenters, vector<Point>& vCirCenters, vector<Point>& vertexPts, int radMin, int radMax, int value);
// 원 검출 Thread
unsigned WINAPI CirDetectionThread(void* para);
//원 검출
void CircleDetection(Mat src, vector<vector<Point>> contours, vector<Point>& cirCenters, int radMin, int radMax);
//미분연산 함수
void Differential(Mat& src, Mat& dst);
// 사각형 내부의 점 포함 여부
bool IsContain(Rect rc, vector<Point>& cirCenters);
// 최소제곱법을 통해 교점 표시
void Drawing(Mat& src, Mat& dst, vector<Point>& hCirCenters, vector<Point>& vCirCenters);
// 최소제곱법 함수 (x, y 좌표 스위칭 후 계산)
Vec2f LSM_Vertical(vector<Point>& pts);
// 최소제곱법 함수
Vec2f LSM_Horizontal(vector<Point>& pts);
//ROI 좌표 검출
void GetVertexPoints(Mat& src, vector<Point>& cornerPts, vector<Point>& vertexPts, int distance);



