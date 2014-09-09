#include <iostream>
#include "lineReconstruction.h"
#include <math.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "support//Trie.hxx"
#include "support/Exception.h"
#include "thinning/thinning.h"
using namespace cv;
using namespace std;


int pos[20000][6];
double points[10];


class InfoPoint
{
    public:
    int x;
    int y;
    int direccion;
    int endCode;
    int endCodeDir;
    bool state;
    InfoPoint()
    {
        this->x = 0;
        this->y = 0;
        this->direccion = 0;
        this->endCode = 0;
        this->endCodeDir = 0;
        this->state = true;
    }

    InfoPoint(int xi, int yi, int dir, int encode, int  encodedir)
    {
        this->x = xi;
        this->y = yi;
        this->direccion = dir;
        this->endCode = encode;
        this->endCodeDir = encodedir;
    }

    void SetAttInfoPoint(int xi, int yi, double dir, int encode, int  encodedir)
    {
        this->x = xi;
        this->y = yi;
        this->direccion = dir;
        this->endCode = encode;
        this->endCodeDir = encodedir;
    }
};

typedef std::vector<InfoPoint> VecPoints;

int isValid(int j, int i, Mat& src1, int x, int y)
{
    uchar pk =  src1.at<uchar>(j,i);
    uchar p =  src1.at<uchar>(j,i) == 255   ? 0:1;
    if(pk==0 && (j>y && i>x) && (j<src1.rows -y && i<src1.cols-x))
    {

         uchar p1 = src1.at<uchar>(j-1,i)== 255   ? 0:1;
         uchar p2 = src1.at<uchar>(j-1,i+1)== 255   ? 0:1;
         uchar p3 = src1.at<uchar>(j,i+1)== 255   ? 0:1;
         uchar p4 = src1.at<uchar>(j+1,i+1)== 255   ? 0:1;
         uchar p5 = src1.at<uchar>(j+1,i)== 255   ? 0:1;
         uchar p6 = src1.at<uchar>(j+1,i-1)== 255   ? 0:1;
         uchar p7 = src1.at<uchar>(j,i-1)== 255   ? 0:1;
         uchar p8 = src1.at<uchar>(j-1,i-1)== 255   ? 0:1;

         int val = getCode(p1,p2,p3,p4,p5,p6,p7,p8);
         return val;
    }

    return -1;
}

double dist(int x0, int x1, int y0, int y1)
{
     double x = (x0-x1)*(x0-x1);
     double y = (y1-y0)*(y1-y0);
     return sqrt(x+y);
}

