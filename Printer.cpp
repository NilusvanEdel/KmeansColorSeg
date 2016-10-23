//
// Created by nilus on 19.07.16.
//

#include "Printer.hpp"
#include <typeinfo>
#include <boost/filesystem.hpp>

using namespace std;
using namespace cv;

void Printer::printImg(Mat img,boost::filesystem::path path , string filename) {
    stringstream file;
    file << path.string() << filename << ".png";
    imwrite(file.str(),img);
}

void Printer::printImg(Mat img, boost::filesystem::path path, string filename,
                       vector <vector<int>> memberOfCluster, vector<Vec6f> centers) {
    Mat temp (img.size(),img.type());
    Vec3b tmp(0,0,0);
    for (int y = 0; y < img.cols; y++)
    {
        for (int x = 0; x < img.rows; x++)
        {
            tmp(0) = centers[memberOfCluster[x][y]](0);
            tmp(1) = centers[memberOfCluster[x][y]](1);
            tmp(2) = centers[memberOfCluster[x][y]](2);
            temp.at<Vec3b>(x,y) = tmp;
        }
    }
    stringstream file;
    file << path.string() << filename << ".png";
    imwrite(file.str(),temp);
}
void Printer::debugPrintImg(Mat img,boost::filesystem::path path, string filename, int clusterCount,
                            vector<vector<int>> memberOfCluster, vector<Vec6f> centers) {
    int counter = 0;
    for (int i = 0; i < centers.size(); i++) {
        stringstream folder;
        folder<<path.string() <<"/clusterCount" << clusterCount <<"/";
        boost::filesystem::create_directories(folder.str());
        Mat temp (img.size(),img.type());
        for (int y = 0; y < img.cols; y++)
        {
            for (int x = 0; x < img.rows; x++)
            {
                int cluster = memberOfCluster[x][y];
                Vec3b tmp;
                if (centers[i][5] == 0) tmp = Vec3b(255,255,255);
                else {
                    tmp = Vec3b(255,255,255);
                    if (memberOfCluster[x][y] == i) {
                        tmp(0) = centers[memberOfCluster[x][y]](0);
                        tmp(1) = centers[memberOfCluster[x][y]](1);
                        tmp(2) = centers[memberOfCluster[x][y]](2);
                    }
                }
                temp.at<Vec3b>(x, y) = tmp;
            }
        }
        stringstream file;
        file << folder.str() << filename << clusterCount << "cluster_" << counter<< ".png";
        imwrite(file.str(),temp);
        counter ++;
    }
    stringstream combindedFile;
    combindedFile << "combined" << clusterCount;
    Printer::printImg(img, path, combindedFile.str(),memberOfCluster,centers);
}
void Printer::printCountours(Mat img,boost::filesystem::path path, vector<vector<Point>> contours) {
    Mat temp (img.size(),img.type());
    Vec3b tmp = Vec3b(255,255,255);
    for (int y = 0; y < img.cols; y++) {
        for (int x = 0; x < img.rows; x++) {
            temp.at<Vec3b>(x, y) = tmp;
        }
    }
    Mat temp2;
    Mat temp3;
    temp.copyTo(temp2);
    temp.copyTo(temp3);
    tmp = Vec3b(0,0,0);
    for( int i = 0; i< contours.size(); i++ ) {
        stringstream file;
        for (int j = 0; j < contours[i].size(); j++) {
            temp.at<Vec3b>(contours[i][j].y,contours[i][j].x) = tmp;
            temp3.at<Vec3b>(contours[i][j].y,contours[i][j].x) = tmp;
        }
        file << path.string() << "contours" << i <<".png";
        imwrite(file.str(),temp);
        temp2.copyTo(temp);
    }
    stringstream file;
    string filedest = "/home/nilus/test/";
    file << filedest << "all_contours.png";
    imwrite(file.str(),temp3);
}
