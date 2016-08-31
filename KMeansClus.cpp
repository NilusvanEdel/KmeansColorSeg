//
// Created by nilus on 10.06.16.
//

#include "KMeansClus.hpp"
#include "Printer.hpp"
#include "CoordEucCalculator.hpp"
#include "CalculateK.hpp"

typedef Vec<float, 5> Vec5f;
typedef Vec<float, 6> Vec6f;
typedef Vec<int, 5> Vec5i;
typedef Vec<int, 6> Vec6i;

KMeansClus::KMeansClus(vector<Mat> frames, Calculator* calculator, bool realVid) {
    this->calculator = calculator;
    this->realVid = realVid;
    this->frames = frames;
    memberOfCluster.resize(frames[0].rows);
    for (int i = 0; i < frames[0].rows; ++i)
        memberOfCluster[i].resize(frames[0].cols);
    for (int y = 0; y < frames[0].cols; y++)
    {
        for (int x = 0; x < frames[0].rows; x++)
        {
            memberOfCluster[x][y] = 0;
        }
    }
    CalculateK::calculateK(frames[0], calculator, &centers, this, &memberOfCluster, realVid, &bestCenters);

}
KMeansClus::KMeansClus(vector<Mat> frames,int k, Calculator* calculator, bool realVid) {
    this->calculator = calculator;
    this->realVid = realVid;
    this->frames = frames;
    centers = vector<Vec6f>(k);
    memberOfCluster.resize(frames[0].cols);
    for (int i = 0; i < frames[0].rows; ++i)
        memberOfCluster[i].resize(frames[0].cols);
    for (int y = 0; y < frames[0].cols; y++)
    {
        for (int x = 0; x < frames[0].rows; x++)
        {
            memberOfCluster[x][y] = 0;
        }
    }
    // generate random initial clusterpoints
    std::random_device generator;
    uniform_int_distribution<int> distribution(0, 255);
    uniform_int_distribution<int> distribution2(0, calculator->getMaxX());
    uniform_int_distribution<int> distribution3(0, calculator->getMaxY());
    for (int i = 0; i < centers.size(); i++) {
        centers[i](0) = distribution(generator);
        centers[i](1) = distribution(generator);
        centers[i](2) = distribution(generator);
        centers[i](3) = distribution2(generator);
        centers[i](4) = distribution3(generator);
        centers[i](5) = 0;
    }
}

//deallocator
KMeansClus::~KMeansClus() {
    vector<Mat>().swap(frames);
    vector<Vec6f>().swap(centers);
    vector<vector<int>>().swap(memberOfCluster);
}


void KMeansClus::startClustering() {
    // begin of the loop
    superClusterPosition.resize(frames.size());
    for (int frameCounter = 0; frameCounter < frames.size(); frameCounter++)
    {
        kMeansAlgorithm(frames[frameCounter], -1, 0);
        float validity = CalculateK::getValidity(frames[frameCounter], calculator, &centers, &memberOfCluster);
        int currentBestk = centers.size();
        // bestk size = 0 equals k = 2
        bestCenters[currentBestk-2] = centers;
        CalculateK::splitCluster(frames[frameCounter], calculator, &centers, &memberOfCluster);
        kMeansAlgorithm(frames[frameCounter], -1, 0);
        float validityKBigger = getValidity(frames[frameCounter]);
        centers = bestCenters[currentBestk-3];
        kMeansAlgorithm(frames[frameCounter], -1, 0);
        //todo implement a better way for checking for smaller k
        float validityKSmaller = getValidity(frames[frameCounter]);
        centers = bestCenters[currentBestk-2];
        // if k is still a local Minima
        if (!(validity <= validityKBigger && validity <= validityKSmaller)) {
            cout << "new K Calculation" << endl;
            CalculateK::calculateK(frames[frameCounter], calculator,
                                   &centers, this, &memberOfCluster, realVid, &bestCenters);
        }
        /* toggle for correct results
        CalculateK::calculateK(frames[frameCounter], calculator,
                               &centers, this, &memberOfCluster, realVid, &bestCenters);
        */
        // if no superclusters exist, the first centers will be the first superclusters
        if (frameCounter == 0) {
            for (int i = 0; i < centers.size(); i++) {
                supercenters.push_back(centers[i]);
                superClusterPosition[frameCounter].push_back(i);
            }
        }
        // check which new clusters belong to which supercluster or if they need to create a new one
        else {
            for (int j = 0; j < centers.size(); j++) {
                float mindistance = 99999999;
                int index = -1;
                int maxX = frames[frameCounter].rows;
                int maxY = frames[frameCounter].cols;
                // the delta which will determine whether it still belongs to the same supercluster
                float delta = 0.15;
                Vec5f deltaDisVector;
                for (int i = 0; i < 5; i++) {
                    if (i < 3) deltaDisVector[i] = 255 * delta;
                    if (i == 3) deltaDisVector[i] == maxX * delta;
                    if (i == 4) deltaDisVector[i] == maxY * delta;
                }
                float deltaDistance = calculator->distance(Vec5f(0,0,0,0,0), deltaDisVector);
                deltaDistance += sqrt(pow(0 - deltaDisVector[3] / maxX * 255, 2) +
                                      pow(0 - deltaDisVector[4] / maxY * 255, 2));
                for (int i = 0; i < supercenters.size(); i++) {
                    float distance_col = calculator->distance(centers[j],supercenters[i]);
                    float distance_pix = sqrt(pow(centers[j](3) / maxX * 255 - supercenters[i](3) / maxX * 255, 2) +
                                              pow(centers[j](4) / maxY * 255 - supercenters[i](4) / maxY * 255, 2));
                    // check which supercluster is the closest one and whether it is in the deltazone
                    float tmpDis = distance_col+distance_pix;
                    if (tmpDis < mindistance && tmpDis <= deltaDistance ) {
                        index = i;
                        mindistance = tmpDis;
                    }
                }
                // if no cluster was in the deltazone it is the beginning of a new supercluster
                if (index == -1) {
                    supercenters.push_back(centers[j]);
                    index = supercenters.size()-1;
                }
                superClusterPosition[frameCounter].push_back(index);
            }
            // update supercenters
            for (int i = 0; i < centers.size(); i++) {
                int pos = superClusterPosition[frameCounter][i];
                supercenters[pos] = centers[i];
            }
        }
        stringstream filename;
        filename << "clustered" << frameCounter;
        Printer::printImg(frames[frameCounter],filename.str(),memberOfCluster,centers);
        cout << "frame: " <<frameCounter <<" written" << endl;
    }
    cout << "Supercenter count: " << supercenters.size() << endl;
    for (int i = 0; i < superClusterPosition.size(); i++) {
        cout << "SuperclusterPosition at " << i <<"   ";
        for (int j = 0; j < superClusterPosition[i].size(); j++) {
            int tmp = superClusterPosition[i][j];
            cout << tmp << " ";
        }
        cout << endl;
    }
    return;
}

