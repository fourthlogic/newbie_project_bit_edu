#include "opencv2/opencv.hpp" 
#include <iostream>  
#include <time.h>  

using namespace cv;
using namespace std;



// �ּ��������� ���� ���� ǥ��
void drawXandLine(Mat& src, Mat& dst, vector<Point>& LCirCenters, vector<Point>& BCirCenters);
// �ּ������� x, y��ǥ ����Ī �� ����� �� ���� ���� �ٽ� y=x ��Ī�̵�
Vec2f LeastSquared_Reverse(vector<Point>& pts);
// �ּ������� �Լ�
Vec2f LeastSquared(vector<Point>& pts);
// �ٸ� �κ� ��ü �׽�Ʈ
void choo(Mat& Ldst);
// ���� ����
void differential(Mat image, Mat& dst, float data1[], float data2[]);
// �ٸ���� �׽�Ʈ �κ�
void threshold_noise(Mat& src, Mat& dst, int min);
// 3�� ��ǥ ���� ������
void CornerCoordinate(Mat& src, Mat& Ldst, Mat& Bdst, vector<Point>& dst);
// ��ȯ�� �̹����� ���� �� �ܽ� ����
void CircleCenterDetection(Mat& Ldst, Mat& Bdst, vector<Vec3f>& LdstCircles, vector<Vec3f>& BdstCircles);
// �������� - prewitt����
void CircleCenterDetection_prewitt(Mat& Ldst, Mat& Bdst, vector<Vec3f>& LdstCircles, vector<Vec3f>& BdstCircles);


// ������
void Outermost(Mat& src, Mat& Ldst, Mat& Bdst, vector<Point>& pts, int range = 20);

void RollBack(vector<Point>& pts, vector<Vec3f>& LCircles, vector<Vec3f>& BCircles, vector<Point>& LCirCenters, vector<Point>& BCirCenters, int range = 20);

void onMouse2(int event, int x, int y, int flags, void* param);

void main()
{
	time_t start;
	time_t end;
	Mat src = imread("image/ass.png", IMREAD_GRAYSCALE);
	if (!src.data)
		return;
	namedWindow("src Image", WINDOW_AUTOSIZE);
	setMouseCallback("src Image", onMouse2, (void*)&src);
	imshow("src Image", src);
	start = clock();
	vector<Point> threePoints; // �ܰ� 3�� ��ǥ ��
	vector<Vec3f> LCircles; // L������Ŭ ��ǥ ��
	vector<Vec3f> BCircles; // B������Ŭ ��ǥ ��
	Vec3f LEquation; // L ������ ��
	Vec3f BEquation; // B ������ ��
	vector<Point> LCirCenters; // L �߽� ��ǥ���� �� 
	vector<Point> BCirCenters; // B �߽� ��ǥ���� �� 
	Mat Ldst; // L �̹���
	Mat Bdst; // B �̹���
	Mat newImage; // ���� �̹���

	CornerCoordinate(src, Ldst, Bdst, threePoints); // 3�� ��ǥ ����
	Outermost(src, Ldst, Bdst, threePoints, 18); // 3���̿� �ܰ� ���� ���
	CircleCenterDetection_prewitt(Ldst, Bdst, LCircles, BCircles); // ROI �̹����� ���� ���� �߽���ǥ ���� (������Ŭ)
	RollBack(threePoints, LCircles, BCircles, LCirCenters, BCirCenters); //
	drawXandLine(src, newImage, LCirCenters, BCirCenters);

	//DrawCircleOnImage(src, newImage, LCirCenters, BCirCenters); // ����� �߽���ǥ�� ���� �� �׸���
	//drawXandLine(newImage, LCirCenters, BCirCenters); //

	end = clock();

	cout << "time = " << end - start << endl;
	imshow("Ldst", Ldst);
	imshow("Bdst", Bdst);
	imshow("newImage", newImage);
	waitKey();
}