void execLineReconstruction(cv::Mat& src, cv::Mat& dst, int h)
{

    /*PARAMETROS*/

    h = 40;
    int limit = 20;
    int limitX = 3;
    int limitY = 3;
    int inc = 5;
    int Max = 100;
    int hInit = h;



    Mat dst2 = src.clone();
    Mat src1 = src.clone();

    dst = src.clone();
    int nPoints = 0;
    int j, i, v, t;

    for(j=1;j<(src.rows);j++)
    {

        for (i=1;i<(src.cols);i++)
        {
            //Mat src2 = src1.clone();
            //Mat src3 = src1.clone();

           //DETERMINO SI ES UN PUNTO DE FIN
           int endVal = isValid(j,i, src1, limitX, limitY);
           if(isEndPoint(endVal))
                {
                    //CALCULO LIMITES DE LA MASCARA
                    int up = j-(h/2);
                    int down = j+(h/2);
                    if(up < 0)
                    {
                        up = 0;
                    }
                    if(down>src.rows)
                    {
                        down = src.rows;
                    }

                    int left = i-(h/2);
                    int right = i+(h/2);
                    if(left < 0)
                    {
                        left = 0;
                    }
                    if(right>src.cols)
                    {
                        right = src.cols;
                    }

                    /*****************************************/

                    Point p = nextPoint(i,j,limit, src1);
                    double dirPoint = dirEndPoint(i, j, 5, src1);
                    int d1 = (int ) changeDir(round(dirPoint));



                    cout<<"j"<<j<<endl;
                    cout<<"i"<<i<<endl;
                    cout<<d1<<endl;

                    int xd = -1;
                    int yd = -1;
                    double max = INFINITY;

                    for(v=up; v<down;  v++)
                    {
                        for(t=left; t<right;  t++)
                        {
                            //Mat src4 = src1.clone();
                            int endValInner = isValid(v,t, src1, limitX, limitY);
                            if(endValInner>=0)
                            {
                                if(isEndPoint(endValInner))
                                {
                                    if(v!=j || t!=i)
                                    {
                                        if(p.x != t && p.y != v)
                                        {
                                            cout<<v<<endl;
                                            cout<<t<<endl;
                                            double dirCandidate = dirEndPoint(t, v, 5, src1);
                                            int d2 = (int ) changeDir(round(dirCandidate));
                                            cout<<"Candidate"<<d2<<endl;
                                            if(deletePoints(d1, d2))
                                            {

                                                        if(abs(j-v)< max)
                                                        {
                                                            max = abs(j-v);
                                                            yd = v;
                                                            xd = t;
                                                        }

                                            }
                                        }
                                    }
                                }
                            }


                        }

                    }

                    if(xd != -1 && yd != -1)
                    {
                        line(src1,Point(i,j),Point(xd,yd),CV_RGB(255,0,0),1,1);

                    }
                    else
                    {

                    }

                    cout<<"***************"<<endl;
                    nPoints++;
                }




        }
    }




    imwrite("c:/img/result222.png", src1);
    execThinningGuoHall(src1, src1);
    /*h = h/2;
    hInit = h;
*/
    for(j=1;j<(src.rows);j++)
    {
        h = hInit;
        for (i=1;i<(src.cols);i++)
        {
            //Mat src2 = src1.clone();
            //Mat src3 = src1.clone();

           //DETERMINO SI ES UN PUNTO DE FIN
           int endVal = isValid(j,i, src1, limitX, limitY);
           if(isEndPoint(endVal))
                {
                    //CALCULO LIMITES DE LA MASCARA
                    int up = j-(h/2);
                    int down = j+(h/2);
                    if(up < 0)
                    {
                        up = 0;
                    }
                    if(down>src.rows)
                    {
                        down = src.rows;
                    }

                    int left = i-(h/2);
                    int right = i+(h/2);
                    if(left < 0)
                    {
                        left = 0;
                    }
                    if(right>src.cols)
                    {
                        right = src.cols;
                    }

                    /*****************************************/

                    Point p = nextPoint(i,j,h, src1);
                    double dirPoint = dirEndPoint(i, j, 10, src1);
                    //int d1 = (int ) changeDir(round(dirPoint));
                    switch((int)dirPoint)
                    {
                        case 0:
                            up = j;
                            down = j+h;
                            if(up < 0)
                            {
                                up = 0;
                            }
                            if(down>src.rows)
                            {
                                down = src.rows;
                            }

                            left = i-(h/2);
                            right = i+(h/2);
                            if(left < 0)
                            {
                                left = 0;
                            }
                            if(right>src.cols)
                            {
                                right = src.cols;
                            }
                        break;

                        case 1:
                             up = j;
                            down = j+h;
                            if(up < 0)
                            {
                                up = 0;
                            }
                            if(down>src.rows)
                            {
                                down = src.rows;
                            }

                            left = i-(h/2);
                            right = i+(h/2);
                            if(left < 0)
                            {
                                left = 0;
                            }
                            if(right>src.cols)
                            {
                                right = src.cols;
                            }
                        break;

                        case 2:
                            up = j-(h/2);
                            down = j+(h/2);
                            if(up < 0)
                            {
                                up = 0;
                            }
                            if(down>src.rows)
                            {
                                down = src.rows;
                            }

                            left = i-h;
                            right = i;
                            if(left < 0)
                            {
                                left = 0;
                            }
                            if(right>src.cols)
                            {
                                right = src.cols;
                            }
                        break;

                        case 3:
                            up = j-h;
                            down = j;
                            if(up < 0)
                            {
                                up = 0;
                            }
                            if(down>src.rows)
                            {
                                down = src.rows;
                            }

                            left = i-(h/2);
                            right = i+(h/2);
                            if(left < 0)
                            {
                                left = 0;
                            }
                            if(right>src.cols)
                            {
                                right = src.cols;
                            }
                        break;

                        case 4:
                             up = j-h;
                             down = j;
                            if(up < 0)
                            {
                                up = 0;
                            }
                            if(down>src.rows)
                            {
                                down = src.rows;
                            }

                             left = i-(h/2);
                             right = i+(h/2);
                            if(left < 0)
                            {
                                left = 0;
                            }
                            if(right>src.cols)
                            {
                                right = src.cols;
                            }
                        break;

                        case 5:
                            up = j-h;
                            down = j;
                            if(up < 0)
                            {
                                up = 0;
                            }
                            if(down>src.rows)
                            {
                                down = src.rows;
                            }

                            left = i-(h/2);
                            right = i+(h/2);
                            if(left < 0)
                            {
                                left = 0;
                            }
                            if(right>src.cols)
                            {
                                right = src.cols;
                            }
                        break;

                        case 6:
                             up = j-(h/2);
                             down = j+(h/2);
                            if(up < 0)
                            {
                                up = 0;
                            }
                            if(down>src.rows)
                            {
                                down = src.rows;
                            }

                             left = i;
                             right = i+h;
                            if(left < 0)
                            {
                                left = 0;
                            }
                            if(right>src.cols)
                            {
                                right = src.cols;
                            }
                        break;

                        case 7:
                           up = j;
                           down = j+h;
                           if(up < 0)
                           {
                               up = 0;
                           }
                           if(down>src.rows)
                           {
                               down = src.rows;
                           }

                           left = i-(h/2);
                           right = i+(h/2);
                           if(left < 0)
                           {
                               left = 0;
                           }
                           if(right>src.cols)
                           {
                               right = src.cols;
                           }
                        break;

                    }
                    cout<<"j"<<j<<endl;
                    cout<<"i"<<i<<endl;
                    //cout<<d1<<endl;

                    int xd = -1;
                    int yd = -1;
                    double max = INFINITY;

                    for(v=up; v<down;  v++)
                    {
                        for(t=left; t<right;  t++)
                        {
                            //Mat src4 = src1.clone();
                            int endValInner = isValid(v,t, src1, limitX, limitY);
                            if(endValInner>=0)
                            {
                                if(isEndPoint(endValInner))
                                {
                                    if(v!=j || t!=i)
                                    {
                                        if(p.x != t && p.y != v)
                                        {
                                            cout<<v<<endl;
                                            cout<<t<<endl;
                                            double dirCandidate = dirEndPoint(t, v, 10, src1);
                                            //int d2 = (int ) changeDir(round(dirCandidate));
                                            //cout<<"Candidate"<<d2<<endl;
                                            if(dirCandidate != dirPoint)
                                            {
                                                double distan = dist(i,t,j,v);
                                                if(distan< max)
                                                {
                                                    max = distan;
                                                    yd = v;
                                                    xd = t;
                                                }
                                            }
                                        }
                                    }
                                }
                            }


                        }

                    }

                    if(xd != -1 && yd != -1)
                    {
                        line(src1,Point(i,j),Point(xd,yd),CV_RGB(255,0,0),1,1);

                    }
                    else
                    {
                        if(h<= Max)
                        {
                            i = i-1;
                            h = h+inc;
                        }

                    }

                    cout<<"***************"<<endl;

                    nPoints++;
                }




        }
    }
    imwrite("c:/img/result2.png", src1);
}

