#pragma once
#include "pch.h"
#include "CircleDetection.h"


HBITMAP CircleDection::MatToBitmap(Mat & src)
{

    Mat Image;
    cvtColor(src, Image, COLOR_GRAY2BGR);
    HBITMAP hbmp = NULL;
    int bpp = Image.channels() * 8;
    Mat tmp = Image.clone();


    BITMAPINFO bmpInfo = { 0 };
    LONG lBmpSize = Image.rows * Image.cols * 3;
    bmpInfo.bmiHeader.biBitCount = bpp;
    bmpInfo.bmiHeader.biHeight = Image.rows;
    bmpInfo.bmiHeader.biWidth = Image.cols;
    bmpInfo.bmiHeader.biPlanes = 1;
    bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    // Pointer to access the pixels of bitmap
    BYTE* pPixels = 0, * pP;
    hbmp = CreateDIBSection(NULL, (BITMAPINFO*)&bmpInfo, DIB_RGB_COLORS, (void**)&pPixels, NULL, 0);

    if (!hbmp)
        return hbmp; // return if invalid bitmaps

       //SetBitmapBits( hBitmap, lBmpSize, pBits);
       // Directly Write
    int left_width = ((bpp * Image.cols + 31) / 32) * 4;
    pP = pPixels;
    for (int y = Image.rows - 1, row = 0; row < Image.rows; row++, y--) {
        for (int x = 0, col = 0; col < Image.cols; x += 3, col++) {
            pP[x] = tmp.at<Vec3b>(y, col).val[0];
            pP[x + 1] = tmp.at<Vec3b>(y, col).val[1];
            pP[x + 2] = tmp.at<Vec3b>(y, col).val[2];
        }
        pP += left_width;
    }
    return hbmp;
}

CircleDection::CircleDection()
{
    this->distance = 0; // ROI 추출 범위
    this->radMin = 0; // 검출원 최소 반지름
    this->radMax = 0; // 검출원 최대 반지름
    this->BGV = 0; // 그레이
    isRotate= false;
    // 주소 연결

    threadParam = new CirDetectionParam();
    threadParam->This = this;
    threadParam->cirCenters = &vCirCenters;

}

CircleDection::~CircleDection()
{
    delete threadParam;
}

// 연결 
void CircleDection::Initialize()
{
    // 원본 이미지 값 set
    if (!this->src.data)
        return;
    else
    {
        this->isRotate = false;
    }
}

bool CircleDection::SelectImage()
{
    char szFilter[] = "Image(*.BMP, *.GOF, *.JPG, *.PNG)|*.BMP; *.GIF; *.JPG; *.bmp; *.gif; *.jpg; *.png; | All Files(*.*) | *.* ||";
    CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, CA2CT(szFilter), AfxGetMainWnd());
    if (dlg.DoModal() == IDOK)
    {
        CString cstrImgPath = dlg.GetPathName();
        CT2CA pszConvertedAnsiString(cstrImgPath);
        std::string strStd(pszConvertedAnsiString);
        this->Original = imread(strStd,IMREAD_GRAYSCALE);
        this->src = this->Original.clone();
        this->Rotate = this->Original.clone();
        this->SetImage(this->src);
        this->SetFilename(strStd);
        this->Initialize();
        isRotate = false;
        return TRUE;
    }
    else
        return FALSE;
}

bool CircleDection::isReady()
{
    if (!this->Original.empty()) return TRUE;
    else return FALSE;
}

// 전체 실행
bool CircleDection::Run()
{
    this->src = this->Original.clone();
    SetImage(src);
    approx.clear();
    cornerPts.clear(); // 외곽 3점 좌표 값
    vCirCenters.clear(); // Vertical의 중심 좌표들의 값 
    hCirCenters.clear(); // Horizontal의 중심 좌표들의 값 
    vertexPts.clear();   // 최외곽 ROI Vertex Points
    CirCenters.clear(); // 중심값  초기화
    LinePoints.clear(); // 직선점 초기화
    if (GetCornerPoints()) // 3점 좌표 추출
    {
        GetVertexPoints(); // 최외곽 ROI
        ContourDetection(); // 원 검출
        getPointOfIntersection();
        return true;
    }
    else
        return false;
}

// 회전 이미지 알고리즘 실행
bool CircleDection::Rotation_Run()
{
    if (isRotate == false) {
        if (Rotation() == false)
            return false;
    }
    this->src = this->Rotate.clone();
    SetImage(src);
    // 최기화
    approx.clear();
    cornerPts.clear(); // 외곽 3점 좌표 값
    vCirCenters.clear(); // Vertical의 중심 좌표들의 값 
    hCirCenters.clear(); // Horizontal의 중심 좌표들의 값 
    vertexPts.clear();   // 최외곽 ROI Vertex Points
    CirCenters.clear(); // 중심값  초기화
    LinePoints.clear(); // 직선점 초기화

    if (GetCornerPoints()) // 3점 좌표 추출
    {
        GetVertexPoints(); // 최외곽 ROI
        ContourDetection(); // 원 검출
        getPointOfIntersection();
        return true;
    }
    return false;
}

int CircleDection::BallCounting(vector<Point2d>& shape_pts)
{
    BallContours.clear();
    vector<Vec3f> CirCenters;
    vector<Point> pts;
    for (int i = 0; i < shape_pts.size(); i++)
        pts.push_back(shape_pts[i]);
    Rect rect = boundRect(pts);
    Mat ROI = this->src(rect);
    Mat imgThreshold = ROI.clone();
    ToZeroThreshold(ROI, imgThreshold, 0, this->BGV);
    //imshow("ROI", ROI);
    imshow("ROI", ROI);
    imshow("imgThreshold", imgThreshold);
    Mat circleImage(ROI.size(), CV_8UC1, Scalar(0));
    //MyContours(imgThreshold, BallContours);
    findContours(imgThreshold, BallContours, RETR_EXTERNAL, CHAIN_APPROX_NONE);
    for (auto pts : BallContours)
    {
        for (auto pt : pts)
        {
            circle(circleImage, pt, 0, Scalar(255));
        }
    }
    imshow("circleImage", circleImage);
    CircleDetection(ROI,BallContours, CirCenters);
    waitKey();
    return CirCenters.size();
}
// 이미지 set
void CircleDection::SetImage(Mat& src)
{
    this->src = src;
}

