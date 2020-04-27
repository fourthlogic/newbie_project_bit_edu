#include "opencv2/opencv.hpp"
#include <iostream>
#include <cmath>



using namespace cv;
using namespace std;

double max_grayscale = 160;
double min_grayscale = 110;

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
	img = imread("images/a8.png", IMREAD_COLOR);
	if (img.empty())
	{
		cout << "Could not open or find the image" << std::endl;
		return -1;
	}
	Matx <uchar, 3, 3> mask(0, 1, 0, 1, 1, 1, 0, 1, 0);

	Mat test160;
	cvtColor(img, test160, COLOR_BGR2GRAY);
	threshold(test160, test160, min_grayscale, 255, THRESH_TOZERO); // min_grayscale이 안되면 0
	imshow("TOZERO", test160);
	threshold(test160, test160, max_grayscale, 255, THRESH_TOZERO_INV); // min_grayscale이 넘어도 0
	imshow("110~160", test160);
	morphologyEx(test160, test160, MORPH_OPEN, mask); // 외곽의 솔트를 제거하기 위해

	imshow("OPEN", test160);

	//contour를 찾는다.
	//findContours는 소스 이미지를 훼손시키므로 주의
	vector<vector<Point>> contours;
	findContours(test160, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

	//contour를 근사화한다.
	vector<Point2f> approx;
	Point2f start_point, farmost_point;
	Point2f left_point, right_point;
	int distance = 0;
	int max_y_farmost = 0;
	int max_x = 0, max_y = 0;
	int min_x = 100000, min_y = 100000;
	int farmost_idx = 0;
	Mat img_result = img.clone();

	for (size_t i = 0; i < contours.size(); i++)
	{
		approxPolyDP(Mat(contours[i]), approx, arcLength(Mat(contours[i]), true) * 0.02, true);
		if (fabs(contourArea(Mat(approx))) > 10000)  //면적이 일정크기 이상이어야 한다. 
		{
			int size = approx.size();

			start_point = approx[0]; // 한 점을 start로 잡음

			line(img_result, approx[0], approx[approx.size() - 1], Scalar(0, 255, 0), 1);

			for (int k = 0; k < size - 1; k++)
				line(img_result, approx[k], approx[k + 1], Scalar(0, 255, 0), 1);
			
			//가장 먼 점 2개 구하기
			for (int i = 0; i < size; i++)
			{
				if (max_y_farmost < approx[i].y) // farmost_point가 가장 아래에 있는지 확인하기 위함 (case1 or case2)
					max_y_farmost = approx[i].y;
				for (int j = 1; j < size - 1; j++)
				{
					if (distance < sqrt(pow(approx[i].x - approx[j].x, 2) + pow(approx[i].y - approx[j].y, 2)))
					{
						distance = sqrt(pow(approx[i].x - approx[j].x, 2) + pow(approx[i].y - approx[j].y, 2));
						start_point = approx[i];
						farmost_point = approx[j];
					}
					
				}
			}
			if (start_point.y > farmost_point.y) // farmost_point를 아래쪽에 둠
			{
				Point2f temp;
				temp = start_point;
				start_point = farmost_point;
				farmost_point = temp;
			}

			

			for (int k = 0; k < size; k++)
			{
				circle(img_result, approx[k], 3, Scalar(0, 0, 255));

				// case1) farmost_point가 가장 아래에 있을 때 => x가 가장 큰 것과 x가 가장 작은 것
				if (farmost_point.y == max_y_farmost)
				{
					if (max_x < approx[k].x)
					{
						max_x = approx[k].x;
						right_point = approx[k];
					}
					if (min_x > approx[k].x)
					{
						min_x = approx[k].x;
						left_point = approx[k];
					}
				}

				// case2) farmost_point가 가장 아래가 아닐 때 => y가 가장 큰 것과 y가 가장 작은 것
				else if (farmost_point.y < max_y_farmost)
				{
					if (max_y < approx[k].y)
					{
						max_y = approx[k].y;
						left_point = approx[k];
					}
					if (min_y > approx[k].y)
					{
						min_y = approx[k].y;
						right_point = approx[k];
					}
				}
				// 문제점 : 튀는 값 중에 y값이 꼭지점보다 더 작은 값이 있으면 그 값이 들어감.
				cout << "approx[" << k << "]= " << approx[k] << endl;
			}
		}
	}
	cout << "start_point=" << start_point << endl;
	cout << "왼쪽=" << left_point << endl;
	cout << "farmost_point=" << farmost_point << endl;
	cout << "오른쪽=" << right_point << endl;

	imshow("결과", img_result);

	waitKey(0);

	return 0;
}