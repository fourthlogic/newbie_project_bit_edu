#pragma once
#pragma once
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

// 3�� ��ǥ ���� ����
void GetCornerPoints(Mat& src, vector<Point>& cornerPts);
// ������ Threshold
void ToZeroThreshold(Mat& src, Mat& dst, int MinValue, int MaxValue);
// ROI ��ǥ ����
void GetVertexPoints(Mat& src, vector<Point>& cornerPts, vector<Point>& vertexPts, int distance);
// �Ͱ��� ��ǥ�� ��� ����
void MyContours(Mat& src, vector<vector<Point>>& Points);
// �󺧸�
void Labeling(Mat& src, int* Map, vector<Vec3i>& startPoint);
// �ش� ��ǥ�� �ܰ� ����
void Contour(Mat& src, int sx, int sy, vector<Point>& outPoints, int* map, int labelNum);
// ��� ����
void ContourDetection(Mat& src, vector<Point>& hCirCenters, vector<Point>& vCirCenters, vector<Point>& vertexPts, int radMin, int radMax, int value);
// �� ���� Thread
unsigned WINAPI CirDetectionThread(void* para);
// �� ����
void CircleDetection(Mat& src, vector<vector<Point>>& contours, vector<Point>& cirCenters, int radMin, int radMax);
// �̺п��� �Լ�
void Differential(Mat& src, Mat& dst);
// �簢�� ������ �� ���� ����
bool IsContain(Rect rc, vector<Point>& cirCenters);
// �ּ��������� ���� ���� ǥ��
void Drawing(Mat& src, Mat& dst, vector<Point>& hCirCenters, vector<Point>& vCirCenters);
// �ּ������� �Լ� (x, y ��ǥ ����Ī �� ���)
Vec2f LSM_Vertical(vector<Point>& pts);
// �ּ������� �Լ�
Vec2f LSM_Horizontal(vector<Point>& pts);

// �Ÿ� ����  �ɼ� : true == ���� ���� �Ÿ� ,  false == ���� ���� �Ÿ�
double PerpendicularDistance(const Point& pt, const Point& lineStart, const Point& lineEnd, bool flag);
// ��� �Ҽ��� ������ ������ ������ �������� ����
void DouglasPeucker(const vector<Point>& polyLine, vector<Point>& simplifiedPolyLine, double epsilon, bool flag = true, bool endFlag = false);
// ��� Point�� �����ϴ� rect ����
Rect boundRect(vector<Point> pts);
// Points�� ���� �ش� �κ��� �̹��� ����
void fillPoly_(Size matSize, Mat& dst, vector<Point> pts);

