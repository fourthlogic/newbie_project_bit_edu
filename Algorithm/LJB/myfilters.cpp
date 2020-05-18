#include "main.h"

void myERODE3(Mat src, Mat& dst, cv::InputArray _mask)
{
    dst = Mat(src.size(), CV_8U, Scalar(0));
    Mat mask = _mask.getMat();
    Point h_m = mask.size() / 2;
    for (int i = h_m.y; i < src.rows - h_m.y; i++)
    {
        const uchar* current = src.ptr<uchar>(i);
        uchar* output = dst.ptr<uchar>(i);
        for (int j = h_m.x; j < src.cols - h_m.x; j++)
        {
            if (current[j] == 0)
                continue;
            uchar min = 255;
            for (int k = 0; k < mask.rows; k++)
            {
                const uchar* curmask = mask.ptr<uchar>(k);
                for (int t = 0; t < mask.cols; t++)
                {
                    int y = i + k - h_m.y;
                    int x = j + t - h_m.x;
                    const uchar* www = src.ptr<uchar>(y);
                    if (curmask[t] * www[x] < min)
                        min = www[x];
                }
            }
            output[j] = min;
        }
    }
}
void myDILATE3(Mat src, Mat& dst, cv::InputArray _mask)
{
    dst = Mat(src.size(), CV_8U, Scalar(0));
    Mat mask = _mask.getMat();
    Point h_m = mask.size() / 2;
    for (int i = h_m.y; i < src.rows - h_m.y; i++)
    {
        const uchar* current = src.ptr<uchar>(i);
        uchar* output = dst.ptr<uchar>(i);
        for (int j = h_m.x; j < src.cols - h_m.x; j++)
        {
            if (current[j] == 1)
                continue;
            uchar max = 0;
            for (int k = 0; k < mask.rows; k++)
            {
                const uchar* curmask = mask.ptr<uchar>(k);
                for (int t = 0; t < mask.cols; t++)
                {
                    int y = i + k - h_m.y;
                    int x = j + t - h_m.x;
                    const uchar* www = src.ptr<uchar>(y);
                    if (curmask[t] * www[x] > max)
                        max = www[x];
                }
            }
            output[j] = max;
        }
    }
}
//void myERODE2(const Mat& src, Mat& dst, cv::InputArray _mask)
//{
//    CV_Assert(src.depth() == CV_8U);
//
//    dst.create(src.size(), src.type());
//    Mat mask = _mask.getMat();
//    for (int j = 1; j < src.rows - 1; ++j)
//    {
//        const uchar* previous = src.ptr<uchar>(j - 1);
//        const uchar* current = src.ptr<uchar>(j);
//        const uchar* next = src.ptr<uchar>(j + 1);
//
//        uchar* output = dst.ptr<uchar>(j);
//
//        for (int i = 1; i < src.cols - 1; ++i)
//        {
//            //*output++ = saturate_cast<uchar>(current[i] - current[i - 1] - current[i + 1] - previous[i] - next[i]);
//            for (int k = 0; k < mask.rows; k++)
//            {
//                const uchar* cmask = mask.ptr<uchar>(k);
//                for (int t = 0; t < mask.cols; t++)
//                {
//                    //cmask[t] * 
//                }
//            }
//        }
//    }
//
//    dst.row(0).setTo(Scalar(0));
//    dst.row(dst.rows - 1).setTo(Scalar(0));
//    dst.col(0).setTo(Scalar(0));
//    dst.col(dst.cols - 1).setTo(Scalar(0));
//}

