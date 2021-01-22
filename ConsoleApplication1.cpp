
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <iostream>


using namespace cv;
using namespace std;

//print 
static void help()
{

    cout << "Hot keys: \n"
        "\tESC - quit the program\n"
        "\tr - restore the original image\n"
        "\tLeft Click -paint with the selected color\n"
       

        << endl;
}

Mat image0, image, color;

int ffillMode = 1;
int loDiff = 60, upDiff = 60;
int connectivity = 8;
int isColor = true;
int newMaskVal = 255;

int b = 0;
int g = 0;
int r = 0;

unsigned char hue = 0;
unsigned char sat = 0;

String WindowName = "ElPaint";
String WindowColor = "ElColorero";


typedef struct {
    double r;       // a fraction between 0 and 1
    double g;       // a fraction between 0 and 1
    double b;       // a fraction between 0 and 1
} rgb;

typedef struct {
    double h;       // angle in degrees
    double s;       // a fraction between 0 and 1
    double v;       // a fraction between 0 and 1
} hsv;


hsv rgb2hsv(rgb in)
{
    hsv         out;
    double      min, max, delta;

    min = in.r < in.g ? in.r : in.g;
    min = min < in.b ? min : in.b;

    max = in.r > in.g ? in.r : in.g;
    max = max > in.b ? max : in.b;

    out.v = max;                                // v
    delta = max - min;
    if (delta < 0.00001)
    {
        out.s = 0;
        out.h = 0; // undefined, maybe nan?
        return out;
    }
    if (max > 0.0) { // NOTE: if Max is == 0, this divide would cause a crash
        out.s = (delta / max);                  // s
    }
    else {
        // if max is 0, then r = g = b = 0              
        // s = 0, h is undefined
        out.s = 0.0;
        out.h = NAN;                            // its now undefined
        return out;
    }
    if (in.r >= max)                           // > is bogus, just keeps compilor happy
        out.h = (in.g - in.b) / delta;        // between yellow & magenta
    else
        if (in.g >= max)
            out.h = 2.0 + (in.b - in.r) / delta;  // between cyan & yellow
        else
            out.h = 4.0 + (in.r - in.g) / delta;  // between magenta & cyan

    out.h *= 60.0;                              // degrees

    if (out.h < 0.0)
        out.h += 360.0;

    out.s = 255;

    return out;
}

//H: 0-179, S: 0-255, V: 0-255
Mat HueShift(Mat frame) {


    Mat hsvMat;
    cvtColor(frame, hsvMat, COLOR_BGR2HSV);
    rgb temp;
    temp.r = r;
    temp.b = b;
    temp.g = g;

    hsv hs = rgb2hsv(temp);

    //Hue calc


    for (int j = 0; j < frame.rows; j++)
    {
        for (int i = 0; i < frame.cols; i++)
        {


            // Set hue.
            hsvMat.at<Vec3b>(j, i)[0] = hs.h / 2;
            hsvMat.at<Vec3b>(j, i)[1] = hs.s;

        }

    }
    Mat rez;

    cvtColor(hsvMat, rez, COLOR_HSV2BGR);


    return rez;
}

int thresh = 150;
void inside(Mat img,int x, int y) {
    Mat canny_output;
    //finds edges on the image with an algorithm
    Canny(img, canny_output, thresh, thresh * 2);

    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    //finds contours
    findContours(canny_output, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);

    //verificam daca punctul este in interior
    Mat mask = Mat::zeros(img.size(), CV_8U);


    int ok = 0;
    
    Scalar color = Scalar(255);
    for (size_t i = contours.size() -1; i > 0; i--)
    {
 
        ok=pointPolygonTest(contours.at(i), Point(x, y), false);
        if (ok == 1)
        {

            drawContours(mask, contours, (int)i, color, FILLED);

            break;
            }


    }
    Mat hue = Mat::zeros(mask.size(), CV_8UC3);

    bitwise_not(hue, hue);

    hue = HueShift(hue);

    Mat hueMask =Mat(mask.size(),CV_8UC3);

   
    bitwise_and(hue, hue, hueMask ,mask);
    
   
    
    addWeighted(hueMask, 0.5, img, 1, 0.0, img);
   

    
    //imshow("mask", mask);
    //imshow("hue", hueMask);
    imshow(WindowName, img);
    
    waitKey();


}

static void onMouse(int event, int x, int y, int, void*)
{
    //in functie de event vom face altceva
    if (event == EVENT_LBUTTONDOWN)
    {
        Point seed = Point(x, y);

       
        Mat dst = image;
        
        inside(dst, x, y);
    }
}
static void UpdateColor(int event, void*) {
    floodFill(color, cv::Point(0, 0), cv::Scalar(b, g, r));
    imshow(WindowColor, color);
}


void CreateTrackbars() {
    createTrackbar("Blue", WindowName, &b, 255, UpdateColor);
    createTrackbar("Green", WindowName, &g, 255, UpdateColor);
    createTrackbar("Red", WindowName, &r, 255, UpdateColor);

}
void Init() {
    //citim Imaginea si verificam daca exista;
    image0 = imread("Images/Color1.jpg", 1);
    if (image0.empty())
    {
        cout << "Image empty\n";

    }
    //apelam help pentru scris in consola
    help();

    //imaginea originala va fi folosita ca backup
    image0.copyTo(image);

    //initializam imaginea culoare
    color = Mat::zeros(cv::Size(40, 40), CV_8UC3);
    


}
void createUI() {

    namedWindow(WindowName, 0);
    namedWindow(WindowColor, 0);
    cv::resizeWindow(WindowName, 800, 800);
    cv::moveWindow(WindowName, 0, 0);

    cv::resizeWindow(WindowColor, 100, 100);
    cv::moveWindow(WindowColor, 800, 0);

    //cele 3 slidere pt culoare
    CreateTrackbars();

    //setam ce sa se intample la click
    setMouseCallback(WindowName, onMouse, 0);
}

int main()
{

    Init();
    createUI();

    //main loop
    while (1)
    {

        imshow(WindowName, image);

        char c = (char)waitKey(0);
        if (c == 27)
        {
            cout << "Exiting ...\n";
            break;
        }

        switch (c)
        {

        case 'r':
            cout << "Original image is restored\n";
            image0.copyTo(image);
            break;

        }
    }
    return 0;
}

