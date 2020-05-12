#pragma once
#include "opencv2/opencv.hpp" 
#include <iostream>  
#include <time.h>  
#include <thread>
#include <Windows.h>

using namespace cv;
using namespace std;


// 3점 좌표 추출 수정판
void GetCornerPoints(Mat& src, vector<Point>& cornerPts);
// 최외곽 ROI Vertex 추출
void GetVertexPoints(Mat& src, vector<Point>& cornerPts, vector<Point>& vVertexPts, vector<Point>& hVertexPts, int distance);
// 원 검출
void CircleDetection(Mat& src, vector<Point>& vertexPts, vector<Point>& circles, int radMin, int radMax, int value);
// 사각형 내부의 점 포함 여부
bool IsContain(Rect rc, vector<Point>& cirCenters);
// 미분 함수(prewitt Edge)
void Differential(Mat& src, Mat& dst);
// 최소제곱법을 통해 Line + Circle 표시
void Drawing(Mat& src, Mat& dst, vector<Point>& vCirCenters, vector<Point>& hCirCenters);
// 최소제곱법 x, y좌표 스위칭 후 계산한 뒤 나온 식을 다시 y=x 대칭이동
Vec2f LSM_Vertical(vector<Point>& pts);
// 최소제곱법 함수
Vec2f LSM_Horizontal(vector<Point>& pts);
// 원 검출 Thread
unsigned WINAPI CirDetectionThread(void* para);

typedef struct CirDetectionParam
{
    Mat& src;
    vector<Point>& Circles;
    vector<Point>& conerPts;
    int radMin;
    int radMax;
    int value;
}cirDetectionParam;