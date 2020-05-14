#include "main.h"

// findContour 제작기 - 8방향
void MyContours(Mat& src, vector<vector<Point>>& Points);
void Contour(Mat& src, int sx, int sy, vector<Point>& outPoints, uchar* SrcPtr, bool* check);

// 라벨링 처리 함수 - 4방향
void MyLabeling(Mat& src, int* LabelMap);
void CalScale(int* Map, int max[2], int& LabelCount);
void Serching(int* Map, int max[2], int& NumCount);
void glassFire(int mask_Y, int mask_X, int* Map, int max[2], int& count);


void main()
{
    time_t start;
    time_t end;
    Mat src = imread("image/a1.png", IMREAD_GRAYSCALE);

    start = clock();

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
    threshold(src, dst, 100, 255, THRESH_BINARY_INV);

    MyContours(newSrc, Points);

    //imshow("dst", dst);
    //imshow("newSrc", newSrc);
    waitKey();

    GetCornerPoints(src, cornerpts); // 3점 좌표 추출
    GetVertexPoints(src, cornerpts, vertexPts, 20);
    ContourDetection(src, vCirCenters, hCirCenters, vertexPts, 2, 4, 100);

    end = clock();
    cout << end - start << endl;
    waitKey();
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
    //MyContours(grayImage, contours);

    findContours(grayImage, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

    vector<Point2f> approx;
    vector<Point2f> corners;

    for (size_t i = 0; i < contours.size(); i++)
    {
        approxPolyDP(Mat(contours[i]), approx, arcLength(Mat(contours[i]), true) * 0.07, true);
        if (fabs(contourArea(Mat(approx))) > 10000)  //면적이 일정크기 이상이어야 한다. 
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

    vector<vector<Point>> contours;
    findContours(imgThreshold, contours, RETR_LIST, CHAIN_APPROX_SIMPLE);

}

// contour 제작기
void MyContours(Mat& src, vector<vector<Point>>& Points)
{
    int height = src.rows;
    int width = src.cols;
    int cnt = 0;
    uchar* ptr;
    bool* Check = new bool[height * width];
    uchar* SrcPtr = new uchar[height * width];
    int* LabelMap = new int[height * width];
    memset(Check, true, sizeof(bool) * height * width);
    memset(LabelMap, 0, sizeof(int) * height * width);

    for (int i = 0; i < height; i++)
    {
        ptr = src.ptr<uchar>(i);
        for (int j = 0; j < width; j++)
        {
            SrcPtr[i * width + j] = ptr[j];
            LabelMap[i * width + j] = (ptr[j] > 0) ? 1 : 0;
        }
    }

    MyLabeling(src, LabelMap);

    for (int i = 0; i < height; i++)
    {

        for (int j = 0; j < width; j++)
        {
            cout << LabelMap[i * width + j] << " ";
        }
        cout << endl;
    }

    waitKey();
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            if (Check[width * y + x] && SrcPtr[y * width + x] == 255)
            {
                Mat dst;
                vector<Point> pts;
                Contour(src, x, y, pts, SrcPtr, Check);
                if (pts.size() != 0)
                    Points.push_back(pts);
            }
            else
                Check[width * y + x] = false;

        }
    }

    /*  for (vector<Point> pts : Points)
      {
          cout << cnt << "번째 " << pts.size() << "개수" << endl;
          for (Point pt : pts)
          {
              cout << pt;
          }
          cout << endl;
      }*/

    delete SrcPtr;
    delete Check;
    delete Check;

}

void Contour(Mat& src, int sx, int sy, vector<Point>& outPoints, uchar* SrcPtr, bool* check)
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

        //if (nx < 0 || nx >= w || ny < 0 || ny >= h || pSrc[ny * w + nx] == 0)
        //if (nx < 0 || nx >= w || ny < 0 || ny >= h || src.ptr<uchar>(ny)[nx] == 0)
        if (nx < 0 || nx >= w || ny < 0 || ny >= h || SrcPtr[ny * w + nx] == 255)
        {
            if (++d > 7) d = 0;
            cnt++;

            if (cnt >= 8)
            {
                outPoints.push_back(Point(x, y));
                check[y * w + x] = false;
                break;
            }
        }
        else
        {
            outPoints.push_back(Point(x, y));
            check[y * w + x] = false;
            x = nx;
            y = ny;

            cnt = 0;
            d = (d + 6) % 8;	// d = d - 2 와 같은 형태
        }

        if (x == sx && y == sy && d == 0)
            break;
    }
}

// 라벨링 부분
void MyLabeling(Mat& src, int* Map)
{
    int max[2] = { src.rows,src.cols };
    int Count = 2;
    Serching(Map, max, Count);
    CalScale(Map, max, Count);
}
// 재귀 4방향
void glassFire(int Y, int X, int* Map, int max[2], int& Count)
{
    if (Map[Y * max[1] + X] == 1) {

        Map[Y * max[1] + X] = Count;
    }
    else return;

    if (X + 1 < max[1]) glassFire(Y, X + 1, Map, max, Count);
    if (Y + 1 < max[0]) glassFire(Y + 1, X, Map, max, Count);
    if (X - 1 >= 0)glassFire(Y, X - 1, Map, max, Count);
    if (Y - 1 <= 0)glassFire(Y - 1, X, Map, max, Count);
}
// 넘버링 
void Serching(int* Map, int max[2], int& Count)
{

    for (int y = 0; y < max[0]; y++)
    {
        for (int x = 0; x < max[1]; x++)
        {
            //씨드
            if (Map[y * max[1] + x] == 1)
            {
                Count++;
                glassFire(y, x, Map, max, Count);
            }
        }
    }
}
// 넓이 값으로 넘버링 변경
void CalScale(int* Map, int max[2], int& Count)
{
    int Area = 0;

    for (int n = 2; n <= Count; n++)
    {
        //counting
        for (int y = 0; y < max[0]; y++)
        {
            for (int x = 0; x < max[1]; x++)
            {
                if (Map[y * max[1] + x] == n) Area++;
            }
        }

        //inserting
        for (int y = 0; y < max[0]; y++)
        {
            for (int x = 0; x < max[1]; x++)
            {
                if (Map[y * max[1] + x] == n) Map[y * max[1] + x] = Area;

            }
        }
        Area = 0;
    }
}