// ������
void RollBack(vector<Point>& pts, vector<Vec3f>& LCircles, vector<Vec3f>& BCircles, vector<Point>& LCirCenters, vector<Point>& BCirCenters, int range)
{
	int rotateX, rotateY;
	double theta;

	if (pts[1].x == pts[0].x) {
		LCirCenters.resize(LCircles.size());
		for (int i = 0; i < LCircles.size(); i++)
		{
			LCirCenters[i].y = LCircles[i][1] + pts[0].y;
			LCirCenters[i].x = LCircles[i][0] + pts[0].x;
		}
	}
	else {

		theta = atan((double)((pts[0].x - pts[1].x)) / (pts[0].y - pts[1].y));
		LCirCenters.resize(LCircles.size());

		for (int i = 0; i < LCircles.size(); i++)
		{
			rotateX = (LCircles[i][0]) * cos(theta) + (LCircles[i][1]) * sin(theta) + pts[0].x;
			rotateY = -(LCircles[i][0]) * sin(theta) + (LCircles[i][1]) * cos(theta) + pts[0].y;
			LCirCenters[i].x = rotateX;
			LCirCenters[i].y = rotateY;
		}
	}


	if (pts[2].y == pts[1].y) {
		BCirCenters.resize(BCircles.size());
		for (int i = 0; i < BCircles.size(); i++)
		{
			BCirCenters[i].y = BCircles[i][1] + pts[1].y;
			BCirCenters[i].x = BCircles[i][0] + pts[1].x;
		}
	}
	else {

		theta = -atan(abs(pts[1].y - pts[2].y) / (double)(abs(pts[1].x - pts[2].x)));
		BCirCenters.resize(BCircles.size());

		for (int i = 0; i < BCircles.size(); i++)
		{
			rotateX = (BCircles[i][0]) * cos(theta) + (BCircles[i][1]) * sin(theta) + pts[1].x;
			rotateY = -(BCircles[i][0]) * sin(theta) + (BCircles[i][1]) * cos(theta) + pts[1].y;
			BCirCenters[i].x = rotateX;
			BCirCenters[i].y = rotateY - range;
		}

	}
}

// ������
void Outermost(Mat& src, Mat& Ldst, Mat& Bdst, vector<Point>& pts, int range)
{
	int rotateX, rotateY;
	double theta;
	uchar* ptr;
	int hL = sqrt(pow(pts[0].y - pts[1].y, 2) + pow(pts[0].x - pts[1].x, 2));
	int wL = range;

	int hB = range;
	int wB = sqrt(pow(pts[1].y - pts[2].y, 2) + pow(pts[1].x - pts[2].x, 2));

	Ldst = Mat(hL, wL, CV_8UC1, Scalar(0));
	Bdst = Mat(hB, wB, CV_8UC1, Scalar(0));

	if (pts[1].x == pts[0].x) {

		uchar* srcPtr;
		for (int y = 0; y < hL; y++)
		{
			ptr = Ldst.ptr<uchar>(y);
			rotateY = pts[0].y + y;
			srcPtr = src.ptr<uchar>(rotateY);
			for (int x = 0; x < wL; x++)
			{
				rotateX = pts[0].x + x;
				ptr[x] = srcPtr[rotateX];
			}
		}
	}
	else {

		theta = atan((double)((pts[0].x - pts[1].x)) / (pts[0].y - pts[1].y));
		for (int y = pts[0].y; y < pts[0].y + hL; y++)
		{
			ptr = Ldst.ptr<uchar>(y - pts[0].y);
			for (int x = pts[0].x; x < pts[0].x + wL; x++)
			{
				rotateX = (x - pts[0].x) * cos(theta) + (y - pts[0].y) * sin(theta) + pts[0].x;
				rotateY = -(x - pts[0].x) * sin(theta) + (y - pts[0].y) * cos(theta) + pts[0].y;
				ptr[x - pts[0].x] = src.ptr<uchar>(rotateY)[rotateX];
			}
		}

	}

	if (pts[2].y == pts[1].y) {
		uchar* srcPtr;
		for (int y = 0; y < hB; y++)
		{
			ptr = Bdst.ptr<uchar>(y);
			rotateY = pts[1].y - hB + y;
			srcPtr = src.ptr<uchar>(rotateY);
			for (int x = 0; x < wB; x++)
			{
				rotateX = pts[1].x + x;
				ptr[x] = srcPtr[rotateX];
			}
		}
	}
	else {

		int i = 0;
		theta = -atan((pts[1].y - pts[2].y) / (double)((pts[1].x - pts[2].x)));
		cout << "theta = "<< theta << endl;
		cout << "- theta = "<< -theta << endl;
		// ��Ÿ�� �����϶� ����϶� ���� ����
		for (int i = 0, y = pts[1].y - hB; y < pts[1].y; y++, i++)
		{
			ptr = Bdst.ptr<uchar>(i);
			for (int x = pts[1].x; x < pts[1].x + wB; x++)
			{
				rotateX = (x - pts[1].x) * cos(theta) + (y - pts[1].y) * sin(theta) + pts[1].x;
				rotateY = -(x - pts[1].x) * sin(theta) + (y - pts[1].y) * cos(theta) + pts[1].y;
				ptr[x - pts[1].x] = src.ptr<uchar>(rotateY)[rotateX];
			}
		}

	}
}


