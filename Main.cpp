#include <iostream>
#include <string.h>
#include "VideoCapturer.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "KMeansClus.hpp"


int main(int argc, char* argv[])
{
    VideoCapturer vidCap("/home/nilus/Videos/sample.avi","/home/nilus/test/");
    if (vidCap.readVideo() == -1) return -1;
    else
    {
        std::vector<Mat> frames = vidCap.readFrames();
        KMeansClus kmeans(frames, 5);
        kmeans.startClustering();
    }

    return 0;

}