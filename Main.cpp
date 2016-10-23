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
    //specifies the location of the video sequence/image this can either be a file or a folder including only images
    string videoSeq = "/home/nilus/Videos/images";
    //specifies the location to which the results will be printed
    string filedest = "/home/nilus/test/";
    // specifies whether it is a realImg or a synthetic, important for calculating the optimal cluster count
    bool realImg = true;
    // if HSVTransformation = true, the source image/vidSeq will be converted into the HSV color space
    bool HSVTransformation = false;

    // creates a folder original_images at the specified location, where the frames of the video seq will be stored
    stringstream orig_folder;
    orig_folder<<filedest<<"/original_images" <<"/";
    boost::filesystem::create_directories(orig_folder.str());
    // responsible for reading (delivering the single frames) of a video/image
    VideoCapturer vidCap(videoSeq,boost::filesystem::path(orig_folder.str()),HSVTransformation);
    if (vidCap.readVideo() == -1) return -1;
    else
    {
        int frameCount = vidCap.readFrames();
        // notice the Opencv's embedded string class
        vector<cv::String> filenames;
        cv::String folder = orig_folder.str();
        // collects all files within the folder, where the source frames are stored
        glob(folder, filenames);
        Mat img;
        img = imread(filenames[0]);
        // creates the respective Calculator responsible for distance/ variance measurement
        Calculator* calculator = new EuclidianCalculator(img.rows, img.cols);
        img.release();
        KMeansClus kmeans(calculator, realImg, orig_folder.str(), filedest);
        // starts the clustering of the whole video sequence (unnecessary when looking at a single image)
        kmeans.startClustering(frameCount);
        delete calculator;
    }
    // removes the orig_folder, if decommented it should be done by hand before running the algorithm
    // boost::filesystem::remove_all(boost::filesystem::path(orig_folder.str()));
    return 0;

}