// ROI 추출 범위 set
void CircleDection::SetDistance(int distance = 20)
{
    this->distance = distance;
}

// ROI 추출 오차범위 set
void CircleDection::SetAdjDist(double adjDist = 2.0)
{
    this->adjDist = adjDist;
}

// 원검출 set
void CircleDection::SetCircleValue(int radMin = 2, int radMax = 4, int BGV = 90)
{
    this->radMin = radMin;
    this->radMax = radMax;
    this->BGV = BGV;
}

// 원본 이미지 가져오기
Mat CircleDection::GetOriginalImage()
{
    return this->Original;
}

// 회전 이미지 가져오기
Mat CircleDection::GetRotateImage()
{
    return this->Rotate;
}

// 서클 중심값 가져오기
vector<Vec3f> CircleDection::GetCirclePoint()
{
    CirCenters.clear();

    std::copy(std::begin(vCirCenters), std::end(vCirCenters), std::back_inserter(CirCenters));
    std::copy(std::begin(hCirCenters), std::end(hCirCenters), std::back_inserter(CirCenters));
    return this->CirCenters;
}

// 서클 중심값의 직선 점 가져오기
vector<pair<Point2d, Point2d>> CircleDection::GetCirLinePoints()
{
    return this->LinePoints;
}

Point2d CircleDection::GetIntersectionPoint()
{
    return this->IntersectionPoint;
}

// 3점 좌표 추출 수정판
bool CircleDection::GetCornerPoints()
{
    Mat grayImage = this->src.clone();
    Matx <uchar, 3, 3> mask(0, 1, 0, 1, 1, 1, 0, 1, 0);
    threshold(grayImage, grayImage, 27, 255, THRESH_BINARY); // 쓰레숄딩 값 설정

    vector<vector<Point>> contours;
    MyContours(grayImage, contours);
    vector<Point> temp1, temp2;
    vector<Point> vApprox;
    vector<Point> hApprox;
    int flg = 0;
    double distance0to1;
    double distance0to3;
    for (size_t i = 0; i < contours.size(); i++)
    {
        if (contourArea(Mat(contours[i])) > 5000)
        {
            DouglasPeucker(contours[i], approx, 0);

            distance0to1 = sqrt(pow(approx[0].x - approx[1].x, 2) + pow(approx[0].y - approx[1].y, 2));
            distance0to3 = sqrt(pow(approx[0].x - approx[3].x, 2) + pow(approx[0].y - approx[3].y, 2));

            if (distance0to1 < distance0to3)
            {
                for (int j = 0; j < contours[i].size(); j++)
                {
                    if (contours[i][j] == approx[2])
                        flg = 2;
                    else if (contours[i][j] == approx[3])
                        flg = 1;
                    else if (contours[i][j] == approx[0])
                        flg = 3;

                    if (flg == 1)
                        temp1.push_back(contours[i][j]);
                    else if (flg == 2)
                        temp2.push_back(contours[i][j]);
                }
            }
            else
            {
                for (int j = 0; j < contours[i].size(); j++)
                {
                    if (contours[i][j] == approx[1])
                        flg = 2;
                    else if (contours[i][j] == approx[2])
                        flg = 1;
                    else if (contours[i][j] == approx[3])
                        flg = 3;

                    if (flg == 1)
                        temp1.push_back(contours[i][j]);
                    else if (flg == 2)
                        temp2.push_back(contours[i][j]);
                }
            }
            break;
        }
    }

    if (temp1.size() == 0 || temp2.size() == 0)
        return false;

    Vec2d veq, heq;
    veq = LSM_Vertical(temp1);
    heq = LSM_Horizontal(temp2);

    double adjDist = 1.5; // 직선과의 거리 조정
    for (int i = 0; i < temp1.size(); i++)
    {
        Point2d center(temp1[i].x, temp1[i].y);
        double dist = abs(veq[0] * center.x - center.y + veq[1]) / sqrt(pow(veq[0], 2) + 1);
        if (dist < adjDist)
        {
            vApprox.push_back(temp1[i]);
        }
    }

    for (int i = 0; i < temp2.size() / 2; i++)
    {
        Point2d center(temp2[i].x, temp2[i].y);
        double dist = abs(heq[0] * center.x - center.y + heq[1]) / sqrt(pow(heq[0], 2) + 1);
        if (dist < adjDist)
        {
            hApprox.push_back(temp2[i]);
        }
    }
    veq = LSM_Vertical(vApprox);
    heq = LSM_Horizontal(hApprox);

    Point2d target;
    target.x = (heq[1] - veq[1]) / (veq[0] - heq[0]);
    target.y = (veq[0] * (heq[1] - veq[1])) / (veq[0] - heq[0]) + veq[1];
    Rect rc = { 0, 0, this->src.cols, this->src.rows };
    if (!target.inside(rc))
        return false;

    if (distance0to1 < distance0to3)
    {
        this->cornerPts.push_back(approx[0]);
        this->cornerPts.push_back(target);
        this->cornerPts.push_back(approx[2]);
    }
    else
    {
        this->cornerPts.push_back(approx[3]);
        this->cornerPts.push_back(target);
        this->cornerPts.push_back(approx[1]);
    }
    return true;
}

// 범위용 Threshold
void CircleDection::ToZeroThreshold(Mat& src, Mat& dst, int MinValue, int MaxValue)
{
    if (!dst.data)
        dst = Mat(src.size(), CV_8UC1, Scalar(0));

    for (int i = 0; i < src.rows; i++)
    {
        uchar* srcPtr = src.ptr<uchar>(i);
        uchar* dstptr = dst.ptr<uchar>(i);
        for (int j = 0; j < src.cols; j++)
        {
            if (srcPtr[j] > MaxValue || srcPtr[j] < MinValue)
                dstptr[j] = 0;
            else
                dstptr[j] = srcPtr[j];
        }
    }
}

// 외곽점 추출
void CircleDection::MyContours(Mat& src, vector<vector<Point>>& Points)
{
    vector<Vec3i> startPoint;
    int height = src.rows;
    int width = src.cols;
    int* Map = new int[height * width];
    memset(Map, 0, sizeof(int) * height * width);
    Labeling(src, Map, startPoint);
    startPoint.erase(startPoint.begin());
    for (int i = 0; i < startPoint.size(); i++)
    {
        vector<Point> pts;
        Contour(src, startPoint[i][0], startPoint[i][1], pts, Map, startPoint[i][2]);
        if (pts.size() != 0)
            Points.push_back(pts);
    }
    delete Map;
}

