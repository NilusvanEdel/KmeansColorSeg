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
    // calculate the optimal value of k
    // the calculated centers for each iteration are stored in bestCenters
    int static calculateK(Mat img, Calculator* calculator, vector<Vec6f>* centers, KMeansClus* kMeans,
                          vector <vector<int>>* memberOfCluster, bool realVid, vector<vector <Vec6f>>* bestCenters);
    // check whether the segmented clusters keep their neighborhoodproperties and if not it creates new clusters
    bool static neighborCheck(Mat img, vector<Vec6f>* centers, KMeansClus* kmeans,
                              vector <vector<int>>* memberOfCluster, Calculator* calculator);
    // calculates the validity
    float static getValidity(Mat img, Calculator* calculator, vector<Vec6f>* centers,
                             vector <vector<int>>* memberOfCluster);
    void static splitCluster(Mat img, Calculator* calculator, vector<Vec6f>* centers,
                             vector <vector<int>>* memberOfCluster);
private:
    int const static kMax = 10;
};


#endif //NAIVEAPPROACHKMEANSCSEG_CALCULATEK_HPP
