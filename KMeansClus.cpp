//
// Created by nilus on 10.06.16.
//

#include "KMeansClus.hpp"
#include <random>
#include <opencv2/imgproc.hpp>
#include "HSVEucCalculator.hpp"
#include <typeinfo>

typedef Vec<float, 5> Vec5f;
typedef Vec<float, 6> Vec6f;
typedef Vec<int, 5> Vec5i;
typedef Vec<int, 6> Vec6i;

KMeansClus::KMeansClus(vector<Mat> f, Calculator* calculator, bool realVid) {
    this->calculator = calculator;
    this->realVid = realVid;
    frames = f;
    memberOfCluster.resize(frames[0].rows);
    for (int i = 0; i < frames[0].rows; ++i)
        memberOfCluster[i].resize(frames[0].cols);
    for (int y = 0; y < frames[0].rows; y++)
    {
        for (int x = 0; x < frames[0].cols; x++)
        {
            memberOfCluster[y][x] = 0;
        }
    }
    calculateK();

}
KMeansClus::KMeansClus(vector<Mat> f,int k, Calculator* calculator, bool realVid) {
    this->calculator = calculator;
    this->realVid = realVid;
    frames = f;
    centers = vector<Vec6f>(k);
    memberOfCluster.resize(frames[0].rows);
    for (int i = 0; i < frames[0].rows; ++i)
        memberOfCluster[i].resize(frames[0].cols);
    for (int y = 0; y < frames[0].rows; y++)
    {
        for (int x = 0; x < frames[0].cols; x++)
        {
            memberOfCluster[y][x] = 0;
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
        kMeansAlgorithm(frames[frameCounter]);
        Mat temp (frames[frameCounter].size(),frames[frameCounter].type());
        Vec3b tmp(0,0,0);
        for (int y = 0; y < frames[frameCounter].rows; y++)
        {
            for (int x = 0; x < frames[frameCounter].cols; x++)
            {
                tmp(0) = centers[memberOfCluster[y][x]](0);
                tmp(1) = centers[memberOfCluster[y][x]](1);
                tmp(2) = centers[memberOfCluster[y][x]](2);
                temp.at<Vec3b>(y,x) = tmp;
            }
        }
        stringstream file;
        file << "/home/nilus/test/test" << frameCounter << ".jpg";
        imwrite(file.str(),temp);
        cout << "frame: " <<frameCounter <<" written" << endl;
    }
    return;
}

void KMeansClus::kMeansAlgorithm(Mat img) {
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
        for (int y = 0; y < img.rows; y++)
        {
            for (int x = 0; x < img.cols; x++)
            {
                Vec3b tmp = img.at<Vec3b>(y, x);
                for (int i = 0; i < 3; i++) ColorAndPixelSpace[i]=tmp[i];
                ColorAndPixelSpace[4] = x;
                ColorAndPixelSpace[5] = y;
                int cluster = 0;
                float min = calculator->distance(ColorAndPixelSpace,centers[0]);
                for (int i = 1; i < centers.size(); i++)
                {
                    float distance = calculator->distance(ColorAndPixelSpace,centers[i]);
                    if (distance < min)
                    {
                        min = distance;
                        cluster = i;
                    }
                }
                if (memberOfCluster[y][x] != cluster)
                {
                    memberOfCluster[y][x] = cluster;
                    changes++;
                }
            }
        }
        // calculate new centers
        vector<Vec6f> newCenters(centers.size());
        std::fill(newCenters.begin(), newCenters.end(), Vec6f(0,0,0,0,0,0));
        for (int y = 0; y < img.rows; y++)
        {
            for (int x = 0; x < img.cols; x++)
            {
                int cluster = memberOfCluster[y][x];
                Vec3b tmp = img.at<Vec3b>(y, x);
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
                else if (j==4) if (newCenters[i](j) > calculator->getMaxX()) newCenters[i](j) = calculator->getMaxX();
                else if (j==5) if (newCenters[i](j) > calculator->getMaxY()) newCenters[i](j) = calculator->getMaxY();
            }
            if (newCenters[i](5) > 0) centers[i] = newCenters[i];
            else centers[i](5)= 0;
        }
        if (counter > 10) limitChanges = pow(limitChanges,2);
    } while (changes > limitChanges);
}

