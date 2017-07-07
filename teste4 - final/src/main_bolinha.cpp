#include <iostream> // for standard I/O
#include <string>   // for strings
#include <iomanip>  // for controlling float print precision
#include <sstream>  // string to number conversion
#include <opencv2/core.hpp>     // Basic OpenCV structures (cv::Mat, Scalar)
#include <opencv2/imgproc.hpp>  // Gaussian Blur
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>  // OpenCV window I/O
#include <opencv2/shape.hpp>
#include <unistd.h>
#include <chrono>
#include "serial.h"


using namespace std;
using namespace cv;

const char* filename = "../aps_repo/data/IMG_3691.MOV";
float position;


float calc_position(Mat& frame);
void marker_setup();
float convert_to_height(cv::Point2f& point);

Mat element_M = getStructuringElement( MORPH_ELLIPSE,
                                     Size( 5, 5 ));
Mat element_P = getStructuringElement( MORPH_ELLIPSE,
                                     Size( 3, 3 ));

VideoCapture video_capture;//video_capture(filename);

Mat frame, view;

Point2f origin, y_vec;

Mat cameraMatrix, distCoeffs;

int main(int argc, char** argv )
{

    /*** setup serial ***/
    const char *portname = "/dev/ttyACM0";
    int fd;
    int wlen;


    fd = open(portname, O_RDWR | O_NOCTTY | O_SYNC);
    if (fd < 0) {
        printf("Error opening %s: %s\n", portname, strerror(errno));
        return -1;
    }
    /*baudrate 115200, 8 bits, no parity, 1 stop bit */
    set_interface_attribs(fd, B115200);
    set_mincount(fd, 0);                /* set to pure timed read */

    /*** load calib matrix ***/
    FileStorage fs2("../data/out_camera_data.xml", FileStorage::READ);


    fs2["Camera_Matrix"] >> cameraMatrix;
    fs2["Distortion_Coefficients"] >> distCoeffs;

    cout << distCoeffs << endl;

    /*** open video ***/
    video_capture.open(1);

    //video_capture.set(CV_CAP_PROP_FPS, 60);

    if (!video_capture.isOpened())
    {
        cout  << "Could not open reference " << filename << endl;
        return -1;
    }


    Size video_size = Size((int) video_capture.get(CAP_PROP_FRAME_WIDTH),
                     (int) video_capture.get(CAP_PROP_FRAME_HEIGHT));

    cout << "size x: " << video_size.width << " size y: " << video_size.height << endl;



    /*Create windows*/
    //namedWindow("video", WINDOW_GUI_NORMAL);
    //namedWindow("debug1", WINDOW_GUI_NORMAL);
    //namedWindow("debug2", WINDOW_GUI_NORMAL);
    //namedWindow("undistort", WINDOW_GUI_NORMAL);
    namedWindow("debug3", WINDOW_GUI_NORMAL);




    marker_setup();

    std::chrono::steady_clock::time_point old_begin, begin, end;

    for(;;) //Show the image captured in the window and repeat
    {
        end = std::chrono::steady_clock::now();
        old_begin = begin;
        begin = std::chrono::steady_clock::now();

        video_capture >> frame;

        //undistort(frame, view, cameraMatrix, distCoeffs);
        //imshow("undistort", view);

        if (frame.empty())
        {
            cout << " < < <  Game over!  > > > ";
            break;
        }
        //


        int altura = (int) 1024*calc_position(frame);
        altura = (altura > 1024)? 1024: altura;
        altura = (altura < 0)? 0 : altura;

        wlen = send_position(fd, altura);

        if (wlen != sizeof(uint16_t))
        {
            printf("Error sending bytes\n");
        }

        cout << "posicao: " << altura << "   ...  Time difference = " << std::chrono::duration_cast<std::chrono::milliseconds> ( end - old_begin).count() <<std::endl;
        waitKey(1);



    }

    return 0;
}


