#include <iostream> // for standard I/O
#include <string>   // for strings
#include <iomanip>  // for controlling float print precision
#include <sstream>  // string to number conversion
#include <opencv2/core.hpp>     // Basic OpenCV structures (cv::Mat, Scalar)
#include <opencv2/imgproc.hpp>  // Gaussian Blur
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>  // OpenCV window I/O
using namespace std;
using namespace cv;

const char* filename = "../aps_repo/data/IMG_3691.MOV";
int delay = 1;


float calc_position(Mat& frame);

Mat element_30 = getStructuringElement(MORPH_ELLIPSE, Size( 30, 30));
Mat element_60 = getStructuringElement(MORPH_ELLIPSE, Size( 60, 60));


int main(int argc, char** argv )
{
    VideoCapture video_capture(filename);

    if (!video_capture.isOpened())
    {
        cout  << "Could not open reference " << filename << endl;
        return -1;
    }

    Size video_size = Size((int) video_capture.get(CAP_PROP_FRAME_WIDTH),
                     (int) video_capture.get(CAP_PROP_FRAME_HEIGHT));



    namedWindow("video", WINDOW_GUI_NORMAL);
    namedWindow("debug1", WINDOW_GUI_NORMAL);
    namedWindow("debug2", WINDOW_GUI_NORMAL);
    namedWindow("debug3", WINDOW_GUI_NORMAL);
    namedWindow("debug4", WINDOW_GUI_NORMAL);
    namedWindow("debug5", WINDOW_GUI_NORMAL);

    Mat frame;

    for(;;) //Show the image captured in the window and repeat
    {
        video_capture >> frame;

        if (frame.empty())
        {
            cout << " < < <  Game over!  > > > ";
            break;
        }
        imshow("video", frame);

        float altura = calc_position(frame);

        cout << "posicao: " << altura << endl;


        char c = (char)waitKey(delay);
        if (c == 27) break;
    }
    return 0;
}

float calc_position(Mat& frame)
{
    Mat hsv_frame;
    cvtColor(frame, hsv_frame, CV_BGR2HSV);



    Mat  green_background, green_background_open, green_background_close;
    Scalar hsv_upper_l(60, 30, 30);
    Scalar hsv_upper_h(180, 255, 255);

    inRange(hsv_frame, hsv_upper_l, hsv_upper_h, green_background);
    imshow("debug1", green_background);



    morphologyEx( green_background, green_background_open, MORPH_OPEN , element_30 );

    imshow("debug2", green_background_open);

    morphologyEx( green_background_open, green_background_close, MORPH_CLOSE, element_60 );

    imshow("debug3", green_background_close);

    Mat bolinha;
    Scalar hsv_upper_l1(0, 0, 100);
    Scalar hsv_upper_h1(360, 50, 255);
    inRange(hsv_frame, hsv_upper_l1, hsv_upper_h1, bolinha);
    imshow("debug4", bolinha);

    Mat bolinha_isolada;

    bitwise_and(bolinha, green_background_close, bolinha_isolada);

    imshow("debug5", bolinha_isolada);

}



