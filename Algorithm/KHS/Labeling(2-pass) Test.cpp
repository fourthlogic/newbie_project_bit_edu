#include "main.h"
#include <map>

// 라벨링  + 외곽선 
void TwoPassLabeling(Mat& src, int* Map, int& Count);
void TwoPass(Mat& src, int* Map, int& Count);
void Contour(Mat& src, int sx, int sy, vector<Point>& outPoints, int* map, int labelNum);
void MyContours(Mat& src, vector<vector<Point>>& Points);

void main()
{
    time_t start;
    time_t end;
    Mat src = imread("image/ah.png", IMREAD_GRAYSCALE);
    if (!src.data)
        return;

    vector<Point> cornerpts; // 외곽 3점 좌표 값

    vector<Point> vCirCenters; // Vertical의 중심 좌표들의 값 
    vector<Point> hCirCenters; // Horizontal의 중심 좌표들의 값 

    vector<Point> vertexPts;   //
    vector<vector<Point>> Points;

    uchar Data[] = {
          0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
          0,  0,  0,  0,  0,  0,255,255,255,255,  0,  0,  0,  0,  0,
          0,255,255,255,  0,  0,255,  0,  0,255,  0,  0,  0,  0,  0,
          0,255,  0,255,  0,  0,255,  0,  0,255,  0,  0,  0,  0,  0,
          0,255,  0,255,  0,  0,255,  0,  0,255,  0,  0,  0,  0,  0,
          0,255,255,255,  0,  0,255,  0,  0,255,  0,  0,  0,  0,  0,
          0,  0,  0,  0,  0,  0,255,  0,  0,255,  0,  0,  0,  0,  0,
          0,  0,  0,  0,  0,  0,255,255,255,255,  0,  0,  0,  0,  0,
          0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
          0,  0,100,100,100,100,100,100,  0,  0,  0,  0,  0,  0,  0,
          0,  0,100,  0,  0,  0,  0,100,  0,  0,255,255,  0,  0,  0,
          0,  0,100,  0,  0,  0,  0,100,  0,  0,255,255,  0,  0,  0,
          0,  0,100,  0,  0,  0,  0,100,  0,  0,255,255,  0,  0,  0,
          0,  0,100,100,100,100,100,100,  0,  0,  0,  0,  0,  0,  0,
          0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
    };
    Mat dst;
    Mat newSrc(15, 15, CV_8UC1, Data);
    imshow("src", src);
    waitKey();
    start = clock();
    GetCornerPoints(src, cornerpts); // 3점 좌표 추출
    GetVertexPoints(src, cornerpts, vertexPts, 20);
    ContourDetection(src, vCirCenters, hCirCenters, vertexPts, 2, 4, 100);
    Drawing(src, dst, vCirCenters, hCirCenters);


    end = clock();
    imshow("result", dst);

    cout << end - start << endl;
    waitKey();
}


