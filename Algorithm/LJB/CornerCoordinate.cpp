#include "opencv2/opencv.hpp"
#include <iostream>
#include <cmath>

using namespace cv;
using namespace std;

int max_grayscale = 160;
int min_grayscale = 110;
double epsilon_control = 0.07;

// 최소제곱법. 1차방정식 도출 및 x값 받으면 y값 출력하는 함수
int LeastSquaredMethod(vector<Point>, int);
// src 이미지의 외곽 좌표 tl, bl, br를 구하는 함수 (dst에 순서대로 저장)
void CornerCoordinate(Mat, vector<Point>&);

int main()
{
	Mat img, img_gray;

	//이미지파일을 로드하여 img에 저장  
	img = imread("images/a8.png", IMREAD_COLOR);
	if (img.empty())
	{
		cout << "Could not open or find the image" << std::endl;
		return -1;
	}

	vector<Point> coordinates;
	CornerCoordinate(img, coordinates);


	return 0;
}

int LeastSquaredMethod(vector<Point> pts, int pts_x)
{
	int a, b;
	int xx = 0, x = 0, xy = 0, y = 0;
	int n = pts.size();
	for (int i = 0; i < n; i++)
	{
		xx += (pts[i].x) * (pts[i].x);
		x += (pts[i].x);
		xy += (pts[i].x) * (pts[i].y);
		y += (pts[i].y);
	}
	a = (n * xy - x * y) / (n * xx - x * x);
	b = (xx * y - x * xy) / (n * xx - x * x);
	cout << "y=" << a << "x+" << b << endl; // y=ax+b
	return a * pts_x + b;
}

void CornerCoordinate(Mat src, vector<Point>& dst)
{
	Mat img_gray;
	cvtColor(src, img_gray, COLOR_BGR2GRAY);
	threshold(img_gray, img_gray, min_grayscale, 255, THRESH_TOZERO); // min_grayscale이 안되면 0
	threshold(img_gray, img_gray, max_grayscale, 255, THRESH_TOZERO_INV); // min_grayscale이 넘어도 0
	Matx <uchar, 3, 3> mask(0, 1, 0, 1, 1, 1, 0, 1, 0);
	morphologyEx(img_gray, img_gray, MORPH_OPEN, mask); // 외곽의 솔트를 제거하기 위해

	//contour를 찾는다.
	//findContours는 소스 이미지를 훼손시키므로 주의
	vector<vector<Point>> contours;
	findContours(img_gray, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

	vector<Point2f> approx;
	vector<Point2f> corners;

	for (size_t i = 0; i < contours.size(); i++)
	{
		approxPolyDP(Mat(contours[i]), approx, arcLength(Mat(contours[i]), true) * epsilon_control, true);
		if (fabs(contourArea(Mat(approx))) > 10000)  //면적이 일정크기 이상이어야 한다. 
		{
			int size = approx.size();

			for (int k = 0; k < size; k++)
			{
				corners.push_back(approx[k]);
			}
		}
	}

	double distance0to1 = sqrt(pow(corners[0].x - corners[1].x, 2) + pow(corners[0].y - corners[1].y, 2));
	double distance0to3 = sqrt(pow(corners[0].x - corners[3].x, 2) + pow(corners[0].y - corners[3].y, 2));
	if (distance0to1 < distance0to3)
	{
		for (int i = 1; i < corners.size(); i++)
			dst.push_back(corners[i]);
	}
	else
	{
		for (int i = 0; i < corners.size() - 1; i++)
			dst.push_back(corners[i]);
	}

	cout << "tl = " << dst[0] << endl;
	cout << "bl = " << dst[1] << endl;
	cout << "br = " << dst[2] << endl;
}