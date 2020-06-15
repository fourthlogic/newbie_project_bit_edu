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
    vector<Point>* cirCenters;
    Mutex* mut;

}cirDetectionParam;

typedef struct CircleValue
{
    int *distance; // ROI 추출 범위
    int *radMin; // 검출원 최소 반지름
    int *radMax; // 검출원 최대 반지름
    int *BGV; // 원 Value
    int *thMinValue; // 외각 좌표 추출 th
    int *thMaxValue; // 외각 좌표 추출 th
    int *height; // 높이
    int *width; // 넓이
    int *size; // 전체 크기
}cirValue;

class CircleDection
{
private:
    Mat src; // 소스 이미지 
    Mat result; // 결과 이미지
    HBITMAP resultBitmap;
    CirDetectionParam* threadParam; // thread용 구조체
    CircleValue cirValue;
    vector<Point> cornerPts; // 외곽 3점 좌표 값
    vector<Point> vCirCenters; // Vertical의 중심 좌표들의 값 
    vector<Point> hCirCenters; // Horizontal의 중심 좌표들의 값 
    vector<Point> vertexPts;   // 최외곽 ROI Vertex Points

    
    // Value
    int distance; // ROI 추출 범위
    int radMin; // 검출원 최소 반지름
    int radMax; // 검출원 최대 반지름
    int BGV; // 그레이
    int thMinValue; // 외각 좌표 추출 th
    int thMaxValue; // 외각 좌표 추출 th
    int height; // 높이
    int width; // 넓이
    int size; // 전체 크기
 
public:

    // 생성자
    CircleDection();
    ~CircleDection();

    // 초기화
    void Initialize();
    void Init(); 
    //void Init(Mat& src); 

    // 이미지 선택
    //void SelectImage();
    bool SelectImage();

    //실행
    void Run(); // 알고리즘 부분 실행
    void ALLRun(); // 초기화 ~ 전체 실행

    // Image Data Type 변환
    HBITMAP MatToBitmap(Mat& src);
    Mat BitmapToMat(HBITMAP hBmp);

    // set 함수
    void SetImage(Mat& src); // 이미지 Set
    void SetThreshValue(int thMinValue, int thMaxValue); // Thresh value set
    void SetDistance(int distance); // ROI 추출 범위값 set
    void SetCircleValue(int radMin, int radMax, int BGV); // 원검출 Value set

    // get 함수
    Mat GetSourceImage();
    Mat GetResultImage();
    CircleValue* GetCircleValue();
    

    // show 함수
    bool ShowSrcImage(); // 소스 이미지 출력
    bool ShowResultImage(); // 결과 이미지 출력

    bool isReady(); // 소스 이미지 준비 여부


    // 알고리즘 실행 부분
private:
    // 3점 좌표 추출 수정판
    void GetCornerPoints();
    // 범위용 Threshold
    void ToZeroThreshold(Mat& src, Mat& dst, int MinValue, int MaxValue);
    // 외곽점 추출
    void MyContours(Mat& src, vector<vector<Point>>& Points);
    // 라벨링
    void Labeling(Mat& src, int* Map, vector<Vec3i>& startPoint);
    // 외곽선 검출
    void Contour(Mat& src, int sx, int sy, vector<Point>& outPoints, int* Map, int value);
    // 곡선을 소수의 점으로 구성한 유사한 직선으로 구분
    // 거리 추출  옵션 : true == 점과 점의 거리 ,  false == 선과 점의 거리
    double PerpendicularDistance(const Point& pt, const Point& lineStart, const Point& lineEnd, bool flag);
    // 곡선을 소수의 점으로 구성한 유사한 직선으로 구분
    void DouglasPeucker(const vector<Point>& polyLine, vector<Point>& simplifiedPolyLine, double epsilon, bool flag = true, bool endFlag = false);
    // 최외곽 ROI Vertex 추출
    void GetVertexPoints();
    // 원검출을 위한 ROI Area 추출
    void ContourDetection();
    // Points를 통해 해당 부분의 이미지 추출
    void fillPoly_(Size matSize, Mat& dst, vector<Point> pts);
    // 원 검출 Thread
    static unsigned WINAPI CirDetectionThread(void* para);
    // 원 검출
    void CircleDetection(vector<vector<Point>>& contours, vector<Point>& cirCenters);
    // 모든 Point를 포함하는 rect 추출
    Rect boundRect(vector<Point> pts);
    // 미분 함수(prewitt Edge)
    void Differential(Mat& src, Mat& dst);
    // 사각형 내부의 점 포함 여부
    bool IsContain(Rect rc, vector<Point>& cirCenters);
    // 최소제곱법을 통해 교점 표시, 직선 및 원 그리기
    void Drawing();
    // 최소제곱법 x, y좌표 스위칭 후 계산한 뒤 나온 식을 다시 y=x 대칭이동
    Vec2f LSM_Vertical(vector<Point>& pts);
    // 최소제곱법 함수
    Vec2f LSM_Horizontal(vector<Point>& pts);
};

