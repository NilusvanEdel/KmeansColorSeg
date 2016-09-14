//
// Created by nilus on 27.07.16.
//

#include <opencv2/photo.hpp>
#include "CalculateK.hpp"
#include "Calculator.hpp"
#include "EuclidianCalculator.hpp"
#include "Printer.hpp"

int CalculateK::calculateK(Mat img, Calculator* calculator, vector<Vec6f>* centers, KMeansClus* kmeans,
                           vector <vector<int>>* memberOfCluster, bool realVid, vector<vector <Vec6f>>* bestCenters) {
    centers->clear();
    bestCenters->clear();
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
        cout << "K: " << k << endl;
        splitCluster(img, calculator, centers, memberOfCluster);
        // reuse the kMeans algorithm and calculate the new intra and inter meassurments
        kmeans->kMeansAlgorithm(img, -1, 0);
        Printer::debugPrintImg(img, "testForK_", k+1, *memberOfCluster, *centers);
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
    cout << "best k before NeighborCheck: " << bestK << endl;
    /*
    CalculateK::neighborCheck(img, centers, kmeans, memberOfCluster, calculator);
    bestK = centers->size();
    cout << "best k after NeighborCheck: " << bestK << endl;
    */
    Printer::debugPrintImg(img, "finalK_", bestK, *memberOfCluster, *centers);
    return bestK;
}

bool CalculateK::neighborCheck(Mat img, vector<Vec6f>* centers, KMeansClus* kmeans,
                               vector <vector<int>>* memberOfCluster, Calculator* calculator) {
    bool change = false;
    int initialCentersize = centers->size();
    for (int center = 0; center < initialCentersize; center++) {
        cout << "Center: "<< center << endl;
        vector<vector<Point>> ptsOfCenter;
        // needed otherwise the pointers will lose the adress after every push_back needs to be increased for
        // huge images
        ptsOfCenter.reserve(10000000);
        ptsOfCenter.resize(0);
        vector<vector<Point>*> pointerToPtsOfCenter;
        pointerToPtsOfCenter.reserve(1000);
        pointerToPtsOfCenter.resize(0);
        vector<vector<vector<Point>**>> clusterOfCenter;
        clusterOfCenter.resize(img.rows);
        for (int i = 0; i < img.rows; i++)
            clusterOfCenter[i].resize(img.cols);
        int counter = 0;
        bool debug = false;
        for (int y = 0; y < img.cols; y++) {
            for (int x = 0; x < img.rows; x++) {
                if ((*memberOfCluster)[x][y] != center) continue;
                bool neigh1 = false, neigh2 = false, neigh3= false, neigh4 = false;
                if (y-1 >= 0) {
                    if (x-1 >= 0 && (*memberOfCluster)[x-1][y-1] == center) neigh1 = true;
                    if ((*memberOfCluster)[x][y-1] == center) neigh2 = true;
                    if (x+1 < calculator->getMaxX() && (*memberOfCluster)[x+1][y-1] == center) neigh3 = true;
                }
                if (x-1 >= 0 && (*memberOfCluster)[x-1][y] == center) neigh4 = true;
                Point point (x,y);
                if (!neigh1 && !neigh2 && !neigh3 && !neigh4) {
                    ptsOfCenter.push_back(vector<Point>(1));
                    ptsOfCenter[ptsOfCenter.size()-1][0] = point;
                    pointerToPtsOfCenter.push_back(&ptsOfCenter[ptsOfCenter.size()-1]);
                    clusterOfCenter[x][y] = &pointerToPtsOfCenter[pointerToPtsOfCenter.size()-1];
                    counter++;
                }
                    // if neigh2 is active all active neighbors belong already to the same neighbor
                else if (neigh2)  {
                    clusterOfCenter[x][y] = clusterOfCenter[x][y-1];
                    (**clusterOfCenter[x][y]).push_back(point);
                }
                    // if neigh1 is active only neigh3 could possible belong to a different neighbor
                    // if neigh3 != neigh1 they need to be combined now
                else if (neigh1 && !neigh2) {
                    if (!neigh3 || neigh3 && (clusterOfCenter[x-1][y-1] == clusterOfCenter[x+1][y-1])) {
                        clusterOfCenter[x][y] = clusterOfCenter[x-1][y-1];
                    }
                    if (neigh3 && clusterOfCenter[x-1][y-1] != clusterOfCenter[x+1][y-1]) {
                        clusterOfCenter[x][y] = clusterOfCenter[x-1][y-1];
                        for (int i = 0; i < (**clusterOfCenter[x+1][y-1]).size(); i++) {
                            int place = (**clusterOfCenter[x+1][y-1]).size()-1;
                            (**clusterOfCenter[x][y]).push_back((**clusterOfCenter[x+1][y-1])[place]);
                            (**clusterOfCenter[x+1][y-1]).pop_back();
                        }
                        *clusterOfCenter[x+1][y-1] = *clusterOfCenter[x][y];
                        counter--;
                    }
                    (**clusterOfCenter[x][y]).push_back(point);
                }
                    // if neigh3 is active and !neigh2 only neigh 4 could possibly belong to a different neighbor
                    // if neigh4 is active they need to be combined
                else if (neigh3 && !neigh1 && !neigh2) {
                    if (!neigh4 || neigh4 && (clusterOfCenter[x+1][y-1] == clusterOfCenter[x-1][y])) {
                        clusterOfCenter[x][y] = clusterOfCenter[x+1][y-1];
                    }
                    if (neigh4 && (clusterOfCenter[x+1][y-1] != clusterOfCenter[x-1][y])) {
                        clusterOfCenter[x][y] = clusterOfCenter[x+1][y-1];
                        for (int i = 0; i < (**clusterOfCenter[x-1][y]).size(); i++) {
                            int place = (**clusterOfCenter[x-1][y]).size()-1;
                            (**clusterOfCenter[x][y]).push_back((**clusterOfCenter[x-1][y])[place]);
                            //Warum ändert sich die Größe hierbei nicht?!?!?!?!?
                            (**clusterOfCenter[x-1][y]).pop_back();
                        }
                        *clusterOfCenter[x-1][y] = *clusterOfCenter[x][y];
                        counter --;
                    }
                    (**clusterOfCenter[x][y]).push_back(point);
                }
                    // if neigh4 is active and not neigh1-3 x,y belong in this neighborhood
                else if (neigh4) {
                    clusterOfCenter[x][y] = clusterOfCenter[x-1][y];
                    (**clusterOfCenter[x][y]).push_back(point);
                }
            }
        }
        if (counter > 1) {
            change = true;
            bool first = true;
            for (int i = 0; i < ptsOfCenter.size(); i++) {
                if (ptsOfCenter[i].empty()) continue;
                int averageX = 0, averageY = 0;
                for (int j = 0; j < ptsOfCenter[i].size();j++) {
                    averageX += ptsOfCenter[i][j].x;
                    averageY += ptsOfCenter[i][j].y;
                }
                averageX /= ptsOfCenter[i].size();
                averageY /= ptsOfCenter[i].size();
                if (first) {
                    (*centers)[center] = Vec6f((*centers)[center][0],(*centers)[center][1],(*centers)[center][2],
                                            averageX,averageY);
                    first = false;
                }
                else {
                    Vec6f temp = Vec6f((*centers)[center][0],(*centers)[center][1],(*centers)[center][2],
                                       averageX,averageY);
                    centers->push_back(temp);
                }
            }
            kmeans->kMeansAlgorithm(img,center, initialCentersize);
        }
    }
    return change;
}

