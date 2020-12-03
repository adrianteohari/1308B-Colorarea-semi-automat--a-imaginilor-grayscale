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
        "\tRight Click -paint with white \n"

        << endl;
}

Mat image0, image, color;

int ffillMode = 1;
int loDiff = 20, upDiff = 20;
int connectivity = 8;
int isColor = true;
int newMaskVal = 255;

int b = 0;
int g = 0;
int r = 0;

String WindowName = "ElPaint";
String WindowColor = "ElColorero";


static void onMouse(int event, int x, int y, int, void*)
{
    //in functie de event vom face altceva
    if (event == EVENT_LBUTTONDOWN)
    {
        Point seed = Point(x, y);

        int lo = loDiff;
        int up = upDiff;
        int flags = connectivity + (newMaskVal << 8) + (FLOODFILL_FIXED_RANGE);
        //parametrii pt floodfill
        Rect ccomp;
        Scalar newVal = Scalar(b, g, r);
        Mat dst = image;
        int area;

        //floodfill
        area = floodFill(dst, seed, newVal, &ccomp, Scalar(lo, lo, lo), Scalar(up, up, up), flags);

        imshow(WindowName, dst);
    }
    else
        if (event == EVENT_RBUTTONDOWN)
        {
            //punctul de unde incepem flood-ul
            Point seed = Point(x, y);


            int lo =  loDiff;
            int up =  upDiff;
            int flags = connectivity + (newMaskVal << 8) + (FLOODFILL_FIXED_RANGE);

            //floodfill
            Rect ccomp;
            Scalar newVal = Scalar(255, 255, 255);
            Mat dst = image;
            int area;


            area = floodFill(dst, seed, newVal, &ccomp, Scalar(lo, lo, lo), Scalar(up, up, up), flags);

            //actualizam imaginea
            imshow(WindowName, dst);
        }
        else return;


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
    //apelam help pentru scris
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