// 라벨링
void CircleDection::Labeling(Mat& src, int* Map, vector<Vec3i>& startPoint)
{
    map<int, int> table;
    int height = src.rows;
    int width = src.cols;
    uchar* ptr2 = src.data;
    uchar* ptr;
    int mult_y, sub_y;
    int Up, Left, Data;
    table[1] = 1;
    int Count = 1;

    for (int y = 0; y < height; y++)
    {
        ptr = src.ptr<uchar>(y);
        mult_y = y * width;
        sub_y = y - 1;
        for (int x = 0; x < width; x++)
        {
            if (ptr[x] != 0)
            {
                if (x - 1 < 0)
                    Left = 0;
                else
                    Left = Map[mult_y + x - 1];

                if (sub_y < 0)
                    Up = 0;
                else
                    Up = Map[sub_y * width + x];

                if (Left == 0 && Up == 0)
                {
                    Map[mult_y + x] = Count;
                    table[Count] = Count++;
                }
                else if (Left != 0 && Up != 0)
                {
                    if (Up > Left)
                    {
                        Map[mult_y + x] = Left;
                        table[Up] = table[Left];
                    }
                    else
                    {
                        Map[mult_y + x] = Up;
                        table[Left] = table[Up];
                    }
                }
                else
                    Map[mult_y + x] = Left + Up;
            }
        }
    }
    map<int, int> cnt;
    startPoint.resize(1);
    int i = 0;
    Count = 0;
    for (int y = 0; y < height; y++)
    {
        mult_y = y * width;
        for (int x = 0; x < width; x++)
        {
            Data = Map[mult_y + x];
            if (Data == 0)
                continue;
            if (Data != table[Data])
            {
                Map[mult_y + x] = table[Data];
            }
            else
            {
                if (cnt[Data] == 0)
                {
                    startPoint.push_back(Vec3i(x, y, Data));
                    cnt[Data]++;
                }
            }
        }
    }
}

// 외곽선 검출
void CircleDection::Contour(Mat& src, int sx, int sy, vector<Point>& outPoints, int* Map, int value)
{
    int x, y, nx, ny;
    int d, cnt;

    int  dir[8][2] = {
    {  1,  0 },
    {  1,  1 },
    {  0,  1 },
    { -1,  1 },
    { -1,  0 },
    { -1, -1 },
    {  0, -1 },
    {  1, -1 }
    };
    outPoints.clear();
    x = sx;
    y = sy;
    d = cnt = 0;


    while (1)
    {
        nx = x + dir[d][0];
        ny = y + dir[d][1];

        if (nx < 0 || nx >= src.cols || ny < 0 || ny >= src.rows || Map[ny * src.rows + nx] != value)
        {
            if (++d > 7) d = 0;
            cnt++;

            if (cnt >= 8)
            {
                outPoints.push_back(Point(x, y));
                break;
            }
        }
        else
        {
            outPoints.push_back(Point(x, y));
            x = nx;
            y = ny;

            cnt = 0;
            d = (d + 6) % 8;
        }

        if (x == sx && y == sy && d == 0)
            break;
    }
}

// 곡선을 소수의 점으로 구성한 유사한 직선으로 구분
void CircleDection::DouglasPeucker(const vector<Point>& polyLine, vector<Point>& simplifiedPolyLine, double epsilon, bool flag, bool endFlag)
{
    if (polyLine.size() < 2)
    {
        return;
    }

    double maxDistance = 0.0;
    int index = 0;
    int end = polyLine.size() - 1;
    int first = 0;
    if (flag) {
        for (int i = 0; i < end; i++)
        {
            double d = PerpendicularDistance(polyLine[i], polyLine[0], polyLine[end], flag);
            if (d > maxDistance)
            {
                index = i;
                maxDistance = d;
            }
        }
        int firstMax = 0;
        for (int i = 0; i < end; i++)
        {
            double d = PerpendicularDistance(polyLine[i], polyLine[index], polyLine[end], flag);
            if (d > firstMax)
            {
                first = i;
                firstMax = d;
            }
        }
        epsilon = PerpendicularDistance(polyLine[first], polyLine[index], polyLine[end], true) / 2.0;
        epsilon -= epsilon / 3;
    }
    else {
        for (int i = 1; i < end; i++)
        {
            double d = PerpendicularDistance(polyLine[i], polyLine[0], polyLine[end], false);
            if (d > maxDistance)
            {
                index = i;
                maxDistance = d;
            }
        }
    }

    if (maxDistance > epsilon)
    {
        vector<Point> recResults1;
        vector<Point> recResults2;
        vector<Point> firstLine;
        Mat temp;
        if (first < index + 1) {
            firstLine.assign(polyLine.begin() + first, polyLine.begin() + index + 1);

        }
        else {
            firstLine.push_back(polyLine[first]);
            firstLine.insert(firstLine.end(), polyLine.begin(), polyLine.begin() + index + 1);
        }

        vector<Point> lastLine(polyLine.begin() + index, polyLine.end());
        DouglasPeucker(firstLine, recResults1, epsilon, false, true);
        DouglasPeucker(lastLine, recResults2, epsilon, false, endFlag);

        simplifiedPolyLine.assign(recResults1.begin(), recResults1.end() - 1);
        simplifiedPolyLine.insert(simplifiedPolyLine.end(), recResults2.begin(), recResults2.end());

    }
    else
    {
        simplifiedPolyLine.push_back(polyLine[first]);
        if (endFlag)
            simplifiedPolyLine.push_back(polyLine[end]);
    }
}

// 거리 추출  옵션 : true == 점과 점의 거리 ,  false == 선과 점의 거리
double CircleDection::PerpendicularDistance(const Point& pt, const Point& lineStart, const Point& lineEnd, bool flag)
{
    if (flag) {
        double dx = (lineEnd.x + lineStart.x) / 2.0;
        double dy = (lineEnd.y + lineStart.y) / 2.0;

        return sqrt(pow(pt.x - lineStart.x, 2) + pow(pt.y - lineStart.y, 2));
    }
    double dx = lineEnd.x - lineStart.x;
    double dy = lineEnd.y - lineStart.y;

    double mag = sqrt(pow(dx, 2) + pow(dy, 2));
    if (mag > 0.0)
    {
        dx /= mag;
        dy /= mag;
    }

    double pvx = pt.x - lineStart.x;
    double pvy = pt.y - lineStart.y;

    double pvdot = dx * pvx + dy * pvy;

    double dsx = pvdot * dx;
    double dsy = pvdot * dy;

    double ax = pvx - dsx;
    double ay = pvy - dsy;

    return sqrt(pow(ax, 2.0) + pow(ay, 2.0));
}

