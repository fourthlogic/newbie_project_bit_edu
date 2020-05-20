#include "opencv2/opencv.hpp" 
#include <iostream>  
#include <time.h>  

using namespace cv;
using namespace std;


//=========================================================================
// �ܰ� �Լ� ���� test code 
//=========================================================================


// ������ ��ǥ���� ���� outerLine ���� �� �����
void outerLine(Mat& src, Mat& dst, vector<Point>& cp, int sx, int sy);

// ������ ���� ��ǥ�� ã�� ���� �Լ�
bool imageDataStart(uchar*& pSrc, int& height, int& width, Point& startPt, Point& firstPt, int value);

// ���̳ʸ� ����  threshold �Լ� ���̰� ���� max ~ min 
void binaryThreshold(Mat& src, Mat& dst, int MinValue, int MaxValue);

// �޵�� ����
void MedianFilter(Mat img, Mat& dst, int size);

// mask ���� ����
void filter(Mat img, Mat& dst, Mat mask);

Mat img; // �����ҽ�

// �ð� ������
clock_t start;
clock_t end;

void main()
{
	img = imread("image/a1.png", IMREAD_GRAYSCALE);
	if (!img.data)
		return;
	namedWindow("srcImage", WINDOW_AUTOSIZE);
	imshow("srcImage", img);
	Mat outerImage;
	Mat binaryImage;
	vector<Point> edgePoints;

	//���� ����
	binaryThreshold(img, binaryImage,34,255);
	imshow("binary", binaryImage);

	// 1.�޵�� - ����ӵ� �ʹ� ���� - ��Ȯ�� up
	//MedianFilter(binaryImage, binaryImage, 5);
	
	// 2.ħ���ۿ� - ����ӵ� ��������� ����
	//Matx <uchar, 3, 3> mask(0, 1, 0, 1, 1, 1, 0, 1, 0);
	//morphologyEx(binaryImage, binaryImage, MORPH_OPEN, mask);

	// 3.��ó�� - 
	//float data[] = {
	//1 / 9.f, 1 / 9.f, 1 / 9.f,
	//1 / 9.f, 1 / 9.f, 1 / 9.f,
	//1 / 9.f, 1 / 9.f, 1 / 9.f};
	//
	//Mat mask(3, 3, CV_32F, data);
	//filter(binaryImage, binaryImage, mask);
	//binaryImage.convertTo(binaryImage, CV_8U);

	// �ܰ���
	outerLine(binaryImage, outerImage, edgePoints, 0 , 0);
	imshow("outer", outerImage);
	waitKey();
}


// ���� �����Լ�
void filter(Mat img, Mat& dst, Mat mask)
{
	dst = Mat(img.size(), CV_32F, Scalar(0));
	Point h_m = mask.size() / 2;
	for (int i = h_m.y; i < img.rows - h_m.y; i++)
	{
		for (int j = h_m.x; j < img.cols - h_m.x; j++)
		{
			float sum = 0;
			for (int k = 0; k < mask.rows; k++)
			{
				for (int t = 0; t < mask.cols; t++)
				{
					int y = i + k - h_m.y;
					int x = j + t - h_m.x;
					sum += mask.at<float>(k, t) * img.at<uchar>(y, x);
				}
			}
			dst.ptr<float>(i)[j] = sum;
		}

	}
}


// ���̳ʸ� ����  threshold �Լ� ���̰� ���� max ~ min 
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

// �ش� value���� ã�� �Լ� 
bool imageDataStart(uchar*& pSrc, int& height, int& width, Point& startPt, Point& firstPt, int value)
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

// ���� �̿� �ܰ��� ���� �Լ� 4���� ����
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


	if (pSrc[w * sy + sx] != 255)
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
}

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
			cv::sort(one_row, sort_m, SORT_EVERY_ROW); // �� ���� ����

			int medi_idx = (int)(one_row.total() / 2); // �߰���ġ
			dst.at<uchar>(i, j) = sort_m.at<uchar>(medi_idx);

		}

	}

}