void myERODE(Mat src, Mat& dst, cv::InputArray _mask)
{
    dst = Mat(src.size(), CV_8U, Scalar(0));
    Mat mask = _mask.getMat();
    Point h_m = mask.size() / 2;
    for (int i = h_m.y; i < src.rows - h_m.y; i++)
    {
        for (int j = h_m.x; j < src.cols - h_m.x; j++)
        {
            if (src.at<uchar>(i, j) == 0)
                continue;
            uchar min = 255;
            for (int k = 0; k < mask.rows; k++)
            {
                for (int t = 0; t < mask.cols; t++)
                {
                    int y = i + k - h_m.y;
                    int x = j + t - h_m.x;
                    if (mask.at<uchar>(k, t) * src.at<uchar>(y, x) < min)
                        min = src.at<uchar>(y, x);
                }
            }
            dst.at<uchar>(i, j) = min;
        }
    }
}
void myDILATE(Mat src, Mat& dst, cv::InputArray _mask)
{
    dst = Mat(src.size(), CV_8U, Scalar(0));
    Mat mask = _mask.getMat();
    Point h_m = mask.size() / 2;
    for (int i = h_m.y; i < src.rows - h_m.y; i++)
    {
        for (int j = h_m.x; j < src.cols - h_m.x; j++)
        {
            if (src.at<uchar>(i, j) == 1)
                continue;
            uchar max = 0;
            for (int k = 0; k < mask.rows; k++)
            {
                for (int t = 0; t < mask.cols; t++)
                {
                    int y = i + k - h_m.y;
                    int x = j + t - h_m.x;
                    if (mask.at<uchar>(k, t) * src.at<uchar>(y, x) > max)
                        max = src.at<uchar>(y, x);
                }
            }
            dst.at<uchar>(i, j) = max;
        }
    }
}
void main()
{
    time_t start;
    time_t end;
    Mat src = imread("ori_images/a2.png", IMREAD_GRAYSCALE);
    Mat dst;
    start = clock();

    vector<Point> cornerpts; // 외곽 3점 좌표 값

    vector<Point> vCirCenters; // Vertical의 중심 좌표들의 값 
    vector<Point> hCirCenters; // Horizontal의 중심 좌표들의 값 

    vector<Point> vertexPts;

    GetCornerPoints(src, cornerpts); // 3점 좌표 추출

    GetVertexPoints(src, cornerpts, vertexPts, 20);
    ContourDetection(src, vCirCenters, hCirCenters, vertexPts, 2, 4, 100);

    Drawing(src, dst, vCirCenters, hCirCenters);
    end = clock();

    imshow("result", dst);

    cout << end - start << endl;
    waitKey();
}

double myArcLength(cv::InputArray curve, bool closed)
{
    Mat src = curve.getMat();
    double length = 0;
    for (int i = 0; i < src.rows - 1; i++)
    {
        int* ptr = src.ptr<int>(i);
        length += sqrt(pow(*ptr - *(ptr + 2), 2) + pow(*(ptr + 1) - *(ptr + 3), 2));
    }
    if (closed)
    {
        int* ptr = src.ptr<int>(0);
        length += sqrt(pow(*ptr - *(ptr + 2 * (src.rows - 1)), 2) + pow(*(ptr + 1) - *(ptr + 2 * (src.rows - 1) + 1), 2));
    }
    return length;
}

