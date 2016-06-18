#include "opencv2/highgui/highgui.hpp"
#include <iostream>
#include <string>
//
// Created by nilus on 10.06.16.
//

#ifndef NAIVEAPPROACHKMEANSCSEG_VIDEOCAPTURER_HPP
#define NAIVEAPPROACHKMEANSCSEG_VIDEOCAPTURER_HPP

using namespace std;
using namespace cv;
class VideoCapturer
{
public:
    VideoCapturer(string filen, string filed, bool hsv);
    ~VideoCapturer();
    VideoCapture cap;
    int readVideo();
    vector<Mat> readFrames();
private:
    // the filename of the videosource
    string filename;
    // the filelocation where the image will be stored (if decommented)
    string filedest;
    double fps;
    vector<Mat> frames;
    // for debugging, returns the type of img as string
    string type2str(int type);
    bool hsv;
};



#endif //NAIVEAPPROACHKMEANSCSEG_VIDEOCAPTURER_HPP
