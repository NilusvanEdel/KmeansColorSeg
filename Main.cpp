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
    VideoCapturer vidCap("/home/nilus/Videos/test_case1.gif","/home/nilus/test/", true);
    if (vidCap.readVideo() == -1) return -1;
    else
    {
        std::vector<Mat> frames = vidCap.readFrames();
        Calculator* calculator = new HSVEucCalculator();
        KMeansClus kmeans(frames, calculator);
        kmeans.startClustering();
    }

    return 0;

}