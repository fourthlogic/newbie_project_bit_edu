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

    Mat Original; // 오리지널 이미지
    Mat src; // 타겟 소스 이미지 
    Mat Rotate; // 회전 이미지
    HBITMAP resultBitmap;
    CirDetectionParam* threadParam; // thread용 구조체
    vector<Point> cornerPts; // 외곽 3점 좌표 값
    vector<Vec3f> vCirCenters; // Vertical의 중심 좌표들의 값 
    vector<Vec3f> hCirCenters; // Horizontal의 중심 좌표들의 값 
    vector<Vec3f> CirCenters; // Horizontal의 중심 좌표들의 값 
    vector<pair<Point2d, Point2d>> LinePoints; // 주황색 Line의 시작과 끝 좌표의 값
    Point2d IntersectionPoint; // 교점 좌표의 값
    vector<Point> vertexPts;   // 최외곽 ROI Vertex Points
    vector<Point> approx;
    vector<vector<Point>> BallContours; // 볼 카운트 

    // Value
    int distance; // ROI 추출 범위
    double adjDist; // ROI 추출 오차범위
    int radMin; // 검출원 최소 반지름
    int radMax; // 검출원 최대 반지름
    int BGV; // 그레이
    string fileName; // 파일 주소
    bool isRotate; // 로테이트 유무
 
public:

    // 생성자
    CircleDection();
    ~CircleDection();

    // 초기화
    void Initialize();

    // 이미지 선택
    bool SelectImage();
    bool isReady(); // 소스 이미지 상태

    //실행
    bool Run(); // 알고리즘 부분 실행
    bool Rotation_Run();
    int BallCounting(vector<Point2d>& shape_pts, bool isCricleShape = false); // 볼 개수 실행

    // Image Data Type 변환
    HBITMAP MatToBitmap(Mat& src);
    // set 함수
    void SetImage(Mat& src); // 이미지 Set
    void SetDistance(int distance); // ROI 추출 범위값 set
    void SetAdjDist(double adjDist); // ROI 추출 오차 범위값 set
    void SetCircleValue(int radMin, int radMax, int BGV); // 원검출 Value set
    void SetFilename(string filename);

    // get 함수
    Mat GetRotateImage(); // 회전된 이미지
    Mat GetOriginalImage(); // 오리지널 이미지
    vector<Vec3f> GetCirclePoint(); // 원 정보 가져오기
    vector<pair<Point2d, Point2d>> GetCirLinePoints(); // 라인 정보 가져오기
    Point2d GetIntersectionPoint(); // 기준점 정보 가져오기
    string getFilename(); // 파일 주소명 가져오기
    

    // 알고리즘 실행 부분
private:
    // 3점 좌표 추출 수정판
    bool GetCornerPoints();
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
    void CircleDetection(vector<vector<Point>>& contours, vector<Vec3f>& cirCenters);
    // 모든 Point를 포함하는 rect 추출
    Rect boundRect(vector<Point> pts);
    // Otsu threshold
    int OtsuThreshold(Mat& src);
    // 선형 보간법 - 밝기값 추출
    template <typename T>
    double BilinearValue(Mat& img, Point2d pt);

    Vec3f OutlierCircle(vector<Point2d>& _pts, vector<double>& edge_angles);
    // 최소제곱법 원 중심 추출
    Vec3f CircleFitByTaubin(vector<Point2d> edges);
    // 사각형 내부의 점 포함 여부
    bool IsContain(Rect rc, vector<Point>& cirCenters);
    // 최소제곱법을 통해 교점과 직선의 좌표 추출
    void getPointOfIntersection();
    // 아웃라이어 제외하며 직선 정확도 증가
    void getRemovedOutlierEquation(vector<Vec3f> cirCenters, Vec2d eq_in, double adjAvg);
    // 특정 거리 이상 떨어진 원 제외
    void RemoveOutlier(vector<Vec3f>& cirCenters, Vec2d eq, double adjDist);
    // 최소제곱법 x, y좌표 스위칭 후 계산한 뒤 나온 식을 다시 y=x 대칭이동
    Vec2f LSM_Vertical(vector<Vec3f>& pts);
    Vec2f LSM_Vertical(vector<Point>& pts);
    // 최소제곱법 함수
    Vec2f LSM_Horizontal(vector<Vec3f>& pts);
    Vec2f LSM_Horizontal(vector<Point>& pts);


    // =========================================================
    // 회전 기능 함수
    bool Rotation(); // 회전 실행
    template <typename T>
    void rotation(Mat src, Mat& dst, double theta, Point pt); // 데이터 실제 회전


    // =========================================================
    // BallCount 함수

    // 실수형 fiilPolt 함수
    void fillPoly_ROI(Size matSize, Mat& dst, vector<Point2d> pts);
    // 원형 모양 fill Poly
    void fillPolyCircle(Mat& src, Mat& dst, vector<Point2d> pts);
    // 실수 좌표 rect 추출
    Rect2d boundRect(vector<Point2d> pts);
    // 미분 함수(prewitt Edge)
    void Differential(Mat& src, Mat& dst);

};

