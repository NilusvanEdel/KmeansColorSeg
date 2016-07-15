//
// Created by nilus on 18.06.16.
//

#include "EuclidianCalculator.hpp"

float EuclidianCalculator::distance(Vec3f vec1, Vec3f vec2) {
    float result = 0;
    for (int i = 0; i < vec1.rows; i++) {
        result += pow((vec1[i] - vec2[i]), 2);
    }
    return sqrt(result);
}
float_t EuclidianCalculator::distance(Vec3f vec1, Vec4f vec2) {
    return distance(vec1, Vec3f(vec2.val[0], vec2.val[1], vec2.val[2]));
}
float EuclidianCalculator::distance(Vec4f vec1, Vec4f vec2) {
    return distance(Vec3f(vec1.val[0],vec1.val[1],vec1.val[2]), Vec3f(vec2.val[0],vec2.val[1],vec2.val[2]));
}
float EuclidianCalculator::distance(Vec4f vec1, Vec3f vec2) {
    return distance(Vec3f(vec1.val[0],vec1.val[1],vec1.val[2]),vec2);
}

Vec3f EuclidianCalculator::variance(Vec3f vec1, Vec3f vec2) {
    Vec3f variance;
    for (int i = 0; i < vec1.rows; i++) variance[i] = pow(vec1.val[i]-vec2.val[i],2);
    return variance;
}
Vec3f EuclidianCalculator::variance(Vec4f vec1, Vec3f vec2) {
    return variance(Vec3f(vec1.val[0],vec1.val[1],vec1.val[2]),vec2);
}
Vec3f EuclidianCalculator::variance(Vec3f vec1, Vec4f vec2) {
    return variance(vec1, Vec3f(vec2.val[0],vec2.val[1],vec2.val[2]));
}