float CalculateK::getValidity(Mat img, Calculator* calculator, vector<Vec6f> *centers,
                              vector <vector<int>>* memberOfCluster) {
    float intraMeassure = 0;
    for (int y = 0; y < img.cols; y++)
    {
        for (int x = 0; x < img.rows; x++)
        {
            Vec3b tmp = img.at<Vec3b>(x, y);
            intraMeassure += calculator->distance(Vec5f(tmp[0],tmp[1],tmp[2],x,y),
                                                  (*centers)[(*memberOfCluster)[x][y]]);
        }
    }
    intraMeassure /= img.rows*img.cols;
    float interMeassure = calculator->distance((*centers)[0],(*centers)[1]);
    for (int i = 1; i < centers->size(); i++) {
        for (int j = 1; j < centers->size(); j++) {
            if (j==i) continue;
            float distance = calculator->distance((*centers)[i],(*centers)[j]);
            if (distance < interMeassure) interMeassure = distance;
        }
    }
    // the actual validity, note: if max at validity[0] --> k=2
    float currentValidity = INT_MAX;
    if (interMeassure!=0) currentValidity = intraMeassure/interMeassure;
    return currentValidity;
}

void CalculateK::splitCluster(Mat img, Calculator* calculator, vector<Vec6f>* centers,
                              vector <vector<int>>* memberOfCluster) {
    // calculate the variance of the clusters
    // fill the vectors with 00000 or the maximum values of the attributes
    vector <Vec5f> variance(centers->size());
    vector <Vec5i> minColAndPixSpace(centers->size());
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
                if (img.at<Vec3b>(y, x)[i] > maxColAndPixSpace[cluster][i])
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
    // the new clusters may not exceed the limits (actually they probably may)
    /* for (int i = 0; i < 5; i++) {
        // limit fix for color space
        if (i < 3) {
            if ((*centers)[clusterToSplit][i] < 0) (*centers)[clusterToSplit][i] = 0;
            if ((*centers)[centers->size() - 1][i] < 0) (*centers)[centers->size() - 1][i] = 0;
            if ((*centers)[clusterToSplit][i] > 255) (*centers)[clusterToSplit][i] = 255;
            if ((*centers)[centers->size() - 1][i] > 255) (*centers)[centers->size() - 1][i] = 255;
        }
            // limit fix for coordinate space
        else if (i >= 3 ){
            if ((*centers)[clusterToSplit][i] < 0) (*centers)[clusterToSplit][i] = 0;
            if ((*centers)[centers->size() - 1][i] < 0) (*centers)[centers->size() - 1][i] = 0;
            if (i == 3) {
                if ((*centers)[clusterToSplit][i] > calculator->getMaxX())
                    (*centers)[clusterToSplit][i] = calculator->getMaxX();
                if ((*centers)[centers->size() - 1][i] > calculator->getMaxX())
                    (*centers)[centers->size() - 1][i] = calculator->getMaxX();
            }
            else if (i == 4)
            {
                if ((*centers)[clusterToSplit][i] > calculator->getMaxY())
                    (*centers)[clusterToSplit][i] = calculator->getMaxY();
                if ((*centers)[centers->size() - 1][i] > calculator->getMaxY())
                    (*centers)[centers->size() - 1][i] = calculator->getMaxY();
            }
        }
    }
     */
}