void marker_setup()
{
//find position of calibration blobs

    namedWindow("calibrate", WINDOW_GUI_NORMAL);
    Mat hsv_frame, violet_mask;
    int c  = '0';
    vector<vector<Point> > contours;
    vector<Point2f> centers;
    vector<float> radii;
    while( 1)
    {

        video_capture >> frame;



        boxFilter(frame, frame, frame.type(), Size(3,3));
        cvtColor(frame, hsv_frame, CV_BGR2HSV);

        Scalar hsv_lower_1(100, 100, 20);
        Scalar hsv_upper_1(255, 255, 255);

        inRange(hsv_frame, hsv_lower_1, hsv_upper_1, violet_mask);
        //imshow("debug1", yellow_mask);


        erode(violet_mask, violet_mask, element_P);
        erode(violet_mask, violet_mask, element_P);
        dilate(violet_mask, violet_mask, element_P);




       findContours(violet_mask, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);



       centers.reserve(contours.size());
       radii.reserve(contours.size());
       for(int i = 0; i < contours.size(); i++)
       {
            minEnclosingCircle(contours[i], centers[i], radii[i]);


           circle(frame, Point(int(centers[i].x), int(centers[i].y)), int(radii[i]), radii[i], 2);
           Scalar color(0, 0, 255);
           char text [2] = {('0' + i), '\0'};
           putText(frame, text,Point(int(centers[i].x), int(centers[i].y)),  FONT_HERSHEY_SIMPLEX, 2.5, color);

       }



        imshow("calibrate", frame);
         //if esc

        cout << "is input good? [y,n]: " << endl ;
        c = waitKey(0);


        if(c == 'y')
            break;



    }

     usleep(5000);


    int top_right, top_left, bottom_right, bottom_left;
    cout << "Index of top right ball: " << endl;
    c = waitKey(0);
    usleep(5000);

    top_right = (int)((char)c - '0');

    cout << top_right << endl;

    cout << "Index of top left ball: "<< endl;

    c = waitKey(0);
     usleep(5000);

     top_left = (int)((char)c - '0');

    cout << top_left << endl;

    cout << "Index of bottom right ball: "<< endl;
    c = waitKey(0);
     usleep(5000);

     bottom_right = (int)((char)c - '0');

    cout << bottom_right << endl;
    cout << "Index of bottom left ball: "<< endl;
    c = waitKey(0);
     usleep(5000);

     bottom_left = (int)((char)c - '0');

    cout << bottom_left << endl;

    cout << "OK! " << endl;
    waitKey(0);
    destroyWindow("calibrate");




    vector<Point2f> in = {centers[top_right], centers[top_left], centers[bottom_right], centers[bottom_left]}, out;
    undistortPoints(in, out, cameraMatrix, distCoeffs);

    origin = (out[2] + out[3])/2.0f;

    y_vec = (out[0] + out[1] -(out[2] + out[3]))/2.0f;

    float normsq = y_vec.dot(y_vec);
    y_vec = y_vec/normsq;

    return;


}

float convert_to_height(cv::Point2f& point)
{
    vector<Point2f> in = {point}, out;
    undistortPoints(in, out, cameraMatrix, distCoeffs);

    return y_vec.dot(out[0]-origin);
}


float calc_position(Mat& frame)
{



    Mat hsv_frame, yellow_mask;
    boxFilter(frame, frame, frame.type(), Size(3,3));
    cvtColor(frame, hsv_frame, CV_BGR2HSV);

    Scalar hsv_lower_1(19, 140, 80);
    Scalar hsv_upper_1(25, 255, 255);

    inRange(hsv_frame, hsv_lower_1, hsv_upper_1, yellow_mask);
    //imshow("debug1", yellow_mask);


    erode(yellow_mask, yellow_mask, element_P);
    erode(yellow_mask, yellow_mask, element_P);
    dilate(yellow_mask, yellow_mask, element_M);
    dilate(yellow_mask, yellow_mask, element_M);
    //imshow("debug2", yellow_mask);

    Moments m = moments(yellow_mask, false);
    Point2f p1(m.m10/m.m00, m.m01/m.m00);

    //cout << m.m00 << endl;

    if( m.m00 > 100)
    {
        Scalar color(0, 0, 255);
        circle(frame, Point(int(p1.x), int(p1.y)), 20, color, 4);
        imshow("debug3", frame);

        position= convert_to_height(p1);
    }


    return position;


}



