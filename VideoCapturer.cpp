//
// Created by nilus on 10.06.16.
//

#include <opencv2/imgproc.hpp>
#include <boost/filesystem.hpp>
#include "VideoCapturer.hpp"
#include "Printer.hpp"

using namespace cv;
using namespace std;

VideoCapturer::VideoCapturer(string filen, boost::filesystem::path path, bool hsv)
{
    filename = filen;
    this->path = path;
    double fps = 0;
    VideoCapture cap;
    if(boost::filesystem::is_directory(filename)) video = false;
    else video = true;
    this->hsv=hsv;
}
VideoCapturer::~VideoCapturer()
{
}
int VideoCapturer::readVideo()
{
    if (video) {
        cap = VideoCapture(filename); // open the video file for reading
        int counter = 0;

        if (!cap.isOpened())  // if not success, exit program
        {
            cout << "Cannot open the video file" << endl;
            return -1;
        }

        //cap.set(CV_CAP_PROP_POS_MSEC, 300); //start the video at 300ms

        // fps = cap.get(CV_CAP_PROP_FPS); //get the frames per seconds of the video

        // cout << "Frame per seconds : " << fps << endl;
    }
    return 0;
}

int VideoCapturer::readFrames()
{
    int counter = 0;
    size_t i= 0;
    vector<cv::String> filenames; // notice the Opencv's embedded "String" class
    cv::String folder = filename;
    if (!video) {
        glob(folder, filenames);
    }
    while(1)
        {
            Mat tmp;
            if (video) {
                cap >> tmp;
                if (tmp.empty()) {
                    //cout << "scanned all frames" << endl;
                    break;
                }
            }
            else {
                    tmp = imread(filenames[i]);
                    if(!tmp.data)
                        break;
            }
            //save frames as jpeg
            stringstream filename;
            if (hsv) {
                cvtColor(tmp, tmp, COLOR_BGR2HSV);
                filename << "original" << counter;
                Printer::printImg(tmp, path, filename.str());
            }
            i++;
            counter++;
            /*
            vector<Vec3b> numberOfColors;
            for (int y = 0; y < tmp.cols; y++) {
                for (int x = 0; x < tmp.rows; x++) {
                    Vec3b temp = tmp.at<Vec3b>(x, y);
                    if (numberOfColors.empty()) numberOfColors.push_back(temp);
                    else {
                        bool newColor = true;
                        for (int i = 0; i < numberOfColors.size(); i++) {
                            if (numberOfColors[i] == temp) {
                                newColor = false;
                                break;
                            }
                        }
                        if (newColor) numberOfColors.push_back(temp);
                    }
                }
            }
            cout << "Used color in Image: " << numberOfColors.size() << endl;
             */

            if(waitKey(30) == 27) //wait for 'esc' key press for 30 ms. If 'esc' key is pressed, break loop
            {
                cout << "esc key is pressed by user" << endl;
                break;
        }
    }
    cout << counter << "frames" << endl;
    return counter;
}

string VideoCapturer::type2str(int type) {
    string r;

    uchar depth = type & CV_MAT_DEPTH_MASK;
    uchar chans = 1 + (type >> CV_CN_SHIFT);

    switch ( depth ) {
        case CV_8U:  r = "8U"; break;
        case CV_8S:  r = "8S"; break;
        case CV_16U: r = "16U"; break;
        case CV_16S: r = "16S"; break;
        case CV_32S: r = "32S"; break;
        case CV_32F: r = "32F"; break;
        case CV_64F: r = "64F"; break;
        default:     r = "User"; break;
    }

    r += "C";
    r += (chans+'0');

    return r;
}