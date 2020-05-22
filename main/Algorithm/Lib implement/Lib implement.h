#pragma once
#include "opencv2/opencv.hpp" 
#include <thread>
#include <Windows.h>
#include <iostream>  
#include <time.h>  
#include <map>

using namespace cv;
using namespace std;

typedef struct CirDetectionParam
{
    Mat* src;
    vector<vector<Point>>* contours;
    vector<Point>* cirCenters;
    int radMin;
    int radMax;
    Mutex* mut;
}cirDetectionParam;

static CirDetectionParam* threadParam;

// 3점 좌표 추출 수정
void GetCornerPoints(Mat& src, vector<Point>& cornerPts);
// 범위용 Threshold
void RangeThreshold(Mat& src, Mat& dst, int MinValue = 0, int MaxValue = 255, int inValue = 0, int outValue = 255);
// ROI 좌표 검출
void GetVertexPoints(Mat& src, vector<Point>& cornerPts, vector<Point>& vertexPts, int distance);
// 와곽의 좌표를 모두 검출
void MyContours(Mat& src, vector<vector<Point>>& Points);
// 라벨링
void Labeling(Mat& src, int* Map, vector<Vec3i>& startPoint);
// 해당 좌표의 외곽 검출
void Contour(Mat& src, int sx, int sy, vector<Point>& outPoints, int* map, int labelNum);
// 경계 검출
void ContourDetection(Mat& src, vector<Point>& hCirCenters, vector<Point>& vCirCenters, vector<Point>& vertexPts, int radMin, int radMax, int value);
// 원 검출 Thread
unsigned WINAPI CirDetectionThread(void* para);
// 원 검출
void CircleDetection(Mat& src, vector<vector<Point>>& contours, vector<Point>& cirCenters, int radMin, int radMax);
// 미분연산 함수
void Differential(Mat& src, Mat& dst);
// 사각형 내부의 점 포함 여부
bool IsContain(Rect rc, vector<Point>& cirCenters);
// 최소제곱법을 통해 교점 표시
void Drawing(Mat& src, Mat& dst, vector<Point>& hCirCenters, vector<Point>& vCirCenters);
// 최소제곱법 함수 (x, y 좌표 스위칭 후 계산)
Vec2f LSM_Vertical(vector<Point>& pts);
// 최소제곱법 함수
Vec2f LSM_Horizontal(vector<Point>& pts);
// 거리 추출  옵션 : true == 점과 점의 거리 ,  false == 선과 점의 거리
double PerpendicularDistance(const Point& pt, const Point& lineStart, const Point& lineEnd, bool flag);
// 곡선을 소수의 점으로 구성한 유사한 직선으로 구분
void DouglasPeucker(const vector<Point>& polyLine, vector<Point>& simplifiedPolyLine, double epsilon, Point lastPt, bool flag = true);
// 모든 Point를 포함하는 rect 추출
Rect boundRect(vector<Point> pts);
// Points를 통해 해당 부분의 이미지 추출
void fillPoly_(Size matSize, Mat& dst, vector<Point> pts);

