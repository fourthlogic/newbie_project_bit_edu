#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <math.h>
#include <iostream>
#include <time.h>
using namespace std;
using namespace cv;

#define PI 3.141592




void setLabel(Mat& img, const vector<Point>& pts, const String& label)
{
	Rect rc = boundingRect(pts);// 컨투어의 박스 
	Point pt = Point((rc.x + rc.width / 2), rc.y + rc.height / 2);
	circle(img, pt, 3, Scalar(0, 0, 255), 1); // 중심좌표로 서클변환

}

void main()
{
	//이미지 로드
	Mat image = imread("image/a (1).png", IMREAD_COLOR);
	Rect roi = Rect(701, 0, 15, 800);

	if (image.empty()) {
		cout << "이미지 로드 실패";
		return;
	}

	//캐니 알고리즘을 위해 그레이 스케일 이미지로 변환
	Mat gray;
	cvtColor(image, gray, COLOR_BGR2GRAY);

	Mat dst;
	Canny(gray(roi), dst, 100, 150);



	clock_t start = clock();
	rectangle(image, roi, Scalar(0, 0, 255));
	// 캐니 알고리즘 적용

	vector<Vec3f> circles;
	HoughCircles(dst, circles, HOUGH_GRADIENT, 1, 3, 100, 5, 2, 4);


	for (size_t i = 0; i < circles.size(); i++)
	{
		//Vec3i c = circles[i];
		Point center(cvRound(circles[i][0])+701, cvRound(circles[i][1]));
		//cout << "center" << center.x << center.y << endl;
		int radius = cvRound(circles[i][2]);

		//circle(img_color, center, 3, Scalar(0, 255, 0), 1);
		circle(image, center, radius, Scalar(0, 0, 255), 1);
	}


	clock_t end = clock();

	cout << end - start << endl;



	imshow("img", image);
	waitKey(0);
}