int KMeansClus::calculateK() {
    Mat img = frames[0];
    vector<float> validity;
    // calculate the mean of all values -> the center for cluster 1
    Vec5f mean= {0,0,0,0,0};
    // the calculated centers for each iteration are stored in this vector
    vector<vector <Vec6f>> bestCenters;
    for (int y = 0; y < img.rows; y++)
    {
        for (int x = 0; x < img.cols; x++)
        {
            Vec3b tmp = img.at<Vec3b>(y, x);
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
        for (auto it= minColAndPixSpace.begin(); it != minColAndPixSpace.end(); ++it)
            *it = Vec5i(255,255,255,calculator->getMaxX(),calculator->getMaxY());
        for (auto it= maxColAndPixSpace.begin(); it != maxColAndPixSpace.end(); ++it) *it = Vec5i(0, 0, 0, 0, 0);
        for (auto it=variance.begin(); it != variance.end(); ++it) *it = Vec5f(0,0,0,0,0);
        // calculate the variances and find the maxColAndPixSpace as well as the minColAndPixSpace in each cluster
        for (int y = 0; y < img.rows; y++)
        {
            for (int x = 0; x < img.cols; x++)
            {
                int cluster = memberOfCluster[y][x];
                Vec3b tmp = img.at<Vec3b>(y,x);
                variance[cluster] += calculator->variance(Vec5f(tmp[0],tmp[1],tmp[2],x,y),centers[cluster]);
                for (int i = 0; i < 3; i++) {
                    if (img.at<Vec3b>(y,x)[i] < minColAndPixSpace[cluster][i]) minColAndPixSpace[cluster][i] = img.at<Vec3b>(y, x)[i];
                    if (img.at<Vec3b>(y,x)[i] > maxColAndPixSpace[cluster][i]) maxColAndPixSpace[cluster][i] = img.at<Vec3b>(y, x)[i];
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
            variance[i] /= centers[i][3];
            for (int j = 0; j < 3; j++) singleVariance[i]+=calculator->singleVariance(variance[i][j],centers[i]);
        }
        float maxVariance = 0;

        int clusterToSplit = 0;
        for (int i = 0; i < singleVariance.size(); i++) {
            if (singleVariance[i]>maxVariance) clusterToSplit = i;
        }
        // calculate the new clusters centers
        Vec5f oldClusterCenter =
                Vec5f(centers[clusterToSplit][0],centers[clusterToSplit][1],centers[clusterToSplit][2],
                      centers[clusterToSplit][3],centers[clusterToSplit][4]);
        Vec5f a = (oldClusterCenter-(Vec5f) maxColAndPixSpace[clusterToSplit]) / 2;
        Vec5f newClusterCenter = oldClusterCenter - a;
        centers[clusterToSplit] = Vec6f(newClusterCenter.val[0],newClusterCenter.val[1],newClusterCenter.val[2],
                                        newClusterCenter.val[3],newClusterCenter.val[4],0);
        newClusterCenter = oldClusterCenter + a;
        centers.push_back(Vec6f(newClusterCenter.val[0],newClusterCenter.val[1],newClusterCenter.val[2],
                                newClusterCenter.val[3],newClusterCenter.val[4],0));
        for (int i = 0; i < 3; i++) {
            int test = centers.size();
            if (centers[clusterToSplit][i] < 0 ) centers[clusterToSplit][i] = 0;
            if (centers[centers.size()-1][i] < 0 ) centers[centers.size()-1][i] = 0;
            if (centers[clusterToSplit][i] > 255 ) centers[clusterToSplit][i] = 255;
            if (centers[centers.size()-1][i] > 255 ) centers[centers.size()-1][i] = 255;
        }
        // reuse the kMeans algorithm and calculate the new intra and inter meassurments
        kMeansAlgorithm(img);
        float intraMeassure = 0;
        for(int y = 0; y < img.rows; y++) {
            for (int x = 0; x < img.cols; x++) {
                Vec3b tmp = img.at<Vec3b>(y,x);
                intraMeassure += calculator->distance(Vec5f(tmp[0],tmp[1],tmp[2],x,y),centers[memberOfCluster[y][x]]);
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
        bestCenters.push_back(centers);
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
                if (!(i == validity.size()-1) && !locMax)
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
    cout << "best k: " << bestK << endl;
    return bestK;
}
