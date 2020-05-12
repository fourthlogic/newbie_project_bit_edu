#pragma once
#include "opencv2/opencv.hpp" 
#include <iostream>  
#include <time.h>  
#include <thread>
#include <Windows.h>

using namespace cv;
using namespace std;


// 3�� ��ǥ ���� ������
void GetCornerPoints(Mat& src, vector<Point>& cornerPts);
// �ֿܰ� ROI Vertex ����
void GetVertexPoints(Mat& src, vector<Point>& cornerPts, vector<Point>& vVertexPts, vector<Point>& hVertexPts, int distance);
// �� ����
void CircleDetection(Mat& src, vector<Point>& vertexPts, vector<Point>& circles, int radMin, int radMax, int value);
// �簢�� ������ �� ���� ����
bool IsContain(Rect rc, vector<Point>& cirCenters);
// �̺� �Լ�(prewitt Edge)
void Differential(Mat& src, Mat& dst);
// �ּ��������� ���� Line + Circle ǥ��
void Drawing(Mat& src, Mat& dst, vector<Point>& vCirCenters, vector<Point>& hCirCenters);
// �ּ������� x, y��ǥ ����Ī �� ����� �� ���� ���� �ٽ� y=x ��Ī�̵�
Vec2f LSM_Vertical(vector<Point>& pts);
// �ּ������� �Լ�
Vec2f LSM_Horizontal(vector<Point>& pts);
// �� ���� Thread
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