// 3점 좌표 추출
void GetCornerPoints(Mat& src, vector<Point>& cornerPts)
{
    Mat grayImage = src.clone();
    // 110~158 사이값만 추출
    //threshold(grayImage, grayImage, 110, 255, THRESH_TOZERO);
    //threshold(grayImage, grayImage, 158, 255, THRESH_TOZERO_INV);
    inRange(grayImage, Scalar(110), Scalar(158), grayImage);
    //imshow("inRange", grayImage);
    //waitKey();

    // 외곽 다듬기
    Matx <uchar, 3, 3> mask(0, 1, 0, 1, 1, 1, 0, 1, 0);
    myERODE3(grayImage, grayImage, mask);
    myDILATE3(grayImage, grayImage, mask);
    //morphologyEx(grayImage, grayImage, MORPH_OPEN, mask);
    //imshow("after", grayImage);
    //waitKey();
    vector<vector<Point>> contours;
    findContours(grayImage, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);


    vector<Point> approx;
    vector<Point> corners;

    for (size_t i = 0; i < contours.size(); i++)
    {
        if (contourArea(Mat(contours[i])) > 10000)
        {
            approxPolyDP(Mat(contours[i]), approx, myArcLength(Mat(contours[i]), true) * 0.07, true);
            int size = approx.size();

            for (int k = 0; k < size; k++)
            {
                corners.push_back(approx[k]);
            }
            circle(grayImage, approx[0], 4, Scalar(255));
        }
    }
    //0(top) 부터 반시계 방향(꼭짓점) 
    double distance0to1 = sqrt(pow(corners[0].x - corners[1].x, 2) + pow(corners[0].y - corners[1].y, 2));
    double distance0to3 = sqrt(pow(corners[0].x - corners[3].x, 2) + pow(corners[0].y - corners[3].y, 2));
    if (distance0to1 < distance0to3)
    {
        for (int i = 1; i < corners.size(); i++)
            cornerPts.push_back(corners[i]);
    }
    else
    {
        for (int i = 0; i < corners.size() - 1; i++)
            cornerPts.push_back(corners[i]);
    }
}
// 최외곽 ROI Vertex 추출
void GetVertexPoints(Mat& src, vector<Point>& cornerPts, vector<Point>& vertexPts, int distance)
{
    int rotateX, rotateY;
    double theta;

    int heightV = sqrt(pow(cornerPts[0].y - cornerPts[1].y, 2) + pow(cornerPts[0].x - cornerPts[1].x, 2));
    int widthV = distance;

    int heightH = distance;
    int widthH = sqrt(pow(cornerPts[1].y - cornerPts[2].y, 2) + pow(cornerPts[1].x - cornerPts[2].x, 2));

    vertexPts.resize(6);

    if (cornerPts[1].x == cornerPts[0].x) {
        vertexPts[0] = cornerPts[0];
        vertexPts[1] = cornerPts[1];
        vertexPts[5] = Point(cornerPts[0].x + distance, cornerPts[0].y);
        vertexPts[4] = Point(cornerPts[1].x + distance, cornerPts[1].y);
    }
    else {
        theta = atan((double)((cornerPts[0].x - cornerPts[1].x)) / (cornerPts[0].y - cornerPts[1].y));
        vertexPts[0] = cornerPts[0];
        vertexPts[1] = cornerPts[1];


        rotateX = (distance)*cos(theta) + cornerPts[0].x;
        rotateY = -(distance)*sin(theta) + cornerPts[0].y;
        vertexPts[5] = Point(rotateX, rotateY);


        rotateX = widthV * cos(theta) + heightV * sin(theta) + cornerPts[0].x;
        rotateY = -widthV * sin(theta) + heightV * cos(theta) + cornerPts[0].y;
        vertexPts[4] = Point(rotateX, rotateY);
    }

    if (cornerPts[2].y == cornerPts[1].y) {
        vertexPts[2] = cornerPts[2];
        vertexPts[3] = Point(cornerPts[2].x, cornerPts[2].y - distance);

        vertexPts[4].x = cornerPts[1].x + vertexPts[4].x - vertexPts[1].x;
        vertexPts[4].y = cornerPts[1].y - distance + vertexPts[4].y - vertexPts[1].y;
    }
    else {
        theta = -atan((cornerPts[1].y - cornerPts[2].y) / (double)((cornerPts[1].x - cornerPts[2].x)));
        rotateX = -distance * sin(theta) + cornerPts[1].x;
        rotateY = -distance * cos(theta) + cornerPts[1].y;

        vertexPts[4].x = rotateX + vertexPts[4].x - vertexPts[1].x;
        vertexPts[4].y = rotateY + vertexPts[4].y - vertexPts[1].y;

        vertexPts[2] = cornerPts[2];
        rotateX = widthH * cos(theta) - heightH * sin(theta) + cornerPts[1].x;
        rotateY = -widthH * sin(theta) - heightH * cos(theta) + cornerPts[1].y;
        vertexPts[3] = Point(rotateX, rotateY);
    }
}
// 원검출을 위한 ROI Area 추출
void ContourDetection(Mat& src, vector<Point>& vCirCenters, vector<Point>& hCirCenters, vector<Point>& vertexPts, int radMin, int radMax, int value) {
    //ROI 꼭짓점 
    vector<vector<Point>> vpts;
    vpts.push_back(vertexPts);
    //ROI 처리
    Mat mask = Mat::zeros(src.size(), CV_8UC1);
    fillPoly(mask, vpts, Scalar(255, 255, 255), 8, 0);
    //imshow("fillPoly", vpts);
    waitKey();
    Mat ROI;
    bitwise_and(src, mask, ROI);


    //findContours전용 Mat
    Mat imgThreshold = ROI.clone();
    threshold(imgThreshold, imgThreshold, value, 250, THRESH_BINARY_INV);

    vector<vector<Point>> contours;
    findContours(imgThreshold, contours, RETR_LIST, CHAIN_APPROX_SIMPLE);

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
    CirDetectionParam2 cir2 = { src,vContours,vCirCenters,radMax,radMin };
    //Thread 매개변수를 넘기기 위한 구조체 초기화
    cir = new CirDetectionParam();
    cir->cirCenters = &vCirCenters;
    cir->contours = &vContours;
    cir->src = &src;
    cir->radMax = radMax;
    cir->radMin = radMin;
    cir->mut = new Mutex;
    unsigned threadId;
    _beginthreadex(NULL, 0, CirDetectionThread, (void*)cir, 0, &threadId);
    CircleDetection(src, hContours, hCirCenters, radMin, radMax);
    cir->mut->lock();
}
// 원 검출 Thread
unsigned WINAPI CirDetectionThread(void* para) {
    CirDetectionParam* cir = (CirDetectionParam*)para;
    cir->mut->lock();
    CircleDetection(*cir->src, *cir->contours, *cir->cirCenters, cir->radMin, cir->radMax);
    cir->mut->unlock();
    return 0;
}
// 원 검출
void CircleDetection(Mat src, vector<vector<Point>> contours, vector<Point>& cirCenters, int radMin, int radMax) {
    for (vector<Point> pts : contours) {
        double areaValue = contourArea(pts); // 면적값 계산
        if (areaValue < 50 && areaValue > 0) {
            Rect rc = boundingRect(pts);
#pragma region 수정 요함
            if (rc.tl().x - 3 < 0 || rc.tl().y - 3 < 0)
                continue;
            rc -= Point(3, 3);
            if (rc.tl().x + rc.width + 4 > src.cols || rc.tl().y + rc.height + 4 > src.rows)
                continue;
            rc += Size(4, 4);
#pragma endregion

            if (IsContain(rc, cirCenters))
                continue;

            Mat cirRect = src(rc);
            Differential(cirRect, cirRect);

            vector<Vec3f> houghCircles;
            HoughCircles(cirRect, houghCircles, HOUGH_GRADIENT, 1, 6, 255, 5, radMin, radMax);
            if (houghCircles.size() != 0) {
                cirCenters.push_back(Point(houghCircles[0][0] + rc.tl().x, houghCircles[0][1] + rc.tl().y));
            }
        }
    }
}

