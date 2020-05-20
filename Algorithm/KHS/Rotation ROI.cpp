#include "opencv2/opencv.hpp" 
#include <iostream>  
#include <time.h>  

using namespace cv;
using namespace std;

// 3�� ��ǥ ���� ����
void CornerCoordinate(Mat& src, Mat& Ldst, Mat& Bdst, vector<Point>& dst);
// ��ȯ�� �̹����� ���� �� �ܽ� ����
void CircleCenterDetection(Mat& Ldst, Mat& Bdst, vector<Vec3f>& LdstCircles, vector<Vec3f>& BdstCircles);
// �� �߽��� ���� ���ο� �̹����� ��Ŭ �׸���
void DrawCircleOnImage(Mat& src, Mat& dst, vector<Point>& LCirCenters, vector<Point>& BCirCenters);
// 3���� ���� �ܰ� ��ǥ ROI ����
void OutermostROI(Mat& src, Mat& Ldst, Mat& Bdst, vector<Point>& pts, Vec3f& LEquation, Vec3f& BEquation, int range);
// ��ǥ ���� �Լ�
void pointsRollBack(Vec3f& LEquation, Vec3f& BEquation, vector<Vec3f>& LCircles, vector<Vec3f>& BCircles, vector<Point>& LCirCenters, vector<Point>& BCirCenters, int range);
// �ּ������� �Լ�
Point2f LeastSquared(vector<Point>& pts);
// �ּ������� x, y��ǥ ����Ī �� ����� �� ���� ���� �ٽ� y=x ��Ī�̵�
Point2f LeastSquared_Reverse(vector<Point>& pts);
// �ּ��������� ���� ���� ǥ��
void drawXandLine(Mat& src, vector<Point>& cols, vector<Point>& rows);
// ������
void Outermost(Mat& src, Mat& Ldst, Mat& Bdst, vector<Point>& pts, Vec3f& LEquation, Vec3f& BEquation, int range);
void RollBack(Vec3f& LEquation, Vec3f& BEquation, vector<Vec3f>& LCircles, vector<Vec3f>& BCircles, vector<Point>& LCirCenters, vector<Point>& BCirCenters, int range = 20);

void onMouse2(int event, int x, int y, int flags, void* param)
{
	switch (event)
	{
	case EVENT_LBUTTONDOWN:
		cout << "x = " << x << " " << "y = " << y << endl;
		cout << "img Value = " << ((int)((Mat*)param)->ptr<uchar>(y)[x]) << endl;
		//cout << (int)(img.ptr<uchar>(y)[x]) << endl;
		//cout << "���콺 ���� ��ư ������--" << *(int*)param << endl;
		break;
	case EVENT_RBUTTONDOWN:
		//cout << "���콺 ������ ��ư ������--" << endl;
		break;
	}
}

void main()
{
	time_t start;
	time_t end;
	Mat src = imread("image/asss.png", IMREAD_GRAYSCALE);
	//Mat src = imread("image/ah.jpg", IMREAD_GRAYSCALE);
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
	Outermost(src, Ldst, Bdst, threePoints, LEquation, BEquation, 15); // 3���̿� �ܰ� ���� ���
	CircleCenterDetection(Ldst, Bdst, LCircles, BCircles); // ROI �̹����� ���� ���� �߽���ǥ ���� (������Ŭ)
	//pointsRollBack(LEquation, BEquation, LCircles, BCircles, LCirCenters, BCirCenters, 20); //
	RollBack(LEquation, BEquation, LCircles, BCircles, LCirCenters, BCirCenters, 15); //
	DrawCircleOnImage(src, newImage, LCirCenters, BCirCenters); // ����� �߽���ǥ�� ���� �� �׸���
	//drawXandLine(newImage, LCirCenters, BCirCenters); //
	end = clock();
	cout << "time = " << end - start << endl;
	imshow("Ldst", Ldst);
	imshow("Bdst", Bdst);
	imshow("newImage", newImage);
	waitKey();
}


