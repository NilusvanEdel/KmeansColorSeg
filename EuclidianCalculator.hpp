//
// Created by nilus on 18.06.16.
//

#ifndef NAIVEAPPROACHKMEANSCSEG_EUCLIDIANCALCULATOR_HPP
#define NAIVEAPPROACHKMEANSCSEG_EUCLIDIANCALCULATOR_HPP

#include "Calculator.hpp"

class EuclidianCalculator : public Calculator{
public:
    EuclidianCalculator() {}
    ~EuclidianCalculator() {}
    float distance(Vec3f vec1, Vec3f vec2);
    float distance(Vec3f vec1, Vec4f vec2);
    float distance(Vec4f vec1, Vec4f vec2);
    float distance(Vec4f vec1, Vec3f vec2);

    Vec3f variance(Vec3f vec1, Vec3f vec2);
    Vec3f variance(Vec4f vec1, Vec3f vec2);
    Vec3f variance(Vec3f vec1, Vec4f vec2);

};

#endif //NAIVEAPPROACHKMEANSCSEG_EUCLIDIANCALCULATOR_HPP
