//
// Created by nilus on 18.06.16.
//

#include "PixEucCalculator.hpp"

PixEucCalculator::PixEucCalculator(int maxX, int maxY) {
    this->maxX = maxX;
    this->maxY = maxY;
}


float PixEucCalculator::distance(Vec5f vec1, Vec5f vec2) {
    float result = 0;
    // distance calculation using every attribute with a relation of 1:1
    for (int i = 0; i < 5; i++) {
        // the enumarator has to be unequal to zero
        if (vec1[i]-vec2[i] != 0) {
            if (i < 3) result += (pow((vec1[i] - vec2[i]), 2) / (pow(maxColorDis, 2)));
            // rgb values will be multiplied with the scaling value and XY-coordinates with (3/2) to get x:1 scaling
            if (i == 3) result += (((pow((vec1[i] - vec2[i]), 2) / (pow(maxX, 2))) * 1.5) * scaling);
            if (i == 4) result += (((pow((vec1[i] - vec2[i]), 2) / (pow(maxY, 2))) * 1.5) * scaling);
        }
    }

    return sqrt(result);
}
float_t PixEucCalculator::distance(Vec5f vec1, Vec6f vec2) {
    return distance(vec1, Vec5f(vec2.val[0], vec2.val[1], vec2.val[2],vec2.val[3],vec2.val[4]));
}
float PixEucCalculator::distance(Vec6f vec1, Vec6f vec2) {
    return distance(Vec5f(vec1.val[0],vec1.val[1],vec1.val[2],vec1.val[3],vec1.val[4]),
                    Vec5f(vec2.val[0],vec2.val[1],vec2.val[2],vec2.val[3],vec2.val[4]));
}
float PixEucCalculator::distance(Vec6f vec1, Vec5f vec2) {
    return distance(Vec5f(vec1.val[0],vec1.val[1],vec1.val[2],vec1.val[3],vec1.val[4]),vec2);
}

Vec5f PixEucCalculator::variance(Vec5f vec1, Vec6f vec2) {
    Vec5f variance;
    for (int i = 0; i < 5; i++) {
        if (i < 3) {
            variance[i] = pow(vec1.val[i] - vec2.val[i], 2);
            //normalization
            if (abs(0-vec2.val[i]) > abs(255-vec2.val[i])) variance[i] /= abs(0-vec2.val[i]);
            else variance[i] /= abs(255-vec2.val[i]);
        }
        if (i == 3) {
            variance[i] = pow(vec1.val[i] - vec2.val[i], 2) ;
            // normalization
            if (abs(0-vec2.val[i]) > abs(maxX-vec2.val[i])) variance[i] /= abs(0-vec2.val[i]);
            else variance[i] /= abs(maxX-vec2.val[i]);
            variance[i] *= (1.5 * scaling);
        }
        if (i == 4) {
            variance[i] = pow(vec1.val[i] - vec2.val[i], 2);
            // normalization
            if (abs(0-vec2.val[i]) > abs(maxY-vec2.val[i])) variance[i] /= abs(0-vec2.val[i]);
            else variance[i] /= abs(maxY-vec2.val[i]);
            variance[i] *= (1.5 * scaling);
        }
    }
    return variance;
}
Vec5f PixEucCalculator::variance(Vec6f vec1, Vec5f vec2) {
    return variance(vec2, vec1);
}

float PixEucCalculator::singleVariance(Vec5f vector) {
    float variance = 0;
    for (int i = 0; i < 5; i++) variance += vector[i];
    return variance/5;
}