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

// 3�� ��ǥ ���� ����
void GetCornerPoints(Mat& src, vector<Point>& cornerPts);
//��� ����
void ContourDetection(Mat& src, vector<Point>& hCirCenters, vector<Point>& vCirCenters, vector<Point>& vertexPts, int radMin, int radMax, int value);
// �� ���� Thread
unsigned WINAPI CirDetectionThread(void* para);
//�� ����
void CircleDetection(Mat src, vector<vector<Point>> contours, vector<Point>& cirCenters, int radMin, int radMax);
//�̺п��� �Լ�
void Differential(Mat& src, Mat& dst);
// �簢�� ������ �� ���� ����
bool IsContain(Rect rc, vector<Point>& cirCenters);
// �ּ��������� ���� ���� ǥ��
void Drawing(Mat& src, Mat& dst, vector<Point>& hCirCenters, vector<Point>& vCirCenters);
// �ּ������� �Լ� (x, y ��ǥ ����Ī �� ���)
Vec2f LSM_Vertical(vector<Point>& pts);
// �ּ������� �Լ�
Vec2f LSM_Horizontal(vector<Point>& pts);
//ROI ��ǥ ����
void GetVertexPoints(Mat& src, vector<Point>& cornerPts, vector<Point>& vertexPts, int distance);



