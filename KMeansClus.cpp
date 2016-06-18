//
// Created by nilus on 10.06.16.
//

#include "KMeansClus.hpp"
#include <random>
#include <opencv2/imgproc.hpp>

KMeansClus::KMeansClus(vector<Mat> f, Calculator* calculator) {
    this->calculator = calculator;
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
    clusterVec.resize(1);
    calculateK();

}
KMeansClus::KMeansClus(vector<Mat> f,int k, Calculator* calculator) {
    this->calculator = calculator;
    frames = f;
    centers = vector<Vec4f>(k);
    clusterVec.resize(k);
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
    uniform_real_distribution<float> distribution2(0, 1);
    for (int i = 0; i < centers.size(); i++) {
        centers[i](0) = distribution(generator);
        centers[i](1) = distribution(generator);
        centers[i](2) = distribution(generator);
    }
}

KMeansClus::~KMeansClus() {
    vector<Mat>().swap(frames);
    vector<Vec4f>().swap(centers);
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
    do
    {
        changes = 0;
        Vec3i hsv;
        for (int y = 0; y < img.rows; y++)
        {
            for (int x = 0; x < img.cols; x++)
            {
                hsv = img.at<Vec3b>(y, x);
                int cluster = 0;
                // using normal euclidian distance
                float min = calculator->distance(hsv,centers[0]);
                for (int i = 1; i < centers.size(); i++)
                {
                    float distance = calculator->distance(hsv,centers[i]);
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
        vector<Vec4f> newCenters(centers.size());
        std::fill(newCenters.begin(), newCenters.end(), Vec4f(0,0,0,0));
        for (int y = 0; y < img.rows; y++)
        {
            for (int x = 0; x < img.cols; x++)
            {
                int cluster = memberOfCluster[y][x];
                Vec3b tmp = img.at<Vec3b>(y, x);
                newCenters[cluster]+=Vec4f(tmp[0],tmp[1],tmp[2],0);
                newCenters[cluster](3)++;
            }
        }
        for (int i = 0; i < centers.size(); i++)
        {
            for (int j = 0; j < 3; j++)
            {
                if (newCenters[i](3) > 0) newCenters[i](j) /= newCenters[i](3);
                if (newCenters[i](j) < 0) newCenters[i](j) = 0;
                if (newCenters[i](j) > 255) newCenters[i](j) = 255;
            }
            if (newCenters[i](3) > 0) centers[i] = newCenters[i];
            else centers[i](3)= 0;
        }
    } while (changes != 0);
}

int KMeansClus::calculateK() {
    Mat img = frames[0];
    vector<float> validity;
    // calculate the mean of all values -> the center for cluster 1
    Vec3f mean= {0,0,0};
    // the calculated centers for each iteration are stored in this vector
    vector<vector <Vec4f>> bestCenters;
    for(int i = 0; i < img.rows; i++)
    {
        const Vec3b* mi = img.ptr<Vec3b>(i);
        for(int j = 0; j < img.cols; j++) {
            mean += mi[j];
        }
    }
    mean /= (float(img.rows)*float(img.cols));
    centers.push_back(Vec4f{mean[0],mean[1],mean[2],img.rows*img.cols});
    // begin the loop to split the center with highest variance
    for (int k = 1; k < kMax; k++)
    {
        cout << "K: " << k << endl;
        // calculate the variance of the clusters
        vector <Vec3f> variance(centers.size());
        vector <Vec3i> minHSV(centers.size());
        vector <Vec3i> maxHSV(centers.size());
        for (auto it=variance.begin(); it != variance.end(); ++it) *it = Vec3f(0,0,0);
        for (auto it=minHSV.begin(); it != minHSV.end(); ++it) *it = Vec3i(255,255,255);
        for (auto it=maxHSV.begin(); it != maxHSV.end(); ++it) *it = Vec3i(0,0,0);
        for (int y = 0; y < img.rows; y++)
        {
            for (int x = 0; x < img.cols; x++)
            {
                int cluster = memberOfCluster[y][x];
                variance[cluster] += calculator->variance(img.at<Vec3b>(y,x),centers[cluster]);
                for (int i = 0; i < 3; i++) {
                    if (img.at<Vec3b>(y,x)[i] < minHSV[cluster][i]) minHSV[cluster][i] = img.at<Vec3b>(y,x)[i];
                    if (img.at<Vec3b>(y,x)[i] > maxHSV[cluster][i]) maxHSV[cluster][i] = img.at<Vec3b>(y,x)[i];
                }
            }
        }
        vector <float> singleVariance(variance.size());
        fill(singleVariance.begin(),singleVariance.end(),0);
        for (int i = 0; i < centers.size(); i++) {
            variance[i] /= centers[i][3];
            // should be < 3 for RGB
            for (int j = 0; j < 2; j++) singleVariance[i]+=variance[i][j];
        }
        // divided by 3 for RGB
        float maxVariance = 0;
        int clusterToSplit = 0;
        for (int i = 0; i < singleVariance.size(); i++) {
            singleVariance[i]/=2;
            if (singleVariance[i]>maxVariance) clusterToSplit = i;
        }
        // calculate the new clusters centers
        Vec3f oldClusterCenter =
                Vec3f(centers[clusterToSplit][0],centers[clusterToSplit][1],centers[clusterToSplit][2]);
        Vec3f a = (oldClusterCenter-(Vec3f)maxHSV[clusterToSplit])/2;
        Vec3f newClusterCenter = oldClusterCenter - a;
        centers[clusterToSplit] = Vec4f(newClusterCenter.val[0],newClusterCenter.val[1],newClusterCenter.val[2],0);
        newClusterCenter = oldClusterCenter + a;
        centers.push_back(Vec4f(newClusterCenter.val[0],newClusterCenter.val[1],newClusterCenter.val[2],0));
        for (int i = 0; i < 3; i++) {
            int test = centers.size();
            if (centers[clusterToSplit][i] < 0 ) centers[clusterToSplit][i] = 0;
            if (centers[centers.size()-1][i] < 0 ) centers[centers.size()-1][i] = 0;
            if (centers[clusterToSplit][i] > 255 ) centers[clusterToSplit][i] = 255;
            if (centers[centers.size()-1][i] > 255 ) centers[centers.size()-1][i] = 255;
        }
        // reuse the kMeans algorithm
        kMeansAlgorithm(img);
        float intraMeassure = 0;
        for(int y = 0; y < img.rows; y++) {
            for (int x = 0; x < img.cols; x++) {
                intraMeassure += calculator->distance(img.at<Vec3b>(y,x),centers[memberOfCluster[y][x]]);
            }
        }
        intraMeassure /= img.rows*img.cols;
        float interMeassure = calculator->distance(centers[0],centers[1]);
        for (int i = 0; i < centers.size(); i++) {
            for (int j = 0; j < centers.size(); j++) {
                if (j==i) continue;
                float distance = calculator->distance(centers[i],centers[j]);
                if (distance < interMeassure) interMeassure = distance;
            }
        }
        // the actual validity, note: if max at validity[0] --> k=2
        float currentValidity = intraMeassure/interMeassure;
        bestCenters.push_back(centers);
        validity.push_back(currentValidity);
    }
    // best K is the minimal value after the first local Maximum
    int bestK, biasedBestK= 2;
    float minValidity, biasedMinValidity = validity[0];
    bool locMax = false;
    // find the best K
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
    centers = bestCenters[bestK-2];
    cout << "best k: " << bestK << endl;
    return bestK;
}
