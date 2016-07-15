//
// Created by nilus on 18.06.16.
//

#include "HSVEucCalculator.hpp"

HSVEucCalculator::HSVEucCalculator() {
    eucCalc = new EuclidianCalculator();
}

float HSVEucCalculator::distance(Vec3f vec1, Vec3f vec2) {
    return eucCalc->distance(Vec3f(vec1[0], vec1[1], 0), Vec3f(vec2[0], vec2[1], 0));
}
float HSVEucCalculator::distance(Vec3f vec1, Vec4f vec2) {
    return eucCalc->distance(vec1, Vec3f(vec2.val[0], vec2.val[1], vec2.val[2]));
}
float HSVEucCalculator::distance(Vec4f vec1, Vec4f vec2) {
    return eucCalc->distance(Vec3f(vec1.val[0],vec1.val[1],0), Vec3f(vec2.val[0], vec2.val[1],0));
}
float HSVEucCalculator::distance(Vec4f vec1, Vec3f vec2) {
    return eucCalc->distance(Vec3f(vec1.val[0],vec1.val[1],0), Vec3f(vec2.val[0], vec2.val[1],0));
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