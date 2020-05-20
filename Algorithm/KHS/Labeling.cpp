#include "opencv2/opencv.hpp" 
#include <iostream>  
#include <map>

using namespace std;
using namespace cv;


// 라벨링(start Point 추출)
void Labeling(Mat& src, int* Map, vector<Vec3i>& startPoint);
// 범위용 Threshold
void RangeThreshold(Mat& src, Mat& dst, int MinValue = 0, int MaxValue = 255, int inValue = 0, int outValue = 255);


// 테스트용 데이터
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

void main()
{
    Mat src = imread("image/a1.png", IMREAD_GRAYSCALE);
    Mat dst = src.clone();
    Mat testData(15, 15, CV_8UC1, Data);
    if (!src.data)
        return;

    vector<Vec3i> startPoint;
    int* Map = new int[src.cols * src.rows];
    memset(Map, 0, sizeof(int) * src.cols * src.rows);

    RangeThreshold(dst, dst, 100, 170, 255, 0);
    Labeling(dst, Map, startPoint);
    cout << "==================== src Points====================" << endl;
    for (int i = 0; i < startPoint.size(); i++)
       cout << startPoint[i] << endl;


    vector<Vec3i> startPoint2;
    int* Map2 = new int[15 * 15];
    memset(Map2, 0, sizeof(int) * 15 * 15);
    Labeling(testData, Map2, startPoint2);
    cout << "==================== testData Points====================" << endl;
    for (int i = 0; i < startPoint2.size(); i++)
        cout << startPoint2[i] << endl;
    cout << "==================== Labeling Data ====================" << endl;
    for (int i = 0; i < 15; i++)
    {
        for (int j = 0; j < 15; j++)
        {
            cout.width(3);
            cout << Map2[i * 15 + j] << " ";
        }
        cout << endl;
    }
    waitKey();
}


// 라벨링(start Point 추출)
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

// 범위용 threshold
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
