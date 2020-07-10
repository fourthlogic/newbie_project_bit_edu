#pragma once
#include "pch.h"
#include <opencv2/opencv.hpp> 
#include <map>
#include <thread>
#include <Windows.h>
#include <time.h>

using namespace std;
using namespace cv;

class CircleDection;

typedef struct CirDetectionParam
{
    CircleDection* This;
    vector<vector<Point>>* contours;
    vector<Vec3f>* cirCenters;
}cirDetectionParam;

class CircleDection
{
private:
    Mat Original; // �������� �̹���
    Mat src; // Ÿ�� �ҽ� �̹��� 
    Mat Rotate; // ȸ�� �̹���
    HBITMAP resultBitmap;
    CirDetectionParam* threadParam; // thread�� ����ü
    vector<Point> cornerPts; // �ܰ� 3�� ��ǥ ��
    vector<Vec3f> vCirCenters; // Vertical�� �߽� ��ǥ���� �� 
    vector<Vec3f> hCirCenters; // Horizontal�� �߽� ��ǥ���� �� 
    vector<Vec3f> CirCenters; // Horizontal�� �߽� ��ǥ���� �� 
    vector<pair<Point2d, Point2d>> LinePoints; // ��Ȳ�� Line�� ���۰� �� ��ǥ�� ��
    Point2d IntersectionPoint; // ���� ��ǥ�� ��
    vector<Point> vertexPts;   // �ֿܰ� ROI Vertex Points
    vector<Point> approx;
    vector<vector<Point>> BallContours; // �� ī��Ʈ 

    // Value
    int distance; // ROI ���� ����
    double adjDist; // ROI ���� ��������
    int radMin; // ����� �ּ� ������
    int radMax; // ����� �ִ� ������
    int BGV; // �׷���
    string fileName;
    bool isRotate; // ������Ʈ ����
 
public:

    // ������
    CircleDection();
    ~CircleDection();

    // �ʱ�ȭ
    void Initialize();

    // �̹��� ����
    bool SelectImage();
    bool isReady(); // �ҽ� �̹��� ����

    //����
    bool Run(); // �˰��� �κ� ����
    bool Rotation_Run();
    int BallCounting(vector<Point2d>& shape_pts, bool isCricleShape = false); // �� ���� ����
    // Image Data Type ��ȯ
    HBITMAP MatToBitmap(Mat& src);
    // set �Լ�
    void SetImage(Mat& src); // �̹��� Set
    void SetDistance(int distance); // ROI ���� ������ set
    void SetAdjDist(double adjDist); // ROI ���� ���� ������ set
    void SetCircleValue(int radMin, int radMax, int BGV); // ������ Value set
    void SetFilename(string filename);
    // get �Լ�
    Mat GetRotateImage(); // ȸ���� �̹���
    Mat GetOriginalImage(); // �������� �̹���
    vector<Vec3f> GetCirclePoint();
    vector<pair<Point2d, Point2d>> GetCirLinePoints();
    Point2d GetIntersectionPoint();
    string getFilename();
    

    // �˰��� ���� �κ�
private:
    // 3�� ��ǥ ���� ������
    bool GetCornerPoints();
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
    void CircleDetection(vector<vector<Point>>& contours, vector<Vec3f>& cirCenters);
    // ��� Point�� �����ϴ� rect ����
    Rect boundRect(vector<Point> pts);
    // Otsu threshold
    int OtsuThreshold(Mat& src);
    // ���� ������ - ��Ⱚ ����
    template <typename T>
    double BilinearValue(Mat& img, Point2d pt);
    // �ּ������� �� �߽� ����
    Vec3f CircleFitByTaubin(vector<Point2d> edges);
    // �簢�� ������ �� ���� ����
    bool IsContain(Rect rc, vector<Point>& cirCenters);
    // �ּ��������� ���� ������ ������ ��ǥ ����
    void getPointOfIntersection();
    // �ּ������� x, y��ǥ ����Ī �� ����� �� ���� ���� �ٽ� y=x ��Ī�̵�
    Vec2f LSM_Vertical(vector<Vec3f>& pts);
    Vec2f LSM_Vertical(vector<Point>& pts);
    // �ּ������� �Լ�
    Vec2f LSM_Horizontal(vector<Vec3f>& pts);
    Vec2f LSM_Horizontal(vector<Point>& pts);

    // �����̼� �Լ�
    template <typename T>
    void rotation(Mat src, Mat& dst, double theta, Point pt);
    // ȸ��
    bool Rotation();

    // �Ǽ��� fiilPolt �Լ�
    void fillPoly_ROI(Size matSize, Mat& dst, vector<Point2d> pts);
    // ���� ��� fill Poly
    void fillPolyCircle(Mat& src, Mat& dst, vector<Point2d> pts);
    // �Ǽ� ��ǥ rect ����
    Rect2d boundRect(vector<Point2d> pts);
    // �̺� �Լ�(prewitt Edge)
    void Differential(Mat& src, Mat& dst);

};

