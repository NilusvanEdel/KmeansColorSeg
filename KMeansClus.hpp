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
    KMeansClus(vector<Mat> frames, Calculator* calculator, bool realVid);
    // Constructor with given k-number
    KMeansClus(vector<Mat> frames, int k, Calculator* calculator, bool realVid);
    ~KMeansClus();
    // the usage of the kmean algorithm
    void startClustering();
    // the actual kMeans algorithm, if clusterToSplit == -1 the regular algorith will be executed
    // otherwise it will split the selectedCluster according to its center and the center of centers.size()-1
    void kMeansAlgorithm(Mat img, int clusterToSplit, int initialCentersize);
    // returns the current validity
    float getValidity (Mat img);
private:
    // vector including all Matrices of the frames
    vector<Mat> frames;
    // vector with the int of the color of clustercenters[0-2],centers[3]=x,centers[4]=y,centers[5]=memberCount
    vector<Vec6f> centers;
    // maximal k
    const int kMax = 8;
    // the index of this vectors assigns clusters to the corresponding coordinates in the particular image
    vector <vector<int>> memberOfCluster;
    // calculates the mean of matrix
    Vec3f meanCalculator(Mat matrix);
    Calculator* calculator;
    // a boolean which determines whether i is a synthetic or real video
    bool realVid;
    // the old calculated Centers using calculateK are stored here
    vector<vector <Vec6f>> bestCenters;
    // at the index of this vector the corresponding supercluster for each cluster is stored
    vector <vector<int>> superClusterPosition;
    // here the centers overlap over each frame
    vector <Vec6f> supercenters;

};

#endif //NAIVEAPPROACHKMEANSCSEG_KMEANS_HPP
