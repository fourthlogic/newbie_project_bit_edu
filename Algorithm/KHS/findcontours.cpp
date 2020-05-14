#include "opencv2/opencv.hpp" 
#include "time.h" 

void Contours(Mat& src, vector<vector<Point>>& Points);
void MyContour2(Mat& src, int sx, int sy, vector<Point>& outPoints, int * ptr);
void outerLine(Mat& src, Mat& dst, vector<Point>& outPoints, int sx = 0, int sy = 0);

void main()
{
    time_t start;
    time_t end;
    Mat src = imread("image/a1.png", IMREAD_GRAYSCALE);

    start = clock();

    vector<Point> cornerpts; // 외곽 3점 좌표 값

    vector<Point> vCirCenters; // Vertical의 중심 좌표들의 값 
    vector<Point> hCirCenters; // Horizontal의 중심 좌표들의 값 

    vector<Point> vertexPts;   //
    vector<vector<Point>> Points;

    uchar Data[]= {
          0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
          0,  0,  0,  0,  0,  0,255,255,255,255,  0,  0,  0,  0,  0,
          0,255,255,255,  0,  0,255,  0,  0,255,  0,  0,  0,  0,  0,
          0,255,  0,255,  0,  0,255,  0,  0,255,  0,  0,  0,  0,  0,
          0,255,  0,255,  0,  0,255,  0,  0,255,  0,  0,  0,  0,  0,
          0,255,255,255,  0,  0,255,  0,  0,255,  0,  0,  0,  0,  0,
          0,  0,  0,  0,  0,  0,255,  0,  0,255,  0,  0,  0,  0,  0,
          0,  0,  0,  0,  0,  0,255,255,255,255,  0,  0,  0,  0,  0,
          0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
          0,  0,100,100,100,100,100,100,  0,  0,  0,  0,  0,  0,  0,
          0,  0,100,  0,  0,  0,  0,100,  0,  0,255,255,  0,  0,  0,
          0,  0,100,  0,  0,  0,  0,100,  0,  0,255,255,  0,  0,  0,
          0,  0,100,  0,  0,  0,  0,100,  0,  0,255,255,  0,  0,  0,
          0,  0,100,100,100,100,100,100,  0,  0,  0,  0,  0,  0,  0,
          0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
    };
    Mat dst;
    Mat newSrc(15, 15, CV_8UC1, Data);
    //imshow("newSrc", newSrc);
    threshold(src, dst, 100, 255, THRESH_BINARY_INV);
    imshow("dst", dst);
    waitKey();
    Contours(dst, Points);


    end = clock();
    //imshow("result", dst);

    cout << end - start << endl;
    waitKey();
}


void Contours(Mat& src, vector<vector<Point>>& Points)
{
    int height = src.rows;
    int width = src.cols;
    int cnt = 0;
    uchar* ptr;
    int* Map = new int[height * width];
    memset(Map, 0, sizeof(int) * height * width);

    for (int y = 0; y < height; y++)
    {
        ptr = src.ptr<uchar>(y);
        for (int x = 0; x < width; x++)
        {
            if (Map[width * y + x] == 0 && ptr[x] == 255)
            {
                Mat dst;
                vector<Point> pts;
                MyContour2(src, x, y, pts, Map);
                //outerLine(src,dst, pts);
                if(pts.size()!=0)
                    Points.push_back(pts);
            }
            else
                continue;
            cnt++;
        }
    }
    
    for (vector<Point> pts : Points)
    {
        cout << cnt << "번째 " << pts.size() <<"개수" <<endl;
        for (Point pt: pts)
        {
            cout << pt;
        }
        cout<<endl;
    }

    //uchar** Map = new uchar*[y];
    //for (int i = 0; i < y; i++)
    //{
    //    Map[i] = new uchar[x];
    //    memset(Map[i], 0, sizeof(uchar) * x);
    //}


   /* for (int i = 0; i < y; i++)
    {
        for (int j = 0; j < x; j++)
        {

        }
    }*/


    //for (int i = 0; i < y; i++)
    //{
    //    for (int j = 0; j < x; j++)
    //    {
    //        cout << Map[i*y+j];
    //    }
    //    cout << endl;
    //}
    //waitKey();





    delete Map;

}

void MyContour2(Mat& src, int sx, int sy, vector<Point>& outPoints, int *ptr)
{
    if (src.channels() != 1)
        return;

    int h = src.rows;
    int w = src.cols;

    //outPoints.clear();

    int x, y, nx, ny;
    int d, cnt;
    int  dir[8][2] = {
        {  1,  0 },
        {  1,  1 },
        {  0,  1 },
        { -1,  1 },
        { -1,  0 },
        { -1, -1 },
        {  0, -1 },
        {  1, -1 }
    };

    x = sx;
    y = sy;
    d = cnt = 0;

    while (1)
    {
        nx = x + dir[d][0];
        ny = y + dir[d][1];

        //if (nx < 0 || nx >= w || ny < 0 || ny >= h || pSrc[ny * w + nx] == 0)
        if (nx < 0 || nx >= w || ny < 0 || ny >= h || src.ptr<uchar>(ny)[nx] == 0)
        {

            if (++d > 7) d = 0;
            cnt++;

            if(ny * w + nx>0)
                ptr[ny * w + nx] = 1;
  
            if (cnt >= 8)
            {
                outPoints.push_back(Point(x, y));
                ptr[y * w + x] = 1;
                break;  
            }
        }
        else
        {

            outPoints.push_back(Point(x, y));
            ptr[y * w + x] = 1;
            x = nx;
            y = ny;

            cnt = 0;
            d = (d + 6) % 8;
        }

        if (x == sx && y == sy && d == 0)
            break;
    }
}



void outerLine(Mat& src, Mat& dst, vector<Point>& outPoints, int sx, int sy)
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
            return;
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
                outPoints.push_back(Point(x, y));
                break;
            }
        }
        else
        {
            outPoints.push_back(Point(x, y));

            x = nx;
            y = ny;
            cnt = 0;

            d = (d + 3) % 4;
        }

        if (x == sx && y == sy && d == 0)
            break;
    }

    for (int i = 0; i < outPoints.size(); i++)
        dst.ptr<uchar>(outPoints[i].y)[outPoints[i].x] = 255;
    //imshow("outline", dst);
}







