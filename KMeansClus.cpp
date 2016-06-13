//
// Created by nilus on 10.06.16.
//

#include "KMeansClus.hpp"
#include <random>
#include <opencv2/imgproc.hpp>

KMeansClus::KMeansClus(vector<Mat> f,int k) {
    frames = f;
    vector<Vec4f> tmp(k);
    centers = tmp;
}

KMeansClus::~KMeansClus() { }

vector<Mat> KMeansClus::startClustering() {
    // generate random initial clusterpoints
    std::random_device generator;
    uniform_int_distribution<int> distribution(0, 255);
    uniform_real_distribution<float> distribution2(0, 1);
    for (int i = 0; i < centers.size(); i++) {
        centers[i](0) = distribution(generator);
        centers[i](1) = distribution(generator);
        centers[i](2) = distribution(generator);
    }
    // the index of this array assigns the pixels of frames[i] to the corresponding clusters
    int memberOfCluster[frames[0].rows][frames[0].cols] = {0};
    int changes;
    // begin of the loop
    for (int frameCounter = 0; frameCounter < frames.size(); frameCounter++)
    {
        do
        {
            changes = 0;
            for (int y = 0; y < frames[frameCounter].rows; y++)
            {
                for (int x = 0; x < frames[frameCounter].cols; x++)
                {
                    Vec3b rgb = frames[frameCounter].at<Vec3b>(y, x);
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

            // calculate new centers
            std::fill(centers.begin(), centers.end(), 0);
            for (int y = 0; y < frames[frameCounter].rows; y++)
            {
                for (int x = 0; x < frames[frameCounter].cols; x++)
                {
                    int cluster = memberOfCluster[y][x];
                    Vec3b tmp = frames[frameCounter].at<Vec3b>(y, x);
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
            cout << "Changes at frame "<<frameCounter<<": " << changes << endl;
        } while (changes > 10);

        // only for test purposes
        Mat test = Mat::zeros(frames[frameCounter].size(),frames[frameCounter].type());
        for (int y = 0; y < frames[frameCounter].rows; y++)
        {
            for (int x = 0; x < frames[frameCounter].cols; x++)
            {
                Vec3b tmp;
                tmp(0) = centers[memberOfCluster[y][x]](0);
                tmp(1) = centers[memberOfCluster[y][x]](1);
                tmp(2) = centers[memberOfCluster[y][x]](2);
                test.at<Vec3b>(y,x) = tmp;
            }
        }
        stringstream file;
        file << "/home/nilus/test/test.jpg" << frameCounter << ".jpg";
        imwrite(file.str(),test);
    }

}