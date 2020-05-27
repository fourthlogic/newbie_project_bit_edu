#pragma once
#include "opencv2/opencv.hpp" 
#include <thread>
#include <Windows.h>
#include <iostream>  
#include <time.h>  
#include <map>

using namespace std;
using namespace cv;

class CircleDection;

typedef struct CirDetectionParam
{
    CircleDection* This;
    vector<vector<Point>>* contours;
    vector<Point>* cirCenters;
    Mutex* mut;
}cirDetectionParam;


class CircleDection
{
private:
    Mat src; // �ҽ� �̹��� 
    Mat result; // ��� �̹���
    HBITMAP resultBitmap;
    vector<Point> cornerPts; // �ܰ� 3�� ��ǥ ��
    vector<Point> vCirCenters; // Vertical�� �߽� ��ǥ���� �� 
    vector<Point> hCirCenters; // Horizontal�� �߽� ��ǥ���� �� 
    vector<Point> vertexPts;   // �ֿܰ� ROI Vertex Points
    CirDetectionParam* threadParam; // thread�� ����ü
    int distance; // ROI ���� ����
    int radMin; // ����� �ּ� ������
    int radMax; // ����� �ִ� ������
    int cirThresh; // �׷���
    int thresh1; // �ܰ� ��ǥ ���� th
    int thresh2; // �ܰ� ��ǥ ���� th
    int height; // ����
    int width; // ����
    int size; // ��ü ũ��

public:
    //bool CircleDection::RunCircleDection();
    //HBITMAP CircleDection::MatToBitmap();
    //bool CircleDection::IsImage();
    //void CircleDection::SelectImage();
    void Init(); // �ʱ�ȭ
    void Run(); // ��ü ����
    void SetImage(Mat& src); // �̹��� ����
    void SetThreshValue(int thresh1, int thresh2); // Thresh value set
    void SetDistance(int distance); // ROI ���� ���� set
    void SetCircleValue(int radMin, int radMax, int cirThresh); // ������ set
    bool ShowSrcImage(); // �ҽ� �̹��� ���
    bool ShowResultImage(); // ��� �̹��� ���
private:
    // 3�� ��ǥ ���� ������
    void GetCornerPoints();
    // ������ Threshold
    void ToZeroThreshold(Mat& src, Mat& dst, int MinValue, int MaxValue);
    // �ܰ��� ����
    void MyContours(Mat& src, vector<vector<Point>>& Points);
    // �󺧸�
    void Labeling(Mat& src, int* Map, vector<Vec3i>& startPoint);
    // �ܰ��� ����
    void Contour(Mat& src, int sx, int sy, vector<Point>& outPoints, int* Map, int value);
    // ��� �Ҽ��� ������ ������ ������ �������� ����
   // �Ÿ� ����  �ɼ� : true == ���� ���� �Ÿ� ,  false == ���� ���� �Ÿ�
    double PerpendicularDistance(const Point& pt, const Point& lineStart, const Point& lineEnd, bool flag);
    // ��� �Ҽ��� ������ ������ ������ �������� ����
    void DouglasPeucker(const vector<Point>& polyLine, vector<Point>& simplifiedPolyLine, double epsilon, bool flag = true, bool endFlag = false);
    // �ֿܰ� ROI Vertex ����
    void GetVertexPoints();
    // �������� ���� ROI Area ����
    void ContourDetection();
    // Points�� ���� �ش� �κ��� �̹��� ����
    void fillPoly_(Size matSize, Mat& dst, vector<Point> pts);
    // �� ���� Thread
    static unsigned WINAPI CirDetectionThread(void* para);
    // �� ����
    void CircleDetection(vector<vector<Point>>& contours, vector<Point>& cirCenters);
    // ��� Point�� �����ϴ� rect ����
    Rect boundRect(vector<Point> pts);
    // �̺� �Լ�(prewitt Edge)
    void Differential(Mat& src, Mat& dst);
    // �簢�� ������ �� ���� ����
    bool IsContain(Rect rc, vector<Point>& cirCenters);
    // �ּ��������� ���� ���� ǥ��, ���� �� �� �׸���
    void Drawing();
    // �ּ������� x, y��ǥ ����Ī �� ����� �� ���� ���� �ٽ� y=x ��Ī�̵�
    Vec2f LSM_Vertical(vector<Point>& pts);
    // �ּ������� �Լ�
    Vec2f LSM_Horizontal(vector<Point>& pts);
};

