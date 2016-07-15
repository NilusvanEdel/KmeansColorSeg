//
// Created by nilus on 18.06.16.
//

#include "EuclidianCalculator.hpp"

EuclidianCalculator::EuclidianCalculator(int maxX, int maxY) {
    this->maxColorDis = maxColorDis;
    this->maxX = maxX;
    this->maxY = maxY;
}


float EuclidianCalculator::distance(Vec5f vec1, Vec5f vec2) {
    float result = 0;
    // distance calculation using every attribute with a relation of 1:1
    for (int i = 0; i < 3; i++) {
        // the enumarator has to be unequal to zero
        if (vec1[i]-vec2[i] != 0) {
            result += (pow((vec1[i] - vec2[i]), 2) / (pow(maxColorDis, 2) / 100));
        }
    }
    if (vec1[4] != vec2[4]) result += (pow((vec1[4] - vec2[4]), 2)/(pow(maxX,2))/100);
    if (vec1[5] != vec2[5]) result += (pow((vec1[5] - vec2[5]), 2)/(pow(maxY,2))/100);
    return sqrt(result);
}
float_t EuclidianCalculator::distance(Vec5f vec1, Vec6f vec2) {
    return distance(vec1, Vec5f(vec2.val[0], vec2.val[1], vec2.val[2],vec2.val[3],vec2.val[4]));
}
float EuclidianCalculator::distance(Vec6f vec1, Vec6f vec2) {
    return distance(Vec5f(vec1.val[0],vec1.val[1],vec1.val[2],vec1.val[3],vec1.val[4]),
                    Vec5f(vec2.val[0],vec2.val[1],vec2.val[2],vec2.val[3],vec2.val[4]));
}
float EuclidianCalculator::distance(Vec6f vec1, Vec5f vec2) {
    return distance(Vec5f(vec1.val[0],vec1.val[1],vec1.val[2],vec1.val[3],vec1.val[4]),vec2);
}


Vec5f EuclidianCalculator::variance(Vec5f vec1, Vec5f vec2) {
    Vec5f variance;
    for (int i = 0; i < vec1.rows; i++) variance[i] = pow(vec1.val[i]-vec2.val[i],2);
    return variance;
}
Vec5f EuclidianCalculator::variance(Vec6f vec1, Vec5f vec2) {
    return variance(Vec5f(vec1.val[0],vec1.val[1],vec1.val[2],vec1.val[3],vec1.val[4]),vec2);
}
Vec5f EuclidianCalculator::variance(Vec5f vec1, Vec6f vec2) {
    return variance(vec1, Vec5f(vec2.val[0],vec2.val[1],vec2.val[2],vec2.val[3],vec2.val[4]));
}


float EuclidianCalculator::singleVariance(Vec5f vector, Vec6f meanVec) {
    float variance = 0;
    int memberCount = meanVec[5];
    for (int i = 0; i < vector.rows; i++) {
        if (i < 3) {
            variance += vector[i]/(max(pow(0-meanVec.val[i],2)*memberCount,pow(255-meanVec.val[i],2)*memberCount)/100);
        }
        if (i == 4)
            variance += vector[i]/(max(pow(0-meanVec.val[i],2)*memberCount,pow(maxX-meanVec.val[i],2)*memberCount)/100);
        if ( i == 5)
            variance += vector[i]/(max(pow(0-meanVec.val[i],2)*memberCount,pow(maxY-meanVec.val[i],2)*memberCount)/100);
    }
    return variance/vector.rows;
}