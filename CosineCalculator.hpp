//
// Created by nilus on 18.06.16.
//

#ifndef NAIVEAPPROACHKMEANSCSEG_COSINECALCULATOR_HPP
#define NAIVEAPPROACHKMEANSCSEG_COSINECALCULATOR_HPP


#include "Calculator.hpp"
#include "EuclidianCalculator.hpp"

class CosineCalculator : public Calculator{
public:
    CosineCalculator();
    ~CosineCalculator() {}
    float distance(Vec3f vec1, Vec3f vec2);
    float distance(Vec3f vec1, Vec4f vec2);
    float distance(Vec4f vec1, Vec4f vec2);
    float distance(Vec4f vec1, Vec3f vec2);

    Vec3f variance(Vec3f vec1, Vec3f vec2);
    Vec3f variance(Vec4f vec1, Vec3f vec2);
    Vec3f variance(Vec3f vec1, Vec4f vec2);
private:
    EuclidianCalculator* eucCalc;

};


#endif //NAIVEAPPROACHKMEANSCSEG_COSINECALCULATOR_HPP
