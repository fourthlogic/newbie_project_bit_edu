#include "opencv2/opencv.hpp" 
#include <map>


using namespace cv;
using namespace std;


// 와곽선들 검출
void MyContours(Mat& src, vector<vector<Point>>& Points);
// 라벨링
void Labeling(Mat& src, int* Map, vector<Vec3i>& startPoint);
// 외곽 검출
void Contour(Mat& src, int sx, int sy, vector<Point>& outPoints, int* map, int labelNum);
// 범위용 Threshold
void RangeThreshold(Mat& src, Mat& dst, int MinValue = 0, int MaxValue = 255, int inValue = 0, int outValue = 255);

void main()
{
    Mat src = imread("image/a1.png", IMREAD_GRAYSCALE);
    Mat dst = src.clone();
    Mat ContourImg;
    if (!src.data)
        return;
    //imshow("src", src);
    RangeThreshold(dst, dst, 100, 170, 255, 0);
    imshow("dst", dst);
    vector<vector<Point>> contours;
    MyContours(dst, contours);
    // 검출한 외곽선 Point 이용 데이터 출력
    for (int i = 0; i < contours.size(); i++)
    {
        ContourImg = src.clone();
        for (int j = 0; j < contours[i].size(); j++)
        {
            circle(ContourImg, contours[i][j], 1, Scalar(255));
        }
        imshow("ContourImg", ContourImg);
        waitKey(300);
    }
    waitKey();
}

// 외곽점 추출
void MyContours(Mat& src, vector<vector<Point>>& Points)
{
    map<int, int> table;
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
void Labeling(Mat& src, int* Map, vector<Vec3i>& startPoint)
{
    map<int, int> table;
    int height = src.rows;
    int width = src.cols;
    int mult_y, sub_y;
    uchar* ptr;
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
void Contour(Mat& src, int sx, int sy, vector<Point>& outPoints, int* Map, int value)
{
    int height = src.rows;
    int width = src.cols;

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

        if (nx < 0 || nx >= width || ny < 0 || ny >= height || Map[ny * width + nx] != value)
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

// 범위용 Threshold
void RangeThreshold(Mat& src, Mat& dst, int MinValue, int MaxValue, int inValue, int outValue)
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
                dstptr[j] = outValue;
            else
                dstptr[j] = inValue;
        }
    }
}
