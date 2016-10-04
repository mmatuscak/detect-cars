#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/video/background_segm.hpp>
#include <opencv2/core.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/imgproc.hpp>

#include "draw.hpp"

#define MIN_AREA     2000
#define FROM_LEFT    300
#define FROM_RIGHT   35
#define CAR_SIZE     5000

using namespace cv;

// NEED MANY MORE MEASUREMENTS
int speed_estimate(long count){
    int speed = count * .4;

    return speed;
}

// This function draws the verticle lines and 
// estimates the speed of the vehicles.
// TODO major refactoring into better logical groupings
void draw_contours(Mat *forMask, Mat *img)
{
    Size sz = img->size();
    int rows = sz.height;
    int cols = sz.width;
    bool countFlag = false;
    long count = 0;
    std::vector<std::vector<Point> > contours;
    std::vector<Vec4i> hierarchy;

    findContours(*forMask, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0,0));

    //drawContours(img, contours, -1, Scalar(0,0,255), 2);
    std::vector<Rect> boundRect(contours.size());
    std::vector<std::vector<Point> > contours_poly(contours.size());

    for (size_t i = 0; i < contours.size(); ++i) {
        approxPolyDP(Mat(contours[i]), contours_poly[i], 3, true);
        boundRect[i] = boundingRect(Mat(contours_poly[i]));
    }

    // Draw verticle lines
    line(*img, Point(rows-FROM_RIGHT, 0), Point(rows-FROM_RIGHT, cols), Scalar(0, 0, 255), 1, 8, 0);
    line(*img, Point(rows-FROM_LEFT, 0), Point(rows-FROM_LEFT, cols), Scalar(255, 0, 255), 1, 8, 0);
    for (size_t i = 0; i < contours.size(); ++i) {
        rectangle(*img, boundRect[i].tl(), boundRect[i].br(), Scalar(0,255,0), 1, 8, 0);
        boundRect.erase(remove_if(boundRect.begin(),boundRect.end(),
                    [] (Rect r)
                    {
                    auto min_area = MIN_AREA;
                    return r.area() < min_area;
                    }), boundRect.end());
        
        if (abs(boundRect[i].tl().x - (rows - FROM_RIGHT)) < 15 && contourArea(contours[i]) > CAR_SIZE) {
            countFlag = true;
            //cout << contourArea(contours[i]) << '\n' << endl;
        }

        if (countFlag) {
            if (abs(boundRect[i].tl().x - (rows - FROM_LEFT)) < 15 && contourArea(contours[i]) > CAR_SIZE) {
                countFlag = true;
                std::cout << "\nThe count is: " << count << '\n';
                //cout << "The contour Area is: " << contourArea(contours[i]) << '\n' << endl;
            }
            ++count;
            //if (count > 200000000000) 
            //    count = 0;
            if (contourArea(contours[i]) > CAR_SIZE && ((abs(boundRect[i].tl().x - (rows - 10)) < 15) || (abs(boundRect[i].tl().x - (rows - 400)) < 15))){ 
                countFlag = false;
                std::cout << "\nThe count is: " << count << '\n';
            }
        }
    }
}
