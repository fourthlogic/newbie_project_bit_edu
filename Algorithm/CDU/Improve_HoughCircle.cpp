#include <opencv2/opencv.hpp>
#include <iostream>

using namespace std;
using namespace cv;



int perpendicularity(Point pt, Point pt1, int x) {
    return -((pt.y - pt1.y) / (pt.x - pt1.x)) * x + (sqrt(pt.x) + sqrt(pt.y) - sqrt(pt1.x) - sqrt(pt1.y)/2*(pt.y-pt1.y));
}

void main()
{
    //이미지 로드
    Mat image = imread("image/a (1).png", IMREAD_COLOR);

    if (image.empty()) {
        cout << "이미지 로드 실패";
        return;
    }
    Mat mask = Mat::zeros(30, 15, CV_8UC1);
    bitwise_not(mask, mask);
    Rect roi = Rect(701, 32, 15, 30);
    imshow("image", image);


    Mat canny;
    Canny(image, canny, 100, 150);
    imshow("canny", canny);

    Mat result;
    cout << "canny(roi) = " <<endl<< canny(roi)<<endl;
    bitwise_and(mask, canny(roi),result);
    
    rectangle(image, roi, Scalar(0, 255, 0));

    for (int x = 701; x < 716; x++) {
        for (int y = 32; y < 62; y++) {
            if (canny.at<uchar>(y, x) != 0 && canny.at<uchar>(y+3,x+3)!=0) {
                Point pt(x, y), pt1(x + 3, y + 3);
                line(image, pt, pt1, Scalar(0, 0, 255), 1, LINE_8);
                Point per_pt(x, perpendicularity(pt, pt1, x));
                Point per_pt1(x+3, perpendicularity(pt, pt1, x + 3));
                line(image, per_pt, per_pt1,Scalar(0,0,255,LINE_8));
                break;
            }
        }
    }

    /*for (int x = 0; x < image.cols-7; x++) {
        for (int y = 0; y < image.rows - 7; y++) {
            if (canny.at<uchar>(y,x) != 0) {
                line(image, Point(x, y), Point(x + 7, y + 7), Scalar(0, 0, 255),1,4);
            }
        }
    }*/


    cout << result;

    Mat canny_gaussain;

    imshow("image", image);
    

    waitKey(0);
}