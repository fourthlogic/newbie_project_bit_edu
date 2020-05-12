#include "opencv2/opencv.hpp" 
#include <iostream>  
#include <time.h>  
#include <thread>
#include <Windows.h>

using namespace cv;
using namespace std;

// ROI 좌표 검출(6-포인트)
void ROI_Points(Mat& src, vector<Point>& conerPts, vector<Point>& vConerPts, vector<Point>& hConerPts, int distance)
{
    int rotateX, rotateY;
    double theta;

    int heightV = sqrt(pow(conerPts[0].y - conerPts[1].y, 2) + pow(conerPts[0].x - conerPts[1].x, 2));
    int widthV = distance;

    int heightH = distance;
    int widthH = sqrt(pow(conerPts[1].y - conerPts[2].y, 2) + pow(conerPts[1].x - conerPts[2].x, 2));


    if (conerPts[1].x == conerPts[0].x) {
        vConerPts.push_back(conerPts[0]);
        vConerPts.push_back(conerPts[1]);
        vConerPts.push_back(Point(conerPts[1].x + distance, conerPts[1].y));
        vConerPts.push_back(Point(conerPts[0].x + distance, conerPts[0].y));
    }
    else {
        theta = atan((double)((conerPts[0].x - conerPts[1].x)) / (conerPts[0].y - conerPts[1].y));

        vConerPts.push_back(conerPts[0]);

        vConerPts.push_back(conerPts[1]);

        rotateX = widthV * cos(theta) + heightV * sin(theta) + conerPts[0].x;
        rotateY = -widthV * sin(theta) + heightV * cos(theta) + conerPts[0].y;
        vConerPts.push_back(Point(rotateX, rotateY));

        rotateX = (distance)*cos(theta) + conerPts[0].x;
        rotateY = -(distance)*sin(theta) + conerPts[0].y;
        vConerPts.push_back(Point(rotateX, rotateY));

    }

    if (conerPts[2].y == conerPts[1].y) {
        hConerPts.push_back(Point(conerPts[1].x, conerPts[1].y - distance));
        hConerPts.push_back(conerPts[1]);
        hConerPts.push_back(conerPts[2]);
        hConerPts.push_back(Point(conerPts[2].x, conerPts[2].y - distance));
    }
    else {
        theta = -atan((conerPts[1].y - conerPts[2].y) / (double)((conerPts[1].x - conerPts[2].x)));
        rotateX = -distance * sin(theta) + conerPts[1].x;
        rotateY = -distance * cos(theta) + conerPts[1].y;
        hConerPts.push_back(Point(rotateX, rotateY));


        hConerPts.push_back(conerPts[1]);
        hConerPts.push_back(conerPts[2]);

        rotateX = widthH * cos(theta) - heightH * sin(theta) + conerPts[1].x;
        rotateY = -widthH * sin(theta) - heightH * cos(theta) + conerPts[1].y;
        hConerPts.push_back(Point(rotateX, rotateY));
    }
}
