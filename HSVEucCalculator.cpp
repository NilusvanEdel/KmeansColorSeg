//
// Created by nilus on 18.06.16.
//

#include "HSVEucCalculator.hpp"

HSVEucCalculator::HSVEucCalculator(int maxX, int maxY) {
    this->maxX = maxX;
    this->maxY = maxY;
}

float HSVEucCalculator::distance(Vec5f vec1, Vec5f vec2) {
    float result = 0;
    // distanceColor calculation using every attribute with a relation of 1:1
    for (int i = 0; i < 2; i++) {
        // the enumarator has to be unequal to zero
        if (vec1[i]-vec2[i] != 0) {
            result += (pow((vec1[i] - vec2[i]), 2));
        }
    }
    return sqrt(result);
}
float HSVEucCalculator::distance(Vec5f vec1, Vec6f vec2) {
    return distance(vec1, Vec5f(vec2.val[0], vec2.val[1], vec2.val[2],vec2.val[3],vec2.val[4]));
}
float HSVEucCalculator::distance(Vec6f vec1, Vec6f vec2) {
    return distance(Vec5f(vec1.val[0],vec1.val[1],vec1.val[2],vec1.val[3],vec1.val[4]),
                    Vec5f(vec2.val[0],vec2.val[1],vec2.val[2],vec2.val[3],vec2.val[4]));
}
float HSVEucCalculator::distance(Vec6f vec1, Vec5f vec2) {
    return distance(Vec5f(vec1.val[0],vec1.val[1],vec1.val[2],vec1.val[3],vec1.val[4]),vec2);
}

Vec5f HSVEucCalculator::variance(Vec5f vec1, Vec6f vec2) {
    Vec5f variance(0,0,0,0,0);
    for (int i = 0; i < 2; i++) {
        variance[i] = pow(vec1.val[i]-vec2.val[i],2);;
    }
    return variance;
}
Vec5f HSVEucCalculator::variance(Vec6f vec1, Vec5f vec2) {
    return variance(vec2, vec1);
}

float HSVEucCalculator::singleVariance(Vec5f vector) {
    float variance = 0;
    for (int i = 0; i < 2; i++) variance += vector[i];
    return variance/2;
}
int HSVEucCalculator::getMaxX() {
    return maxX;
}
int HSVEucCalculator::getMaxY() {
    return maxY;
}