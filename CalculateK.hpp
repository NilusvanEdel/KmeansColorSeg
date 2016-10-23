//
// Created by nilus on 27.07.16.
//

#ifndef NAIVEAPPROACHKMEANSCSEG_CALCULATEK_HPP
#define NAIVEAPPROACHKMEANSCSEG_CALCULATEK_HPP
#include "opencv2/highgui/highgui.hpp"
#include <iostream>
#include <string>
#include "KMeansClus.hpp"
#include "Calculator.hpp"

class CalculateK {
public:
    // calculate the "optimal" number of clusters, this algorithm has been proposed by Ray, Siddheswar and Turi, Rose H
    // in "Determination of number of clusters in k-means clustering and application in colour image segmentation"
    // the calculated centers for each iteration are stored in bestCenters
    int static calculateK(Mat img, Calculator* calculator, vector<Vec6f>* centers, KMeansClus* kMeans,
                          boost::filesystem::path path, vector <vector<int>>* memberOfCluster,
                          bool realVid, vector<vector <Vec6f>>* bestCenters);
    // check if the clusters span boundaries and if so create new boundaries
    bool static boundaryCheck(Mat img, vector<Vec6f>* centers, KMeansClus* kmeans, boost::filesystem::path path,
                              vector <vector<int>>* memberOfCluster, Calculator* calculator);
    // calculates the validity
    float static getValidity(Mat img, Calculator* calculator, vector<Vec6f>* centers,
                             vector <vector<int>>* memberOfCluster);
    // splits the cluster with the highest variance
    void static splitCluster(Mat img, Calculator* calculator, vector<Vec6f>* centers,
                             vector <vector<int>>* memberOfCluster);
private:
    int const static kMax = 25;
};


#endif //NAIVEAPPROACHKMEANSCSEG_CALCULATEK_HPP