// 최외곽 ROI Vertex 추출
void CircleDection::GetVertexPoints()
{
    int rotateX, rotateY;
    double theta;
    int errorV = 8;
    int errorH = 8;
    int heightV = sqrt(pow(this->cornerPts[0].y - this->cornerPts[1].y, 2) + pow(this->cornerPts[0].x - this->cornerPts[1].x, 2));
    int widthV = this->distance + errorV;

    int heightH = this->distance + errorH;
    int widthH = sqrt(pow(this->cornerPts[1].y - this->cornerPts[2].y, 2) + pow(this->cornerPts[1].x - this->cornerPts[2].x, 2));

    this->vertexPts.resize(6);

    if (this->cornerPts[1].x == this->cornerPts[0].x) {
        this->vertexPts[0] = this->cornerPts[0];
        this->vertexPts[0].x += errorV;
        this->vertexPts[5] = Point(this->cornerPts[0].x + widthV, this->cornerPts[0].y);
        this->vertexPts[4] = Point(this->cornerPts[1].x + widthV, this->cornerPts[1].y);

        this->vertexPts[1] = this->cornerPts[1];
        this->vertexPts[1].x += errorV;
    }
    else {
        theta = atan((double)((this->cornerPts[0].x - this->cornerPts[1].x)) / (this->cornerPts[0].y - this->cornerPts[1].y));

        rotateX = (widthV)*cos(theta) + this->cornerPts[0].x;
        rotateY = -(widthV)*sin(theta) + this->cornerPts[0].y;
        this->vertexPts[5] = Point(rotateX, rotateY);


        rotateX = widthV * cos(theta) + heightV * sin(theta) + this->cornerPts[0].x;
        rotateY = -widthV * sin(theta) + heightV * cos(theta) + this->cornerPts[0].y;
        this->vertexPts[4] = Point(rotateX, rotateY);


        rotateX = (errorV)*cos(theta) + this->cornerPts[0].x;
        rotateY = -(errorV)*sin(theta) + this->cornerPts[0].y;
        this->vertexPts[0] = Point(rotateX, rotateY);


        rotateX = errorV * cos(theta) + heightV * sin(theta) + this->cornerPts[0].x;
        rotateY = -errorV * sin(theta) + heightV * cos(theta) + this->cornerPts[0].y;
        this->vertexPts[1] = Point(rotateX, rotateY);
    }


    if (this->cornerPts[2].y == this->cornerPts[1].y) {
        this->vertexPts[2] = this->cornerPts[2];
        this->vertexPts[2].y -= errorH;

        this->vertexPts[3] = Point(this->cornerPts[2].x, this->cornerPts[2].y - heightH);

        this->vertexPts[4].x = this->vertexPts[4].x;
        this->vertexPts[4].y = this->vertexPts[4].y - heightH;

        this->vertexPts[1].y -= errorH;
    }
    else
    {
        theta = -atan((this->cornerPts[1].y - this->cornerPts[2].y) / (double)((this->cornerPts[1].x - this->cornerPts[2].x)));

        this->vertexPts[4].x = -heightH * sin(theta) + this->vertexPts[4].x;
        this->vertexPts[4].y = -heightH * cos(theta) + this->vertexPts[4].y;


        rotateX = widthH * cos(theta) - heightH * sin(theta) + this->cornerPts[1].x;
        rotateY = -widthH * sin(theta) - heightH * cos(theta) + this->cornerPts[1].y;
        this->vertexPts[3] = Point(rotateX, rotateY);

        rotateX = widthH * cos(theta) - errorH * sin(theta) + this->cornerPts[1].x;
        rotateY = -widthH * sin(theta) - errorH * cos(theta) + this->cornerPts[1].y;
        this->vertexPts[2] = Point(rotateX, rotateY);

        this->vertexPts[1].x = -errorH * sin(theta) + this->vertexPts[1].x;
        this->vertexPts[1].y = -errorH * cos(theta) + this->vertexPts[1].y;
    }
}

// 원검출을 위한 ROI Area 추출
void CircleDection::ContourDetection() {
    Mat ROI;
    Mat mask;
    // range 범위 영역 추출
    fillPoly_(this->src.size(), mask, this->vertexPts);
    bitwise_and(this->src, mask, ROI);
    Mat imgThreshold = ROI.clone();

    ToZeroThreshold(imgThreshold, imgThreshold, 0, this->BGV);
    vector<vector<Point>> contours;
    MyContours(imgThreshold, contours);

    //Vertical과 Horizontal 구분
    double a = (vertexPts[1].y - vertexPts[4].y) * 1.0 / (vertexPts[1].x - vertexPts[4].x) * 1.0;
    double b = vertexPts[1].y - a * vertexPts[1].x;

    vector<vector<Point>> vContours, hContours;
    for (vector<Point> pts : contours) {
        if (a > 0) {
            if (a * pts[0].x - pts[0].y + b < 0) {
                vContours.push_back(pts);
            }
            else {
                hContours.push_back(pts);
            }
        }
        else {
            if (a * pts[0].x - pts[0].y + b > 0) {
                vContours.push_back(pts);
            }
            else {
                hContours.push_back(pts);
            }
        }
    }
    unsigned threadId;
    threadParam->cirCenters = &this->vCirCenters;
    threadParam->contours = &vContours;
    CircleDetection(hContours, this->hCirCenters);
    WaitForSingleObject((HANDLE)_beginthreadex(NULL, 0, CirDetectionThread, (void*)threadParam, 0, &threadId) , INFINITE);
}

