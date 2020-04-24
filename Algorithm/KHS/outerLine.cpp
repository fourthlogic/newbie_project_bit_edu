#include "opencv2/opencv.hpp" 
#include <iostream>  
#include <time.h>  

using namespace cv;
using namespace std;

void binaryThreshold(Mat& src, Mat& dst, int MinValue , int MaxValue);
void outerLine(Mat& src, Mat& dst);
void onEdgetrackBar(int value, void* userData);

Mat img;
int maxValue = 150;
int minValue = 255;


// 바이너리 전용 threshold 함수 
void binaryThreshold(Mat& src, Mat& dst, int MinValue =0 , int MaxValue=255 )
{
	dst = Mat(src.rows, src.cols, CV_8UC1);
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

// 필터 이용 외곽선 검출 함수 제작중
void outerLine(Mat& src, Mat& dst)
{
	float data[] = {
		 0, 0,
		-1, 0,
		-1, 1,
		 0, 1
	};
	Mat mask(2, 2, CV_32F, data);

	Point A, B;
	uchar a, b;
	for (int i = 1; i < src.rows - 1; i++)
	{
		uchar* ptr = src.ptr<uchar>(i);
		A.y = i - 1;
		B.y = i - 1;
		for (int j = 1; j < src.cols - 1; j++)
		{
			if (*src.ptr<uchar>(i, j) == 0)
			{
				A.x = j;
				B.x = j + 1;
				a = *src.ptr<uchar>(A.y, A.x);
				b = *src.ptr<uchar>(B.y, B.x);
				if (a == 0 && b == 0);
				//x;
				else if (a == 0 && b == 1);
				//x;
				else if (a == 1 && b == 0);
				//x;
				else;
					//x;

			}
		}

	}
}

// 트랙바 콜백함수
void onEdgetrackBar(int value, void* userData)
{
	clock_t start;
	clock_t end;
	Mat dst = img.clone();
	// 1. OpenCV제공 Canny 검출 속도측정
	//start = clock();
	//Canny(dst, dst, th1, th2);
	//end = clock();
	//cout << "Canny = " << (double)(end - start) << endl;
	//imshow("Canny", dst);

	// 2. OpenCV제공 Laplacian 검출 속도측정
	start = clock();
	//binaryThreshold(dst, dst, maxValue, minValue);
	Laplacian(img, dst, CV_8U, 3);// == mask8
	convertScaleAbs(dst, dst);
	end = clock();
	cout << "Laplacian = " << (double)(end - start) << endl;
	imshow("Laplacian ", dst);

	//cout << "Canny = " << (double)(end - start) / CLOCKS_PER_SEC << endl;
}


void main()
{
	img = imread("images/20200115220714674 CAM_3 ULC.png", IMREAD_GRAYSCALE);
	namedWindow("threshold", WINDOW_AUTOSIZE);
	createTrackbar("threshold1", "threshold", &maxValue, 255, onEdgetrackBar);
	createTrackbar("threshold2", "threshold", &minValue, 255, onEdgetrackBar);
	imshow("threshold", img);
	waitKey();

}