///////////////////////////////////////////////////
void filter(Mat src, Mat& dst, Mat mask)
{
    dst = Mat(src.size(), CV_32F, Scalar(0));
    Point h_m = mask.size() / 2;
    for (int i = 1; i < src.rows - h_m.y; i++)
    {
        float* ptr_current = src.ptr<float>(i);
        float* ptr_output = dst.ptr<float>(i);
        for (int j = 1; j < src.cols - h_m.x; j++)
        {
            float current = ptr_current[j];
            float sum = 0;
            for (int k = 0; k < mask.rows; k++)
            {
                for (int t = 0; t < mask.cols; t++)
                {
                    int y = i + k - h_m.y;
                    int x = j + t - h_m.x;
                    sum += mask.ptr<float>(k)[t] * src.ptr<uchar>(y)[x];
                }
            }
            ptr_output[j] = sum;
        }
    }
}

void myMagnitude(Mat& src1, Mat& src2, Mat& dst)
{
    if (src1.size() != src2.size())
        return;
    dst = Mat(src1.size(), CV_32F, Scalar(0));
    for (int y = 0; y < src1.rows; y++)
    {
        for (int x = 0; x < src1.cols; x++)
        {
            dst.at<float>(y, x) = sqrt(pow(src1.at<float>(y, x), 2) + pow(src2.at<float>(y, x), 2));
        }
    }
}
///////////////////////////////////////////////////