//todo check if initialCentersize is needed
void KMeansClus::kMeansAlgorithm(Mat img, int clusterToSplit, int initialCentersize) {
    Calculator* calculator = this->calculator;
    if (clusterToSplit != -1) {
        calculator = new CoordEucCalculator(calculator->getMaxX(),calculator->getMaxY());
    }
    // the index of this array assigns the pixels of frames[i] to the corresponding clusters
    int changes;
    float limitChanges = (float)(img.cols*img.rows);
    limitChanges*=0.001;
    int counter = 0;
    do
    {
        counter ++;
        changes = 0;
        Vec5i ColorAndPixelSpace;
        for (int y = 0; y < img.cols; y++)
        {
            for (int x = 0; x < img.rows; x++)
            {
                if ((clusterToSplit != -1) &&
                    !(memberOfCluster[x][y] == clusterToSplit || memberOfCluster[x][y] > initialCentersize-1) ) continue;
                Vec3b tmp = img.at<Vec3b>(x, y);
                for (int i = 0; i < 3; i++) ColorAndPixelSpace[i]=(int)tmp[i];
                ColorAndPixelSpace[3] = x;
                ColorAndPixelSpace[4] = y;
                int cluster = 0;
                if (clusterToSplit != -1)  cluster = clusterToSplit;
                float min = calculator->distance((Vec5f)ColorAndPixelSpace, centers[cluster]);
                if (clusterToSplit == -1) {
                    for (int i = 1; i < centers.size(); i++)
                    {
                        float distance = calculator->distance(ColorAndPixelSpace,centers[i]);
                        if (distance < min)
                        {
                            min = distance;
                            cluster = i;
                        }
                    }
                }
                else {
                    for (int i = initialCentersize; i < centers.size(); i++) {
                        float distance = calculator->distance(ColorAndPixelSpace, centers[i]);
                        if (distance < min) {
                        min = distance;
                        cluster = i;
                        }
                    }
                }
                if (memberOfCluster[x][y] != cluster)
                {
                    memberOfCluster[x][y] = cluster;
                    changes++;
                }
            }
        }
        // calculate new centers
        vector<Vec6f> newCenters(centers.size());
        std::fill(newCenters.begin(), newCenters.end(), Vec6f(0,0,0,0,0,0));
        for (int y = 0; y < img.cols; y++)
        {
            for (int x = 0; x < img.rows; x++)
            {
                int cluster = memberOfCluster[x][y];
                Vec3b tmp = img.at<Vec3b>(x, y);
                newCenters[cluster]+=Vec6f(tmp[0],tmp[1],tmp[2],x,y,0);
                newCenters[cluster](5)++;
            }
        }
        for (int i = 0; i < centers.size(); i++)
        {
            for (int j = 0; j < 5; j++)
            {
                if (newCenters[i](5) > 0) newCenters[i](j) /= newCenters[i](5);
                if (newCenters[i](j) < 0) newCenters[i](j) = 0;
                if (j < 3 ) if (newCenters[i](j) > 255) newCenters[i](j) = 255;
                if (j==3) if (newCenters[i](j) > calculator->getMaxX()) newCenters[i](j) = calculator->getMaxX();
                if (j==4) if (newCenters[i](j) > calculator->getMaxY()) newCenters[i](j) = calculator->getMaxY();
            }
            if (newCenters[i](5) > 0) centers[i] = newCenters[i];
            else centers[i](5)= 0;
        }
        if (counter > 10) limitChanges = pow(limitChanges,2);
    } while (changes > limitChanges);
}

float KMeansClus::getValidity(Mat img) {
    return CalculateK::getValidity(img, calculator, &centers, &memberOfCluster);
}


