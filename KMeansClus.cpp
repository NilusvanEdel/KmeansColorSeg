//
// Created by nilus on 10.06.16.
//

#include "KMeansClus.hpp"
#include <random>
#include <opencv2/imgproc.hpp>

KMeansClus::KMeansClus(vector<Mat> f) {
    frames = f;
    // generate a 2D dynamic array
    memberOfCluster = new int*[frames[0].rows];
    for (int i = 0; i < frames[0].rows; ++i) {
        memberOfCluster[i] = new int[frames[0].cols];
    }
    for (int y = 0; y < frames[0].rows; y++)
    {
        for (int x = 0; x < frames[0].cols; x++)
        {
            memberOfCluster[y][x] = 0;
        }
    }
    calculateK();
}
KMeansClus::KMeansClus(vector<Mat> f,int k) {
    frames = f;
    vector<Vec4f> tmp(k);
    centers = tmp;
    //generate a 2D dynamic array
    memberOfCluster = new int*[frames[0].rows];
    for (int i = 0; i < frames[0].rows; ++i) {
        memberOfCluster[i] = new int[frames[0].cols];
    }
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
    for (int i = 0; i < frames[0].rows; ++i) {
        delete memberOfCluster[i];
    }
}

vector<Mat> KMeansClus::startClustering() {
    // begin of the loop
    for (int frameCounter = 0; frameCounter < frames.size(); frameCounter++)
    {
        frames[frameCounter] = kMeansAlgorithm(frames[frameCounter]);
        stringstream file;
        file << "/home/nilus/test/test.jpg" << frameCounter << ".jpg";
        imwrite(file.str(),frames[frameCounter]);
        cout << "frame: " <<frameCounter <<" written" << endl;
    }
}

int KMeansClus::calculateK() {
    vector<Vec4f> tmp(1);
    fill (tmp.begin(),tmp.end(),0);
    centers = tmp;
    vector <int> validity;
    Mat mat = kMeansAlgorithm(frames[0]);
    Vec3b minHSV = {255,255,255};
    Vec3b maxHSV = {0,0,0};
    int maxH, maxS, maxV = 0;
    for (int k=0; k<kMax; k++)
    {
        vector<Vec3f> variance(centers.size());
        std::fill (variance.begin(),variance.end(),0);
        // the intraMeassurement
        int intraMeassure = 0;
        // the interMeassurement
        int interMeassure = INT_MAX;
        for (int y = 0; y < frames[0].rows; y++)
        {
            for (int x = 0; x < frames[0].cols; x++)
            {
                // calculates (x-center)² and intraMeassurement=(sqrt(x-correspondingCenter)²)
                for (int i = 0; i < 3; i++) {
                    variance[memberOfCluster[y][x]](i) += pow(frames[0].at<Vec3b>(y,x)(i) - mat.at<Vec3b>(y,x)(i), 2);
                    if (!(y == 0&& x == 0)) variance[memberOfCluster[y][x]]/=2;
                    intraMeassure = sqrt(pow(variance[memberOfCluster[y][x]](i)-mat.at<Vec3b>(y,x)(i), 2));
                    if (frames[0].at<Vec3b>(y,x)(i) < minHSV(i))  minHSV(i) = frames[0].at<Vec3b>(y,x)(i);
                    if (frames[0].at<Vec3b>(y,x)(i) > maxHSV(i))  maxHSV(i) = frames[0].at<Vec3b>(y,x)(i);
                }

            }
        }
        // the actual intraMeassurement
        intraMeassure /= frames[0].rows * frames[0].cols;
        vector <int> singleVariance(centers.size());
        fill(singleVariance.begin(),singleVariance.end(),0);
        cout << "K: " << k << endl;
        for (int i=0;i < variance.size(); i++)
        {
            float tmp = 0;
            // divides by the actual numberOfClustermember and adds all to one single Variance
            for (int j = 0; j < 3; j++) singleVariance[i] += variance[i](j);
            for (auto it = singleVariance.begin(); it != singleVariance.end(); it++) *it /= 3;
            // calcuated the smallest interMeassurement sqrt((center i - center j)²)
            for (int j = 0; j < centers.size(); j++)
            {
                if (j==i) continue;
                for (int t = 0; t < 3; t++)
                {
                    tmp += sqrt(pow(centers[i](t) - centers[j](t),2));
                }
                tmp/=3;
                if (tmp < interMeassure) interMeassure = tmp;
            }

        }
        if (interMeassure == 0) validity.push_back(INT_MAX);
        else validity.push_back(intraMeassure/interMeassure);
        auto clusterToSplit = min_element(begin(singleVariance), end(singleVariance));
        cout <<  "cluster To Split: "<< *clusterToSplit << endl;
        Vec4f newCenter = {0,0,0,0};
        cout << "New Center: ";
        for (int t = 0; t < 3; t++)
        {
            centers[*clusterToSplit](t) -= (minHSV[t]+maxHSV[t])/2;
            newCenter(t) += (minHSV[t]+maxHSV[t])/2;
            if (centers[*clusterToSplit][t] < 0) centers[*clusterToSplit][t] = 0;
            if (newCenter[t] > 255) newCenter[t] = 255;
            cout << newCenter[t]<< " ";
        }
        cout << endl;
        centers.push_back(newCenter);
        kMeansAlgorithm(frames[0]);
    }
    auto minValidity = min_element(begin(validity), end(validity));
    cout << "best found k: " << *minValidity << endl;
}

