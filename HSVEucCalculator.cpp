//
// Created by nilus on 18.06.16.
//

#include "HSVEucCalculator.hpp"

HSVEucCalculator::HSVEucCalculator(int maxColorDis, int maxX, int maxY) {
    this->maxColorDis -= 255;
    eucCalc = new EuclidianCalculator(this->maxColorDis, maxX, maxY);
}

float HSVEucCalculator::distance(Vec5f vec1, Vec5f vec2) {
    return eucCalc->distance(Vec5f(vec1[0], vec1[1], 0, vec1[3],vec1[4]), Vec3f(vec2[0], vec2[1], 0, vec2[3],vec2[4]));
}
float HSVEucCalculator::distance(Vec5f vec1, Vec6f vec2) {
    return eucCalc->distance(vec1, Vec5f(vec2.val[0], vec2.val[1], 0),vec2.val[3],vec2.val[4]);
}
float HSVEucCalculator::distance(Vec6f vec1, Vec6f vec2) {
    return eucCalc->distance(Vec5f(vec1.val[0],vec1.val[1],0,vec1.val[3],vec1.val[4]),
                             Vec5f(vec2.val[0], vec2.val[1],0,vec2.val[3],vec2.val[4]));
}
float HSVEucCalculator::distance(Vec6f vec1, Vec5f vec2) {
    return eucCalc->distance(Vec5f(vec1.val[0],vec1.val[1],0,vec1.val[3],vec1.val[4]),
                             Vec5f(vec2.val[0], vec2.val[1],0,vec2.val[3],vec2.val[4]));
}

Vec3f HSVEucCalculator::variance(Vec3f vec1, Vec3f vec2) {
    Vec3f variance;
    for (int i = 0; i < 2; i++) variance[i] = pow(vec1.val[i]-vec2.val[i],2);
    return variance;
}
Vec3f HSVEucCalculator::variance(Vec4f vec1, Vec3f vec2) {
    return variance(Vec3f(vec1.val[0],vec1.val[1],vec1.val[2]),vec2);
}
Vec3f HSVEucCalculator::variance(Vec3f vec1, Vec4f vec2) {
    return variance(vec1, Vec3f(vec2.val[0],vec2.val[1],vec2.val[2]));
}