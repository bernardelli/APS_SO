#include <iostream> // for standard I/O
#include <string>   // for strings
#include <iomanip>  // for controlling float print precision
#include <sstream>  // string to number conversion
#include <opencv2/core.hpp>     // Basic OpenCV structures (cv::Mat, Scalar)
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>  // OpenCV window I/O
#include <opencv2/cudaimgproc.hpp>
using namespace std;
using namespace cv;

const char* filename = "../aps_repo/data/IMG_3691.MOV";
int delay = 1;


float calc_position(Mat& frame);


const float dp = 1.0f;
const float minDist = 5.0f;
const int minRadius = 1;
const int maxRadius = 100;
const int cannyThreshold = 120;
const int votesThreshold = 10;
const int maxCircles = 5;

cv::Ptr<cv::cuda::HoughCirclesDetector> houghCircles = cv::cuda::createHoughCirclesDetector(dp, minDist, cannyThreshold, votesThreshold, minRadius, maxRadius, maxCircles);

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
    namedWindow("debug", WINDOW_GUI_NORMAL);

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
}

float calc_position(Mat& frame)
{


    Mat viz;
    cv::cuda::GpuMat dFrame, dGray;
    dFrame.upload(frame);

   cv::cuda::cvtColor(dFrame, dGray, CV_BGR2GRAY);
   dGray.download(viz);


   cv::cuda::GpuMat dCircles;
   {


       houghCircles->detect(dGray, dCircles);

   }
   std::vector<cv::Vec3f> circlesGPU;
   if(!dCircles.empty())
   {

       circlesGPU.resize(dCircles.size().width);
       dCircles.row(0).download(cv::Mat(circlesGPU).reshape(3, 1));
   }

   std::vector<cv::Point> result;

   for( size_t i = 0; i < circlesGPU.size(); i++ )
   {
       cv::Point center(cvRound(circlesGPU[i][0]), cvRound(circlesGPU[i][1]));
       result.push_back(center);
       int radius = cvRound(circlesGPU[i][2]);
       // draw the circle center
       std::cout << center.x << " " << center.y  << " " << radius << std::endl;
       circle( viz, center, 3, cv::Scalar(0,255,0), -1, 8, 0 );
       // draw the circle outline
       circle( viz, center, radius, cv::Scalar(0,0,255), 3, 8, 0 );

   }

   imshow("video", viz);
}
