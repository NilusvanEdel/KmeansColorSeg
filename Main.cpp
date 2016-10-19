#include <iostream>
#include <string.h>
#include <opencv2/photo.hpp>
#include <boost/filesystem/operations.hpp>
#include "VideoCapturer.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "KMeansClus.hpp"
#include "Calculator.hpp"
#include "HSVEucCalculator.hpp"
#include "EuclidianCalculator.hpp"
#include "CosineCalculator.hpp"
#include "Printer.hpp"
#include "PixEucCalculator.hpp"


int main(int argc, char* argv[])
{
    string videoSeq = "/home/nilus/Videos/images";
    string filedest = "/home/nilus/test/";
    bool realImg = true;
    bool HSVTransformation = true;

    stringstream orig_folder;
    orig_folder<<filedest<<"/original_images" <<"/";
    boost::filesystem::create_directories(orig_folder.str());

    VideoCapturer vidCap(videoSeq,boost::filesystem::path(orig_folder.str()),HSVTransformation);
    if (vidCap.readVideo() == -1) return -1;
    else
    {
        int frameCount = vidCap.readFrames();
        vector<cv::String> filenames; // notice the Opencv's embedded "String" class
        cv::String folder = orig_folder.str();
        glob(folder, filenames);
        Mat img;
        img = imread(filenames[0]);
        Calculator* calculator = new EuclidianCalculator(img.rows, img.cols);
        img.release();
        KMeansClus kmeans(calculator, realImg, orig_folder.str(), filedest);
        kmeans.startClustering(frameCount);
        delete calculator;
    }
    // removes the orig_folder, if decommented should be done by hand before running the algorithm
    // boost::filesystem::remove_all(boost::filesystem::path(orig_folder.str()));
    return 0;

}