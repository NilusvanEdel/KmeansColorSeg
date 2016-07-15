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


class Calculator {

public:
    virtual ~Calculator()  {}
    virtual float distance(Vec3f vec1, Vec3f vec2) = 0;
    virtual float distance(Vec3f vec1, Vec4f vec2) = 0;
    virtual float distance(Vec4f vec1, Vec4f vec2) = 0;
    virtual float distance(Vec4f vec1, Vec3f vec2) = 0;

    virtual Vec3f variance(Vec3f vec1, Vec3f vec2) = 0;
    virtual Vec3f variance(Vec4f vec1, Vec3f vec2) = 0;
    virtual Vec3f variance(Vec3f vec1, Vec4f vec2) = 0;
};

#endif //NAIVEAPPROACHKMEANSCSEG_CALCULATOR_HPP
