#pragma once
#include "pch.h"
#include "CircleDection.h"


//void main()
//{
//    CircleDection cir;
//    cir.Run();
//    cir.ShowSrcImage();
//    cir.ShowResultImage();
//    waitKey();
//}


HBITMAP CircleDection::MatToBitmap(Mat & src)
{
    Mat mat_temp;
    HDC hDC = ::CreateCompatibleDC(0);
    HBITMAP hBmp;

    //화면에 보여주기 위한 처리
    //CString tmp;
    //tmp.Format(_T("%d"), src.elemSize());
    //MessageBox(tmp);
    int bpp = 8 * src.elemSize(); // elemSize() 는 한화소에 데이터 개수 Color = 3개 Gray = 1개, 한 화소에 비트수 구하는거
    assert((bpp == 8 || bpp == 24 || bpp == 32)); // 이 3개가 아니면 종료

    int padding = 0;

    if (bpp < 32)
        padding = 4 - (src.cols % 4);

    if (padding == 4)
        padding = 0;

    int border = 0;

    if (bpp < 32)
    {
        border = 4 - (src.cols % 4);
    }

    if (border > 0 || src.isContinuous() == false)
    {
        cv::copyMakeBorder(src, mat_temp, 0, 0, 0, border, cv::BORDER_CONSTANT, 0);
    }
    else
    {
        mat_temp = src;
    }
    BITMAPINFO* pBitmapInfo;
    if (bpp == 8) 
    {
        pBitmapInfo = (BITMAPINFO*)malloc(sizeof(BITMAPINFO) + 4 * 256);
    }
    else
    {
        pBitmapInfo = (BITMAPINFO*)malloc(sizeof(BITMAPINFO));
    }

    pBitmapInfo->bmiHeader.biYPelsPerMeter = 0;
    pBitmapInfo->bmiHeader.biBitCount = bpp;
    pBitmapInfo->bmiHeader.biWidth = mat_temp.cols;
    pBitmapInfo->bmiHeader.biHeight = -mat_temp.rows;
    pBitmapInfo->bmiHeader.biPlanes = 1;
    pBitmapInfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    pBitmapInfo->bmiHeader.biCompression = BI_RGB;
    pBitmapInfo->bmiHeader.biClrImportant = 0;
    if (bpp == 8) { //그레이스케일인경우 팔레트가 필요
        pBitmapInfo->bmiHeader.biClrUsed = 4 * 256;
        for (int i = 0; i < 256; i++)
        {
            pBitmapInfo->bmiColors[i].rgbBlue = (BYTE)i;
            pBitmapInfo->bmiColors[i].rgbGreen = (BYTE)i;
            pBitmapInfo->bmiColors[i].rgbRed = (BYTE)i;
            pBitmapInfo->bmiColors[i].rgbReserved = (BYTE)0;
        }
    }
    else {
        pBitmapInfo->bmiHeader.biClrUsed = 0;
    }
    pBitmapInfo->bmiHeader.biSizeImage = 0;
    pBitmapInfo->bmiHeader.biXPelsPerMeter = 0;

    hBmp = ::CreateDIBSection(hDC, pBitmapInfo, DIB_RGB_COLORS, NULL, 0, 0);
    ::SetBitmapBits(hBmp, mat_temp.total() * mat_temp.channels(), mat_temp.data);
    ::DeleteDC(hDC);

    BITMAP bmp_info;
    // h_src_bmp 비트맵의 속성 정보를 얻는다.
    GetObject(hBmp, sizeof(BITMAP), &bmp_info);
    cout << " rows " << src.rows << " cols " << src.cols << endl;
    cout << " elemSize " << src.elemSize() << " channels " << src.channels() << endl;
    cout << " 상위 " << (src.type() >> 3) << " 하위 " << (src.type() & 0x0fff) << endl;
    cout << " bmHeight " <<bmp_info.bmHeight << " bmWidth " << bmp_info.bmWidth << endl;
    cout << " bmBits " << bmp_info.bmBits << " bmBitsPixel " << bmp_info.bmBitsPixel << endl;
    cout << " bmPlanes " << bmp_info.bmPlanes << endl;

    return hBmp;
}

