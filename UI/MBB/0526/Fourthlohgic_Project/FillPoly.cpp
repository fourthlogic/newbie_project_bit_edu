#include "pch.h"

#define PI 3.14
void DouglasPeucker(const vector<Point>& polyLine, vector<Point2f>& simplifiedPolyLine, double epsilon, bool flag = true);
Rect boundRect(vector<Point> pts) {
    int maxX = 0, maxY = 0;
    int minX = MAXINT32,minY=MAXINT32;
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
    return Rect(minX, minY, maxX - minX+1, maxY - minY+1);
}

void fillPoly_(Size matSize, Mat& dst, vector<Point> pts) {
    Rect rc = boundRect(pts);
    dst = Mat::zeros(matSize, CV_8UC1);
#pragma region 선 그리기
    int size = pts.size();
    for (int i = 0; i < size-1; i++) {
        line(dst, pts[i], pts[i + 1], Scalar(255, 255, 255));
    }
    line(dst, pts[0], pts[size-1], Scalar(255, 255, 255));
#pragma endregion
    waitKey();
    Point pt = rc.tl();
    for (int i = pt.y; i <rc.height+pt.y; i++) {
        bool isVertex = false;
        vector<int> idx;
        uchar* ptr = dst.ptr<uchar>(i);
        
        for (int j = pt.x; j < pt.x + rc.width; j++) {
            if (ptr[j - 1] == 0 && ptr[j] == 255) {
                idx.push_back(j);
           }
        }
        if (idx.size() % 2 != 0) {
            for (int k = 0; k < idx.size() - 1;k++) {
                for (Point pt : pts) {
                    if (i == pt.y && idx[k] == pt.x) {
                        idx.erase(idx.begin()+k);
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


// 3점 좌표 추출 수정판
void GetCornerPoints(Mat& src, vector<Point>& cornerPts)
{
    Mat grayImage = src.clone();

    threshold(grayImage, grayImage, 110, 255, THRESH_TOZERO); // min_grayscale이 안되면 0
    threshold(grayImage, grayImage, 158, 255, THRESH_TOZERO_INV); // min_grayscale이 넘어도 0

    Matx <uchar, 3, 3> mask(0, 1, 0, 1, 1, 1, 0, 1, 0);
    morphologyEx(grayImage, grayImage, MORPH_OPEN, mask); // 외곽의 솔트를 제거하기 위해

    vector<vector<Point>> contours;
    findContours(grayImage, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
    //MyContours(grayImage, contours);
    vector<Point2f> approx;

    for (size_t i = 0; i < contours.size(); i++)
    {
        //approxPolyDP(Mat(contours[i]), approx, arcLength(Mat(contours[i]), true) * 0.07, true);
        if (contourArea(Mat(contours[i])) > 10000)  //면적이 일정크기 이상이어야 한다. 
        {
           /* for (Point pt : contours[i]) {
                circle(src, pt, 1, Scalar(255, 0, 0));
                imshow("dd", src);
                waitKey();
            }*/
            DouglasPeucker(contours[i], approx, arcLength(Mat(contours[i]), true) * 0.07);

         
          /*  int size = approx.size();

            for (int k = 0; k < size; k++)
            {
                corners.push_back(approx[k]);
            }*/
        }
    }
   /* for (Point pt : corners) {
        circle(src, pt, 1, Scalar(255, 0, 0));
        imshow("src", src);
        waitKey();
    }*/
    //0(top) 부터 반시계 방향(꼭짓점) 
    double distance0to1 = sqrt(pow(approx[0].x - approx[1].x, 2) + pow(approx[0].y - approx[1].y, 2));
    double distance0to3 = sqrt(pow(approx[0].x - approx[3].x, 2) + pow(approx[0].y - approx[3].y, 2));
    if (distance0to1 < distance0to3)
    {
        for (int i = 1; i < approx.size(); i++)
            cornerPts.push_back(approx[i]);
    }
    else
    {
        for (int i = 0; i < approx.size() - 1; i++)
            cornerPts.push_back(approx[i]);
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
        vertexPts[4] = Point(cornerPts[1].x + distance, cornerPts[1].y);
        vertexPts[5] = Point(cornerPts[0].x + distance, cornerPts[0].y);
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
    Mat mask;
    //mask= Mat::zeros(src.size(), CV_8UC1);
    fillPoly_(src.size(), mask, vertexPts);
    //fillPoly(mask, vpts, Scalar(255, 255, 255), 8, 0);
    
    Mat ROI;
    bitwise_and(src, mask, ROI);


    //findContours전용 Mat
    Mat imgThreshold = ROI.clone();
    threshold(imgThreshold, imgThreshold, value, 250, THRESH_BINARY_INV);

    vector<vector<Point>> contours;
    findContours(imgThreshold, contours, RETR_LIST, CHAIN_APPROX_SIMPLE);
    //imshow("dd", ROI);
    waitKey();
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
            //Rect rc = boundingRect(pts);
            Rect rc = boundRect(pts);
#pragma region 수정 요함
            if (rc.tl().x - 3 > 0 && rc.tl().y - 3 > 0) {
                rc -= Point(3, 3);
            }
            if (rc.tl().x + rc.width + 4 < src.cols && rc.tl().y + rc.height + 4 < src.rows) {
                rc += Size(4, 4);
            }
            else {
                rc.width = src.cols - rc.tl().x;
                rc.height += 4;
            }
#pragma endregion

            if (IsContain(rc, cirCenters))
                continue;

            Mat cirRect = src(rc);
            Differential(cirRect, cirRect);

            int** vote = (int**)calloc(cirRect.rows, sizeof(int*));
            for (int i = 0; i < cirRect.rows; i++) {
                vote[i] = (int*)calloc(cirRect.cols, sizeof(int));
            }
            for (int i = 0; i < cirRect.cols; i++) {
                uchar* ptr = src.ptr<uchar>(i);
                for (int j = 0; j < cirRect.rows; j++) {
                    if (ptr[j] >= 100)
                        continue;
                    for (int t = 0; t <= 360; t += 1)
                    {
                        int a = i - 3 * cvRound(cos(t * PI / 180));
                        int b = j - 3 * cvRound(sin(t * PI / 180));
                        if (a >= 0 && a < cirRect.cols && b >= 0 && b < cirRect.rows) {
                            vote[b][a] += 1;
                        }
                    }
                }
            }

            int max = -1;
            vector<Point> pt;
            int count_i;
            int max_j = -1;
            for (int i = 0; i < cirRect.cols; i++) {
                int count_j = 0;
                for (int j = 0; j < cirRect.rows; j++) {
                    if (count_j > max_j)
                        max_j = count_j;
                    if (vote[j][i] >= 360) {
                        pt.push_back(Point(i, j));
                        count_j++;
                    }
                }
            }
            int size = pt.size();
            if (size == 0) {
                continue;
            }
            int quotient = size / max_j;
            count_i = (size % max_j == 0) ? quotient : quotient + 1;
            int n = (count_i / 2) * max_j + max_j / 2;
            for (int i = 0; i < cirRect.rows; i++)
                free(vote[i]);
            free(vote);
                cirCenters.push_back(Point(pt[n].x + rc.tl().x, pt[n].y + rc.tl().y));
        }
    }
}
// 미분 함수(prewitt Edge)
void Differential(Mat& src, Mat& dst) {
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


double PerpendicularDistance(const Point& pt, const Point& lineStart, const Point& lineEnd,bool flag)
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

void DouglasPeucker(const vector<Point>& polyLine, vector<Point2f>& simplifiedPolyLine, double epsilon, bool flag)
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
            double d = PerpendicularDistance(polyLine[i], polyLine[0], polyLine[end],flag);
            if (d > maxDistance)
            {
                index = i;
                maxDistance = d;
            }
        }
        int firstMax = 0;
        for (int i = 0; i < end; i++)
        {
            double d = PerpendicularDistance(polyLine[i], polyLine[index], polyLine[end],flag);
            if (d > firstMax)
            {
                first = i;
                firstMax = d;
            }
        }
        flag = false;
    }
    else {
        for (int i = 1; i < end; i++)
        {
            double d = PerpendicularDistance(polyLine[i], polyLine[0], polyLine[end],flag);
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
        vector<Point2f> recResults1;
        vector<Point2f> recResults2;
        vector<Point> firstLine(polyLine.begin() + first, polyLine.begin() + index + 1);
        
        vector<Point> lastLine(polyLine.begin() + index, polyLine.end());
        
        DouglasPeucker(firstLine, recResults1, epsilon, flag);
        DouglasPeucker(lastLine, recResults2, epsilon, flag);
        
        // Build the result list
        simplifiedPolyLine.assign(recResults1.begin(), recResults1.end() - 1);
        simplifiedPolyLine.insert(simplifiedPolyLine.end(), recResults2.begin(), recResults2.end());

    }
    else
    {
        if (flag) {
            simplifiedPolyLine.push_back(polyLine[index]);
            simplifiedPolyLine.push_back(polyLine[end]);
        }
        else {

        //Just return start and end points
        simplifiedPolyLine.push_back(polyLine[0]);
        simplifiedPolyLine.push_back(polyLine[end]);
        }
    }
}
