#include "opencv2/opencv.hpp" 
#include <iostream>  
#include <time.h>  

using namespace cv;
using namespace std;

void binaryThreshold(Mat& src, Mat& dst, int MinValue, int MaxValue);
void outerLine(Mat& src, Mat& dst, vector<Point>& cp, int sx, int sy );
void onEdgetrackBar(int value, void* userData);
bool imageDataStart(uchar*& pSrc, int& height, int& width, Point& startPt, Point& firstPt, int value);

Mat img; // 원본소스
int maxValue = 255;
int minValue = 40;

// 시간 측정용
clock_t start;
clock_t end;

// 범위 max <> min threshold 함수 - 제외
void rangeThreshold(Mat& src, Mat& dst, int MinValue = 0, int MaxValue = 255)
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
				dstptr[j] = 255;
			else
				dstptr[j] = 0;
		}
	}
}

// 바이너리 전용  threshold 함수 사이값 범위 max ~ min 
void binaryThreshold(Mat& src, Mat& dst, int MinValue = 0, int MaxValue = 255)
{
	if (!dst.data)
		dst = Mat(src.size(), CV_8UC1, Scalar(0));

	for (int i = 0; i < src.rows; i++)
	{
		uchar* srcPtr = src.ptr<uchar>(i);
		uchar* dstptr = dst.ptr<uchar>(i);
		for (int j = 0; j < src.cols; j++)
		{
			if (srcPtr[j] < MaxValue && srcPtr[j] > MinValue)
				dstptr[j] = 255;
			else
				dstptr[j] = 0;
		}
	}
}

// 해당 value값을 찾는 함수 
bool imageDataStart(uchar* &pSrc, int& height, int& width, Point & startPt, Point & firstPt, int value)
{

	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			if (pSrc[y * width + x] == value)
			{
				firstPt.y = y;
				firstPt.x = x;
				return true;
			}
		}
	}
	return false;
}

// 필터 이용 외곽선 검출 함수 4방향 검출
void outerLine(Mat& src, Mat& dst, vector<Point>& cp, int sx = 0, int sy = 0)
{
	if (!dst.data)
		dst = Mat(src.size(), CV_8UC1, Scalar(0));

	int w = src.cols;
	int h = src.rows;

	uchar* pSrc = new uchar[w * h];

	for (int i = 0; i < h; i++)
	{
		for (int j = 0; j < w; j++)
		{
			pSrc[w * i + j] = src.ptr(i)[j];
		}
	}

	int x, y, nx, ny;
	int d, cnt;

	int dir[4][2] = {
		{  1,  0 },
		{  0,  1 },
		{ -1,  0 },
		{  0, -1 },
	};


	if (pSrc[w*sy + sx]!=255)
	{
		Point strPt(0, 0);
		Point firstPt;
		if (!imageDataStart(pSrc, h, w, strPt, firstPt, 255))
			return;
		sx = firstPt.x;
		sy = firstPt.y;
	}

	x = sx;
	y = sy;

	d = cnt = 0;

	while (1)
	{
		nx = x + dir[d][0];
		ny = y + dir[d][1];

		if (nx < 0 || nx >= w || ny < 0 || ny >= h || pSrc[ny * w + nx] == 0)
		{
			if (++d > 3)
				d = 0;
			cnt++;

			if (cnt >= 4) 
			{
				cp.push_back(Point(x, y));
				break;
			}
		}
		else
		{
			cp.push_back(Point(x, y));

			x = nx;
			y = ny;
			cnt = 0;

			d = (d + 3) % 4;
		}

		if (x == sx && y == sy && d == 0)
			break;
	}

	for (int i = 0; i < cp.size(); i++)
		dst.ptr<uchar>(cp[i].y)[cp[i].x] = 255;

	//imshow("outline", dst);
}

// 트랙바 콜백함수
void onEdgetrackBar(int value, void* userData)
{

	Mat dst;// = img.clone();

	// 1. OpenCV제공 Canny 검출 속도측정
	//start = clock();
	//Canny(img, dst, minValue, maxValue);
	//end = clock();
	//cout << "Canny = " << (double)(end - start) << endl;
	//imshow("Canny", dst);

	// 2. OpenCV제공 Laplacian 검출 속도측정
	//start = clock();
	//binaryThreshold(img, dst, maxValue, minValue);
	//Laplacian(img, dst, CV_8U, 3);// == mask8
	//convertScaleAbs(dst, dst);
	//end = clock();
	//cout << "Laplacian = " << (double)(end - start) << endl;
	//imshow("Laplacian ", dst);

	// 3. Binary Threshold - 사이값 추출 
	//start = clock();
	//binaryThreshold(img, dst, minValue, maxValue);
	//imshow("binaryThreshold", dst);
	//end = clock();
	
	// 4. range Threshold  - 범위 제외
	//start = clock();
	//rangeThreshold(img, dst, minValue, maxValue);
	//imshow("rangeThreshold", dst);
	//end = clock();
}

// 메디안 필터
void MedianFilter(Mat img, Mat& dst, int size)
{
	dst = Mat(img.size(), CV_8U, Scalar(0));

	Size mSize(size, size);
	Point h_m = mSize / 2;

	for (int i = h_m.y; i < img.rows - h_m.y; i++)
	{

		for (int j = h_m.x; j < img.cols - h_m.x; j++)
		{
			Point start = Point(j, i) - h_m;
			Rect roi(start, mSize);
			Mat mask, sort_m;
			img(roi).copyTo(mask);

			Mat one_row = mask.reshape(1, 1);
			cv::sort(one_row, sort_m, SORT_EVERY_ROW); // 행 단위 정렬

			int medi_idx = (int)(one_row.total() / 2); // 중간위치
			dst.at<uchar>(i, j) = sort_m.at<uchar>(medi_idx);

		}

	}

}

void main()
{
	img = imread("images/20200115220714674 CAM_3 ULC.png", IMREAD_GRAYSCALE);
	namedWindow("srcImage", WINDOW_AUTOSIZE);
	Mat outerImage;
	Mat binaryImage;
	vector<Point> edgePoints;
	
	// 범위 지정
	//binaryThreshold(img, binaryImage,34,255);
	//imshow("binary", binaryImage);
	//waitKey();
	// 메디안
	//MedianFilter(binaryImage, binaryImage, 5);
	//imshow("Median", binaryImage);
	//waitKey();

	// 외곽선
	//outerLine(binaryImage, outerImage, edgePoints, 643 , 1);
	//imshow("outer", outerImage);
	//waitKey();
	
	// thrsold 트랙바
	//createTrackbar("MaxValue", "srcImage", &maxValue, 255, onEdgetrackBar);
	//createTrackbar("MinValue", "srcImage", &minValue, 255, onEdgetrackBar);


	// 원본소스 출력
	imshow("srcImage", img);
	waitKey();

}