Mat CircleDection::BitmapToMat(HBITMAP hBmp)
{
    BITMAP bmp_info;
    GetObject(hBmp, sizeof(BITMAP), &bmp_info);

    uchar* MatPtr;
    Mat temp(bmp_info.bmHeight, bmp_info.bmWidth, CV_MAKETYPE(bmp_info.bmBitsPixel/8-1, bmp_info.bmPlanes));
    for (int i = 0; i < bmp_info.bmHeight; i++)
    {
        MatPtr = temp.ptr(i);
        for (int j = 0; j < bmp_info.bmWidth; j++)
        {
            MatPtr[j] = *((uchar*)bmp_info.bmBits + ((i * bmp_info.bmWidth) + j));
        }
    }

    return temp;
}

CircleDection::CircleDection()
{
    this->distance = 0; // ROI 추출 범위
    this->radMin = 0; // 검출원 최소 반지름
    this->radMax = 0; // 검출원 최대 반지름
    this->BGV = 0; // 그레이
    this->thMinValue = 0; // 외각 좌표 추출 th
    this->thMaxValue = 0; // 외각 좌표 추출 th
    this->height = 0; // 높이
    this->width = 0; // 넓이
    this->size = 0; // 전체 크기


    // 주소 연결

    threadParam = new CirDetectionParam();
    threadParam->This = this;
    threadParam->cirCenters = &vCirCenters;
    threadParam->mut = new Mutex;

    this->cirValue.BGV = &this->BGV;
    this->cirValue.distance = &this->distance;
    this->cirValue.height = &this->height;
    this->cirValue.radMax = &this->radMax;
    this->cirValue.radMin = &this->radMin;
    this->cirValue.size = &this->size;
    this->cirValue.thMinValue = &this->thMinValue;
    this->cirValue.thMaxValue = &this->thMaxValue;
    this->cirValue.width = &this->width;
}

CircleDection::~CircleDection()
{

}

// 연결 
void CircleDection::Initialize()
{
    // 원본 이미지 값 set
    if (!this->src.data)
        return;
    else
    {
        this->height = this->src.rows;
        this->width = this->src.cols;
        this->size = height * width;
    }
}

// 초기화 initialize
void CircleDection::Init()
{
    if (!this->src.data)
        return;
    else 
    {
        this->height = this->src.rows;
        this->width = this->src.cols;
        this->size = height * width;
    }
    // 파라미터 값 초기화
    SetThreshValue(100, 158);
    SetDistance(20);
    SetCircleValue(2, 4, 80);

    // threadParam 초기화
    threadParam = new CirDetectionParam();
    threadParam->This = this;
    threadParam->cirCenters = &vCirCenters;
    threadParam->mut = new Mutex;


    this->cirValue.BGV = &this->BGV;
    this->cirValue.distance = &this->distance;
    this->cirValue.height = &this->height;
    this->cirValue.radMax = &this->radMax;
    this->cirValue.radMin = &this->radMin;
    this->cirValue.size = &this->size;
    this->cirValue.thMinValue = &this->thMinValue;
    this->cirValue.thMaxValue = &this->thMaxValue;
    this->cirValue.width = &this->width;

}

// 이미지 선택
//void CircleDection::SelectImage()
//{
//    char szFilter[] = "Image(*.BMP, *.GOF, *.JPG, *.PNG)|*.BMP; *.GIF; *.JPG; *.bmp; *.gif; *.jpg; *.png; | All Files(*.*) | *.* ||";
//    CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, CA2CT(szFilter), AfxGetMainWnd());
//    if (dlg.DoModal() == IDOK)
//    {
//        CString cstrImgPath = dlg.GetPathName();
//        CT2CA pszConvertedAnsiString(cstrImgPath);
//        std::string strStd(pszConvertedAnsiString);
//
//        this->SetImage(imread(strStd, IMREAD_GRAYSCALE));
//        this->Init();
//        //this->Run();
//        //this->ShowResultImage();
//    }
//}
bool CircleDection::SelectImage()
{
    char szFilter[] = "Image(*.BMP, *.GOF, *.JPG, *.PNG)|*.BMP; *.GIF; *.JPG; *.bmp; *.gif; *.jpg; *.png; | All Files(*.*) | *.* ||";
    CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, CA2CT(szFilter), AfxGetMainWnd());
    if (dlg.DoModal() == IDOK)
    {
        CString cstrImgPath = dlg.GetPathName();
        CT2CA pszConvertedAnsiString(cstrImgPath);
        std::string strStd(pszConvertedAnsiString);

        this->SetImage(imread(strStd, IMREAD_GRAYSCALE));
        this->Init();
        //this->Run();
        //this->ShowResultImage();
        return TRUE;
    }
    else
        return FALSE;
}

