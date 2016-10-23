//
// Created by nilus on 10.06.16.
//

#ifndef NAIVEAPPROACHKMEANSCSEG_KMEANS_HPP
#define NAIVEAPPROACHKMEANSCSEG_KMEANS_HPP
#include "opencv2/highgui/highgui.hpp"
#include <iostream>
#include <string>
#include <boost/filesystem/path.hpp>
#include "Calculator.hpp"

using namespace std;
using namespace cv;
class KMeansClus
{
public:
    // Constructor where the optimal number of k is calculated automatically using the first frame of the video seq
    KMeansClus(Calculator* calculator, bool realVid, string fileLoc, string filedest);
    // Constructor with given k-number, which assigns the centers randomly (only clusters the first image)
    KMeansClus(int k, Calculator* calculator, bool realVid, string fileLoc, string filedest);
    // deconstructor
    ~KMeansClus();
    // clusters the whole video Sequence
    void startClustering(int frameCount);
    // the actual kMeans algorithm
    void kMeansAlgorithm(Mat img, Calculator* calculator);
    // k-means algorithm which uses the member variables
    void kMeansAlgorithm(Mat img);
    // k-means algorithm where only the pixels which are part of the clusters described in limitedCluster are clustered
    // necessary for splitting the clustering process of color and geomatrical segmentation
    void kMeansLimitedAlgorithm(Mat img, vector <vector<int>>* memberOfCluster, Calculator* calculator,
                                             vector <int> limitedCluster);

    // returns the current validity
    float getValidity (Mat img);
    //  set Calculator
    void setCalculator(Calculator* calculator);

private:
    // vector with the int of the color of clustercenters[0-2],centers[3]=x,centers[4]=y,centers[5]=memberCount
    vector<Vec6f> centers;
    // maximal k
    const int kMax = 8;
    // the index of this vectors describes to which clusters the corresponding coordinates belong
    // it shares the column and row size of the examined image
    vector <vector<int>> memberOfCluster;
    // for data reduction test purposes
    // it stores the memberOfCluster resulting in each frame
    vector <vector <vector<int>>> vecMemOfCluster;
    // calculates the mean of matrix
    Vec3f meanCalculator(Mat matrix);
    // The used calculator responsible for variance/distance calculation
    Calculator* calculator;
    // a boolean which determines whether it is a synthetic or real video
    bool realVid;
    // the old calculated centers are stored here (for determing the validity for k-1/k+1)
    vector<vector <Vec6f>> bestCenters;
    // at the index of this vector the corresponding super-cluster for each cluster is stored
    vector <vector<int>> superClusterPosition;
    // the centers of the super-clusters
    vector <Vec6f> supercenters;
    // the folder where the images are stored
    boost::filesystem::path path;
    // the folder where the images of the video seq are
    boost::filesystem::path fileLoc;
    // the delta determines the maximal possible deviation from one cluster to its super-cluster
    float delta = 0.20;
};

#endif //NAIVEAPPROACHKMEANSCSEG_KMEANS_HPP