// Points를 통해 해당 부분의 이미지 추출
void CircleDection::fillPoly_(Size matSize, Mat& dst, vector<Point> pts) {
    Rect rc = boundRect(pts);
    dst = Mat::zeros(matSize, CV_8UC1);

    int size = pts.size();
    for (int i = 0; i < size - 1; i++) {
        line(dst, pts[i], pts[i + 1], Scalar(255, 255, 255));
    }
    line(dst, pts[0], pts[size - 1], Scalar(255, 255, 255));

    Point pt = rc.tl();
    for (int i = pt.y; i < rc.height + pt.y; i++) {
        bool isVertex = false;
        vector<int> idx;
        uchar* ptr = dst.ptr<uchar>(i);

        for (int j = pt.x; j < pt.x + rc.width; j++) {
            if (ptr[j - 1] == 0 && ptr[j] == 255) {
                idx.push_back(j);
            }
        }
        if (idx.size() % 2 != 0) {
            for (int k = 0; k < idx.size() - 1; k++) {
                for (Point pt : pts) {
                    if (i == pt.y && idx[k] == pt.x) {
                        idx.erase(idx.begin() + k);
                        break;
                    }
                }
            }
        }
        if (idx.size() != 0) {
            for (int j = 0; j < idx.size() - 1; j += 2) {
                for (int k = idx[j]; k < idx[j + 1]; k++)
                    ptr[k] = 255;
            }
        }

    }
}

// 원 검출 Thread
unsigned WINAPI CircleDection::CirDetectionThread(void* para) {
    CirDetectionParam* Param = (CirDetectionParam*)para;
    Param->This->CircleDetection(*Param->contours, *Param->cirCenters);
    return 0;
}

// 원 검출
void CircleDection::CircleDetection(vector<vector<Point>>& contours, vector<Vec3f>& cirCenters) {

    for (vector<Point> pts : contours) {
        double areaValue = contourArea(pts); // 면적값 계산
        if (areaValue < 50 && areaValue > 0) {
            Rect rc = boundRect(pts); // 면적 사이즈를 지정할수 있게 설정하면 좋을듯 

            if (rc.tl().x - 3 > 0 && rc.tl().y - 3 > 0) {
                rc -= Point(3, 3);
            }
            if (rc.tl().x + rc.width + 4 < this->src.cols && rc.tl().y + rc.height + 4 < this->src.rows) {
                rc += Size(4, 4);
            }
            else {
                rc.width = this->src.cols - rc.tl().x;
                rc.height += 4;
            }

            Mat cirRect = this->src(rc);
            for (int i = 0; i < cirRect.rows; i++)
            {
                for (int j = 0; j < cirRect.cols; j++)
                {
                    if (cirRect.at<uchar>(i, j) >= 190)
                        cirRect.at<uchar>(i, j) = 140;
                }
            }

            Mat dstX, dstY, norm, angles, re;
            Sobel(cirRect, dstX, CV_32F, 1, 0, 3);
            Sobel(cirRect, dstY, CV_32F, 0, 1, 3);
            magnitude(dstX, dstY, norm); // 1차미분
            phase(dstX, dstY, angles, false);


            Point2d a, b, c;
            double cc, aa, bb;
            int thresh = OtsuThreshold(cirRect);

            vector<Point2d> _pts;
            vector<double> edge_angles;
            for (int y = 1; y < cirRect.rows - 1; y++)
            {
                for (int x = 1; x < cirRect.cols - 1; x++)
                {
                    b = Point2d(x, y);
                    bb = cirRect.at<uchar>(y, x);
                    if (norm.at<float>(y, x) >= thresh)
                    {
                        double theta = angles.at<float>(y, x);
                        c = { b.x + cos(theta), b.y + sin(theta) };
                        a = { b.x - cos(theta), b.y - sin(theta) };

                        cc = BilinearValue<uchar>(cirRect, c);
                        aa = BilinearValue<uchar>(cirRect, a);
                        if (aa < cirRect.at<uchar>(y, x) && cc > cirRect.at<uchar>(y, x))
                        {
                            double lambda = abs(cc - bb) / (abs(bb - aa) + abs(bb - cc));
                            b = { b.x + lambda * cos(theta) + 0.5, b.y + lambda * sin(theta) + 0.5 };
                            _pts.push_back(b);
                            edge_angles.push_back(theta * 180 / CV_PI);
                        }
                    }
                }
            }

            Vec3f Center;
            vector<double> dataX;

            Center = CircleFitByTaubin(_pts);
            int angle_count = edge_angles.size();
            for (int i = 0; i < edge_angles.size(); i++) {

                double dy = Center[1] - _pts[i].y;
                double dx = Center[0] - _pts[i].x;
                double angle = atan(dy / dx) * (180.0 / CV_PI);
                angle += 180.f;
                if (dx < 0.0) {
                    angle += 180.0;
                }
                
                if (angle > 360.f)
                    angle -= 360.f;
                if (edge_angles[i] - 20 < angle && angle < edge_angles[i] + 20)
                    angle_count--;
            }
            if (angle_count > _pts.size() / 4)
                continue;
            int vote = 0;


            if (this->radMax >= Center[2] && this->radMin <= Center[2]) {
                cirCenters.push_back(Vec3f(Center[0] + rc.x, Center[1] + rc.y, Center[2]));
                for (int t = 0; t <= 360; t += 5)
                {
                    double a = Center[0] - Center[2] * (cos(t * CV_PI / 180));
                    double b = Center[1] - Center[2] * (sin(t * CV_PI / 180));
                    if (a >= 0 && a < cirRect.cols && b >= 0 && b < cirRect.rows) {

                        if (BilinearValue<float>(norm, Point2d(a, b)) >= 100)
                            vote += 1;
                    }
                }
                if (vote >= 60) // 허프 깊이 
                    cirCenters.push_back(Vec3f(Center[0] + rc.x, Center[1] + rc.y, Center[2]));
            }

        }
    }
}