// ���� ��ǥ�� ��ȯ
void pointsRollBack(Vec3f& LEquation, Vec3f& BEquation, vector<Vec3f>& LCircles, vector<Vec3f>& BCircles, vector<Point>& LCirCenters, vector<Point>& BCirCenters, int range = 20)
{
	double rotateX;
	double rotateY;
	if (LEquation[0] == 0)
	{
		LCirCenters.resize(LCircles.size());
		for (int i = 0; i < LCircles.size(); i++)
		{
			rotateY = LCircles[i][1] + LEquation[2];
			rotateX = LCircles[i][0] + LEquation[1];
			LCirCenters[i].x = rotateX;
			LCirCenters[i].y = rotateY;
		}
	}
	else
	{
		LCirCenters.resize(LCircles.size());
		for (int i = 0; i < LCircles.size(); i++)
		{
			rotateY = LCircles[i][1] + LEquation[2];
			rotateX = (rotateY - LEquation[1]) / LEquation[0] + LCircles[i][0];
			LCirCenters[i].x = rotateX;
			LCirCenters[i].y = rotateY;
		}
	}

	if (BEquation[0] == 0)
	{

		BCirCenters.resize(BCircles.size());
		for (int i = 0; i < BCircles.size(); i++)
		{
			rotateX = BCircles[i][0] + BEquation[2]; // x + st.x
			rotateY = (BCircles[i][1] + 1) + BEquation[1] - range; // hb - y -1 + b
			BCirCenters[i].x = rotateX;
			BCirCenters[i].y = rotateY;
		}
	}
	else
	{
		BCirCenters.resize(BCircles.size());
		for (int i = 0; i < BCircles.size(); i++)
		{
			rotateX = BCircles[i][0] + BEquation[2]; // x + st.x
			rotateY = BEquation[0] * rotateX + BEquation[1] - (range - BCircles[i][1] - 1); // alpha * x + beta - y
			BCirCenters[i].x = rotateX;
			BCirCenters[i].y = rotateY;
		}
	}
}

// ���İ� ����
void OutermostROI(Mat& src, Mat& Ldst, Mat& Bdst, vector<Point>& pts, Vec3f& LEquation, Vec3f& BEquation, int range = 20)
{
	int hL = sqrt(pow(pts[0].y - pts[1].y, 2) + pow(pts[0].x - pts[1].x, 2));
	int wL = range;

	int hB = range;
	int wB = sqrt(pow(pts[1].y - pts[2].y, 2) + pow(pts[1].x - pts[2].x, 2));

	Ldst = Mat(hL, wL, CV_8UC1, Scalar(0));
	Bdst = Mat(hB, wB, CV_8UC1, Scalar(0));

	double AlphaL;
	double BetaL;

	double AlphaB;
	double BetaB;

	double rotateX;
	double rotateY;

	if (pts[1].x == pts[0].x) {
		LEquation[0] = AlphaL = 0;
		LEquation[1] = BetaL = pts[0].x;
		LEquation[2] = pts[0].y;

		for (int y = pts[0].y; y < pts[0].y + hL; y++)
		{
			for (int x = 0; x < wL; x++)
			{
				rotateX = x + BetaL;
				Ldst.ptr<uchar>(y - pts[0].y)[x] = src.ptr<uchar>(y)[cvRound(rotateX)];

			}
		}
	}
	else {
		LEquation[0] = AlphaL = (double)(pts[1].y - pts[0].y) / (pts[1].x - pts[0].x);
		LEquation[1] = BetaL = pts[0].y - (AlphaL * pts[0].x);
		LEquation[2] = pts[0].y;

		for (int y = pts[0].y; y < pts[0].y + hL; y++)
		{
			for (int x = 0; x < wL; x++)
			{
				rotateX = (y - BetaL) / AlphaL + x;
				Ldst.ptr<uchar>(y - pts[0].y)[x] = src.ptr<uchar>(y)[cvRound(rotateX)];
				//imshow("Ldst", Ldst);
				//waitKey();
			}
		}
	}

	if (pts[2].x == pts[1].x) {

		BEquation[0] = AlphaB = 0;
		BEquation[1] = BetaB = pts[1].x;
		BEquation[2] = pts[1].x;

		for (int x = pts[1].x; x < pts[1].x + wB; x++)
		{
			for (int y = 0; y < hB; y++)
			{
				rotateY = y + BetaB;
				Bdst.ptr<uchar>(hB - y - 1)[x - pts[1].x] = src.ptr<uchar>(cvRound(rotateY))[x];
			}
		}
	}
	else {
		BEquation[0] = AlphaB = (double)(pts[2].y - pts[1].y) / (pts[2].x - pts[1].x);
		BEquation[1] = BetaB = pts[1].y - (AlphaB * pts[1].x);
		BEquation[2] = pts[1].x;

		for (int x = pts[1].x; x < pts[1].x + wB; x++)
		{
			for (int y = 0; y < hB; y++)
			{
				rotateY = AlphaB * x + BetaB - y;
				Bdst.ptr<uchar>(hB - y - 1)[x - pts[1].x] = src.ptr<uchar>(cvRound(rotateY))[x];
			}
		}
	}

}

