//
// Created by nilus on 27.07.16.
//

#include <opencv2/photo.hpp>
#include "CalculateK.hpp"
#include "Calculator.hpp"
#include "EuclidianCalculator.hpp"
#include "Printer.hpp"
#include "PixEucCalculator.hpp"
#include "CoordEucCalculator.hpp"

int CalculateK::calculateK(Mat img, Calculator* calculator, vector<Vec6f>* centers, KMeansClus* kmeans,
                           boost::filesystem::path path,vector <vector<int>>* memberOfCluster,
                           bool realVid, vector<vector <Vec6f>>* bestCenters) {
    cout << "start calculation of K" << endl;
    centers->clear();
    bestCenters->clear();
    // clear possible old values in memberOfCluster
    for (int i = 0; i < img.rows; ++i)
        (*memberOfCluster)[i].resize(img.cols);
    for (int y = 0; y < img.cols; y++)
    {
        for (int x = 0; x < img.rows; x++)
        {
            (*memberOfCluster)[x][y] = 0;
        }
    }
    vector<float> validity;
    // calculate the mean of all values -> the center for cluster 1
    Vec5f mean= {0,0,0,0,0};
    for (int y = 0; y < img.cols; y++)
    {
        for (int x = 0; x < img.rows; x++)
        {
            Vec3b tmp = img.at<Vec3b>(x, y);
            for(int j = 0; j < 3; j++) {
                mean[j] += tmp[j];
            }
            mean[3] += x;
            mean[4] += y;
        }
    }
    mean /= (float(img.rows)*float(img.cols));
    centers->push_back(Vec6f{mean[0],mean[1],mean[2],mean[3],mean[4],(float)img.rows*img.cols});

    // begin the loop to split the center with highest variance
    for (int k = 1; k < kMax; k++)
    {
        // cout << "K: " << k << endl;
        splitCluster(img, calculator, centers, memberOfCluster);
        // reuse the kMeans algorithm and calculate the new intra and inter meassurments
        kmeans->kMeansAlgorithm(img);
        Printer::debugPrintImg(img, path, "testForK_", k+1, *memberOfCluster, *centers);
        float currentValidity = getValidity(img, calculator, centers, memberOfCluster);
        //save current center
        bestCenters->push_back(*centers);
        // if the validity doesn't change anymore, stop.
        if (validity.size()>2) {
            if (currentValidity == validity[validity.size() - 1] &&
                currentValidity == validity[validity.size() - 2])
                break;
        }
        validity.push_back(currentValidity);
    }
    // best K is the minimal value after the first local Maximum
    int bestK, biasedBestK= 2;
    float minValidity, biasedMinValidity = validity[0];
    bool locMax = false;
    // find the best K
    // for synthethic videos
    if (!realVid) {
        minValidity = validity[0];
        bestK = 2;
        for (int i = 1; i < validity.size(); i++) {
            if (validity[i] < minValidity) {
                minValidity = validity[i];
                bestK = i + 2;
            }
        }
    }
        // for real videos
    else {
        if (validity.size()>1) {
            for (int i = 1; i < validity.size(); i++) {
                // solange nicht letzte Stelle erreicht und noch kein lokales Max existiert
                if (!(i == validity.size()-1) && !locMax)
                    // falls validity[i] ein lokales Maximum ist
                if (validity[i-1] < validity[i] && validity[i]  > validity[i+1]) {
                    locMax = true;
                    minValidity = validity[i];
                }
                if (!locMax && validity[i] < biasedMinValidity) {
                    biasedBestK = i+2;
                    biasedMinValidity = validity[i];
                }
                if (locMax && validity[i] < minValidity) {
                    bestK = i+2;
                    minValidity = validity[i];
                }
            }
            if (!locMax) {
                minValidity = biasedMinValidity;
                bestK = biasedBestK;
            }
        }
    }
    *centers = (*bestCenters)[bestK-2];
    kmeans->kMeansAlgorithm(img);
    cout << "best k before NeighborCheck: " << bestK << endl;
    // implements the boundary Check, which splits clusters further according to detected boundaries
    /*CalculateK::boundaryCheck(img, centers, kmeans, memberOfCluster, calculator);
    bestK = centers->size();
    cout << "best k after NeighborCheck: " << bestK << endl;
    Printer::debugPrintImg(img, "finalK_", bestK, *memberOfCluster, *centers);
    */
    Printer::printImg(img, path, "finalK", *memberOfCluster, *centers);
    return bestK;
}


