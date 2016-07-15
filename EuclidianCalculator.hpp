//
// Created by nilus on 18.06.16.
//

#ifndef NAIVEAPPROACHKMEANSCSEG_EUCLIDIANCALCULATOR_HPP
#define NAIVEAPPROACHKMEANSCSEG_EUCLIDIANCALCULATOR_HPP

#include "Calculator.hpp"

class EuclidianCalculator : public Calculator{
public:
    EuclidianCalculator(int maxX, int maxY);
    ~EuclidianCalculator() {}
    float distance(Vec5f vec1, Vec5f vec2);
    float distance(Vec5f vec1, Vec6f vec2);
    float distance(Vec6f vec1, Vec6f vec2);
    float distance(Vec6f vec1, Vec5f vec2);

    Vec5f variance(Vec5f vec1, Vec5f vec2);
    Vec5f variance(Vec6f vec1, Vec5f vec2);
    Vec5f variance(Vec5f vec1, Vec6f vec2);

    /* combines the variances of each attribute into a single one
     * the problem is to mash the two metrices (collorSpace and pixelSpace) together
     * for each attribute I define the maximal possible Variance with regards to the mean as 100%
     * hence the single variance = sum of (variance[i]/(maxVariance[i]/100)) for all i
    */
    float singleVariance(Vec5f vector, Vec6f meanVec);

    int getMaxX() { return maxX; }
    int getMaxY() { return maxY; }
private:
    // the maximal possible distances for maxColorDis and maxPixelDis
    int maxColorDis = 255;
    int maxX;
    int maxY;

};

#endif //NAIVEAPPROACHKMEANSCSEG_EUCLIDIANCALCULATOR_HPP