// 미분 함수(prewitt Edge)
void Differential(Mat& src, Mat& dst) {
    Mat dstX, dstY;

    float prewittX[] = {
        -1,-1,-1,
         0, 0, 0,
         1, 1, 1
    };
    float prewittY[] = {
        -1, 0, 1,
        -1, 0, 1,
        -1, 0, 1
    };
    Mat maskX(3, 3, CV_32F, prewittX);
    Mat maskY(3, 3, CV_32F, prewittY);

    filter(src, dstX, maskX);
    filter(src, dstY, maskY);
    myMagnitude(dstX, dstY, dst);

    //filter2D(src, dstX, CV_32F, maskX, Point(-1, -1), 3, 1);
    //filter2D(src, dstY, CV_32F, maskY, Point(-1, -1), 3, 1);
    //magnitude(dstX, dstY, dst);

    dst.convertTo(dst, CV_8U);
}
// 사각형 내부의 점 포함 여부
bool IsContain(Rect rc, vector<Point>& cirCenters) {
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
void Drawing(Mat& src, Mat& dst, vector<Point>& vCirCenters, vector<Point>& hCirCenters)
{
    if (!src.data)
        return;
    if (!dst.data)
        cvtColor(src, dst, COLOR_GRAY2BGR);
    Vec2f vEquation, hEquation;
    Point target, temp1, temp2;
    vEquation = LSM_Vertical(vCirCenters);
    hEquation = LSM_Horizontal(hCirCenters);

    target.x = cvRound((hEquation[1] - vEquation[1]) / (vEquation[0] - hEquation[0]));
    target.y = cvRound((vEquation[0] * (hEquation[1] - vEquation[1])) / (vEquation[0] - hEquation[0]) + vEquation[1]);

    // 주황색 직선의 방정식 그리기
    temp1 = { cvRound(-vEquation[1] / vEquation[0]), 0 }; // y=0 이고, 최소제곱법 직선의 방정식을 지나는 점
    //temp1 = { 0, cvRound(vEquation[1]) }; // y=0 이고, 최소제곱법 직선의 방정식을 지나는 점
    temp2 = { cvRound((target.y + 20 - vEquation[1]) / vEquation[0]), target.y + 20 }; // target에서 좀 더 아래쪽의 점
    cout << vEquation[0] << " " << vEquation[1] << endl;
    line(dst, temp1, temp2, Scalar(0, 127, 255));
    temp1 = { dst.cols, cvRound(hEquation[0] * dst.cols + hEquation[1]) }; // x = src.cols 이고, 최소제곱법 직선의 방정식을 지나는 점
    temp2 = { target.x - 20, cvRound(hEquation[0] * (target.x - 20) + hEquation[1]) }; // target에서 좀 더 왼쪽의 점
    line(dst, temp1, temp2, Scalar(0, 127, 255));

    // 라임색 X 그리기
    temp1 = { target.x - 7, target.y - 7 };
    temp2 = { target.x + 7,target.y + 7 };
    line(dst, temp1, temp2, Scalar(0, 255, 191));
    temp1 = { target.x - 7, target.y + 7 };
    temp2 = { target.x + 7,target.y - 7 };
    line(dst, temp1, temp2, Scalar(0, 255, 191));

    //cout << "교점의 좌표 = " << target << endl;

    for (int i = 0; i < vCirCenters.size(); i++)
    {
        Point center(vCirCenters[i].x, vCirCenters[i].y);
        circle(dst, center, 5, Scalar(0, 0, 255), 1, -1);
    }

    for (int i = 0; i < hCirCenters.size(); i++)
    {
        Point center(hCirCenters[i].x, hCirCenters[i].y);
        circle(dst, center, 5, Scalar(0, 0, 255), 1, -1);
    }
}
// 최소제곱법 x, y좌표 스위칭 후 계산한 뒤 나온 식을 다시 y=x 대칭이동
Vec2f LSM_Vertical(vector<Point>& pts)
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
Vec2f LSM_Horizontal(vector<Point>& pts)
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