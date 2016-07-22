//
// Created by nilus on 10.06.16.
//

#include "KMeansClus.hpp"
#include "Printer.hpp"
#include "CoordEucCalculator.hpp"

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
    calculateK(frames[0]);

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
    }
}

KMeansClus::~KMeansClus() {
    vector<Mat>().swap(frames);
    vector<Vec6f>().swap(centers);
    vector<vector<int>>().swap(memberOfCluster);
}

void KMeansClus::startClustering() {
    // begin of the loop
    for (int frameCounter = 0; frameCounter < frames.size(); frameCounter++)
    {
        kMeansAlgorithm(frames[frameCounter], -1);
        stringstream filename;
        filename << "clustered" << frameCounter;
        Printer::printImg(frames[frameCounter],filename.str(),memberOfCluster,centers);
        cout << "frame: " <<frameCounter <<" written" << endl;
    }
    return;
}

void KMeansClus::kMeansAlgorithm(Mat img, int clusterToSplit) {
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
                    !(memberOfCluster[x][y] == clusterToSplit || memberOfCluster[x][y] == centers.size()) ) continue;
                Vec3b tmp = img.at<Vec3b>(x, y);
                for (int i = 0; i < 3; i++) ColorAndPixelSpace[i]=(int)tmp[i];
                ColorAndPixelSpace[3] = x;
                ColorAndPixelSpace[4] = y;
                int cluster = 0;
                if (clusterToSplit != -1)  cluster = clusterToSplit;
                float min = calculator->distance((Vec5f)ColorAndPixelSpace,centers[cluster]);
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
                    if (calculator->distance(ColorAndPixelSpace,centers[centers.size()-1]) < min)
                        cluster = centers.size()-1;
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


