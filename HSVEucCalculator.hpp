//
// Created by nilus on 18.06.16.
//

#ifndef NAIVEAPPROACHKMEANSCSEG_HSVEUCCALCULATOR_HPP
#define NAIVEAPPROACHKMEANSCSEG_HSVEUCCALCULATOR_HPP

#include "Calculator.hpp"
#include "EuclidianCalculator.hpp"

class HSVEucCalculator : public Calculator{
public:
    HSVEucCalculator(int maxColorDis, int maxX, int maxY);
    ~HSVEucCalculator() {}
    float distance(Vec5f vec1, Vec5f vec2);
    float distance(Vec5f vec1, Vec6f vec2);
    float distance(Vec6f vec1, Vec6f vec2);
    float distance(Vec6f vec1, Vec5f vec2);

    Vec5f variance(Vec5f vec1, Vec5f vec2);
    Vec5f variance(Vec6f vec1, Vec5f vec2);
    Vec5f variance(Vec5f vec1, Vec6f vec2);

private:
    EuclidianCalculator* eucCalc;
    // the maximal possible distances for maxColorDis and maxPixelDis
    int maxColorDis;
    int maxX;
    int maxY;
};

#endif //NAIVEAPPROACHKMEANSCSEG_HSVEUCCALCULATOR_HPP