// 3점 좌표 추출 수정판
void GetCornerPoints(Mat& src, vector<Point>& cornerPts)
{
    Mat grayImage = src.clone();

    threshold(grayImage, grayImage, 100, 255, THRESH_TOZERO); // min_grayscale이 안되면 0
    threshold(grayImage, grayImage, 170, 255, THRESH_TOZERO_INV); // min_grayscale이 넘어도 0

    Matx <uchar, 3, 3> mask(0, 1, 0, 1, 1, 1, 0, 1, 0);
    morphologyEx(grayImage, grayImage, MORPH_OPEN, mask); // 외곽의 솔트를 제거하기 위해
    //imshow("grayImage", grayImage);
    //waitKey();
    vector<vector<Point>> contours;
    time_t  start = clock();
    MyContours(grayImage, contours);
    time_t  end = clock();
    cout << end - start << endl;
    int i = 0;
    while (1)
    {
        drawContours(grayImage, contours, i++, Scalar(255));
        imshow("grayImage", grayImage);
        waitKey();
        if (i == contours.size())
            break;
    }

    //findContours(grayImage, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

    vector<Point2f> approx;
    vector<Point2f> corners;

    for (size_t i = 0; i < contours.size(); i++)
    {
        //RamerDouglasPeucker(contours[i],0.7, approx);
        approxPolyDP(Mat(contours[i]), approx, arcLength(Mat(contours[i]), true) * 0.1, true);
        if (fabs(contourArea(Mat(approx))) > 5000)  //면적이 일정크기 이상이어야 한다. 
        {

            int size = approx.size();

            for (int k = 0; k < size; k++)
            {
                corners.push_back(approx[k]);
            }
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



    for (int i = 0; i < cornerPts.size(); i++)
    {
        circle(grayImage, cornerPts[i], 4, Scalar(255));
        cout << cornerPts[i] << endl;
    }
    //imshow("grayImage", grayImage);
    //waitKey();

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
    Mat ROI;
    bitwise_and(src, mask, ROI);

    //findContours전용 Mat
    Mat imgThreshold = ROI.clone();
    threshold(imgThreshold, imgThreshold, value, 250, THRESH_BINARY_INV);

    //imshow("imgThreshold", imgThreshold);


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


    //long cnt = 0;

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

            //putText(src, to_string(cnt), rc.tl(), 1, 1, Scalar(255));
            //imshow("cirRect", cirRect);
            //imshow("src", src);
            //cnt++;
            //waitKey();

            vector<Vec3f> houghCircles;
            HoughCircles(cirRect, houghCircles, HOUGH_GRADIENT, 1, 6, 255, 5, radMin, radMax);
            if (houghCircles.size() != 0) {
                cirCenters.push_back(Point(houghCircles[0][0] + rc.tl().x, houghCircles[0][1] + rc.tl().y));
            }
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
    vEquation = LSM_Horizontal(vCirCenters);
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

// contour 제작기
void MyContours(Mat& src, vector<vector<Point>>& Points)
{
    int height = src.rows;
    int width = src.cols;
    int count = 1;
    int cnt = 1;
    int* Map = new int[height * width];
    memset(Map, 0, sizeof(int) * height * width);
    TwoPassLabeling(src, Map, count);
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            if (Map[y * width + x] == cnt)
            {
                vector<Point> pts;
                Contour(src, x, y, pts, Map, cnt);
                if (pts.size() != 0)
                    Points.push_back(pts);
                cnt++;
            }
        }
    }
    delete Map;
}

void Contour(Mat& src, int sx, int sy, vector<Point>& outPoints, int* map, int labelNum)
{
    int h = src.rows;
    int w = src.cols;

    int x, y, nx, ny;
    int d, cnt;

    int  dir[8][2] = {
    {  0,  1 },
    {  1,  1 },
    {  1,  0 },
    {  1, -1 },
    {  0, -1 },
    { -1, -1 },
    { -1,  0 },
    { -1,  1 }
    };

    x = sx;
    y = sy;
    d = cnt = 0;

    while (1)
    {
        nx = x + dir[d][0];
        ny = y + dir[d][1];

        if (nx < 0 || nx >= w || ny < 0 || ny >= h || map[ny * w + nx] != labelNum)
        {
            if (++d > 7) d = 0;
            cnt++;

            if (cnt >= 8)
            {
                outPoints.push_back(Point(x, y));
                //check[y * w + x] = false;
                break;
            }
        }
        else
        {
            outPoints.push_back(Point(x, y));
            //check[y * w + x] = false;
            x = nx;
            y = ny;

            cnt = 0;
            d = (d + 6) % 8;	// d = d - 2 와 같은 형태
        }

        if (x == sx && y == sy && d == 0)
            break;
    }
}


// 라벨링 + 
void TwoPassLabeling(Mat& src, int* Map, int& Count)
{
    int height = src.rows;
    int width = src.cols;
    int NextData;
    int minNum;
    int nx, ny;
    uchar* ptr;
    map<int, int> table;
    int dir[4][2] = {
        { -1,  0 },
        {  0, -1 },
        {  1,  0 },
        {  0,  1} 
    };
    table[1]=1;
    for (int y = 0; y < height; y++)
    {
        ptr = src.ptr<uchar>(y);
        for (int x = 0; x < width; x++)
        {
            if (ptr[x] != 0) // 원본 이미지 
            {
                minNum = Count;
                for (int i = 0; i < 4; i++)
                {
                    nx = x + dir[i][0];
                    ny = y + dir[i][1];
                    if (nx < 0 || nx >= width || ny < 0 || ny >= height)
                        continue;
                    else
                    {
                        NextData = Map[ny * width + nx];
                        if (NextData != 0)
                        {
                            if (minNum > NextData) {
                                while (1)
                                {
                                    if (table[NextData] != NextData)
                                        NextData = table[NextData];
                                    else
                                        break;
                                }
                                table[minNum] = NextData;
                                minNum = NextData;
                            }
                        }
                    }
                }
                Map[y * width + x] = minNum;
                if (Count == minNum) {
                    table[Count] = Count++;
                    //Count++;
                }
            }
        }
    }

}

// 라벨링 부분
void TwoPass(Mat& src, int* Map)
{
    int height = src.rows;
    int width = src.cols;
    int minNum;
    int nx, ny;

    int dir[3][2] = {
    {  1,  0 },
    { -1,  0 },
    {  0, -1 },
    };

    for (int y = 1; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            if (Map[y * width + x] != 0)  
            {
                minNum = Map[y * width + x]; // 내부값
                for (int i = 0; i < 4; i++)
                {
                    nx = x + dir[i][0];
                    ny = y + dir[i][1];
                    if (nx < 0 || nx >= width || ny < 0 || ny >= height)
                        continue;
                    else
                    {
                        if (Map[ny * width + nx] != minNum)
                        {
                            if (minNum > Map[ny * width + nx])
                                minNum = Map[ny * width + nx];
                        }
                    }

                }
                Map[y * width + x] = minNum;
            }
            else
                continue;
        }
    }
}