void oexecLineReconstruction(cv::Mat& src, cv::Mat& dst, int h)
{

    /**************NUEVOS PARAMETROS**********************/

    int limitSearch = 10;


    //Mat dst2 = imread("c:/img/result.png", CV_LOAD_IMAGE_COLOR);
    //int h = 10;
    //Mat src = imread("c:/img/result.png", CV_LOAD_IMAGE_GRAYSCALE);

    Mat dst2 = src.clone();
    Mat src1 = src.clone();
    dst = src.clone();
    //cvtColor(dst, dst2, CV_GRAY2BGR);
    int nPoints = 0;

        for(int j=1;j<(src.rows);j++)
        {
            for (int i=1;i<(src.cols);i++)
            {

               uchar pk =  src1.at<uchar>(j,i);
               uchar p =  src1.at<uchar>(j,i) == 255   ? 0:1;
               if(pk==0)
               {

                    uchar p1 = src1.at<uchar>(j-1,i)== 255   ? 0:1;
                    uchar p2 = src1.at<uchar>(j-1,i+1)== 255   ? 0:1;
                    uchar p3 = src1.at<uchar>(j,i+1)== 255   ? 0:1;
                    uchar p4 = src1.at<uchar>(j+1,i+1)== 255   ? 0:1;
                    uchar p5 = src1.at<uchar>(j+1,i)== 255   ? 0:1;
                    uchar p6 = src1.at<uchar>(j+1,i-1)== 255   ? 0:1;
                    uchar p7 = src1.at<uchar>(j,i-1)== 255   ? 0:1;
                    uchar p8 = src1.at<uchar>(j-1,i-1)== 255   ? 0:1;

                    int val = getCode(p1,p2,p3,p4,p5,p6,p7,p8);

                    if(isEndPoint(val))
                    {

                        pos[nPoints][0] = j;
                        pos[nPoints][1] = i;
                        pos[nPoints][2] = val;
                        pos[nPoints][3] = 0;
                        pos[nPoints][4] = 0;
                        pos[nPoints][5] = 0;

                        nPoints++;
                    }

               }


            }
        }
        Mat src2 = src.clone();
        for(int k=0; k<nPoints; k++)
        {

            int y0 = pos[k][0];
            int x0 = pos[k][1];
            //int dir = pos[k][2];
            int flag = pos[k][3];
            int val = pos[k][2];
            int a = val;
            double v1 = dirEndPoint(x0, y0, 50, src2);
            pos[k][2] = (int ) changeDir(round(v1));
            int dir = pos[k][2];
            int xd = -1;
            int yd = -1;
            double max = INFINITY;
            cout<<k<<endl;
            cout<<y0<<endl;
            cout<<x0<<endl;
            if(flag != 1)
            {
                pos[k][3] = 1;
                Point p = nextPoint(x0, y0, limitSearch, src2);
                for(int m=0; m<nPoints; m++)
                {
                    int y1 = pos[m][0];
                    int x1 = pos[m][1];
                    bool a1 = pos[m][3]!= 1;
                    bool a2 = (y1 >= (y0-h) && y1 <= (y0+h));
                    bool a3 = (x1 >= (x0-h) && x1 <= (x0+h));
                    bool a4 = x1 != p.x && y1 != p.y;
                    if( a1 && a2  && a3 && a4)
                    {
                        pos[m][3] = 1;
                        pos[m][4] = y0;
                        pos[m][5] = x0;

                        int yi = pos[m][0];
                        int xi = pos[m][1];
                        double vi = dirEndPoint(xi, yi, limitSearch, src2);
                        pos[m][2] = (int ) changeDir(round(vi));
                        int y1 = pos[m][4];
                        int x1 = pos[m][5];
                        int dir1 = pos[m][2];


                        if(y1==y0 && x1==x0 && (yi != y0  || xi != x0))
                        {
                            if(!deletePoints(dir, dir1))
                            {
                                pos[m][3] = 0;

                            }
                            else{
                                if(abs(y0-y1)< max)
                                {
                                    max = abs(y0-y1);
                                    yd = yi;
                                    xd = xi;
                                }
                            }
                        }

                    }
                }

                if(xd != -1 && yd != -1)
                {
                    //Mat dst2 = dst.clone();

                    line(dst,Point(x0,y0),Point(xd,yd),CV_RGB(0,255,0),1,1);
                    line(dst2,Point(x0,y0),Point(xd,yd),CV_RGB(255,0,0),1,1);
                }
                cout<<"*****************************"<<endl;
            }


        }

        imwrite("c:/img/result2.png", dst2);


        /*for(int k=0; k<nPoints; k++)
        {
            Mat src3 = src.clone();
            cout<<k<<endl;
            int y0 = pos[k][0];
            int x0 = pos[k][1];
            int flag = pos[k][3];
            double v1 = dirEndPoint(x0, y0, 10, src3);
            pos[k][2] = (int ) changeDir(round(v1));
            int dir = pos[k][2];
            int xd = -1;
            int yd = -1;
            double max = INFINITY;
           // cout<<"************"<<endl;
            for(int m=0; m<nPoints; m++)
            {
                cout<<m<<endl;
                Mat src4 = src.clone();
                int yi = pos[m][0];
                int xi = pos[m][1];
                int flag = pos[m][3];
                double vi = dirEndPoint(xi, yi, 10, src4);
                pos[m][2] = (int ) changeDir(round(vi));
                int y1 = pos[m][4];
                int x1 = pos[m][5];
                int dir1 = pos[m][2];
                if(y1==y0 && x1==x0 && (yi != y0  || xi != x0))
                {
                    if(!deletePoints(dir, dir1))
                    {
                        pos[m][2] = 0;

                    }
                    else{
                        if(abs(y0-y1)< max)
                        {
                            max = abs(y0-y1);
                            yd = yi;
                            xd = xi;

                        }
                    }



                }
            }

            if(xd != -1 && yd != -1)
            {
                //Mat dst2 = dst.clone();

                line(dst,Point(x0,y0),Point(xd,yd),CV_RGB(0,255,0),1,1);
                line(dst2,Point(x0,y0),Point(xd,yd),CV_RGB(255,0,0),1,1);
            }
            cout<<"*****************************"<<endl;

        }

        imwrite("c:/img/result2.png", dst2);*/

}

