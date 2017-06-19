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


using namespace std;
using namespace cv;

const char* filename = "../aps_repo/data/IMG_3691.MOV";


float calc_position(Mat& frame);
void marker_setup();
float convert_to_height(cv::Point2f& point);

Mat element_M = getStructuringElement( MORPH_ELLIPSE,
                                     Size( 5, 5 ));
Mat element_P = getStructuringElement( MORPH_ELLIPSE,
                                     Size( 3, 3 ));

VideoCapture video_capture;//video_capture(filename);

Mat frame;

Point2f origin, y_vec;

int main(int argc, char** argv )
{
    video_capture.open(1);



    if (!video_capture.isOpened())
    {
        cout  << "Could not open reference " << filename << endl;
        return -1;
    }


    Size video_size = Size((int) video_capture.get(CAP_PROP_FRAME_WIDTH),
                     (int) video_capture.get(CAP_PROP_FRAME_HEIGHT));



    //namedWindow("video", WINDOW_GUI_NORMAL);
    //namedWindow("debug1", WINDOW_GUI_NORMAL);
    //namedWindow("debug2", WINDOW_GUI_NORMAL);





    marker_setup();


    namedWindow("debug3", WINDOW_GUI_NORMAL);
    for(;;) //Show the image captured in the window and repeat
    {

        video_capture >> frame;

        if (frame.empty())
        {
            cout << " < < <  Game over!  > > > ";
            break;
        }
        //


        float altura = calc_position(frame);
        cout << "posicao: " << altura << endl;
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

        Scalar hsv_lower_1(100, 100, 30);
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


    origin = (centers[bottom_left] + centers[bottom_right])/2.0f;

    y_vec = (centers[top_left] + centers[top_right] -(centers[bottom_left] + centers[bottom_right]))/2.0f;

    float normsq = y_vec.dot(y_vec);
    y_vec = y_vec/normsq;

    return;


}

float convert_to_height(cv::Point2f& point)
{
    return y_vec.dot(point-origin);
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

    Scalar color(0, 0, 255);
    circle(frame, Point(int(p1.x), int(p1.y)), 20, color, 4);
    imshow("debug3", frame);

    return convert_to_height(p1);

    /*vector<vector<Point> > contours;
     *


    findContours(yellow_mask, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

    if(contours.size() > 0)
    {

        int min_size = INT_MAX, min_idx;
        for(int i = 0; i < contours.size(); i++)
         {
            if(contours[i].size() < min_size)
            {
                min_size = contours[i].size() ;
                min_idx = i;
            }
        }
        Point2f center(0.0f, 0.0f);
        float radius;
        minEnclosingCircle(contours[min_idx], center, radius);

        Scalar color(0, 0, 255);
        circle(frame, Point(int(center.x), int(center.y)), int(radius), color, 4);
        imshow("debug3", frame);
    }*/

            /*
    cnts = cv2.findContours(mask.copy(), cv2.RETR_EXTERNAL,
        cv2.CHAIN_APPROX_SIMPLE)[-2]
    center = None

    # only proceed if at least one contour was found
    if len(cnts) > 0:
        # find the largest contour in the mask, then use
        # it to compute the minimum enclosing circle and
        # centroid
        c = max(cnts, key=cv2.contourArea)
        ((x, y), radius) = cv2.minEnclosingCircle(c)
        M = cv2.moments(c)
        center = (int(M["m10"] / M["m00"]), int(M["m01"] / M["m00"]))

        # only proceed if the radius meets a minimum size
        if radius > 10:
            # draw the circle and centroid on the frame,
            # then update the list of tracked points
            cv2.circle(frame, (int(x), int(y)), int(radius),
                (0, 255, 255), 2)
            cv2.circle(frame, center, 5, (0, 0, 255), -1)*/


}



