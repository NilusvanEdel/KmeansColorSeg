//
// Created by nilus on 18.06.16.
//

#include "CosineCalculator.hpp"

CosineCalculator::CosineCalculator() {
    eucCalc = new EuclidianCalculator();
}
float CosineCalculator::distance(Vec3f vec1, Vec3f vec2) {
    float counter = 0;
    float eucLengthVec1 = 0;
    float eucLengthVec2 = 0;
    for (int i=0; i < 3; i++) {
        counter += vec1[i]*vec2[i];
        eucLengthVec1 += vec1[i]*vec1[i];
        eucLengthVec2 += vec2[i]*vec2[i];
    }
    float similarity = 0;
    if (!(counter==0||eucLengthVec1==0||eucLengthVec2==0)) similarity =
                                                                   counter/(sqrt(eucLengthVec1)*sqrt(eucLengthVec2));
    return 1-similarity;
}
float CosineCalculator::distance(Vec3f vec1, Vec4f vec2) {
    return distance(vec1, Vec3f(vec2.val[0], vec2.val[1], vec2.val[2]));
}
float CosineCalculator::distance(Vec4f vec1, Vec4f vec2) {
    return distance(Vec3f(vec1.val[0],vec1.val[1],vec1.val[2]), Vec3f(vec2.val[0],vec2.val[1],vec2.val[2]));
}
float CosineCalculator::distance(Vec4f vec1, Vec3f vec2) {
    return distance(Vec3f(vec1.val[0],vec1.val[1],vec1.val[2]),vec2);
}

Vec3f CosineCalculator::variance(Vec3f vec1, Vec3f vec2) {
    Vec3f variance;
    for (int i = 0; i < 2; i++) variance[i] = pow(vec1.val[i]-vec2.val[i],2);
    return variance;
}
Vec3f CosineCalculator::variance(Vec4f vec1, Vec3f vec2) {
    return variance(Vec3f(vec1.val[0],vec1.val[1],vec1.val[2]),vec2);
}
Vec3f CosineCalculator::variance(Vec3f vec1, Vec4f vec2) {
    return variance(vec1, Vec3f(vec2.val[0],vec2.val[1],vec2.val[2]));
}