bool isEndPoint(int x)
{
    return x==128 || x==2 || x==8 || x==32 || x==1 || x == 4|| x==16 || x== 32 || x==64;
}

int getCode(uchar p1, uchar p2, uchar p3, uchar p4, uchar p5, uchar p6, uchar p7, uchar p8)
{
    return (p8*64)+(p1*128)+(p2*1)+(p3*2)+ (p4*4) + (p5*8)+ (p6*16)+ (p7*32);
}

Point nextPoint(int x, int y, int limit, Mat dst)
{
    int paint[limit][2];

    for(int i=0; i<limit; i++)
    {
       int val = getDir(x,y, dst);
       int a = i;
       //stringstream ss;
      // ss << a;
       //string str = ss.str();
       dst.at<uchar>(y,x) = 255;

       paint[i][0] = y;
       paint[i][1] = x;

       int x0 = x;
       int y0 = y;

       switch(val)
       {
           case 1:
               y = y-1;
               x = x+1;
           break;
           case 2:
               x = x+1;
           break;
           case 4:
               y = y+1;
               x = x+1;
           break;
           case 8:
               y=y+1;
           break;
           case 16:
               y=y+1;
               x=x-1;
           break;
           case 32:
               x= x-1;
           break;
           case 64:
               y = y-1;
               x = x-1;
           break;
           case 128:
               y = y - 1;
           break;
       }
       dst.at<uchar>(y0,x0) = 0;
       int dir =getDir(x,y, dst);
       if(isEndPoint(dir))
       {
          for(int k = 0; k<=i; k++)
          {
              dst.at<uchar>(paint[k][0],paint[k][1]) = 0;
          }
          return Point(x,y);
       }
       dst.at<uchar>(y0,x0) = 255;

    }


    for(int k = 0; k<limit; k++)
    {
        dst.at<uchar>(paint[k][0],paint[k][1]) = 0;
    }
    return Point(-1,-1);
}