// ��ȯ�� �̹����� ���� �� �ܽ� ���� - ���� ���
void CircleCenterDetection(Mat& Ldst, Mat& Bdst, vector<Vec3f>& LdstCircles, vector<Vec3f>& BdstCircles)
{
	Mat Ledge;
	Mat Bedge;
	Canny(Ldst, Ledge, 100, 150);
	Canny(Bdst, Bedge, 100, 150);
	//imshow("Ledge", Ledge);
	//imshow("Bedge", Bedge);
	HoughCircles(Ledge, LdstCircles, HOUGH_GRADIENT, 1, 6, 100, 4, 2, 3);
	HoughCircles(Bedge, BdstCircles, HOUGH_GRADIENT, 1, 6, 100, 4, 2, 3);
}

// 3�� ��ǥ ���� ������
void CornerCoordinate(Mat& src, Mat& Ldst, Mat& Bdst, vector<Point>& dst)
{
	Mat img_gray = src.clone();

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

}

// �ּ������� �Լ�
Vec2f LeastSquared(vector<Point>& pts)
{
	double a, b;
	double xx = 0, x = 0, xy = 0, y = 0;
	int n = pts.size();
	Vec2f result;
	for (int i = 0; i < n; i++)
	{
		xx += (pts[i].x) * (pts[i].x);
		x += (pts[i].x);
		xy += (pts[i].x) * (pts[i].y);
		y += (pts[i].y);

	}
	a = (n * xy - x * y) / (n * xx - x * x);
	b = (xx * y - x * xy) / (n * xx - x * x);
	result[0] = a;
	result[1] = b;

	cout << "y=" << a << "x+" << b << endl; // y=ax+b
	return result;
}

// �ּ������� x, y��ǥ ����Ī �� ����� �� ���� ���� �ٽ� y=x ��Ī�̵�
Vec2f LeastSquared_Reverse(vector<Point>& pts)
{
	double a, b;
	double xx = 0, x = 0, xy = 0, y = 0;
	int n = pts.size();
	Vec2f result;
	for (int i = 0; i < n; i++)
	{
		xx += (pts[i].y) * (pts[i].y);
		x += (pts[i].y);
		xy += (pts[i].y) * (pts[i].x);
		y += (pts[i].x);

	}
	a = (n * xy - x * y) / (n * xx - x * x);
	b = (xx * y - x * xy) / (n * xx - x * x);
	// x=ya+b ==> y = (1/a) * x - b/a
	result[0] = 1 / a;
	result[1] = -b / a;
	cout << "y=" << a << "x+" << b << endl; // y=ax+b
	return result;
}

// �ּ��������� ���� ���� ǥ��
void drawXandLine(Mat& src, Mat& dst, vector<Point>& LCirCenters, vector<Point>& BCirCenters)
{
	vector<Point> NewLCirCenters, NewBCirCenters;
	if (!src.data)
		return;
	if (!dst.data)
		cvtColor(src, dst, COLOR_GRAY2BGR);

	Vec2f LEquation, BEquation;
	Point target, temp1, temp2;
	LEquation = LeastSquared_Reverse(LCirCenters);
	BEquation = LeastSquared(BCirCenters);

	// ������ �����İ� �Ÿ��� �� ��츦 �����ϰ� �� ǥ��
	for (int i = 0; i < LCirCenters.size(); i++)
	{
		Point center(LCirCenters[i].x, LCirCenters[i].y);
		if (4 > abs(LEquation[0] * center.x - center.y + LEquation[1]) / sqrt(pow(LEquation[0], 2) + 1))
		{
			circle(dst, center, 5, Scalar(0, 0, 255), 1, -1);
			NewLCirCenters.push_back(center);
		}

	}

	for (int i = 0; i < BCirCenters.size(); i++)
	{
		Point center(BCirCenters[i].x, BCirCenters[i].y);
		if (4 > abs(BEquation[0] * center.x - center.y + BEquation[1]) / sqrt(pow(BEquation[0], 2) + 1))
		{
			circle(dst, center, 5, Scalar(0, 0, 255), 1, -1);
			NewBCirCenters.push_back(center);
		}
	}

	LEquation = LeastSquared_Reverse(NewLCirCenters);
	BEquation = LeastSquared(NewBCirCenters);
	target.x = cvRound((BEquation[1] - LEquation[1]) / (LEquation[0] - BEquation[0]));
	target.y = cvRound((LEquation[0] * (BEquation[1] - LEquation[1])) / (LEquation[0] - BEquation[0]) + LEquation[1]);

	// ��Ȳ�� ������ ������ �׸���
	temp1 = { cvRound(-LEquation[1] / LEquation[0]), 0 }; // y=0 �̰�, �ּ������� ������ �������� ������ ��
	temp2 = { cvRound((target.y + 20 - LEquation[1]) / LEquation[0]), target.y + 20 }; // target���� �� �� �Ʒ����� ��
	line(dst, temp1, temp2, Scalar(0, 127, 255));
	temp1 = { dst.cols, cvRound(BEquation[0] * dst.cols + BEquation[1]) }; // x = src.cols �̰�, �ּ������� ������ �������� ������ ��
	temp2 = { target.x - 20, cvRound(BEquation[0] * (target.x - 20) + BEquation[1]) }; // target���� �� �� ������ ��
	line(dst, temp1, temp2, Scalar(0, 127, 255));

	// ���ӻ� X �׸���
	temp1 = { target.x - 7, target.y - 7 };
	temp2 = { target.x + 7,target.y + 7 };
	line(dst, temp1, temp2, Scalar(0, 255, 191));
	temp1 = { target.x - 7, target.y + 7 };
	temp2 = { target.x + 7,target.y - 7 };
	line(dst, temp1, temp2, Scalar(0, 255, 191));
}