// ������
void RollBack(Vec3f& LEquation, Vec3f& BEquation, vector<Vec3f>& LCircles, vector<Vec3f>& BCircles, vector<Point>& LCirCenters, vector<Point>& BCirCenters, int range)
{
	double rotateX;
	double rotateY;
	double t_Alpha, t_beta;
	if (LEquation[0] == 0)
	{
		LCirCenters.resize(LCircles.size());
		for (int i = 0; i < LCircles.size(); i++)
		{
			rotateY = LCircles[i][1] + LEquation[2];
			rotateX = LCircles[i][0] + LEquation[1];
			LCirCenters[i].x = rotateX;
			LCirCenters[i].y = rotateY;
		}
	}
	else
	{
		LCirCenters.resize(LCircles.size());
		t_Alpha = -1 / LEquation[0];


		for (int i = 0; i < LCircles.size(); i++)
		{
			// ( center.y + st.y - beta ) / alpha  == X
			rotateX = (LCircles[i][1] + LEquation[2] - LEquation[1]) / LEquation[0];
			// cneter.y + st.y - tA * X
			t_beta = LCircles[i][1] + LEquation[2] - (t_Alpha * rotateX);
			// center.y + B
			rotateX = (LCircles[i][1] + LEquation[2] - t_beta) / t_Alpha + LCircles[i][0];
			// tA * (X + st.x) + B
			rotateY = t_Alpha * (rotateX)+t_beta;

			LCirCenters[i].x = rotateX;
			LCirCenters[i].y = rotateY;
		}
	}

	if (BEquation[0] == 0)
	{

		BCirCenters.resize(BCircles.size());
		for (int i = 0; i < BCircles.size(); i++)
		{
			rotateX = BCircles[i][0] + BEquation[2]; // x + st.x
			rotateY = BCircles[i][1] + BEquation[1]; // y + st.y - range
			BCirCenters[i].x = rotateX;
			BCirCenters[i].y = rotateY;
		}
	}
	else
	{
		BCirCenters.resize(BCircles.size());
		t_Alpha = -1 / BEquation[0];

		for (int i = 0; i < BCircles.size(); i++)
		{
			// Y  = alpha * (center.x + st.x) + beta
			rotateY = BEquation[0] * (BCircles[i][0] + BEquation[2]) + BEquation[1];
			// beat =  Y + st.y tA * (center.x + st.x)
			t_beta = rotateY + BCircles[i][1] - t_Alpha * (BCircles[i][0] + BEquation[2]);

			// Y  = alpha * (center.x + st.x) + beta
			rotateY = t_Alpha * (BCircles[i][0] + BEquation[2]) + t_beta;

			// X = (Y - B) / tA
			rotateX = (rotateY - t_beta )/ t_Alpha;

			BCirCenters[i].x = cvRound(rotateX);
			BCirCenters[i].y = cvRound(rotateY);
		}
	}
}