bool CircleDection::isReady()
{
    if (!src.empty()) return TRUE;
    else return FALSE;
}

// 전체 실행
void CircleDection::Run()
{
    cvtColor(this->src, this->result, COLOR_GRAY2BGR);
    cornerPts.clear(); // 외곽 3점 좌표 값
    vCirCenters.clear(); // Vertical의 중심 좌표들의 값 
    hCirCenters.clear(); // Horizontal의 중심 좌표들의 값 
    vertexPts.clear();   // 최외곽 ROI Vertex Points
    GetCornerPoints(); // 3점 좌표 추출
    GetVertexPoints(); // 최외곽 ROI
    ContourDetection(); // 원 검출
    Drawing(); // Drawing
}

// 전체 실행
void CircleDection::ALLRun()
{
    Init();
    GetCornerPoints(); // 3점 좌표 추출
    GetVertexPoints(); // 최외곽 ROI
    ContourDetection(); // 원 검출
    Drawing(); // Drawing
}

// 이미지 set
void CircleDection::SetImage(Mat& src)
{
    this->src = src;
    this->height = src.rows;
    this->width = src.cols;
    this->size = src.rows * src.cols;
}

// Thresh value set
void CircleDection::SetThreshValue(int thMinValue = 100, int thMaxValue = 158)
{
    this->thMinValue = thMinValue;
    this->thMaxValue = thMaxValue;
}

// ROI 추출 범위 set
void CircleDection::SetDistance(int distance = 20)
{
    this->distance = distance;
}

// 원검출 set
void CircleDection::SetCircleValue(int radMin = 2, int radMax = 4, int BGV = 100)
{
    this->radMin = radMin;
    this->radMax = radMax;
    this->BGV = BGV;
}

// 원본 이미지 가져오기
Mat CircleDection::GetSourceImage()
{
    return this->src;
}

// 결과 이미지 가져오기
Mat CircleDection::GetResultImage()
{
    return this->result;
}

// value 구조체 값 가져오기
CircleValue* CircleDection::GetCircleValue()
{
    return &this->cirValue;
}

// 소스 이미지 출력
bool CircleDection::ShowSrcImage()
{
    if (!this->src.data) {
        // error
        return false;
    }
    imshow("src", src);
    return true;
}

// 결과 이미지 출력
bool CircleDection::ShowResultImage()
{
    if (!this->result.data) {
        // error
        return false;
    }
    imshow("result", result);
    return true;
}



//==============================================================================