double dirEndPoint(int x, int y, int limit, Mat dst)
{
    double dir = 0.0;
    int cont = 0;
    int paint[limit][2];
    for(int i=0; i<limit; i++)
    {
        int vaj = dst.at<uchar>(y,x);
        if(dst.at<uchar>(y,x) == 0)
        {
            int val = getDir(x,y, dst);
            if(isEndPoint(val))
            {
                //cout<<val<<endl;
                //dir += getChainDir(val);
                points[i] = getChainDir(val);
                int a = i;
                //stringstream ss;
               // ss << a;
               // string str = ss.str();
                dst.at<uchar>(y,x) = 255;
                int x0 = x;
                int y0 = y;
                paint[i][0] = y;
                paint[i][1] = x;


                switch(val)
                {
                    case 1:
                        y = y-1;
                        x = x+1;
                    break;
                    case 2:
                        x = x+1;
                    break;
                    case 4:
                        y = y+1;
                        x = x+1;
                    break;
                    case 8:
                        y=y+1;
                    break;
                    case 16:
                        y=y+1;
                        x=x-1;
                    break;
                    case 32:
                        x= x-1;
                    break;
                    case 64:
                        y = y-1;
                        x = x-1;
                    break;
                    case 128:
                        y = y - 1;
                    break;
                }

                cont++;
            }
            else
            {
                break;
            }

        }


    }

    double p1 = points[0];
    double acum = 0.0;


    for(int k=0; k<cont; k++)
    {
        double p2 = points[k];
        if(abs(p1-p2)<=180)
        {
            p1 = (p1+p2)/2;
        }
        else
        {
            p1 = (p1+p2)/2;
            p1 = p1+180.0;
        }
        dst.at<uchar>(paint[k][0],paint[k][1]) = 0;
    }

     return getCodeDir(p1);
}

