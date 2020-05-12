#include "main(ver.ROIseparate).h"

void main()
{
    time_t start;
    time_t end;

    Mat src = imread("image/a (12).png", IMREAD_GRAYSCALE);
    
    cv::imshow("src Image", src);
    start = clock();

    vector<Point> cornerPts; // �ܰ� 3�� ��ǥ ��
    vector<Point> vCornerPts; // Vertical 
    vector<Point> hCornerPts; // Horizontal
    vector<Point> vCirCenters; // �߽� ��ǥ���� �� 
    vector<Point> hCirCenters; // �߽� ��ǥ���� �� 

    Mat dst;

    GetCornerPoints(src, cornerPts); // 3�� ��ǥ ����
    GetVertexPoints(src, cornerPts, vCornerPts, hCornerPts, 20);

    CirDetectionParam data1 = { src, vCornerPts, vCirCenters, 2, 4, 100 };
    CirDetectionParam data2 = { src, hCornerPts, hCirCenters, 2, 4, 100 };

    HANDLE hThread[2];
    unsigned threadID[2];

    hThread[0] = (HANDLE)_beginthreadex(NULL, 0, CirDetectionThread, &data1, 0, &threadID[0]);
    if (hThread[0] == NULL)
        return;
    hThread[1] = (HANDLE)_beginthreadex(NULL, 0, CirDetectionThread, &data2, 0, &threadID[1]);

    if (hThread[1] == NULL)
        return;

    // ���� ������ ���� ���
    WaitForMultipleObjects(sizeof(hThread) / sizeof(HANDLE), hThread, true, INFINITE);

    Drawing(src, dst, vCirCenters, hCirCenters);
    end = clock();
    cout << end - start << endl;
    imshow("result", dst);
    waitKey();
}