int KMeansClus::calculateK(Mat img) {
    vector<float> validity;
    // calculate the mean of all values -> the center for cluster 1
    Vec5f mean= {0,0,0,0,0};
    // the calculated centers for each iteration are stored in this vector
    vector<vector <Vec6f>> bestCenters;
    for (int y = 0; y < frames[0].cols; y++)
    {
        for (int x = 0; x < frames[0].rows; x++)
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
    centers.push_back(Vec6f{mean[0],mean[1],mean[2],mean[3],mean[4],(float)img.rows*img.cols});
    // begin the loop to split the center with highest variance
    for (int k = 1; k < kMax; k++)
    {
        cout << "K: " << k << endl;
        // calculate the variance of the clusters
        // fill the vectors with 00000 or the maximum values of the attributes
        vector <Vec5f> variance(centers.size());
        vector <Vec5i> minColAndPixSpace(centers.size());
        vector <Vec5i> maxColAndPixSpace(centers.size());
        for (auto it= minColAndPixSpace.begin(); it != minColAndPixSpace.end(); it++)
            *it = Vec5i(255,255,255,calculator->getMaxX(),calculator->getMaxY());
        for (auto it= maxColAndPixSpace.begin(); it != maxColAndPixSpace.end(); it++) *it = Vec5i(0, 0, 0, 0, 0);
        for (auto it=variance.begin(); it != variance.end(); it++) *it = Vec5f(0,0,0,0,0);
        // calculate the variances and find the maxColAndPixSpace as well as the minColAndPixSpace in each cluster
        for (int y = 0; y < frames[0].cols; y++)
        {
            for (int x = 0; x < frames[0].rows; x++)
            {
                int cluster = memberOfCluster[x][y];
                Vec3b tmp = img.at<Vec3b>(x, y);
                variance[cluster] += calculator->variance(Vec5f(tmp[0],tmp[1],tmp[2],x,y),centers[cluster]);
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
        for (int i = 0; i < centers.size(); i++) {
            singleVariance[i]+=calculator->singleVariance(variance[i]);
        }
        float maxVariance = 0;

        int clusterToSplit = 0;
        for (int i = 0; i < singleVariance.size(); i++) {
            if (singleVariance[i]>maxVariance) clusterToSplit = i;
        }
        // calculate the new clusters centers
        Vec5f oldCCenter =
                Vec5f(centers[clusterToSplit][0],centers[clusterToSplit][1],centers[clusterToSplit][2],
                      centers[clusterToSplit][3],centers[clusterToSplit][4]);
        Vec5f a = (oldCCenter - (Vec5f)maxColAndPixSpace[clusterToSplit])/2;
        Vec5f newClusterCenter = oldCCenter - a;
        centers[clusterToSplit] = Vec6f(newClusterCenter.val[0],newClusterCenter.val[1],newClusterCenter.val[2],
                                        newClusterCenter.val[3],newClusterCenter.val[4],0);
        newClusterCenter = oldCCenter + a;
        centers.push_back(Vec6f(newClusterCenter.val[0],newClusterCenter.val[1],newClusterCenter.val[2],
                                newClusterCenter.val[3],newClusterCenter.val[4],0));
        // the new clusters may not exceed the limits
        for (int i = 0; i < 5; i++) {
            // limit fix for color space
            if (i < 3) {
                if (centers[clusterToSplit][i] < 0) centers[clusterToSplit][i] = 0;
                if (centers[centers.size() - 1][i] < 0) centers[centers.size() - 1][i] = 0;
                if (centers[clusterToSplit][i] > 255) centers[clusterToSplit][i] = 255;
                if (centers[centers.size() - 1][i] > 255) centers[centers.size() - 1][i] = 255;
            }
            // limit fix for coordinate space
            else if (i >= 3 ){
                if (centers[clusterToSplit][i] < 0) centers[clusterToSplit][i] = 0;
                if (centers[centers.size() - 1][i] < 0) centers[centers.size() - 1][i] = 0;
                if (i == 3) {
                    if (centers[clusterToSplit][i] > calculator->getMaxX())
                                centers[clusterToSplit][i] = calculator->getMaxX();
                    if (centers[centers.size() - 1][i] > calculator->getMaxX())
                                centers[centers.size() - 1][i] = calculator->getMaxX();
                }
                else if (i == 4)
                {
                    if (centers[clusterToSplit][i] > calculator->getMaxY())
                            centers[clusterToSplit][i] = calculator->getMaxY();
                    if (centers[centers.size() - 1][i] > calculator->getMaxY())
                            centers[centers.size() - 1][i] = calculator->getMaxY();
                }
            }
        }
        // reuse the kMeans algorithm and calculate the new intra and inter meassurments
        kMeansAlgorithm(img, -1);
        Printer::debugPrintImg(img, "testForK_", k+1, memberOfCluster, centers);
        float intraMeassure = 0;
        for (int y = 0; y < frames[0].cols; y++)
        {
            for (int x = 0; x < frames[0].rows; x++)
            {
                Vec3b tmp = img.at<Vec3b>(x, y);
                intraMeassure += calculator->distance(Vec5f(tmp[0],tmp[1],tmp[2],x,y),centers[memberOfCluster[x][y]]);
            }
        }
        intraMeassure /= img.rows*img.cols;
        float interMeassure = calculator->distance(centers[0],centers[1]);
        for (int i = 1; i < centers.size(); i++) {
            for (int j = 1; j < centers.size(); j++) {
                if (j==i) continue;
                float distance = calculator->distance(centers[i],centers[j]);
                if (distance < interMeassure) interMeassure = distance;
            }
        }
        // the actual validity, note: if max at validity[0] --> k=2
        float currentValidity = INT_MAX;
        if (interMeassure!=0) currentValidity = intraMeassure/interMeassure;
        //save current center
        bestCenters.push_back(centers);
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
    centers = bestCenters[bestK-2];
    cout << "best k before NeighborCheck: " << bestK << endl;
    centers[2][3] = 2;
    centers[2][4] = 8;
    centers.push_back(Vec6f(centers[2][0],centers[2][1],centers[2][2],11,0,0));
    kMeansAlgorithm(img,2);
    Printer::debugPrintImg(img, "PixelDisK_", 12, memberOfCluster, centers);
    /*
    neighborCheck(img, 4);
    bestK = centers.size();
    cout << "best k after NeighborCheck: " << bestK << endl;
     */
    return bestK;
}

bool KMeansClus::neighborCheck(Mat img, int neighbors) {
    for (int center = 0; center < centers.size(); center++) {
        for (int y = 0; y < frames[0].cols; y++) {
            for (int x = 0; x < frames[0].rows; x++) {
                if (memberOfCluster[x][y] != center) continue;
            }
        }
    }
}