// �ٸ���� �׽�Ʈ �κ�
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

// ���� ����
void differential(Mat image, Mat& dst, float data1[], float data2[]) {
	Mat dst1, dst2;
	Mat mask1(3, 3, CV_32F, data1);
	Mat mask2(3, 3, CV_32F, data2);


	filter2D(image, dst1, CV_32F, mask1, Point(-1, -1), 3, 1);   //OpenCV ���� �Լ�
	filter2D(image, dst2, CV_32F, mask2, Point(-1, -1), 3, 1);   //OpenCV ���� �Լ�

	magnitude(dst1, dst2, dst);   //SQRT(dst1^2, dst2^2)

	dst1 = abs(dst1);
	dst2 = abs(dst2);
	dst.convertTo(dst, CV_8U);

}

// prewitt
void CircleCenterDetection_prewitt(Mat& Ldst, Mat& Bdst, vector<Vec3f>& LdstCircles, vector<Vec3f>& BdstCircles)
{
	Mat Ledge = Ldst.clone();
	Mat Bedge = Bdst.clone();

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

	//������ ����
	threshold_noise(Ldst, Ledge, 160);
	threshold_noise(Bdst, Bedge, 160);
	//1�� �̺� ����
	differential(Ledge, Ledge, prewitt_y, prewitt_x);
	differential(Bedge, Bedge, prewitt_y, prewitt_x);

	imshow("Ledge", Ledge);
	imshow("Bedge", Bedge);

	HoughCircles(Ledge, LdstCircles, HOUGH_GRADIENT, 1, 6, 255, 4, 3, 3);
	HoughCircles(Bedge, BdstCircles, HOUGH_GRADIENT, 1, 6, 255, 4, 3, 3);



}

// �ٸ� �κ� ��ü �׽�Ʈ
void choo(Mat& Ldst)
{
	//�̹��� �ε�
	Mat image = Ldst;

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

	//cvtColor(image, thres, COLOR_BGR2GRAY);
	//cvtColor(image, prewitt, COLOR_BGR2GRAY);
	thres = image.clone();
	prewitt = image.clone();

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
	//   for (Point pt : pts) {
	//      circle(image, pt, 0 / 1, Scalar(255, 0, 0));
	//   }
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
			if (LdstCircles.size() != 0)
				Circles.push_back(Point(LdstCircles[0][0] + rc.tl().x, LdstCircles[0][1] + rc.tl().y));
		}
	}




	for (Point pt : Circles) {
		circle(image, pt, 3, Scalar(255, 0, 0));
	}




	end = clock();
	cout << "time : " << end - start << endl;

	imshow("img", image);

	waitKey(0);
}



void onMouse2(int event, int x, int y, int flags, void* param)
{
	switch (event)
	{
	case EVENT_LBUTTONDOWN:
		cout << "x = " << x << " " << "y = " << y << endl;
		cout << "img Value = " << ((int)((Mat*)param)->ptr<uchar>(y)[x]) << endl;
		break;
	case EVENT_RBUTTONDOWN:

		break;
	}
}