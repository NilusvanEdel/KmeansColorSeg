//
// Created by nilus on 19.07.16.
//

#ifndef NAIVEAPPROACHKMEANSCSEG_PRINTER_HPP
#define NAIVEAPPROACHKMEANSCSEG_PRINTER_HPP

#include <iostream>
#include <string.h>
#include "opencv2/highgui/highgui.hpp"

using namespace std;
using namespace cv;

/*
 * a class to print images
 */
class Printer {
public:
    // basic image printing
    void static printImg(Mat img, string filename);
    // image printing according to the clusters
    void static printImg(Mat img, string filename, vector <vector<int>> memberOfCluster, vector<Vec6f> centers);
    //image printing for debugging, prints one cluster after the other
    void static debugPrintImg(Mat img, string filename, int clusterCount,
                              vector <vector<int>> memberOfCluster, vector<Vec6f> centers);
};


#endif //NAIVEAPPROACHKMEANSCSEG_PRINTER_HPP