bool CalculateK::boundaryCheck(Mat img, vector<Vec6f>* centers, KMeansClus* kmeans, boost::filesystem::path path,
                               vector <vector<int>>* memberOfCluster, Calculator* calculator) {
    bool change = false;
    int initialCentersize = centers->size();
    // queue to check all clusters (despite the clusters which have been created in this function)
    for (int i = 0; i < initialCentersize; i++) {
        cout << i << endl;
        Mat temp(img.size(), img.type());
        // create a matrix with the same size as the source image, but filled only by the pixels of the certain cluster
        // the rest will be filled with the rgb values for white
        for (int y = 0; y < img.cols; y++) {
            for (int x = 0; x < img.rows; x++) {
                int cluster = (*memberOfCluster)[x][y];
                Vec3b tmp;
                if ((*centers)[i][5] == 0) tmp = Vec3b(255, 255, 255);
                else {
                    tmp = Vec3b(255, 255, 255);
                    if ((*memberOfCluster)[x][y] == i) {
                        tmp(0) = (*centers)[(*memberOfCluster)[x][y]](0);
                        tmp(1) = (*centers)[(*memberOfCluster)[x][y]](1);
                        tmp(2) = (*centers)[(*memberOfCluster)[x][y]](2);
                    }
                }
                temp.at<Vec3b>(x, y) = tmp;
            }
        }
        // convert the image of this particular cluster into a gray scale image
        Mat img_gray;
        cvtColor( temp, img_gray, CV_BGR2GRAY );
        // adjust the saturation
        equalizeHist(img_gray, img_gray);
        // blur the image
        blur( img_gray, img_gray, Size(2,2) );
        //convert the blured grayscale image to a binary image
        Mat binaryMat(img_gray.size(), img_gray.type());
        //Apply thresholding
        threshold(img_gray, binaryMat, 100, 255, THRESH_BINARY);
        Printer::printImg(img_gray, path, "gray_and_blur");
        Printer::printImg(binaryMat, path, "binary");
        Mat canny_output;
        vector<vector<Point> > contours;
        vector<Vec4i> hierarchy;
        /// Detect edges using canny algorithm
        Canny( binaryMat, canny_output, 100, 100*2, 3 );
        stringstream file;
        file << "contours" << i;
        Printer::printImg(binaryMat, path, file.str());
        // Find contours (RETR_External ignoring child/parent cause kmeans can't deal with it anyways)
        // CHAIN_APPROX NONE to draw it easier manually
        findContours( canny_output, contours, hierarchy, RETR_EXTERNAL, CV_CHAIN_APPROX_NONE, Point(0, 0) );
        cout<< "contours size: " << contours.size() << endl;
        if (contours.size() == 1) continue;
        Printer::printCountours(img, path, contours);
        bool background = false;
        bool newCenters = false;
        int x = 0; int y = 0;
        for( int t = 0; t< contours.size(); t++ ) {
            // if the contour surrounds only a black polygon, this cluster seems to be the background
            x = 0;
            y = 0;
            for (int j = 0; j < contours[t].size(); j++) {
                x += contours[t][j].x;
                y += contours[t][j].y;
            }
            x /= contours[t].size();
            y /= contours[t].size();
            //check if the polygon actually surrounds pixel inside of this cluster
            background = true;
            for (int z = -1; z < 2; z++) {
                for (int l = -1; l < 2; l++) {
                    int xTmp = contours[t][0].y;
                    int yTmp = contours[t][0].x;
                    if (pointPolygonTest(contours[t], Point(yTmp + z, xTmp + l), false) > 0) {
                        if ((*memberOfCluster)[xTmp + l][yTmp + z] == i) background = false;
                    }
                }
            }
        }
        // if the polygon does not surround pixel inside of this cluster
        if (background) {
            cout << "Background at cluster: " << i << endl;
            break;
        }
        // else split the cluster centers according to the polygon

        if (!background) {
            // if the current cluster has not been split yet, change the position of it
            if (!newCenters) {
                (*centers)[i] = Vec6f((*centers)[i][0], (*centers)[i][1], (*centers)[i][2], y, x, 0);
            }
            // otherwise ceate a new cluster consisting of the old color but with new coordinates
            else {
                Vec6f newCenter = Vec6f((*centers)[i][0], (*centers)[i][1], (*centers)[i][2], y, x, 0);
                centers->push_back(newCenter);
            }
            // find the center of this polygon and put it in the respective cluster center
            // decomment this part and toggle the k-means part if the splitting should occure according to k-means
            // _________________________________________________________________________________________________________
            for (int y = 0; y < img.cols; y++) {
                for (int x = 0; x < img.rows; x++) {
                    if ((*memberOfCluster)[x][y] == i) {
                        if (pointPolygonTest(contours[i], Point(y, x), false) >= 0) {
                            if (newCenters) (*memberOfCluster)[x][y] = centers->size() - 1;
                            (*centers)[(*memberOfCluster)[x][y]][5]++;
                        }
                    }
                }
            }
            newCenters = true;
            //__________________________________________________________________________________________________________
        }
        change = true;
        // toggle to assign the new clusters instead via k_Means using a 5d or 2d-feature vectore
        //______________________________________________________________________________________________________________
        /*
        Calculator* coordEucCalculator = new PixEucCalculator(temp.rows, temp.cols);
        vector <int> limitedCluster;
        limitedCluster.push_back(i);
        for (int cl = centers->size() - initialCentersize; cl < centers->size(); cl++) {
            limitedCluster.push_back(cl);
        }
        kmeans->kMeansLimitedAlgorithm(img, memberOfCluster, coordEucCalculator,limitedCluster);
        */
        //______________________________________________________________________________________________________________
    }
    return change;
}

