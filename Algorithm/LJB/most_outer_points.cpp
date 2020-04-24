#include "opencv2/opencv.hpp"
#include <iostream>

using namespace cv;
using namespace std;

// 최소제곱법. 1차방정식 도출 및 x값 받으면 y값 출력하는 함수
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

int main()
{
	Mat img, img_gray;

	//이미지파일을 로드하여 img에 저장  
	img = imread("images/a10.png", IMREAD_COLOR);
	if (img.empty())
	{
		cout << "Could not open or find the image" << std::endl;
		return -1;
	}

	Mat test160;
	cvtColor(img, test160, COLOR_BGR2GRAY);
	threshold(test160, test160, 110, 255, THRESH_TOZERO); // 110이 안되면 0
	threshold(test160, test160, 160, 255, THRESH_TOZERO_INV); // 160이 넘어도 0
	imshow("110~160", test160);
	Matx <uchar, 3, 3> mask(0, 1, 0, 1, 1, 1, 0, 1, 0);
	morphologyEx(test160, test160, MORPH_OPEN, mask); // 외곽의 솔트를 제거하기 위해
	imshow("OPEN", test160);

	//contour를 찾는다.
	//findContours는 소스 이미지를 훼손시키므로 주의
	vector<vector<Point>> contours;
	findContours(test160, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

	//contour를 근사화한다.
	vector<Point2f> approx;
	vector<Point2f> rectpoints; // 칩 외곽 꼭지점들
	int max_x = 0, max_y = 0, min_x = 10000, min_y = 10000;
	Mat img_result = img.clone();

	for (size_t i = 0; i < contours.size(); i++)
	{
		approxPolyDP(Mat(contours[i]), approx, arcLength(Mat(contours[i]), true) * 0.02, true);

		if (fabs(contourArea(Mat(approx))) > 10000)  //면적이 일정크기 이상이어야 한다. 
		{
			int size = approx.size();

			line(img_result, approx[0], approx[approx.size() - 1], Scalar(0, 255, 0), 1);

			for (int k = 0; k < size - 1; k++)
				line(img_result, approx[k], approx[k + 1], Scalar(0, 255, 0), 1);


			for (int k = 0; k < size; k++)
			{
				circle(img_result, approx[k], 3, Scalar(0, 0, 255));
				//if (approx[k].x > max_x)
				//	max_x = approx[k].x;
				//else if (approx[k].x < min_x)
				//	min_x = approx[k].x;
				//if (approx[k].y > max_y)
				//	max_y = approx[k].y;
				//else if (approx[k].x < min_y)
				//	min_y = approx[k].y;

				rectpoints.push_back(approx[k]); // 내부에 원치 않는 엣지값들도 저장됨
				cout << approx[k] << endl;
				
			}
		}
	}

	

	imshow("test", test160);
	imshow("결과", img_result);

	waitKey(0);

	return 0;
}