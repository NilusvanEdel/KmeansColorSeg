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
    VideoCapturer vidCap("/home/nilus/Bilder/002.png","/home/nilus/test/",false);
    if (vidCap.readVideo() == -1) return -1;
    else
    {
        std::vector<Mat> frames = vidCap.readFrames();
        Calculator* calculator = new EuclidianCalculator(frames[0].rows, frames[0].cols);
        KMeansClus kmeans(frames, calculator, true);
        kmeans.startClustering();
        delete calculator;
    }
    return 0;

}