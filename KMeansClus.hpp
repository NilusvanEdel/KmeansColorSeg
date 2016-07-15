//
// Created by nilus on 10.06.16.
//

#ifndef NAIVEAPPROACHKMEANSCSEG_KMEANS_HPP
#define NAIVEAPPROACHKMEANSCSEG_KMEANS_HPP
#include "opencv2/highgui/highgui.hpp"
#include <iostream>
#include <string>
#include "Calculator.hpp"

using namespace std;
using namespace cv;
class KMeansClus
{
public:
    // Constructor where the optimal number of k is calculated automatically
    KMeansClus(vector<Mat> f, Calculator* calculator, bool realVid);
    // Constructor with given k-number
    KMeansClus(vector<Mat> f, int k, Calculator* calculator, bool realVid);
    ~KMeansClus();
    void startClustering();
private:
    // vector including all Matrices of the frames
    vector<Mat> frames;
    // vector with the int of the color of clustercenters[0-2],centers[3]=x,centers[4]=y,centers[5]=memberCount
    vector<Vec6f> centers;
    // calculate the optimal value of k
    int calculateK();
    // the actual kMeans algorithm
    void kMeansAlgorithm(Mat img);
    // maximal k
    const int kMax = 25;
    vector <vector<int>> memberOfCluster;
    // calculates the mean of matrix
    Vec3f meanCalculator(Mat matrix);
    Calculator* calculator;
    // a boolean which determines whether it is a synthetic or real video
    bool realVid;
};

#endif //NAIVEAPPROACHKMEANSCSEG_KMEANS_HPP
