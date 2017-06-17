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
#include <opencv2/cudacodec.hpp>
#include <opencv2/cudaarithm.hpp>
//#include <opencv2/cudalegacy.hpp>
#include "inRange.cuh"

using namespace std;
using namespace cv;

const char* filename = "../aps_repo/data/IMG_3691.MOV";


float calc_position(Mat& frame);

Mat element_40 = getStructuringElement(MORPH_ELLIPSE, Size( 40, 40));
Mat element_6 = getStructuringElement(MORPH_ELLIPSE, Size( 6, 6));

Ptr<cv::cuda::Filter> erode_b = cuda::createMorphologyFilter(MORPH_OPEN, CV_8UC1, element_6);

Ptr<cv::cuda::Filter> close_30 = cuda::createMorphologyFilter(MORPH_CLOSE, CV_8UC1, element_40);
cv::cuda::GpuMat dFrame, dhsv_frame, dGreen_background, dwhite, dGreen_background_eroded, dbloninhas;

int main(int argc, char** argv )
{

    VideoCapture video_capture(0);//video_capture(filename);


    if (!video_capture.isOpened())
    {
        cout  << "Could not open reference " << filename << endl;
        return -1;
    }

    //cout << "FORMAT " << video_capture.get(CAP_PROP_FORMAT) << endl;
    //video_capture.set(CAP_PROP_FORMAT, CAP_MODE_RGB)
    Size video_size = Size((int) video_capture.get(CAP_PROP_FRAME_WIDTH),
                     (int) video_capture.get(CAP_PROP_FRAME_HEIGHT));



    namedWindow("video", WINDOW_GUI_NORMAL);
    namedWindow("debug1", WINDOW_GUI_NORMAL);
    namedWindow("debug2", WINDOW_GUI_NORMAL);

    dhsv_frame.create(video_size.height, video_size.width, CV_8UC3);
    dGreen_background.create(video_size.height, video_size.width, CV_8UC1);
    dwhite.create(video_size.height, video_size.width, CV_8UC1);

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
         waitKey(1);

        float altura = calc_position(frame);

        //cout << "posicao: " << altura << endl;

    }

    return 0;
}


float calc_position(Mat& frame)
{

    dFrame.upload(frame);
    cv::cuda::cvtColor(dFrame, dhsv_frame, CV_RGB2HSV);

    Scalar hsv_lower_1(60, 80, 40);
    Scalar hsv_upper_1(100, 255, 190);

    inRange_gpu(dhsv_frame, hsv_lower_1, hsv_upper_1, dGreen_background);


   close_30->apply(dGreen_background, dGreen_background_eroded);

   Mat show;
   dGreen_background_eroded.download(show);
   imshow("debug1", show);


    //dGreen_background_eroded.download(show);
    //imshow("debug2", show);


    Scalar hsv_lower_2(0, 0, 100);
    Scalar hsv_upper_2(255, 50, 255);
    inRange_gpu(dhsv_frame, hsv_lower_2, hsv_upper_2, dwhite);

    dwhite.download(show);
    imshow("debug2", show);

    cv::cuda::bitwise_and(dGreen_background_eroded, dwhite, dbloninhas);

    erode_b->apply(dbloninhas, dGreen_background_eroded);
    dGreen_background_eroded.download(show);
    imshow("debug3", show);




/*
    Mat labels, stats, centroids;

    int n = connectedComponentsWithStats(show,labels,stats,centroids,8,CV_8UC1);


    for( size_t i = 0; i < n; i++ )
    {
        cv::Point center(cvRound(centroids.at(i, 0), cvRound(centroids.at(i, 1))));

        // draw the circle outline
        circle( show, center, 4, cv::Scalar(0,0,255), 3, 8, 0 );

    }
    imshow("test", show);*/

}