float CalculateK::getValidity(Mat img, Calculator* calculator, vector<Vec6f> *centers,
                              vector <vector<int>>* memberOfCluster) {
    float intraMeasure = 0;
    // calculate the intra meassure (the average distance from each pixel to its respective cluster center)
    for (int y = 0; y < img.cols; y++)
    {
        for (int x = 0; x < img.rows; x++)
        {
            Vec3b tmp = img.at<Vec3b>(x, y);
            intraMeasure += calculator->distance(Vec5f(tmp[0],tmp[1],tmp[2],x,y),
                                                  (*centers)[(*memberOfCluster)[x][y]]);
        }
    }
    intraMeasure /= img.rows*img.cols;
    // calculate the inter measure (the minmal distance from two clusters)
    float interMeasure = calculator->distance((*centers)[0],(*centers)[1]);
    for (int i = 1; i < centers->size(); i++) {
        for (int j = 1; j < centers->size(); j++) {
            if (j==i) continue;
            float distance = calculator->distance((*centers)[i],(*centers)[j]);
            if (distance < interMeasure) interMeasure = distance;
        }
    }
    // the actual validity, note: if max at validity[0] --> k=2
    float currentValidity = INT_MAX;
    if (interMeasure!=0) currentValidity = intraMeasure/interMeasure;
    return currentValidity;
}


