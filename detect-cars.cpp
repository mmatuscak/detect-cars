#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/video/background_segm.hpp>
#include <opencv2/core.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/imgproc.hpp>
#include "draw.hpp"

using namespace cv;

static bool updateModel;

void poll_keyboard(char k){
        if (k == ' ') {
            updateModel = !updateModel;
            if (updateModel)
                std::cout << "Background update is on\n";
            else
                std::cout << "Background update is off\n";
        }
}

void smooth_mask(Mat *forMask){
        GaussianBlur(*forMask, *forMask, Size(11,11), 3.5, 3.5);
        threshold(*forMask, *forMask, 100, 255, THRESH_BINARY);
        dilate(*forMask, *forMask, Mat());
        erode(*forMask, *forMask, Mat());
}

int main()
{
    VideoCapture cap(0);          // the default webcam

    if (!cap.isOpened()){
        std::cerr << "No cam found.\n";
        return -1;
    }

    Mat foreMask, foreImg, img, img0;
    Ptr<BackgroundSubtractor> bgModel;
    updateModel = false;

    namedWindow("cam", WINDOW_NORMAL);
    //namedWindow("foreground mask", WINDOW_NORMAL);
    //namedWindow("foreground cam", WINDOW_NORMAL);
    namedWindow("mean background image", WINDOW_NORMAL);

    // Maybe try MOG for less noise / lessen shadow effects
    //createBackgroundSubtractorMOG2(history=500, double varThreshold=16, detectShadows=true)
    bgModel = createBackgroundSubtractorMOG2(400, 16, false).dynamicCast<BackgroundSubtractor>();

    while (1)
    {   
        cap >> img0;

        if (img0.empty()){
            break;
        }

        resize(img0, img, Size(640, 640*img0.rows/img0.cols), INTER_LINEAR);

        if (foreImg.empty())
            foreImg.create(img.size(), img.type());

        bgModel->apply(img, foreMask, updateModel ? -1 : 0);
        smooth_mask(&foreMask);

        foreImg = Scalar::all(0);
        img.copyTo(foreImg, foreMask);

        Mat bgImg;
        bgModel->getBackgroundImage(bgImg);

        draw_contours(&foreMask, &img);

        imshow("cam", img);
        //imshow("foreground mask", foreMask);
        //imshow("foreground cam", foreImg);
        if (!bgImg.empty())
            imshow("mean background image", bgImg);

        char k = (char)waitKey(30);
        if (k == 27) break; // Esc key
        poll_keyboard(k);
    }
    
    return 0;
}