int getCodeDir(double d)
{
    if(d> 337.5 && d<= 22.5)
    {
        return 2;
    }
    if(d>22.5 && d<= 67.5)
    {
        return 1;
    }
    if(d>67.5 && d <= 112.5)
    {
        return 0;
    }
    if(d> 112.5 && d <= 157.5)
    {
        return 7;
    }
    if(d> 157.5 && d <= 202.5)
    {
        return 6;
    }
    if(d> 202.5 && d<= 247.5)
    {
        return 5;
    }
    if(d>247.5 & d<= 292.5)
    {
        return 4;
    }
    if(d>292.5 && d<=337.5)
    {
        return 3;
    }

    return 0;
}

int getDir(int x, int y, Mat dst)
{
       uchar p1 = dst.at<uchar>(y-1,x)== 255   ? 0:1;

       uchar p2 = dst.at<uchar>(y-1,x+1)== 255   ? 0:1;

       uchar p3 = dst.at<uchar>(y,x+1)== 255   ? 0:1;

       uchar p4 = dst.at<uchar>(y+1,x+1)== 255   ? 0:1;

       uchar p5 = dst.at<uchar>(y+1,x)== 255   ? 0:1;

       uchar p6 = dst.at<uchar>(y+1,x-1)== 255   ? 0:1;

       uchar p7 = dst.at<uchar>(y,x-1)== 255   ? 0:1;

       uchar p8 = dst.at<uchar>(y-1,x-1)== 255   ? 0:1;

       int val = getCode(p1,p2,p3,p4,p5,p6,p7,p8);

       return val;
}

double getChainDir(int dir)
{
    switch(dir)
    {
        case 1:
            return 45.0;
        case 2:
            return 0.0;
        case 4:
            return 315.0;
        case 8:
          return 270.0;
        case 16:
            return 224.0;
        case 32:
            return 180.0;
        case 64:
            return 135.0;
        case 128:
            return 90.0;
    }
}

int changeDir(int val)
{
    switch(val)
    {
        case 0:
            return 7;
        case 6:
            return 5;
        case 4:
            return 3;
        case 2:
            return 1;
        default:
            return val;
    }

}