// ������
void Outermost(Mat& src, Mat& Ldst, Mat& Bdst, vector<Point>& pts, Vec3f& LEquation, Vec3f& BEquation, int range = 20)
{
	double AlphaL, BetaL;
	double AlphaB, BetaB;
	int rotateX, rotateY;
	double t_Alpha, t_beta;

	int hL = sqrt(pow(pts[0].y - pts[1].y, 2) + pow(pts[0].x - pts[1].x, 2));
	int wL = range;

	int hB = range;
	int wB = sqrt(pow(pts[1].y - pts[2].y, 2) + pow(pts[1].x - pts[2].x, 2));

	Ldst = Mat(hL, wL, CV_8UC1, Scalar(0));
	Bdst = Mat(hB, wB, CV_8UC1, Scalar(0));

	// ������
	if (pts[1].x == pts[0].x) {
		LEquation[0] = AlphaL = 0;
		LEquation[1] = BetaL = pts[0].x;
		LEquation[2] = pts[0].y;

		for (int y = 0; y < hL; y++)
		{
			rotateY = pts[0].y + y;
			for (int x = 0; x < wL; x++)
			{
				rotateX = BetaL + x;
				Ldst.ptr<uchar>(y)[x] = src.ptr<uchar>(rotateY)[rotateX];
			}
		}
	}
	else {
		LEquation[0] = AlphaL = (double)(pts[1].y - pts[0].y) / (pts[1].x - pts[0].x);
		LEquation[1] = BetaL = pts[0].y - (AlphaL * pts[0].x);
		LEquation[2] = pts[0].y;

		t_Alpha = -1 / AlphaL;

		for (int y = 0; y < hL; y++)
		{
			rotateX = (y + pts[0].y - BetaL) / AlphaL;
			t_beta = y - t_Alpha * rotateX;
			for (int x = 0; x < wL; x++)
			{
				rotateY = t_Alpha * (rotateX + x) + t_beta;
				Ldst.ptr<uchar>(y)[x] = src.ptr<uchar>(rotateY)[rotateX];
			}
		}


		//for (int y = pts[0].y; y < pts[0].y + hL; y++)
		//{
		//	rotateX = (y - BetaL) / AlphaL;
		//	t_beta = y - t_Alpha * rotateX;
		//	for (int x = rotateX; x < rotateX + wL; x++)
		//	{
		//		rotateY = t_Alpha * x + t_beta;
		//		Ldst.ptr<uchar>(y - pts[0].y)[x - rotateX] = src.ptr<uchar>(rotateY)[x];
		//	}
		//}
	}

	if (pts[2].y == pts[1].y) {

		BEquation[0] = AlphaB = 0;
		BetaB = pts[1].y;
		BEquation[1] = pts[1].y - range + 1;
		BEquation[2] = pts[1].x;
		int YBottom = hB;
		for (int y = 0; y < hB; y++)
		{
			rotateY = BetaB - y;
			YBottom--;
			for (int x = 0; x < wB; x++)
			{
				rotateX = pts[1].x + x;
				Bdst.ptr<uchar>(YBottom)[x] = src.ptr<uchar>(rotateY)[rotateX];
			}
		}
	}
	else {
		BEquation[0] = AlphaB = (double)(pts[2].y - pts[1].y) / (pts[2].x - pts[1].x);
		BetaB = pts[1].y - (AlphaB * pts[1].x);
		BEquation[1] = BetaB - range + 1;
		BEquation[2] = pts[1].x;

		t_Alpha = -1 / AlphaB;
		for (int x = pts[1].x; x < pts[1].x + wB; x++)
		{
			rotateY = AlphaB * x + BetaB;
			t_beta = rotateY - t_Alpha * x;
			for (int y = rotateY; y > rotateY - hB; y--)
			{
				rotateX = (y - t_beta) / t_Alpha;
				Bdst.ptr<uchar>(hB - (rotateY - y) - 1)[x - pts[1].x] = src.ptr<uchar>(y)[rotateX];
				//imshow("Bdst", Bdst);
				//waitKey();
			}
		}
	}

}

// ��ȯ�� �̹����� ���� �� �ܽ� ����
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

