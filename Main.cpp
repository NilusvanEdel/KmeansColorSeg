#include <iostream>
#include <string.h>
#include "VideoCapturer.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "KMeansClus.hpp"
#include "Calculator.hpp"
#include "HSVEucCalculator.hpp"
#include "EuclidianCalculator.hpp"
#include "CosineCalculator.hpp"


int main(int argc, char* argv[])
{
    VideoCapturer vidCap("/home/nilus/Videos/ownSample.avi","/home/nilus/test/",false);
    if (vidCap.readVideo() == -1) return -1;
    else
    {
        std::vector<Mat> frames = vidCap.readFrames();
        Calculator* calculator = new EuclidianCalculator(frames[0].cols,frames[0].rows);
        KMeansClus kmeans(frames, calculator, false);
        kmeans.startClustering();
    }

    return 0;

}