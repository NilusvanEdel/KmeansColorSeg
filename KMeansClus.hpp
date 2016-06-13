//
// Created by nilus on 10.06.16.
//

#ifndef NAIVEAPPROACHKMEANSCSEG_KMEANS_HPP
#define NAIVEAPPROACHKMEANSCSEG_KMEANS_HPP
#include "opencv2/highgui/highgui.hpp"
#include <iostream>
#include <string>

using namespace std;
using namespace cv;
class KMeansClus
{
public:
    KMeansClus(vector<Mat> f, int k);
    ~KMeansClus();
    vector<Mat> startClustering();
private:
    // vector including all Matrices of the frames
    vector<Mat> frames;
    // vector with the int of the color of clustercenters
    vector<Vec4f> centers;
};

#endif //NAIVEAPPROACHKMEANSCSEG_KMEANS_HPP