// ROI 영역 원 검출
void CircleDection::CircleDetection(Mat& ROI,vector<vector<Point>>& contours, vector<Vec3f>& cirCenters) {

    for (vector<Point> pts : contours) {
        double areaValue = contourArea(pts); // 면적값 계산
        if (areaValue < 50 && areaValue > 0) {
            Rect rc = boundRect(pts); // 면적 사이즈를 지정할수 있게 설정하면 좋을듯 

            if (rc.tl().x - 3 > 0 && rc.tl().y - 3 > 0) {
                rc -= Point(3, 3);
            }
            if (rc.tl().x + rc.width + 4 < ROI.cols ) {
                rc += Size(4, 0);
            }
            else {
                rc.width = ROI.cols - rc.tl().x;
            }
            if (rc.tl().y + rc.height + 4 < ROI.rows)
            {
                rc += Size(0, 4);
            }
            else {
                rc.height = ROI.rows - rc.tl().y;
            }

            Mat cirRect = ROI(rc);
            for (int i = 0; i < cirRect.rows; i++)
            {
                for (int j = 0; j < cirRect.cols; j++)
                {
                    if (cirRect.at<uchar>(i, j) >= 190)
                        cirRect.at<uchar>(i, j) = 140;
                }
            }

            Mat dstX, dstY, norm, angles, re;
            Sobel(cirRect, dstX, CV_32F, 1, 0, 3);
            Sobel(cirRect, dstY, CV_32F, 0, 1, 3);
            magnitude(dstX, dstY, norm); // 1차미분
            phase(dstX, dstY, angles, false);


            Point2d a, b, c;
            double cc, aa, bb;
            int thresh = OtsuThreshold(cirRect);

            vector<Point2d> _pts;
            vector<double> edge_angles;
            for (int y = 1; y < cirRect.rows - 1; y++)
            {
                for (int x = 1; x < cirRect.cols - 1; x++)
                {
                    b = Point2d(x, y);
                    bb = cirRect.at<uchar>(y, x);
                    if (norm.at<float>(y, x) >= thresh)
                    {
                        double theta = angles.at<float>(y, x);
                        c = { b.x + cos(theta), b.y + sin(theta) };
                        a = { b.x - cos(theta), b.y - sin(theta) };

                        cc = BilinearValue<uchar>(cirRect, c);
                        aa = BilinearValue<uchar>(cirRect, a);
                        if (aa < cirRect.at<uchar>(y, x) && cc > cirRect.at<uchar>(y, x))
                        {
                            double lambda = abs(cc - bb) / (abs(bb - aa) + abs(bb - cc));
                            b = { b.x + lambda * cos(theta) + 0.5, b.y + lambda * sin(theta) + 0.5 };
                            _pts.push_back(b);
                            edge_angles.push_back(theta * 180 / CV_PI);
                        }
                    }
                }
            }

            Vec3f Center;
            vector<double> dataX;

            Center = CircleFitByTaubin(_pts);
            int angle_count = edge_angles.size();
            for (int i = 0; i < edge_angles.size(); i++) {

                double dy = Center[1] - _pts[i].y;
                double dx = Center[0] - _pts[i].x;
                double angle = atan(dy / dx) * (180.0 / CV_PI);
                angle += 180.f;
                if (dx < 0.0) {
                    angle += 180.0;
                }

                if (angle > 360.f)
                    angle -= 360.f;
                if (edge_angles[i] - 20 < angle && angle < edge_angles[i] + 20)
                    angle_count--;
            }
            if (angle_count > _pts.size() / 4)
                continue;
            int vote = 0;


            if (this->radMax >= Center[2] && this->radMin <= Center[2]) {
                cirCenters.push_back(Vec3f(Center[0] + rc.x, Center[1] + rc.y, Center[2]));
                for (int t = 0; t <= 360; t += 5)
                {
                    double a = Center[0] - Center[2] * (cos(t * CV_PI / 180));
                    double b = Center[1] - Center[2] * (sin(t * CV_PI / 180));
                    if (a >= 0 && a < cirRect.cols && b >= 0 && b < cirRect.rows) {

                        if (BilinearValue<float>(norm, Point2d(a, b)) >= 100)
                            vote += 1;
                    }
                }
                if (vote >= 60) // 허프 깊이 
                    cirCenters.push_back(Vec3f(Center[0] + rc.x, Center[1] + rc.y, Center[2]));
            }

        }
    }
    cout << CirCenters.size();
}

// 모든 Point를 포함하는 rect 추출
Rect CircleDection::boundRect(vector<Point> pts) {
    int maxX = 0, maxY = 0;
    int minX = MAXINT32, minY = MAXINT32;
    for (Point pt : pts) {
        if (pt.x > maxX)
            maxX = pt.x;
        if (pt.x <= minX)
            minX = pt.x;
        if (pt.y > maxY)
            maxY = pt.y;
        if (pt.y < minY)
            minY = pt.y;
    }
    return Rect(minX, minY, maxX - minX + 1, maxY - minY + 1);
}


int CircleDection::OtsuThreshold(Mat& src) {
    if (!src.data)
        return 0;

    uchar* pSrc;
    double hist[256], chist[256], cxhist[256];
    memset(hist, 0, sizeof(hist));
    memset(chist, 0, sizeof(chist));
    memset(cxhist, 0, sizeof(cxhist));
    int ntot = src.rows * src.cols;
    int i = 0;

    for (int i = 0; i < src.rows; i++)
    {
        uchar* pSrc = src.ptr<uchar>(i);
        for (int j = 0; j < src.cols; j++)
        {
            hist[pSrc[j]] += 1;
        }
    }

    for (i = 0; i < 256; i++)
        hist[i] /= ntot;

    chist[0] = hist[0];
    cxhist[0] = 0;
    for (i = 1; i < 256; i++) {
        chist[i] = chist[i - 1] + hist[i];                    
        cxhist[i] = cxhist[i - 1] + double(i) * hist[i]; 
    };

    double gain_max = 0;
    int thresh = 0;
    double m = cxhist[255];                   
    int mul_count = 1;                          
    for (i = 0; i < 256; i++) {
        if (chist[i] == 0.) continue;
        double q1 = chist[i];                
        double q2 = 1 - q1;                    
        if (q2 == 0.) break;
        double m1 = cxhist[i] / q1;             
        double m2 = (m - cxhist[i]) / q2;       
        double gain = q1 * q2 * (m1 - m2) * (m1 - m2);

        if (gain_max < gain) {
            gain_max = gain;
            thresh = i;
            mul_count = 1;                         
        }
        else if (gain_max == gain)            
            mul_count++;
    }
    if (mul_count > 1)
        thresh = thresh + (mul_count - 1) / 2;   

    return thresh;
}

