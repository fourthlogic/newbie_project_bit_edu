#pragma once
#include "opencv2/opencv.hpp" 
#include <thread>
#include <Windows.h>
#include <iostream>  
#include <time.h>  
#include <map>
#include <reason.h>

using namespace std;
using namespace cv;

class CircleDection;

typedef struct CirDetectionParam
{
    CircleDection* This;
	Mat* src;
	vector<vector<Point>>* contours;
	vector<Point>* cirCenters;
	int radMin;
	int radMax;
	Mutex* mut;
}cirDetectionParam;


class CircleDection
{
private:
	Mat src; // �ҽ� �̹��� 
	Mat result; // ��� �̹���
    HBITMAP resultBitmap;
    vector<Point> cornerpts; // �ܰ� 3�� ��ǥ ��
    vector<Point> vCirCenters; // Vertical�� �߽� ��ǥ���� �� 
    vector<Point> hCirCenters; // Horizontal�� �߽� ��ǥ���� �� 
    vector<Point> vertexPts;   // �ֿܰ� ROI Vertex Points
    CirDetectionParam* threadParam; // thread�� ����ü

public:
    void SetImage(Mat& src);
    void SelectImage(CFileDialog& dlg);
    bool RunCircleDection();
    HBITMAP MatToBitmap();
    void GetImageBitmap();
    bool IsImage();
public:
    // 3�� ��ǥ ���� ������
    void GetCornerPoints(Mat& src, vector<Point>& cornerPts);
    // ������ Threshold
    void RangeThreshold(Mat& src, Mat& dst, int MinValue, int MaxValue, int inValue, int outValue);
    // �ܰ��� ����
    void MyContours(Mat& src, vector<vector<Point>>& Points);
    // �󺧸�
    void Labeling(Mat& src, int* Map, vector<Vec3i>& startPoint);
    // �ܰ��� ����
    void Contour(Mat& src, int sx, int sy, vector<Point>& outPoints, int* Map, int value);
    // ��� �Ҽ��� ������ ������ ������ �������� ����
    void DouglasPeucker(const vector<Point>& polyLine, vector<Point>& simplifiedPolyLine, double epsilon, Point lastPt, bool = true);
    // �Ÿ� ����  �ɼ� : true == ���� ���� �Ÿ� ,  false == ���� ���� �Ÿ�
    double PerpendicularDistance(const Point& pt, const Point& lineStart, const Point& lineEnd, bool flag);
    // �ֿܰ� ROI Vertex ����
    void GetVertexPoints(Mat& src, vector<Point>& cornerPts, vector<Point>& vertexPts, int distance);
    // �������� ���� ROI Area ����
    void ContourDetection(Mat& src, vector<Point>& vCirCenters, vector<Point>& hCirCenters, vector<Point>& vertexPts, int radMin, int radMax, int value);
    // Points�� ���� �ش� �κ��� �̹��� ����
    void fillPoly_(Size matSize, Mat& dst, vector<Point> pts);
    // �� ���� Thread
    static unsigned WINAPI CirDetectionThread(void* para);
    // �� ����
    void CircleDetection(Mat& src, vector<vector<Point>>& contours, vector<Point>& cirCenters, int radMin, int radMax);
    // ��� Point�� �����ϴ� rect ����
    Rect boundRect(vector<Point> pts);
    // �̺� �Լ�(prewitt Edge)
    void Differential(Mat& src, Mat& dst);
    // �簢�� ������ �� ���� ����
    bool IsContain(Rect rc, vector<Point>& cirCenters);
    // �ּ��������� ���� ���� ǥ��, ���� �� �� �׸���
    void Drawing(Mat& src, Mat& dst, vector<Point>& vCirCenters, vector<Point>& hCirCenters);
    // �ּ������� x, y��ǥ ����Ī �� ����� �� ���� ���� �ٽ� y=x ��Ī�̵�
    Vec2f LSM_Vertical(vector<Point>& pts);
    // �ּ������� �Լ�
    Vec2f LSM_Horizontal(vector<Point>& pts);
};