void CalculateK::splitCluster(Mat img, Calculator* calculator, vector<Vec6f>* centers,
                              vector <vector<int>>* memberOfCluster) {
    // calculate the variance of the clusters
    // fill the vectors with 00000 or the maximum values of the attributes
    vector <Vec5f> variance(centers->size());
    // will be used to get the mimal values for colorspace and coordinates for the corresponding cluster
    vector <Vec5i> minColAndPixSpace(centers->size());
    // will be used to get the maximal values for colorspace and coordinates for the corresponding cluster
    vector <Vec5i> maxColAndPixSpace(centers->size());
    for (auto it= minColAndPixSpace.begin(); it != minColAndPixSpace.end(); it++)
        *it = Vec5i(255,255,255,calculator->getMaxX(),calculator->getMaxY());
    for (auto it= maxColAndPixSpace.begin(); it != maxColAndPixSpace.end(); it++) *it = Vec5i(0, 0, 0, 0, 0);
    for (auto it=variance.begin(); it != variance.end(); it++) *it = Vec5f(0,0,0,0,0);
    // calculate the variances and find the maxColAndPixSpace as well as the minColAndPixSpace in each cluster
    for (int y = 0; y < img.cols; y++)
    {
        for (int x = 0; x < img.rows; x++)
        {
            int cluster = (*memberOfCluster)[x][y];
            Vec3b tmp = img.at<Vec3b>(x, y);
            variance[cluster] += calculator->variance(Vec5f(tmp[0],tmp[1],tmp[2],x,y),(*centers)[cluster]);
            for (int i = 0; i < 3; i++) {
                if (img.at<Vec3b> (x, y)[i] < minColAndPixSpace[cluster][i])
                    minColAndPixSpace[cluster][i] = img.at<Vec3b> (x, y)[i];
                if (img.at<Vec3b>(x, y)[i] > maxColAndPixSpace[cluster][i])
                    maxColAndPixSpace[cluster][i] = img.at<Vec3b>(x, y)[i];
            }
            if (x < minColAndPixSpace[cluster][3]) minColAndPixSpace[cluster][3] = x;
            if (x > maxColAndPixSpace[cluster][3]) maxColAndPixSpace[cluster][3] = x;
            if (y < minColAndPixSpace[cluster][4]) minColAndPixSpace[cluster][4] = y;
            if (y > maxColAndPixSpace[cluster][4]) maxColAndPixSpace[cluster][4] = y;
        }
    }
    // calculate the singleVariance out of the variances for each attribute
    vector <float> singleVariance(variance.size());
    fill(singleVariance.begin(),singleVariance.end(),0);
    for (int i = 0; i < centers->size(); i++) {
        variance[i] /= (*centers)[i][5];
        singleVariance[i]+=calculator->singleVariance(variance[i]);
    }
    float maxVariance = 0;

    int clusterToSplit = 0;
    for (int i = 0; i < singleVariance.size(); i++) {
        if (singleVariance[i] > maxVariance) {
            clusterToSplit = i;
            maxVariance = singleVariance[i];
        }
    }
    // calculate the new clusters centers
    Vec5f oldCCenter =
            Vec5f((*centers)[clusterToSplit][0],(*centers)[clusterToSplit][1],(*centers)[clusterToSplit][2],
                  (*centers)[clusterToSplit][3],(*centers)[clusterToSplit][4]);
    Vec5f a = (oldCCenter - (Vec5f)maxColAndPixSpace[clusterToSplit])/2;
    Vec5f newClusterCenter = oldCCenter - a;
    (*centers)[clusterToSplit] = Vec6f(newClusterCenter.val[0],newClusterCenter.val[1],newClusterCenter.val[2],
                                       newClusterCenter.val[3],newClusterCenter.val[4],0);
    newClusterCenter = oldCCenter + a;
    centers->push_back(Vec6f(newClusterCenter.val[0],newClusterCenter.val[1],newClusterCenter.val[2],
                             newClusterCenter.val[3],newClusterCenter.val[4],0));
    for (int y = 0; y < img.cols; y++)
    {
        for (int x = 0; x < img.rows; x++)
        {
            (*memberOfCluster)[x][y] = 0;
        }
    }
}
