#include "opencv2/opencv.hpp" 
#include <iostream>  
#include <time.h>  

using namespace cv;
using namespace std;


//=========================================================================
// �ֿܰ� ��ǥ ���� �̿��Ͽ� ROI������ ����Ͽ� �̹��� ���� code 
//=========================================================================

Mat img;

// �ܰ� ROI ����
void OutLineROI(Mat& src, Mat& Ldst, Mat& Bdst, vector<Point>& pts, int range);

// �ܰ� ������ ��ǥ ����
void CornerCoordinate(Mat& src, vector<Point>& dst);

void main()
{
	time_t start;
	time_t end;
	start = clock();
	img = imread("image/a1.png", IMREAD_GRAYSCALE);
	namedWindow("src Image", WINDOW_AUTOSIZE);
	imshow("src Image", img);

	vector<Point> coordinates;
	CornerCoordinate(img, coordinates);
	end = clock();
	cout << end - start << endl;
	waitKey();
}


// 3���� ���� �ܰ� ��ǥ ROI ���� 
void OutLineROI(Mat& src, Mat& Ldst, Mat& Bdst, vector<Point>& pts, int range = 30)
{
	int hL = abs(pts[0].y - pts[1].y);
	int wL = range;

	int hB = range;
	int wB = abs(pts[1].x - pts[2].x);

	Ldst = Mat(hL, wL, CV_8UC1, Scalar(0));
	Bdst = Mat(hB, wB, CV_8UC1, Scalar(0));


	double AlphaL = ((double)pts[1].y - pts[0].y) / ((double)pts[1].x - pts[0].x);
	double BetaL = pts[0].y - (AlphaL * pts[0].x);

	double AlphaB = ((double)pts[2].y - pts[1].y) / ((double)pts[2].x - pts[1].x);
	double BetaB = pts[1].y - (AlphaB * pts[1].x);

	double rotateX;
	double rotateY;

	for (int y = pts[0].y; y < pts[0].y + hL; y++)
	{
		for (int x = 0; x < wL; x++)
		{
			rotateX = (y - BetaL) / AlphaL + x;
			Ldst.ptr<uchar>(y - pts[0].y)[x] = src.ptr<uchar>(y)[(int)rotateX];
		}
	}

	for (int x = pts[1].x; x < pts[1].x + wB; x++)
	{
		for (int y = 0; y < hB; y++)
		{
			rotateY = AlphaB * x + BetaB - y;
			Bdst.ptr<uchar>(hB - y - 1)[x - pts[1].x] = src.ptr<uchar>((int)rotateY)[x];
		}
	}

	imshow("Ldst", Ldst);
	imshow("Bdst", Bdst);
}

// �ܰ� ������ ��ǥ ����
void CornerCoordinate(Mat& src, vector<Point>& dst)
{
	Mat img_gray = img.clone();

	threshold(img_gray, img_gray, 110, 255, THRESH_TOZERO); // min_grayscale�� �ȵǸ� 0
	threshold(img_gray, img_gray, 160, 255, THRESH_TOZERO_INV); // min_grayscale�� �Ѿ 0

	Matx <uchar, 3, 3> mask(0, 1, 0, 1, 1, 1, 0, 1, 0);
	morphologyEx(img_gray, img_gray, MORPH_OPEN, mask); // �ܰ��� ��Ʈ�� �����ϱ� ����

	vector<vector<Point>> contours;
	findContours(img_gray, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

	vector<Point2f> approx;
	vector<Point2f> corners;

	for (size_t i = 0; i < contours.size(); i++)
	{
		approxPolyDP(Mat(contours[i]), approx, arcLength(Mat(contours[i]), true) * 0.07, true);
		if (fabs(contourArea(Mat(approx))) > 10000)  //������ ����ũ�� �̻��̾�� �Ѵ�. 
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

	Mat Ldst, Bdst;
	OutLineROI(img, Ldst, Bdst, dst, 20);
}







