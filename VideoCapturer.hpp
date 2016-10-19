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
    VideoCapturer(string filen, boost::filesystem::path path, bool hsv);
    ~VideoCapturer();
    VideoCapture cap;
    int readVideo();
    int readFrames();
private:
    // the filename of the videosource
    string filename;
    // the filelocation where the image will be stored (if decommented)
    boost::filesystem::path path;
    double fps;
    vector<Mat> frames;
    // for debugging, returns the type of img as string
    string type2str(int type);
    // if true a video is expected, if false a folder including a videoSeq is expected
    bool video;
    bool hsv;
};



#endif //NAIVEAPPROACHKMEANSCSEG_VIDEOCAPTURER_HPP