bool deletePoints(int d1, int d2)
{
    switch(d1)
    {
        case 1:
            if(d2 == 5 || d2 == 3 || d2 == 7)
            {
                return true;
            }
        break;
        case 3:
            if(d2 == 7 || d2 == 5 || d2 == 1)
            {
                return true;
            }
        break;
        case 5:
            if(d2 == 7 || d2 == 3 || d2 == 1)
            {
                return true;
            }
        break;
        case 7:
            if(d2 == 5 || d2 == 3 || d2 == 1)
            {
                return true;
            }
        break;


    }

    return false;
}

char* parsePoint(int x, int y)
{
    char*a = "x";
    char*b = "y";
    stringstream sstm;
    sstm << a <<x<< b<<y;
    string ans = sstm.str();
    char * ans2 = (char *) ans.c_str();
    return ans2;
}

void execLineReconstructiondd(cv::Mat& src, cv::Mat& dst, int h)
{

   //InfoPoint points[src.rows][src.cols];}
    ToolBox::Trie<InfoPoint*>    trie(0);
    //PARAMETROS NUEVOS
    int x = 3;
    int y = 3;

    Mat dst2 = src.clone();
    Mat src1 = src.clone();
    Mat src3 = src.clone();

    //copyMakeBorder(src1, src1, 1, 1, 1, 1, BORDER_CONSTANT, 0);
   // copyMakeBorder(dst2, dst2, 1, 1, 1, 1, BORDER_CONSTANT, 0);
    dst = src.clone();
    cvtColor(dst, dst2, CV_GRAY2BGR);
    Mat endPoints = src1.clone();
    int nPoints = 0;
    for(int j=0;j<(src.rows);j++)
    {
            for (int i=0;i<(src.cols);i++)
            {
               uchar pk =  src1.at<uchar>(j,i);
               uchar p =  src1.at<uchar>(j,i) == 255   ? 0:1;
               if(pk==0 && (j>y && i>x) && (j<src.rows -y && i<src.cols-x))
               {

                    uchar p1 = src1.at<uchar>(j-1,i)== 255   ? 0:1;
                    uchar p2 = src1.at<uchar>(j-1,i+1)== 255   ? 0:1;
                    uchar p3 = src1.at<uchar>(j,i+1)== 255   ? 0:1;
                    uchar p4 = src1.at<uchar>(j+1,i+1)== 255   ? 0:1;
                    uchar p5 = src1.at<uchar>(j+1,i)== 255   ? 0:1;
                    uchar p6 = src1.at<uchar>(j+1,i-1)== 255   ? 0:1;
                    uchar p7 = src1.at<uchar>(j,i-1)== 255   ? 0:1;
                    uchar p8 = src1.at<uchar>(j-1,i-1)== 255   ? 0:1;
                    int val = getCode(p1,p2,p3,p4,p5,p6,p7,p8);

                    if(isEndPoint(val))
                    {
                        cout<<"*********"<<endl;
                       /* (points[j][i]).x = j;
                        (points[j][i]).y = i;*/

                       // int xi, int yi, double dir, int encode, int  encodedir
                        int dd = src3.at<uchar>(j,i);
                       // cout<<dd<<"}}}}"<<endl;
                        double v1 = dirEndPoint(i, j, 10, src3);
                        //cout<<v1<<endl;
                        int fdir = (int ) changeDir(round(v1));
                        InfoPoint* f = new InfoPoint(i,j,fdir,val, val);
                        char* key = parsePoint(i,j);
                        int size = strlen(key);
                        trie.addEntry(key, 32, f);
                        endPoints.at<uchar>(i,j) = 0;
                        InfoPoint* k = trie.getEntry(key, 32);
                        //cout<<k->endCode<<"......"<<endl;
                        //cout<<"*********"<<endl;
                    }
                    else
                    {
                        endPoints.at<uchar>(j,i) = 255;
                    }
               }
               else
               {
                   endPoints.at<uchar>(j,i) = 255;
               }
            }
    }


    for(int j=0;j<(src.rows);j++)
    {
            for (int i=0;i<(src.cols);i++)
            {
                /*if((points[j][i]).x != 0)
                {
                    cout<<(points[j][i]).x<<endl;
                }*/

            }
    }

    imwrite("d:/res/result2.png", endPoints);
}