// �� �߽��� ���� ���ο� �̹����� ��Ŭ �׸���
void DrawCircleOnImage(Mat& src, Mat& dst, vector<Point>& LCirCenters, vector<Point>& BCirCenters)
{
	if (!src.data)
		return;
	if (!dst.data)
		cvtColor(src, dst, COLOR_GRAY2BGR);

	for (int i = 0; i < LCirCenters.size(); i++)
	{
		Point center(cvRound(LCirCenters[i].x), cvRound(LCirCenters[i].y));
		circle(dst, center, 6, Scalar(0, 0, 255), 1, -1);
	}

	for (int i = 0; i < BCirCenters.size(); i++)
	{
		Point center(cvRound(BCirCenters[i].x), cvRound(BCirCenters[i].y));
		circle(dst, center, 6, Scalar(0, 0, 255), 1, -1);
	}
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
Point2f LeastSquared(vector<Point>& pts)
{
	double a, b;
	double xx = 0, x = 0, xy = 0, y = 0;
	int n = pts.size();
	Point2f result;
	for (int i = 0; i < n; i++)
	{
		xx += (pts[i].x) * (pts[i].x);
		x += (pts[i].x);
		xy += (pts[i].x) * (pts[i].y);
		y += (pts[i].y);

	}
	a = (n * xy - x * y) / (n * xx - x * x);
	b = (xx * y - x * xy) / (n * xx - x * x);
	result.x = a;
	result.y = b;

	cout << "y=" << a << "x+" << b << endl; // y=ax+b
	return result;
}

// �ּ������� x, y��ǥ ����Ī �� ����� �� ���� ���� �ٽ� y=x ��Ī�̵�
Point2f LeastSquared_Reverse(vector<Point>& pts)
{
	double a, b;
	double xx = 0, x = 0, xy = 0, y = 0;
	int n = pts.size();
	Point2f result;
	for (int i = 0; i < n; i++)
	{
		xx += (pts[i].y) * (pts[i].y);
		x += (pts[i].y);
		xy += (pts[i].y) * (pts[i].x);
		y += (pts[i].x);

	}
	a = (n * xy - x * y) / (n * xx - x * x);
	b = (xx * y - x * xy) / (n * xx - x * x);
	result.x = 1 / a;
	result.y = -b / a;

	return result;
}

// �ּ��������� ���� ���� ǥ��
void drawXandLine(Mat& src, vector<Point>& cols, vector<Point>& rows)
{
	Point2f const_cols, const_rows;
	Point target, temp1, temp2;
	const_cols = LeastSquared_Reverse(cols);
	const_rows = LeastSquared(rows);

	target.x = cvRound((const_rows.y - const_cols.y) / (const_cols.x - const_rows.x));
	target.y = cvRound((const_cols.x * (const_rows.y - const_cols.y)) / (const_cols.x - const_rows.x) + const_cols.y);

	// ��Ȳ�� ������ ������ �׸���
	temp1 = { cvRound(-const_cols.y / const_cols.x), 0 }; // y=0 �̰�, �ּ������� ������ �������� ������ ��
	temp2 = { cvRound((target.y + 20 - const_cols.y) / const_cols.x), target.y + 20 }; // target���� �� �� �Ʒ����� ��
	line(src, temp1, temp2, Scalar(0, 127, 255));
	temp1 = { src.cols, cvRound(const_rows.x * src.cols + const_rows.y) }; // x = src.cols �̰�, �ּ������� ������ �������� ������ ��
	temp2 = { target.x - 20, cvRound(const_rows.x * (target.x - 20) + const_rows.y) }; // target���� �� �� ������ ��
	line(src, temp1, temp2, Scalar(0, 127, 255));

	// ���ӻ� X �׸���
	temp1 = { target.x - 7, target.y - 7 };
	temp2 = { target.x + 7,target.y + 7 };
	line(src, temp1, temp2, Scalar(0, 255, 191));
	temp1 = { target.x - 7, target.y + 7 };
	temp2 = { target.x + 7,target.y - 7 };
	line(src, temp1, temp2, Scalar(0, 255, 191));

	cout << "������ ��ǥ = " << target << endl;
}