Mat KMeansClus::kMeansAlgorithm(Mat mat) {
    // the index of this array assigns the pixels of frames[i] to the corresponding clusters
    int changes;
    do
    {
        changes = 0;
        if (centers.size() > 1)
        {
            for (int y = 0; y < mat.rows; y++)
            {
                for (int x = 0; x < mat.cols; x++)
                {
                    Vec3b rgb = mat.at<Vec3b>(y, x);
                    int h = rgb(0);
                    int s = rgb(1);
                    int v = rgb(2);
                    int cluster = 0;
                    // using normal euclidian distance
                    float min = sqrt(pow(centers[0](0) - h, 2) + pow(centers[0](1) - s, 2));
                    for (int i = 1; i < centers.size(); i++)
                    {
                        float distance = sqrt(
                                pow(centers[i](0) - h, 2) + pow(centers[i](1) - s, 2));
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
        }
        // calculate new centers
        std::fill(centers.begin(), centers.end(), 0);
        for (int y = 0; y < mat.rows; y++)
        {
            for (int x = 0; x < mat.cols; x++)
            {
                int cluster = memberOfCluster[y][x];
                Vec3b tmp = mat.at<Vec3b>(y, x);
                for (int j = 0; j < 3; j++) centers[cluster](j) += tmp(j);
                centers[cluster](3)++;
            }
        }
        for (int i = 0; i < centers.size(); i++)
        {
            for (int j = 0; j < 3; j++)
            {
                if (centers[i](3) > 0) centers[i](j) /= centers[i](3);
                if (centers[i](j) < 0) centers[i](j) = 0;
                if (centers[i](j) > 255) centers[i](j) = 255;
            }
            // cout << "Centers " << i << " H: " << centers[i](0) << " S: " << centers[i](1) << endl;
        }
    } while (changes != 0);
    // only for test purposes
    for (int y = 0; y < mat.rows; y++)
    {
        for (int x = 0; x < mat.cols; x++)
        {
            Vec3b tmp;
            tmp(0) = centers[memberOfCluster[y][x]](0);
            tmp(1) = centers[memberOfCluster[y][x]](1);
            tmp(2) = centers[memberOfCluster[y][x]](2);
            mat.at<Vec3b>(y,x) = tmp;
        }
    }
    return mat;
}