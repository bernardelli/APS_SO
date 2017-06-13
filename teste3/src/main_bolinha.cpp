#include <iostream> // for standard I/O
#include <string>   // for strings
#include <iomanip>  // for controlling float print precision
#include <sstream>  // string to number conversion
#include <opencv2/core.hpp>     // Basic OpenCV structures (cv::Mat, Scalar)
#include <opencv2/imgproc.hpp>  // Gaussian Blur
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>  // OpenCV window I/O
#include <opencv2/cudaimgproc.hpp>
#include <opencv2/cudafilters.hpp>
#include <opencv2/cudabgsegm.hpp>
#include "inRange.cuh"

using namespace std;
using namespace cv;

const char* filename = "../aps_repo/data/IMG_3691.MOV";


float calc_position(Mat& frame);

Mat element_30 = getStructuringElement(MORPH_ELLIPSE, Size( 30, 30));
Mat element_60 = getStructuringElement(MORPH_ELLIPSE, Size( 60, 60));

Ptr<cv::cuda::Filter> close_30 = cuda::createMorphologyFilter(MORPH_CLOSE, CV_8UC1, element_30);
Ptr<cv::cuda::Filter> erode_30 = cuda::createMorphologyFilter(MORPH_ERODE, CV_8UC1, element_30);

cv::cuda::GpuMat dFrame, dhsv_frame, dGreen_background, dGreen_background_eroded;

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
    dGreen_background.create(video_size.height, video_size.width, CV_8UC1);

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
    }
    return 0;
}

float calc_position(Mat& frame)
{

    dFrame.upload(frame);
    cv::cuda::cvtColor(dFrame, dhsv_frame, CV_BGR2HSV);




    Scalar hsv_lower_1(60, 30, 30);
    Scalar hsv_upper_1(180, 255, 255);

    inRange_gpu(dhsv_frame, hsv_lower_1, hsv_upper_1, dGreen_background);

    Mat show;
    //dGreen_background.download(show);
    //imshow("debug1", show);

    erode_30->apply(dGreen_background, dGreen_background_eroded);

    dGreen_background_eroded.download(show);
    imshow("debug2", show);

/*
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

    imshow("debug5", bolinha_isolada);*/

}



