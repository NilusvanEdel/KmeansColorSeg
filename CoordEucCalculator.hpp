//
// Created by nilus on 21.07.16.
//

#ifndef NAIVEAPPROACHKMEANSCSEG_COORDEUCCALCULATOR_HPP
#define NAIVEAPPROACHKMEANSCSEG_COORDEUCCALCULATOR_HPP

#include "Calculator.hpp"

// returns the distance/variance when only considering the geomtrical features (xy) and the euclidean distance
class CoordEucCalculator: public Calculator {
public:
    CoordEucCalculator(int maxX, int maxY);
    ~CoordEucCalculator()  {}
    float distance(Vec5f vec1, Vec5f vec2);
    float distance(Vec5f vec1, Vec6f vec2);
    float distance(Vec6f vec1, Vec6f vec2);
    float distance(Vec6f vec1, Vec5f vec2);

    Vec5f variance(Vec5f vec1, Vec6f vec2);
    Vec5f variance(Vec6f vec1, Vec5f vec2);

    float singleVariance(Vec5f vector);

    int getMaxX() {return maxX; }
    int getMaxY() {return maxY; }

private:
    int maxX;
    int maxY;
};


#endif //NAIVEAPPROACHKMEANSCSEG_COORDEUCCALCULATOR_HPP
