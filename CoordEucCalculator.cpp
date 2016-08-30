//
// Created by nilus on 21.07.16.
//

#include "CoordEucCalculator.hpp"

CoordEucCalculator::CoordEucCalculator(int maxX, int maxY) {
    this->maxX = maxX;
    this->maxY = maxY;
}

float CoordEucCalculator::distance(Vec5f vec1, Vec5f vec2) {
    float result = 0;
    // distanceColor calculation using every attribute with a relation of 1:1
    for (int i = 3; i < 5; i++) {
            result += (pow((vec1[i] - vec2[i]), 2));
    }
    result = sqrt(result);
    return result;
}
float_t CoordEucCalculator::distance(Vec5f vec1, Vec6f vec2) {
    return distance(vec1, Vec5f(vec2.val[0], vec2.val[1], vec2.val[2],vec2.val[3],vec2.val[4]));
}
float CoordEucCalculator::distance(Vec6f vec1, Vec6f vec2) {
    return distance(Vec5f(vec1.val[0],vec1.val[1],vec1.val[2],vec1.val[3],vec1.val[4]),
                    Vec5f(vec2.val[0],vec2.val[1],vec2.val[2],vec2.val[3],vec2.val[4]));
}
float CoordEucCalculator::distance(Vec6f vec1, Vec5f vec2) {
    return distance(Vec5f(vec1.val[0],vec1.val[1],vec1.val[2],vec1.val[3],vec1.val[4]),vec2);
}

Vec5f CoordEucCalculator::variance(Vec5f vec1, Vec6f vec2) {
    Vec5f variance;
    for (int i = 3; i < 5; i++) {
        variance[i] = pow(vec1.val[i]-vec2.val[i],2);;
    }
    return variance;
}
Vec5f CoordEucCalculator::variance(Vec6f vec1, Vec5f vec2) {
    return variance(vec2, vec1);
}

float CoordEucCalculator::singleVariance(Vec5f vector) {
    float variance = 0;
    for (int i = 3; i < 5; i++) variance += vector[i];
    return variance/2;
}