// 최소제곱법 원 중심 추출
Vec3f CircleDection::CircleFitByTaubin(vector<Point2d> edges)
{
    int i, iter, IterMAX = 99;

    double Xi, Yi, Zi;
    double Mz, Mxy, Mxx, Myy, Mxz, Myz, Mzz, Cov_xy, Var_z;
    double A0, A1, A2, A22, A3, A33;
    double Dy, xnew, x, ynew, y;
    double DET, Xcenter, Ycenter;
    double meanX = 0., meanY = 0.;

    Vec3f circle;

    for (int i = 0; i < edges.size(); i++)
    {
        meanX += edges[i].x;
        meanY += edges[i].y;
    }
    meanX /= edges.size();
    meanY /= edges.size();

    Mxx = Myy = Mxy = Mxz = Myz = Mzz = 0.;

    for (i = 0; i < edges.size(); i++)
    {
        Xi = edges[i].x - meanX;  
        Yi = edges[i].y - meanY;  
        Zi = Xi * Xi + Yi * Yi;

        Mxy += Xi * Yi;
        Mxx += Xi * Xi;
        Myy += Yi * Yi;
        Mxz += Xi * Zi;
        Myz += Yi * Zi;
        Mzz += Zi * Zi;
    }
    Mxx /= edges.size();
    Myy /= edges.size();
    Mxy /= edges.size();
    Mxz /= edges.size();
    Myz /= edges.size();
    Mzz /= edges.size();

    Mz = Mxx + Myy;
    Cov_xy = Mxx * Myy - Mxy * Mxy;
    Var_z = Mzz - Mz * Mz;
    A3 = 4.f * Mz;
    A2 = -3.f * Mz * Mz - Mzz;
    A1 = Var_z * Mz + 4.f * Cov_xy * Mz - Mxz * Mxz - Myz * Myz;
    A0 = Mxz * (Mxz * Myy - Myz * Mxy) + Myz * (Myz * Mxx - Mxz * Mxy) - Var_z * Cov_xy;
    A22 = A2 + A2;
    A33 = A3 + A3 + A3;

    for (x = 0., y = A0, iter = 0; iter < IterMAX; iter++)  
    {
        Dy = A1 + x * (A22 + A33 * x);
        xnew = x - y / Dy;
        if ((xnew == x) || (!isfinite(xnew))) break;
        ynew = A0 + xnew * (A1 + xnew * (A2 + xnew * A3));
        if (abs(ynew) >= abs(y))  break;
        x = xnew;  y = ynew;
    }

    DET = x * x - x * Mz + Cov_xy;
    Xcenter = (Mxz * (Myy - x) - Myz * Mxy) / DET / 2.f;
    Ycenter = (Myz * (Mxx - x) - Mxz * Mxy) / DET / 2.f;


    circle[0] = Xcenter + meanX;
    circle[1] = Ycenter + meanY;
    circle[2] = sqrt(Xcenter * Xcenter + Ycenter * Ycenter + Mz);

    return circle;
}

// 선형 보간법 - 밝기값 추출
template <typename T>
double CircleDection::BilinearValue(Mat& img, Point2d pt)
{
    if (pt.x >= img.cols - 1) pt.x--;
    if (pt.y >= img.rows - 1) pt.y--;

    Point pt_int((int)pt.x, (int)pt.y);
    double A = img.at<T>(pt_int);
    double B = img.at<T>(pt_int + Point(0, 1));
    double C = img.at<T>(pt_int + Point(1, 0));
    double D = img.at<T>(pt_int + Point(1, 1));

    double alpha = pt.y - pt_int.y;
    double beta = pt.x - pt_int.x;
    double M1 = A + alpha * (B - A);
    double M2 = C + alpha * (D - C);
    double P = M1 + beta * (M2 - M1);

    return P;
}

// 사각형 내부의 점 포함 여부
bool CircleDection::IsContain(Rect rc, vector<Point>& cirCenters) {
    for (Point pt : cirCenters) {
        int rcX = rc.tl().x;
        int rcY = rc.tl().y;
        if (pt.x >= rcX && pt.x <= rcX + rc.width) {
            if (pt.y >= rcY && pt.y <= rcY + rc.height) {
                return true;
            }
        }
    }
    return false;
}

// 최소제곱법을 통해 교점과 직선의 좌표 구하기
void CircleDection::getPointOfIntersection()
{
    Vec2d vEquation, hEquation;
    Point2d target, temp1, temp2;
    pair<Point2d, Point2d> templines;
    vEquation = LSM_Vertical(this->vCirCenters); // 수직 원들의 최소제곱법 직선식
    hEquation = LSM_Horizontal(this->hCirCenters); // 수평 원들의 최소제곱법 직선식
    // 직선의 방정식과 거리가 먼 경우를 제외
    vector<Vec3f> vCirCenters_, hCirCenters_;
    for (int i = 0; i < this->vCirCenters.size(); i++)
    {
        Point2d center(this->vCirCenters[i][0], this->vCirCenters[i][1]);
        if (this->adjDist > abs(vEquation[0] * center.x - center.y + vEquation[1]) / sqrt(pow(vEquation[0], 2) + 1))
        {
            vCirCenters_.push_back(vCirCenters[i]);
        }
    }

    for (int i = 0; i < this->hCirCenters.size(); i++)
    {
        Point2d center(this->hCirCenters[i][0], this->hCirCenters[i][1]);
        if (this->adjDist > abs(hEquation[0] * center.x - center.y + hEquation[1]) / sqrt(pow(hEquation[0], 2) + 1))
        {
            hCirCenters_.push_back(hCirCenters[i]);
        }
    }
    vCirCenters.clear();
    hCirCenters.clear();
    vCirCenters = vCirCenters_;
    hCirCenters = hCirCenters_;

    vEquation = LSM_Vertical(this->vCirCenters); // 수직 원들의 최소제곱법 직선식
    hEquation = LSM_Horizontal(this->hCirCenters); // 수평 원들의 최소제곱법 직선식
    // 최소제곱법 직선식들의 교점
    target.x = (hEquation[1] - vEquation[1]) / (vEquation[0] - hEquation[0]);
    target.y = (vEquation[0] * (hEquation[1] - vEquation[1])) / (vEquation[0] - hEquation[0]) + vEquation[1];
    this->IntersectionPoint = target;

    // 주황색 직선의 방정식 라인 점
    temp1 = { -vEquation[1] / vEquation[0], 0 }; // y=0 이고, 최소제곱법 직선의 방정식을 지나는 점
    temp2 = { ((double)this->src.rows - vEquation[1]) / vEquation[0], (double)this->src.rows }; //y=src.rows 이고, 최소제곱법 직선의 방정식을 지나는 점
    templines = { temp1, temp2 };
    this->LinePoints.push_back(templines);

    temp1 = { 0, hEquation[1] }; // x=0 이고, 최소제곱법 직선의 방정식을 지나는 점
    temp2 = { (double)this->src.cols, hEquation[0] * (double)this->src.cols + hEquation[1] }; // x=src.cols 이고, 최소제곱법 직선의 방정식을 지나는 점
    templines = { temp1, temp2 };
    this->LinePoints.push_back(templines);
}

