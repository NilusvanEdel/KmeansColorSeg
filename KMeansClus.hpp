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
    // Constructor where the optimal number of k is calculated automatically
    KMeansClus(vector<Mat> f);
    // Constructor with given k-number
    KMeansClus(vector<Mat> f, int k);
    ~KMeansClus();
    vector<Mat> startClustering();
private:
    // vector including all Matrices of the frames
    vector<Mat> frames;
    // vector with the int of the color of clustercenters
    vector<Vec4f> centers;
    // calculate the optimal value of k
    int calculateK();
    // the actual kMeans algorithm
    void kMeansAlgorithm(Mat mat);
    // maximal k
    const int kMax = 25;
    int** memberOfCluster;
    vector <int> occCounter;
};

#endif //NAIVEAPPROACHKMEANSCSEG_KMEANS_HPP
