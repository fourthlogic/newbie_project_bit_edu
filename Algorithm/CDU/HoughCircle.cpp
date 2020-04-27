#include opencv2corecore.hpp
#include opencv2highguihighgui.hpp
#include opencv2imgprocimgproc.hpp
#include math.h
#include iostream

using namespace std;
using namespace cv;

#define PI 3.141592
#define WIDTH 200
#define HEIGHT 200



void main()
{
	이미지 로드
	Mat image = imread(aa.png,IMREAD_COLOR);

	캐니 알고리즘을 위해 그레이 스케일 이미지로 변환
	Mat gray;
	
	cvtColor(image, gray, COLOR_BGR2GRAY);
	 캐니 알고리즘 적용
	Mat contours;
	Canny(gray, contours, 50, 200);

	이미지 가로, 세로, 대각선
	const int width = image.cols;
	const int height = image.rows;
	const int diagonal = sqrt(width  width + height  height);

	반지름 최소최대값, 각도 범위
	const int rad_min = 5;
	const int rad_max = min(width, height)  2;
	const int thetha = 360;

	int vote = new int[width  height  rad_max];

	선택할 원의 최소 voting 수 
	int threshold = 200;


	초기화
	for (int i = 0; i  width; ++i)
	{
		for (int j = 0; j  height; ++j)
		{
			for (int k = 0; k  rad_max; ++k)
			{
				vote[i  (height  rad_max) + j  (rad_max)+k] = 0;
			}
		}
	}


	cout  초기화  endl;
	
	허프 변환 voting 구현
	...
	for (int i = 0; i  width; ++i)
	{
		for (int j = 0; j  height; ++j)
		{
			if (contours.atuchar(j, i) == 0)
				continue;
			for (int k = rad_min; k  rad_max; ++k)
			{
				for (int t = 0; t  thetha; t++)
				{
					int a = i - k  cos(t  PI  180);
					int b = j - k  sin(t  PI  180);
					if (a = 0 && a  width && b = 0 && b  height)
						vote[a  (height  rad_max) + b  rad_max + k] += 1;
				}
			}
		}
		cout  voting...  endl;
	}

	그리기

	int threshold = -1;
	for (int i = 0; i  width; i++)
	{
		for (int j = 0; j  height; j++)
		{
			for (int k = rad_min; k  rad_max; k++)
			{
				if (vote[i  (height  rad_max) + j  rad_max + k]  threshold)
				{
					Point center(i, j);
					circle(image, center, k, Scalar(255, 0, 0), 1);
				}
			}
		}
	}

	float threshold_persent = 0.8f;
	int real_threshold = 0;

	for (int i = 0; i  width; ++i)
	{
		for (int j = 0; j  height; ++j)
		{
			for (int k = rad_min; k  rad_max; ++k)
			{
				if (vote[i  (height  rad_max) + j  (rad_max)+k]  threshold)
				{
					threshold = vote[i  (height  rad_max) + j  (rad_max)+k];
				}
			}
		}
	}
	포괄적인 수정값
	int maxvote = threshold;
	threshold = threshold_persent;
	세부적인 수정값
	if (real_threshold != 0)
		threshold = real_threshold;

	printf(max vote  %d , maxvote);
	if (real_threshold == 0)
		printf(range  %.1f%% , (1 - threshold_persent)  100);
	printf(threshold  %dn, threshold);

	imshow(image, image);
	waitKey(0);
}