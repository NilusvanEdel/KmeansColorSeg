//
// Created by nilus on 10.06.16.
//

#include <boost/filesystem/operations.hpp>
#include "KMeansClus.hpp"
#include "Printer.hpp"
#include "CoordEucCalculator.hpp"
#include "CalculateK.hpp"

typedef Vec<float, 5> Vec5f;
typedef Vec<float, 6> Vec6f;
typedef Vec<int, 5> Vec5i;
typedef Vec<int, 6> Vec6i;

KMeansClus::KMeansClus(Calculator* calculator, bool realVid, string fileLoc, string filedest) {
    this->calculator = calculator;
    this->realVid = realVid;
    path = boost::filesystem::path(filedest);
    this->fileLoc = boost::filesystem::path(fileLoc);
    vector<cv::String> filenames; // notice the Opencv's embedded "String" class
    cv::String folder = fileLoc;
    glob(folder, filenames);
    Mat img;
    img = imread(filenames[0]);
    memberOfCluster.resize(img.rows);
    for (int i = 0; i < img.rows; ++i)
        memberOfCluster[i].resize(img.cols);
    for (int y = 0; y < img.cols; y++)
    {
        for (int x = 0; x < img.rows; x++)
        {
            memberOfCluster[x][y] = 0;
        }
    }
    CalculateK::calculateK(img, calculator, &centers, this, path, &memberOfCluster,
                           realVid, &bestCenters);
    img.release();
}

