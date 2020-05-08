#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <math.h>
#include <iostream>

using namespace std;
using namespace cv;

#define PI 3.141592
#define WIDTH 200
#define HEIGHT 200

//������ ����
void threshold_noise(Mat& src, Mat& dst, int threshold_);
//�̺� ����
void differential(Mat image, Mat& dst, float data1[], float data2[]);

void setLabel(Mat& img, const vector<Point>& pts, const String& label)
{
	Rect rc = boundingRect(pts);// �������� �ڽ� 
	Point pt = Point((rc.x + rc.width / 2), rc.y + rc.height / 2);
	circle(img, pt, 3, Scalar(0, 0, 255), 1); // �߽���ǥ�� ��Ŭ��ȯ

}

void main()
{
	//�̹��� �ε�
	Mat image = imread("t.png", IMREAD_COLOR);
	//Mat image = imread("ssss.png", IMREAD_COLOR);
	if (image.empty()) {
		cout << "�̹��� �ε� ����";
		return;
	}

	//ĳ�� �˰����� ���� �׷��� ������ �̹����� ��ȯ
	

	const int width = image.cols;
	const int height = image.rows;
	int rad_min = 3;
	int rad_max = 4;
	int thetha = 360;

	Mat prewitt, thres;

	cvtColor(image, thres, COLOR_BGR2GRAY);
	cvtColor(image, prewitt, COLOR_BGR2GRAY);
	

	// ĳ�� �˰��� ����

	int n = 30;

	time_t start, end;

	

	threshold(thres, thres, 200, 250, THRESH_BINARY_INV | THRESH_OTSU);
	float prewitt_y[] = {
	-1,0,1,
	-1,0,1,
	-1,0,1
	};
	float prewitt_x[] = {
		-1,-1,-1,
		0,0,0,
		1,1,1
	};

	start = clock();

	vector<Point> Circles;
	vector<vector<Point>> contours;
	findContours(thres, contours, 3, CHAIN_APPROX_SIMPLE); // ���ø�� ���ͼ��� ���̱�����

	cout << "find Contours" << endl;

	int min = 2;
	int max = 4;
	vector<Vec3f> LdstCircles;


	//for (vector<Point> pts : contours) {
	//	for (Point pt : pts) {
	//		circle(image, pt, 0 / 1, Scalar(255, 0, 0));
	//	}
	//}

	for (vector<Point> pts : contours) {
		double areaValue = contourArea(pts); // ������ ���
		if (areaValue < 50 && areaValue > 2) {
			Rect rc = boundingRect(pts);
			rc -= Point(3, 3);
			rc += Size(3, 3);
			Mat test = prewitt(rc);
			
			//rectangle(image, rc, Scalar(0, 0, 255), 1);
			threshold_noise(test, test, 160);
			differential(test, test, prewitt_y, prewitt_x);
			threshold(test, test, 100, 120, THRESH_TOZERO);

			//imshow("test", test);


			HoughCircles(test, LdstCircles, HOUGH_GRADIENT, 1, 6, 255, 5, 3, 3);
			//cout << cvRound(LdstCircles[0][0]);
			if(LdstCircles.size()!=0)
				Circles.push_back(Point(LdstCircles[0][0]+rc.tl().x, LdstCircles[0][1]+ rc.tl().y));
		}
	}

	
	

	for (Point pt : Circles) {
		circle(image, pt, 3, Scalar(255, 0, 0));
	}




	end = clock();
	cout << "time : " << end - start << endl;

	imshow("img", image);
	//imshow("dst", dst);
	waitKey(0);
}


void threshold_noise(Mat& src, Mat& dst, int min) {
	uchar* ptr;
	uchar* ptr2;
	for (int x = 0; x < src.rows; x++) {
		ptr = src.ptr < uchar >(x);
		ptr2 = dst.ptr < uchar >(x);

		for (int y = 0; y < src.cols; y++) {
			if (ptr[y] > min)
				ptr2[y] = 130;
		}
	}
}


void differential(Mat image, Mat& dst, float data1[], float data2[]) {
	Mat dst1, dst2;
	Mat mask1(3, 3, CV_32F, data1);
	Mat mask2(3, 3, CV_32F, data2);


	filter2D(image, dst1, CV_32F, mask1, Point(-1, -1), 3, 1);	//OpenCV ���� �Լ�
	filter2D(image, dst2, CV_32F, mask2, Point(-1, -1), 3, 1);	//OpenCV ���� �Լ�

	magnitude(dst1, dst2, dst);	//SQRT(dst1^2, dst2^2)

	dst1 = abs(dst1);
	dst2 = abs(dst2);
	dst.convertTo(dst, CV_8U);

	//dst1.convertTo(dst1, CV_8U);
	//dst2.convertTo(dst2, CV_8U);
}