// 3점 좌표 추출 수정판
void CircleDection::GetCornerPoints()
{
    Mat grayImage = this->src.clone();
    Matx <uchar, 3, 3> mask(0, 1, 0, 1, 1, 1, 0, 1, 0);
    //threshold(grayImage, grayImage, this->thMinValue, 255, THRESH_TOZERO); // min_grayscale이 안되면 0
    //threshold(grayImage, grayImage, this->thMaxValue, 255, THRESH_TOZERO_INV); // min_grayscale이 넘어도 0
    ToZeroThreshold(grayImage, grayImage, this->thMinValue, this->thMaxValue);
    morphologyEx(grayImage, grayImage, MORPH_OPEN, mask); // 외곽의 솔트를 제거하기 위해

    vector<vector<Point>> contours;
    MyContours(grayImage, contours);
    //findContours(grayImage, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

    vector<Point> approx;
    for (size_t i = 0; i < contours.size(); i++)
    {
        if (contourArea(Mat(contours[i])) > 5000)
        {
            DouglasPeucker(contours[i], approx, 0);
            //approxPolyDP(Mat(contours[i]), approx, arcLength(Mat(contours[i]), true) * 0.1, true);
            break;
        }
    }
    //for (int i = 0; i < approx.size(); i++)
    //    circle(this->src, approx[i], 3, Scalar(255));

    double distance0to1 = sqrt(pow(approx[0].x - approx[1].x, 2) + pow(approx[0].y - approx[1].y, 2));
    double distance0to3 = sqrt(pow(approx[0].x - approx[3].x, 2) + pow(approx[0].y - approx[3].y, 2));

    if (distance0to1 < distance0to3)
    {
        this->cornerPts.push_back(approx[0]);
        this->cornerPts.push_back(approx[3]);
        this->cornerPts.push_back(approx[2]);
    }
    else
    {
        this->cornerPts.push_back(approx[3]);
        this->cornerPts.push_back(approx[2]);
        this->cornerPts.push_back(approx[1]);
    }
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
            //if (ptr[y * width  + x] != '\0')
            //if (ptr[x] != 0)
            //if (ptr2[y*width+x] != 0)
            //if (src.at<uchar>(x,y) != 0)
            //if (src.at<uchar*>(x,y) != 0)
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

        if (nx < 0 || nx >= this->width || ny < 0 || ny >= this->height || Map[ny * this->width + nx] != value)
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
        std::cout << "Not enough points to simplify" << std::endl;
        return;
    }
    // Find the point with the maximum distance from line between start and end
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
        //flag = false;
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
    // If max distance is greater than epsilon, recursively simplify
    if (maxDistance > epsilon)
    {
        // Recursive call
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

        // Build the result list
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
        //return sqrt(pow(pt.x - dx, 2) + pow(pt.y - dy, 2));
        return sqrt(pow(pt.x - lineStart.x, 2) + pow(pt.y - lineStart.y, 2));
    }
    double dx = lineEnd.x - lineStart.x;
    double dy = lineEnd.y - lineStart.y;

    //Normalise
    double mag = sqrt(pow(dx, 2) + pow(dy, 2));
    if (mag > 0.0)
    {
        dx /= mag;
        dy /= mag;
    }

    double pvx = pt.x - lineStart.x;
    double pvy = pt.y - lineStart.y;

    //Get dot product (project pv onto normalized direction)
    double pvdot = dx * pvx + dy * pvy;

    //Scale line direction vector
    double dsx = pvdot * dx;
    double dsy = pvdot * dy;

    //Subtract this from pv
    double ax = pvx - dsx;
    double ay = pvy - dsy;

    return sqrt(pow(ax, 2.0) + pow(ay, 2.0));
}