KMeansClus::KMeansClus(int k, Calculator* calculator, bool realVid, string fileLoc, string filedest) {
    this->calculator = calculator;
    this->realVid = realVid;
    path = boost::filesystem::path(filedest);
    this->fileLoc = boost::filesystem::path(fileLoc);
    centers = vector<Vec6f>(k);
    vector<cv::String> filenames; // notice the Opencv's embedded "String" class
    cv::String folder = fileLoc;
    glob(folder, filenames);
    Mat img;
    img = imread(filenames[0]);
    memberOfCluster.resize(img.cols);
    for (int i = 0; i < img.rows; ++i)
        memberOfCluster[i].resize(img.cols);
    for (int y = 0; y < img.cols; y++)
    {
        for (int x = 0; x < img.rows; x++)
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
    kMeansAlgorithm(img);
    Printer::debugPrintImg(img, path, "testForK_", k, memberOfCluster, centers);
    img.release();
}

//deallocator
KMeansClus::~KMeansClus() {
    vector<Vec6f>().swap(centers);
    vector<vector<int>>().swap(memberOfCluster);
}


void KMeansClus::startClustering(int frameCount) {
    // begin of the loop
    vector<cv::String> filenames; // notice the Opencv's embedded "String" class
    cv::String folder = fileLoc.string();
    glob(folder, filenames);
    superClusterPosition.resize(frameCount);
    for (int i = 0; i < centers.size(); i++) {
        supercenters.push_back(centers[i]);
        superClusterPosition[0].push_back(i);
    }
    Mat img;
    img = imread(filenames[0]);
    stringstream filename;
    filename << "clustered0" ;
    Printer::debugPrintImg(img, path, "testForK_", centers.size(), memberOfCluster, centers);
    Printer::printImg(img,path, filename.str(),memberOfCluster,centers);


    vecMemOfCluster = vector<vector<vector<int>>>(frameCount);
    vecMemOfCluster[0]= memberOfCluster;
    //toggle here and below to print all clusters
    stringstream newFolder;
    newFolder<<path.string() <<"/clusters_" << 0 <<"/";
    boost::filesystem::create_directories(newFolder.str());
    boost::filesystem::path newPath = boost::filesystem::path(newFolder.str());
    Printer::debugPrintImg(img, newPath, "test_", centers.size(), memberOfCluster, centers);

    cout << "frame: " << 0 <<" written" << endl;
    for (int frameCounter = 1; frameCounter < frameCount; frameCounter++)
    {
        img = imread(filenames[frameCounter]);
        // bestk size = 0 equals k = 2
        int currentBestk = centers.size();
        float validityKSmaller = INT_MAX;
        if (currentBestk > 3) {
            centers = bestCenters[currentBestk - 4];
            kMeansAlgorithm(img);
            bestCenters[currentBestk - 4] = centers;
            CalculateK::splitCluster(img, calculator, &centers, &memberOfCluster);
        }
        else if (currentBestk == 3) {
            centers = bestCenters[currentBestk - 3];
        }
        if (currentBestk >= 3) {
            kMeansAlgorithm(img);
            bestCenters[currentBestk - 3] = centers;
            validityKSmaller = getValidity(img);
            CalculateK::splitCluster(img, calculator, &centers, &memberOfCluster);
        }
        kMeansAlgorithm(img);
        float validity = CalculateK::getValidity(img, calculator, &centers, &memberOfCluster);
        bestCenters[currentBestk-2] = centers;
        vector <vector<int>> memberOfClusterTemp(memberOfCluster);
        float validityKBigger = INT_MAX;
        if (currentBestk < bestCenters.size()) {
            CalculateK::splitCluster(img, calculator, &centers, &memberOfCluster);
            kMeansAlgorithm(img);
            validityKBigger = getValidity(img);
            bestCenters[currentBestk-1] = centers;
        }
        centers = bestCenters[currentBestk-2];
        memberOfCluster = memberOfClusterTemp;
        // if k is still a local Minima
        if (!(validity <= validityKBigger && validity < validityKSmaller)) {
            cout << "new K Calculation" << endl;
            CalculateK::calculateK(img, calculator, &centers, this,
                                   path, &memberOfCluster,realVid, &bestCenters);
        }

        // toggle for correct results (will perform full dynamic clustercount algorithm on every frame)
        //
        /* CalculateK::calculateK(img, calculator,
                               &centers, this, path, &memberOfCluster, realVid, &bestCenters); */

        // if no superclusters exist, the first centers will be the first superclusters
        // check which new clusters belong to which supercluster or if they need to create a new one

        for (int j = 0; j < centers.size(); j++) {
            float mindistance = 99999999;
            int index = -1;
            int maxX = img.rows;
            int maxY = img.cols;
            // the delta which will determine whether it still belongs to the same supercluster
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

        stringstream filename;
        filename << "clustered" << frameCounter;
        Printer::debugPrintImg(img, path,"testForK_", centers.size(), memberOfCluster, centers);
        Printer::printImg(img, path, filename.str(),memberOfCluster,centers);
        cout << "frame: " <<frameCounter <<" written" << endl;

        vecMemOfCluster[frameCounter]= memberOfCluster;
        //toggle to print all clusters
        stringstream newFolder;
        newFolder<<path.string() <<"/clusters_" << frameCounter <<"/";
        boost::filesystem::create_directories(newFolder.str());
        boost::filesystem::path newPath = boost::filesystem::path(newFolder.str());
        Printer::debugPrintImg(img, newPath, "test_", centers.size(), memberOfCluster, centers);
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

    // print only using the superClusters
    for (int i = 0; i < superClusterPosition.size(); i++) {
        for (int j = 0; j < superClusterPosition[i].size(); j++) {
            centers.resize(superClusterPosition[i].size());
            centers[j] = supercenters[superClusterPosition[i][j]];
        }
        stringstream superClusterPrintName;
        superClusterPrintName << "superCluster_" << i;
        Printer::printImg(img, path, superClusterPrintName.str(),vecMemOfCluster[i],centers);
    }
    return;
}

void KMeansClus::kMeansAlgorithm(Mat img) {
    kMeansAlgorithm(img, this->calculator);
}

void KMeansClus::kMeansAlgorithm(Mat img, Calculator* calculator) {
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
                Vec3b tmp = img.at<Vec3b>(x, y);
                for (int i = 0; i < 3; i++) ColorAndPixelSpace[i]=(int)tmp[i];
                ColorAndPixelSpace[3] = x;
                ColorAndPixelSpace[4] = y;
                int cluster = 0;
                float min = calculator->distance((Vec5f)ColorAndPixelSpace, centers[cluster]);
                for (int i = 1; i < centers.size(); i++)
                {
                    float distance = calculator->distance(ColorAndPixelSpace,centers[i]);
                    if (distance < min)
                    {
                        min = distance;
                        cluster = i;
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
                newCenters[cluster] += Vec6f(tmp[0],tmp[1],tmp[2],x,y,0);
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

void KMeansClus::setCalculator(Calculator *calculator) {
    this->calculator = calculator;
}

void KMeansClus::kMeansLimitedAlgorithm(Mat img, vector <vector<int>>* memberOfCluster, Calculator* calculator,
                                         vector <int> limitedCluster) {
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
            bool partOfCluster;
            for (int x = 0; x < img.rows; x++)
            {
                partOfCluster = false;
                for (int i = 0; i < limitedCluster.size(); i++) {
                    if ((*memberOfCluster)[x][y] == limitedCluster[i]) {
                        partOfCluster = true;
                    }
                }
                if (!partOfCluster) continue;
                Vec3b tmp = img.at<Vec3b>(x, y);
                for (int i = 0; i < 3; i++) ColorAndPixelSpace[i]=(int)tmp[i];
                ColorAndPixelSpace[3] = x;
                ColorAndPixelSpace[4] = y;
                int cluster = limitedCluster[0];
                float min = calculator->distance((Vec5f)ColorAndPixelSpace, centers[cluster]);
                int i = cluster;
                for (int i = 1; i < limitedCluster.size(); i++){
                    float distance = calculator->distance(ColorAndPixelSpace,centers[limitedCluster[i]]);
                    if (distance < min)
                    {
                        min = distance;
                        cluster = limitedCluster[i];
                    }
                }
                if ((*memberOfCluster)[x][y] != cluster)
                {
                    (*memberOfCluster)[x][y] = cluster;
                    changes++;
                }
            }
            if (!partOfCluster) continue;
        }
        // calculate new centers
        vector<Vec6f> newCenters(centers.size());
        std::fill(newCenters.begin(), newCenters.end(), Vec6f(0,0,0,0,0,0));
        for (int y = 0; y < img.cols; y++)
        {
            for (int x = 0; x < img.rows; x++)
            {
                int cluster = (*memberOfCluster)[x][y];
                Vec3b tmp = img.at<Vec3b>(x, y);
                newCenters[cluster] += Vec6f(tmp[0],tmp[1],tmp[2],x,y,0);
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