// 3�� ��ǥ ���� ������
void GetCornerPoints(Mat& src, vector<Point>& cornerPts)
{
    Mat grayImage = src.clone();

    threshold(grayImage, grayImage, 110, 255, THRESH_TOZERO); // min_grayscale�� �ȵǸ� 0
    threshold(grayImage, grayImage, 158, 255, THRESH_TOZERO_INV); // min_grayscale�� �Ѿ 0

    Matx <uchar, 3, 3> mask(0, 1, 0, 1, 1, 1, 0, 1, 0);
    morphologyEx(grayImage, grayImage, MORPH_OPEN, mask); // �ܰ��� ��Ʈ�� �����ϱ� ����

    vector<vector<Point>> contours;
    findContours(grayImage, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

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
    //0(top) ���� �ݽð� ����(������) 
    double distance0to1 = sqrt(pow(corners[0].x - corners[1].x, 2) + pow(corners[0].y - corners[1].y, 2));
    double distance0to3 = sqrt(pow(corners[0].x - corners[3].x, 2) + pow(corners[0].y - corners[3].y, 2));
    if (distance0to1 < distance0to3)
    {
        for (int i = 1; i < corners.size(); i++)
            cornerPts.push_back(corners[i]);
    }
    else
    {
        for (int i = 0; i < corners.size() - 1; i++)
            cornerPts.push_back(corners[i]);
    }
}
// �ֿܰ� ROI Vertex ����
void GetVertexPoints(Mat& src, vector<Point>& cornerPts, vector<Point>& vVertexPts, vector<Point>& hVertexPts, int distance)
{
    int rotateX, rotateY;
    double theta;

    int heightV = sqrt(pow(cornerPts[0].y - cornerPts[1].y, 2) + pow(cornerPts[0].x - cornerPts[1].x, 2));
    int widthV = distance;

    int heightH = distance;
    int widthH = sqrt(pow(cornerPts[1].y - cornerPts[2].y, 2) + pow(cornerPts[1].x - cornerPts[2].x, 2));


    if (cornerPts[1].x == cornerPts[0].x) {
        vVertexPts.push_back(cornerPts[0]);
        vVertexPts.push_back(cornerPts[1]);
        vVertexPts.push_back(Point(cornerPts[1].x + distance, cornerPts[1].y));
        vVertexPts.push_back(Point(cornerPts[0].x + distance, cornerPts[0].y));
    }
    else {
        theta = atan((double)((cornerPts[0].x - cornerPts[1].x)) / (cornerPts[0].y - cornerPts[1].y));

        vVertexPts.push_back(cornerPts[0]);

        vVertexPts.push_back(cornerPts[1]);

        rotateX = widthV * cos(theta) + heightV * sin(theta) + cornerPts[0].x;
        rotateY = -widthV * sin(theta) + heightV * cos(theta) + cornerPts[0].y;
        vVertexPts.push_back(Point(rotateX, rotateY));

        rotateX = (distance)*cos(theta) + cornerPts[0].x;
        rotateY = -(distance)*sin(theta) + cornerPts[0].y;
        vVertexPts.push_back(Point(rotateX, rotateY));

    }

    if (cornerPts[2].y == cornerPts[1].y) {
        hVertexPts.push_back(Point(cornerPts[1].x, cornerPts[1].y - distance));
        hVertexPts.push_back(cornerPts[1]);
        hVertexPts.push_back(cornerPts[2]);
        hVertexPts.push_back(Point(cornerPts[2].x, cornerPts[2].y - distance));
    }
    else {
        theta = -atan((cornerPts[1].y - cornerPts[2].y) / (double)((cornerPts[1].x - cornerPts[2].x)));
        rotateX = -distance * sin(theta) + cornerPts[1].x;
        rotateY = -distance * cos(theta) + cornerPts[1].y;
        hVertexPts.push_back(Point(rotateX, rotateY));


        hVertexPts.push_back(cornerPts[1]);
        hVertexPts.push_back(cornerPts[2]);

        rotateX = widthH * cos(theta) - heightH * sin(theta) + cornerPts[1].x;
        rotateY = -widthH * sin(theta) - heightH * cos(theta) + cornerPts[1].y;
        hVertexPts.push_back(Point(rotateX, rotateY));
    }
}
// �� ���� Thread
unsigned WINAPI CirDetectionThread(void* para)
{
    CirDetectionParam* data = (CirDetectionParam*)para;
    CircleDetection(data->src, data->Circles, data->conerPts, data->radMin, data->radMax, data->value);
    return 0;
}
// �� ����
void CircleDetection(Mat& src, vector<Point>& vertexPts, vector<Point>& circles, int radMin, int radMax, int value) {
    vector<vector<Point>> vpts;
    vpts.push_back(vertexPts);

    Mat mask = Mat::zeros(src.size(), CV_8UC1);
    fillPoly(mask, vpts, Scalar(255, 255, 255), 8, 0);
    Mat ROI;
    bitwise_and(src, mask, ROI);


    Mat imgThreshold = ROI.clone();
    Mat imgHoughCir = ROI.clone();
    threshold(imgThreshold, imgThreshold, 100, 250, THRESH_BINARY_INV);

    vector<vector<Point>> contours;
    findContours(imgThreshold, contours, RETR_LIST, CHAIN_APPROX_SIMPLE); // ���ø�� ���ͼ��� ���̱�����

    vector<Vec3f> houghCircles;

    for (vector<Point> pts : contours) {
        double areaValue = contourArea(pts); // ������ ���
        if (areaValue < 50 && areaValue > 0) {
            Rect rc = boundingRect(pts);

            if (rc.tl().x - 3 < 0 || rc.tl().y - 3 < 0)
                continue;
            rc -= Point(3, 3);
            if (rc.tl().x + rc.width + 3 > src.cols || rc.tl().y + rc.height + 3 > src.rows)
                continue;
            rc += Size(3, 3);

            //�ߺ� ����
            if (IsContain(rc, circles))
                continue;

            Mat cirRect = imgHoughCir(rc);

            Differential(cirRect, cirRect);

            HoughCircles(cirRect, houghCircles, HOUGH_GRADIENT, 1, 6, 255, 5, radMin, radMax);
            if (houghCircles.size() != 0)
                circles.push_back(Point(houghCircles[0][0] + rc.tl().x, houghCircles[0][1] + rc.tl().y));
        }
    }
}
// �簢�� ������ �� ���� ����
bool IsContain(Rect rc, vector<Point>& cirCenters) {
    for (Point pt : cirCenters) {
        int rcX = rc.tl().x;
        int rcY = rc.tl().y;
        if (pt.x >= rcX && pt.x <= rcX + rc.width) {
            if (pt.y >= rcY && pt.y <= rcY + rc.height) {
                return true;
            }
        }
    }
    return false;
}
// �̺� �Լ�(prewitt Edge)
void Differential(Mat& src, Mat& dst) {
    Mat dstY, dstX;
    float prewittY[] = {
        -1,0,1,
        -1,0,1,
        -1,0,1
    };
    float prewittX[] = {
        -1,-1,-1,
        0,0,0,
        1,1,1
    };
    Mat maskY(3, 3, CV_32F, prewittY);
    Mat maskX(3, 3, CV_32F, prewittX);

    filter2D(src, dstY, CV_32F, maskY, Point(-1, -1), 3, 1);
    filter2D(src, dstX, CV_32F, maskX, Point(-1, -1), 3, 1);

    magnitude(dstY, dstX, dst);  

    dst.convertTo(dst, CV_8U);
}
// �ּ��������� ���� Line + Circle ǥ��
void Drawing(Mat& src, Mat& dst, vector<Point>& vCirCenters, vector<Point>& hCirCenters)
{
    if (!src.data)
        return;
    if (!dst.data)
        cvtColor(src, dst, COLOR_GRAY2BGR);
    Vec2f vEquation, hEquation;
    Point target, temp1, temp2;
    vEquation = LSM_Vertical(vCirCenters);
    hEquation = LSM_Horizontal(hCirCenters);

    target.x = cvRound((hEquation[1] - vEquation[1]) / (vEquation[0] - hEquation[0]));
    target.y = cvRound((vEquation[0] * (hEquation[1] - vEquation[1])) / (vEquation[0] - hEquation[0]) + vEquation[1]);

    // ��Ȳ�� ������ ������ �׸���
    temp1 = { cvRound(-vEquation[1] / vEquation[0]), 0 }; // y=0 �̰�, �ּ������� ������ �������� ������ ��
    temp2 = { cvRound((target.y + 20 - vEquation[1]) / vEquation[0]), target.y + 20 }; // target���� �� �� �Ʒ����� ��
    cout << vEquation[0] << " " << vEquation[1] << endl;
    line(dst, temp1, temp2, Scalar(0, 127, 255));
    temp1 = { dst.cols, cvRound(hEquation[0] * dst.cols + hEquation[1]) }; // x = src.cols �̰�, �ּ������� ������ �������� ������ ��
    temp2 = { target.x - 20, cvRound(hEquation[0] * (target.x - 20) + hEquation[1]) }; // target���� �� �� ������ ��
    line(dst, temp1, temp2, Scalar(0, 127, 255));

    // ���ӻ� X �׸���
    temp1 = { target.x - 7, target.y - 7 };
    temp2 = { target.x + 7,target.y + 7 };
    line(dst, temp1, temp2, Scalar(0, 255, 191));
    temp1 = { target.x - 7, target.y + 7 };
    temp2 = { target.x + 7,target.y - 7 };
    line(dst, temp1, temp2, Scalar(0, 255, 191));


    for (int i = 0; i < vCirCenters.size(); i++)
    {
        Point center(vCirCenters[i].x, vCirCenters[i].y);
        circle(dst, center, 5, Scalar(0, 0, 255), 1, -1);
    }

    for (int i = 0; i < hCirCenters.size(); i++)
    {
        Point center(hCirCenters[i].x, hCirCenters[i].y);
        circle(dst, center, 5, Scalar(0, 0, 255), 1, -1);
    }
}
// �ּ������� x, y��ǥ ����Ī �� ����� �� ���� ���� �ٽ� y=x ��Ī�̵�
Vec2f LSM_Vertical(vector<Point>& pts)
{
    //x=ay+b
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

    return result;
}
// �ּ������� �Լ�
Vec2f LSM_Horizontal(vector<Point>& pts)
{
    //y=ax+b
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
    //cout << "y=" << a << "x+" << b << endl; // y=ax+b

    return result;
}