// 최소제곱법 x, y좌표 스위칭 후 계산한 뒤 나온 식을 다시 y=x 대칭이동
Vec2f CircleDection::LSM_Vertical(vector<Vec3f>& pts)
{
    //x=ay+b
    double a, b;
    double xx = 0, x = 0, xy = 0, y = 0;
    int n = pts.size();
    Vec2f result;
    for (int i = 0; i < n; i++)
    {
        xx += (pts[i][1]) * (pts[i][1]);
        x += (pts[i][1]);
        xy += (pts[i][1]) * (pts[i][0]);
        y += (pts[i][0]);

    }
    a = (n * xy - x * y) / (n * xx - x * x);
    b = (xx * y - x * xy) / (n * xx - x * x);
    // x=ya+b ==> y = (1/a) * x - b/a
    result[0] = 1 / a;
    result[1] = -b / a;

    return result;
}

// 최소제곱법 함수
Vec2f CircleDection::LSM_Horizontal(vector<Vec3f>& pts)
{
    //y=ax+b
    double a, b;
    double xx = 0, x = 0, xy = 0, y = 0;

    int n = pts.size();
    Vec2f result;
    for (int i = 0; i < n; i++)
    {
        xx += (pts[i][0]) * (pts[i][0]);
        x += (pts[i][0]);
        xy += (pts[i][0]) * (pts[i][1]);
        y += (pts[i][1]);

    }
    a = (n * xy - x * y) / (n * xx - x * x);
    b = (xx * y - x * xy) / (n * xx - x * x);
    result[0] = a;
    result[1] = b;
    
    return result;
}

// 최소제곱법 x, y좌표 스위칭 후 계산한 뒤 나온 식을 다시 y=x 대칭이동
Vec2f CircleDection::LSM_Vertical(vector<Point>& pts)
{
    //x=ay+b
    double a, b;
    double xx = 0, x = 0, xy = 0, y = 0;
    int n = pts.size();
    Vec2f result;
    for (int i = 0; i < n; i++)
    {
        xx += (pts[i].y) * (pts[i].y);
        x += (pts[i].y);
        xy += (pts[i].y) * (pts[i].x);
        y += (pts[i].x);

    }
    a = (n * xy - x * y) / (n * xx - x * x);
    b = (xx * y - x * xy) / (n * xx - x * x);
    // x=ya+b ==> y = (1/a) * x - b/a
    result[0] = 1 / a;
    result[1] = -b / a;

    return result;
}

// 최소제곱법 함수
Vec2f CircleDection::LSM_Horizontal(vector<Point>& pts)
{
    //y=ax+b
    double a, b;
    double xx = 0, x = 0, xy = 0, y = 0;

    int n = pts.size();
    Vec2f result;
    for (int i = 0; i < n; i++)
    {
        xx += (pts[i].x) * (pts[i].x);
        x += (pts[i].x);
        xy += (pts[i].x) * (pts[i].y);
        y += (pts[i].y);

    }
    a = (n * xy - x * y) / (n * xx - x * x);
    b = (xx * y - x * xy) / (n * xx - x * x);
    result[0] = a;
    result[1] = b;

    return result;
}

void CircleDection::SetFilename(string filename) {
    this->fileName = filename;
}

string CircleDection::getFilename() {
    return this->fileName;
}

// 회전이미지 생성
bool CircleDection::Rotation()
{
    this->src = this->Original.clone();
    SetImage(src);
    // 최기화
    approx.clear();
    cornerPts.clear(); // 외곽 3점 좌표 값
    if (!GetCornerPoints()) // 3점 좌표 추출
        return false;
    // 회전
    double theta = atan2((this->cornerPts[0].x - this->cornerPts[0].x), (1000 - this->cornerPts[0].y))
        - atan2((this->cornerPts[1].x - this->cornerPts[0].x), this->cornerPts[1].y - this->cornerPts[0].y);
    this->rotation<uchar>(this->src, this->Rotate, -theta, this->cornerPts[0]);

    this->src = this->Rotate.clone();
    SetImage(src);
    // 최기화
    approx.clear();
    cornerPts.clear(); // 외곽 3점 좌표 값
    if (!GetCornerPoints()) // 3점 좌표 추출
        return false;
    int min_x = this->Rotate.rows, min_y = this->Rotate.cols, max_x = 0, max_y = 0;
    
    for (int i = 0; i < cornerPts.size(); i++)
    {
        int x = cornerPts[i].x;
        int y = cornerPts[i].y;

        if (min_x > x)
            min_x = x;
        else if (max_x < x)
            max_x = x;

        if (min_y > y)
            min_y = y;
        else if (max_y < y)
            max_y = y;
    }
    
    if (this->src.rows > max_y + 4)
        max_y += 4;
    if (0> min_y - 4)
        min_y -= 4;

    if (this->src.cols > max_x + 4)
        max_x += 4;
    if (0 > min_x - 4)
        min_x -= 4;
   
    this->Rotate = this->src(Rect(Point(min_x, min_y), Point(max_x, max_y)));
    isRotate = true;
    return true;
}
// 이미지 회전 함수
template <typename T>
void CircleDection::rotation(Mat src, Mat& dst, double theta, Point pt)
{
    double sin_value = sin(theta);
    double cos_value = cos(theta);

    int h = src.rows;
    int w = src.cols;

    Rect rect(Point(0, 0), src.size());
    dst = Mat(src.size(), src.type(), Scalar(0));
    T* ptr;

    Point2d target;
    for (int i = 0; i < h; i++)
    {
        ptr = (T*)dst.ptr<T>(i);
        for (int j = 0; j < w; j++)
        {
            // 좌표 이동전 -을 해서 이동시키기고 복원하려고
            int jj = j - pt.x;
            int ii = i - pt.y;

            // 회전 좌표계산 후 + 좌표이동
            target.x = jj * cos_value + ii * sin_value + pt.x;
            target.y = -jj * sin_value + ii * cos_value + pt.y; // 

            if (rect.contains(target))
                ptr[j] = BilinearValue<T>(src, target);
        }
    }
}