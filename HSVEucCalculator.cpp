//
// Created by nilus on 18.06.16.
//

#include "HSVEucCalculator.hpp"

HSVEucCalculator::HSVEucCalculator(int maxX, int maxY) {
    eucCalc = new EuclidianCalculator();
}

float HSVEucCalculator::distance(Vec5f vec1, Vec5f vec2) {
    return eucCalc->distance(Vec5f(vec1[0], vec1[1], 0, vec1[3],vec1[4]), Vec5f(vec2[0], vec2[1], 0, vec2[3],vec2[4]));
}
float HSVEucCalculator::distance(Vec5f vec1, Vec6f vec2) {
    return distance(vec1, Vec5f(vec2.val[0], vec2.val[1], 0,vec2.val[3],vec2.val[4]));
}
float HSVEucCalculator::distance(Vec6f vec1, Vec6f vec2) {
    return eucCalc->distance(Vec5f(vec1.val[0],vec1.val[1],0,vec1.val[3],vec1.val[4]),
                             Vec5f(vec2.val[0], vec2.val[1],0,vec2.val[3],vec2.val[4]));
}
float HSVEucCalculator::distance(Vec6f vec1, Vec5f vec2) {
    return distance(Vec5f(vec1.val[0],vec1.val[1],0,vec1.val[3],vec1.val[4]),vec2);
}

Vec5f HSVEucCalculator::variance(Vec5f vec1, Vec5f vec2) {
    Vec5f variance = {0,0,0,0,0};
    for (int i = 0; i < 5; i++) if (!(i == 2)) variance[i] = pow(vec1.val[i]-vec2.val[i],2);
    return variance;
}
Vec5f HSVEucCalculator::variance(Vec6f vec1, Vec5f vec2) {
    return variance(Vec5f(vec1.val[0],vec1.val[1],vec1.val[2],vec1.val[3],vec1.val[4]),vec2);
}
Vec5f HSVEucCalculator::variance(Vec5f vec1, Vec6f vec2) {
    return variance(vec1, Vec5f(vec2.val[0],vec2.val[1],vec2.val[2],vec2.val[3],vec2.val[4]));
}

float HSVEucCalculator::singleVariance(Vec5f vector, Vec6f meanVec) {
    float variance = 0;
    int memberCount = meanVec[5];
    for (int i = 0; i < vector.rows; i++) {
        if (i ==2 ) continue;
        if (i < 2) {
            variance += vector[i]/(max(pow(0-meanVec.val[i],2), pow(255-meanVec.val[i],2))/100);
        }
        if (i == 4)
            variance += vector[i]/(max(pow(0-meanVec.val[i],2), pow(eucCalc->getMaxX()-meanVec.val[i],2))/100);
        if ( i == 5)
            variance += vector[i]/(max(pow(0-meanVec.val[i],2), pow(eucCalc->getMaxY()-meanVec.val[i],2))/100);
    }
    return variance/(vector.rows-1);
}

int HSVEucCalculator::getMaxX() {
    return eucCalc->getMaxX();
}
int HSVEucCalculator::getMaxY() {
    return eucCalc->getMaxY();
}