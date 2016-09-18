//
// Created by nilus on 16.06.16.
//

#ifndef NAIVEAPPROACHKMEANSCSEG_CALCULATOR_HPP
#define NAIVEAPPROACHKMEANSCSEG_CALCULATOR_HPP

#include "opencv2/highgui/highgui.hpp"
#include <iostream>
#include <string>

using namespace std;
using namespace cv;

typedef Vec<float, 5> Vec5f;
typedef Vec<float, 6> Vec6f;
typedef Vec<int, 5> Vec5i;
typedef Vec<int, 5> Vec5i;

class Calculator {

public:
    virtual ~Calculator()  {}
    virtual float distance(Vec5f vec1, Vec5f vec2) = 0;
    virtual float distance(Vec5f vec1, Vec6f vec2) = 0;
    virtual float distance(Vec6f vec1, Vec6f vec2) = 0;
    virtual float distance(Vec6f vec1, Vec5f vec2) = 0;

    // the division needed to get the actual variance occurs later on
    virtual Vec5f variance(Vec5f vec1, Vec6f vec2) = 0;
    virtual Vec5f variance(Vec6f vec1, Vec5f vec2) = 0;

    virtual float singleVariance(Vec5f vector) = 0;

    virtual int getMaxX() = 0;
    virtual int getMaxY() = 0;
private:
    int maxX;
    int maxY;
};

#endif //NAIVEAPPROACHKMEANSCSEG_CALCULATOR_HPP
