#include <iostream> // for standard I/O
#include <string>   // for strings
#include <iomanip>  // for controlling float print precision
#include <sstream>  // string to number conversion
#include <opencv2/core.hpp>     // Basic OpenCV structures (cv::Mat, Scalar)
#include <opencv2/imgproc.hpp>  // Gaussian Blur
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>  // OpenCV window I/O
#include <opencv2/shape.hpp>


using namespace std;
using namespace cv;

const char* filename = "../aps_repo/data/IMG_3691.MOV";


float calc_position(Mat& frame);

Mat element = getStructuringElement( MORPH_ELLIPSE,
                                     Size( 3, 3 ));
int main(int argc, char** argv )
{

    VideoCapture video_capture(0);//video_capture(filename);


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
        waitKey(1);

        //cout << "posicao: " << altura << endl;

    }

    return 0;
}


float calc_position(Mat& frame)
{

    Mat hsv_frame, yellow_mask;
    cvtColor(frame, hsv_frame, CV_BGR2HSV);

    Scalar hsv_lower_1(20, 100, 100);
    Scalar hsv_upper_1(35, 255, 255);

    inRange(hsv_frame, hsv_lower_1, hsv_upper_1, yellow_mask);
    imshow("debug1", yellow_mask);



    erode(yellow_mask, yellow_mask, element);
    erode(yellow_mask, yellow_mask, element);
    dilate(yellow_mask, yellow_mask, element);
    dilate(yellow_mask, yellow_mask, element);
    imshow("debug2", yellow_mask);

    vector<vector<Point> > contours;


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
        circle(frame, Point(int(center.x), int(center.y)), int(radius), color);
        imshow("debug3", frame);
    }

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



