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
	Mat src; // 소스 이미지 
	Mat result; // 결과 이미지
    HBITMAP resultBitmap;
    vector<Point> cornerpts; // 외곽 3점 좌표 값
    vector<Point> vCirCenters; // Vertical의 중심 좌표들의 값 
    vector<Point> hCirCenters; // Horizontal의 중심 좌표들의 값 
    vector<Point> vertexPts;   // 최외곽 ROI Vertex Points
    CirDetectionParam* threadParam; // thread용 구조체

public:
    void SetImage(Mat& src);
    void SelectImage(CFileDialog& dlg);
    bool RunCircleDection();
    HBITMAP MatToBitmap();
    void GetImageBitmap();
    bool IsImage();
public:
    // 3점 좌표 추출 수정판
    void GetCornerPoints(Mat& src, vector<Point>& cornerPts);
    // 범위용 Threshold
    void RangeThreshold(Mat& src, Mat& dst, int MinValue, int MaxValue, int inValue, int outValue);
    // 외곽점 추출
    void MyContours(Mat& src, vector<vector<Point>>& Points);
    // 라벨링
    void Labeling(Mat& src, int* Map, vector<Vec3i>& startPoint);
    // 외곽선 검출
    void Contour(Mat& src, int sx, int sy, vector<Point>& outPoints, int* Map, int value);
    // 곡선을 소수의 점으로 구성한 유사한 직선으로 구분
    void DouglasPeucker(const vector<Point>& polyLine, vector<Point>& simplifiedPolyLine, double epsilon, Point lastPt, bool = true);
    // 거리 추출  옵션 : true == 점과 점의 거리 ,  false == 선과 점의 거리
    double PerpendicularDistance(const Point& pt, const Point& lineStart, const Point& lineEnd, bool flag);
    // 최외곽 ROI Vertex 추출
    void GetVertexPoints(Mat& src, vector<Point>& cornerPts, vector<Point>& vertexPts, int distance);
    // 원검출을 위한 ROI Area 추출
    void ContourDetection(Mat& src, vector<Point>& vCirCenters, vector<Point>& hCirCenters, vector<Point>& vertexPts, int radMin, int radMax, int value);
    // Points를 통해 해당 부분의 이미지 추출
    void fillPoly_(Size matSize, Mat& dst, vector<Point> pts);
    // 원 검출 Thread
    static unsigned WINAPI CirDetectionThread(void* para);
    // 원 검출
    void CircleDetection(Mat& src, vector<vector<Point>>& contours, vector<Point>& cirCenters, int radMin, int radMax);
    // 모든 Point를 포함하는 rect 추출
    Rect boundRect(vector<Point> pts);
    // 미분 함수(prewitt Edge)
    void Differential(Mat& src, Mat& dst);
    // 사각형 내부의 점 포함 여부
    bool IsContain(Rect rc, vector<Point>& cirCenters);
    // 최소제곱법을 통해 교점 표시, 직선 및 원 그리기
    void Drawing(Mat& src, Mat& dst, vector<Point>& vCirCenters, vector<Point>& hCirCenters);
    // 최소제곱법 x, y좌표 스위칭 후 계산한 뒤 나온 식을 다시 y=x 대칭이동
    Vec2f LSM_Vertical(vector<Point>& pts);
    // 최소제곱법 함수
    Vec2f LSM_Horizontal(vector<Point>& pts);
};