// 최외곽 ROI Vertex 추출
void CircleDection::GetVertexPoints()
{
    int rotateX, rotateY;
    double theta;

    int heightV = sqrt(pow(this->cornerPts[0].y - this->cornerPts[1].y, 2) + pow(this->cornerPts[0].x - this->cornerPts[1].x, 2));
    int widthV = this->distance;

    int heightH = this->distance;
    int widthH = sqrt(pow(this->cornerPts[1].y - this->cornerPts[2].y, 2) + pow(this->cornerPts[1].x - this->cornerPts[2].x, 2));

    this->vertexPts.resize(6);

    if (this->cornerPts[1].x == this->cornerPts[0].x) {
        this->vertexPts[0] = this->cornerPts[0];
        this->vertexPts[1] = this->cornerPts[1];
        this->vertexPts[5] = Point(this->cornerPts[0].x + this->distance, this->cornerPts[0].y);
        this->vertexPts[4] = Point(this->cornerPts[1].x + this->distance, this->cornerPts[1].y);
    }
    else {
        theta = atan((double)((this->cornerPts[0].x - this->cornerPts[1].x)) / (this->cornerPts[0].y - this->cornerPts[1].y));
        this->vertexPts[0] = this->cornerPts[0];
        this->vertexPts[1] = this->cornerPts[1];


        rotateX = (this->distance) * cos(theta) + this->cornerPts[0].x;
        rotateY = -(this->distance) * sin(theta) + this->cornerPts[0].y;
        this->vertexPts[5] = Point(rotateX, rotateY);


        rotateX = widthV * cos(theta) + heightV * sin(theta) + this->cornerPts[0].x;
        rotateY = -widthV * sin(theta) + heightV * cos(theta) + this->cornerPts[0].y;
        this->vertexPts[4] = Point(rotateX, rotateY);
    }

    if (this->cornerPts[2].y == this->cornerPts[1].y) {
        this->vertexPts[2] = this->cornerPts[2];
        this->vertexPts[3] = Point(this->cornerPts[2].x, this->cornerPts[2].y - this->distance);

        this->vertexPts[4].x = this->cornerPts[1].x + this->vertexPts[4].x - this->vertexPts[1].x;
        this->vertexPts[4].y = this->cornerPts[1].y - this->distance + this->vertexPts[4].y - this->vertexPts[1].y;
    }
    else {
        theta = -atan((this->cornerPts[1].y - this->cornerPts[2].y) / (double)((this->cornerPts[1].x - this->cornerPts[2].x)));
        rotateX = -this->distance * sin(theta) + this->cornerPts[1].x;
        rotateY = -this->distance * cos(theta) + this->cornerPts[1].y;

        this->vertexPts[4].x = rotateX + this->vertexPts[4].x - this->vertexPts[1].x;
        this->vertexPts[4].y = rotateY + this->vertexPts[4].y - this->vertexPts[1].y;

        this->vertexPts[2] = this->cornerPts[2];
        rotateX = widthH * cos(theta) - heightH * sin(theta) + this->cornerPts[1].x;
        rotateY = -widthH * sin(theta) - heightH * cos(theta) + this->cornerPts[1].y;
        this->vertexPts[3] = Point(rotateX, rotateY);
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
    //findContours(imgThreshold, contours, RETR_LIST, CHAIN_APPROX_SIMPLE);

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
    //_beginthreadex(NULL, 0, CirDetectionThread, (void*)threadParam, 0, &threadId);
    CircleDetection(hContours, this->hCirCenters);
    //threadParam->mut->lock();
    WaitForSingleObject((HANDLE)_beginthreadex(NULL, 0, CirDetectionThread, (void*)threadParam, 0, &threadId) , INFINITE);
}

// Points를 통해 해당 부분의 이미지 추출
void CircleDection::fillPoly_(Size matSize, Mat& dst, vector<Point> pts) {
    Rect rc = boundRect(pts);
    dst = Mat::zeros(matSize, CV_8UC1);
#pragma region 선 그리기
    int size = pts.size();
    for (int i = 0; i < size - 1; i++) {
        line(dst, pts[i], pts[i + 1], Scalar(255, 255, 255));
    }
    line(dst, pts[0], pts[size - 1], Scalar(255, 255, 255));
#pragma endregion
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
    Param->mut->lock();
    Param->This->CircleDetection(*Param->contours, *Param->cirCenters);
    Param->mut->unlock();
    return 0;
}

// 원 검출
void CircleDection::CircleDetection(vector<vector<Point>>& contours, vector<Point>& cirCenters) {

    for (vector<Point> pts : contours) {
        double areaValue = contourArea(pts); // 면적값 계산
        if (areaValue < 50 && areaValue > 0) {
            //Rect rc = boundingRect(pts);
            Rect rc = boundRect(pts);
#pragma region 수정 요함
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
#pragma endregion

            Mat cirRect = this->src(rc);
            Differential(cirRect, cirRect);

            int rCount = 0;
            for (int r = this->radMin; r <= this->radMax; r++, rCount++)
            {
                int rotate = 360 / (8 * r);
                int** vote = (int**)calloc(cirRect.rows, sizeof(int*));
                for (int i = 0; i < cirRect.rows; i++) {
                    vote[i] = (int*)calloc(cirRect.cols, sizeof(int));
                }
                if (IsContain(rc, cirCenters))
                    continue;
                for (int i = 0; i < cirRect.cols; i++) {
                    uchar* ptr = this->src.ptr<uchar>(i);
                    for (int j = 0; j < cirRect.rows; j++) {
                        if (ptr[j] >= 100)
                            continue;
                        for (int t = 1; t <= 360; t += rotate)
                        {
                            int a = i - r * cvRound(cos(t * CV_PI / 180));
                            int b = j - r * cvRound(sin(t * CV_PI / 180));
                            if (a >= 0 && a < cirRect.cols && b >= 0 && b < cirRect.rows) {
                                if (cirRect.at<uchar>(b, a) >= 100)
                                    vote[j][i] += 1;
                            }
                        }

                    }
                }
                int max = -1;
                Point idx;

                for (int i = 0; i < cirRect.cols; i++)
                {
                    for (int j = 0; j < cirRect.rows; j++)
                    {
                        if (vote[j][i] >= max)
                        {
                            max = vote[j][i];
                            idx = Point(i, j);
                        }
                    }
                }
                /*if (r != 0)
                    continue;*/
                if (max >= 360 / rotate)
                {
                    cirCenters.push_back(Point(idx.x + rc.tl().x, idx.y + rc.tl().y));
                }
                for (int i = 0; i < cirRect.rows; i++)
                    free(vote[i]);
                free(vote);
            }
        }
    }
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

// 미분 함수(prewitt Edge)
void CircleDection::Differential(Mat& src, Mat& dst) {
    Mat dstX, dstY;

    float prewittX[] = {
        -1,-1,-1,
        0,0,0,
        1,1,1
    };
    float prewittY[] = {
        -1,0,1,
        -1,0,1,
        -1,0,1
    };
    Mat maskX(3, 3, CV_32F, prewittX);
    Mat maskY(3, 3, CV_32F, prewittY);

    filter2D(src, dstX, CV_32F, maskX, Point(-1, -1), 3, 1);
    filter2D(src, dstY, CV_32F, maskY, Point(-1, -1), 3, 1);

    magnitude(dstX, dstY, dst);

    dst.convertTo(dst, CV_8U);
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

// 최소제곱법을 통해 교점 표시, 직선 및 원 그리기
void CircleDection::Drawing()
{
    if (!this->src.data)
        return;
    if (!this->result.data)
        cvtColor(this->src, this->result, COLOR_GRAY2BGR);
    Vec2f vEquation, hEquation;
    Point target, temp1, temp2;
    vEquation = LSM_Vertical(this->vCirCenters); // 수직 원들의 최소제곱법 직선식
    hEquation = LSM_Horizontal(this->hCirCenters); // 수평 원들의 최소제곱법 직선식

    // 최소제곱법 직선식들의 교점
    target.x = cvRound((hEquation[1] - vEquation[1]) / (vEquation[0] - hEquation[0]));
    target.y = cvRound((vEquation[0] * (hEquation[1] - vEquation[1])) / (vEquation[0] - hEquation[0]) + vEquation[1]);

    // 교점이 이미지 범위 밖에 있는 경우
    if (target.x < 0 || target.x > this->width || target.y<0 || target.y > this->height)
        return;

    // 주황색 직선의 방정식 그리기
    temp1 = { cvRound(-vEquation[1] / vEquation[0]), 0 }; // y=0 이고, 최소제곱법 직선의 방정식을 지나는 점
    //temp1 = { 0, cvRound(vEquation[1]) }; // x=0 이고, 최소제곱법 직선의 방정식을 지나는 점
    temp2 = { cvRound((target.y + 20 - vEquation[1]) / vEquation[0]), target.y + 20 }; // target에서 좀 더 아래쪽의 점

    line(this->result, temp1, temp2, Scalar(0, 127, 255));
    temp1 = { this->result.cols, cvRound(hEquation[0] * this->result.cols + hEquation[1]) }; // x = src.cols 이고, 최소제곱법 직선의 방정식을 지나는 점
    temp2 = { target.x - 20, cvRound(hEquation[0] * (target.x - 20) + hEquation[1]) }; // target에서 좀 더 왼쪽의 점
    line(this->result, temp1, temp2, Scalar(0, 127, 255));

    // 라임색 X 그리기
    temp1 = { target.x - 7, target.y - 7 };
    temp2 = { target.x + 7,target.y + 7 };
    line(this->result, temp1, temp2, Scalar(0, 255, 191));
    temp1 = { target.x - 7, target.y + 7 };
    temp2 = { target.x + 7,target.y - 7 };
    line(this->result, temp1, temp2, Scalar(0, 255, 191));

    //cout << "교점의 좌표 = " << target << endl;

    for (int i = 0; i < this->vCirCenters.size(); i++)
    {
        Point center(this->vCirCenters[i].x, this->vCirCenters[i].y);
        circle(this->result, center, 5, Scalar(0, 0, 255), 1, -1);
    }

    for (int i = 0; i < this->hCirCenters.size(); i++)
    {
        Point center(this->hCirCenters[i].x, this->hCirCenters[i].y);
        circle(this->result, center, 5, Scalar(0, 0, 255), 1, -1);
    }
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
    //cout << "y=" << a << "x+" << b << endl; // y=ax+b

    return result;
}





