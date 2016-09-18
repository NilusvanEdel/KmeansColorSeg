//
// Created by nilus on 18.06.16.
//

#ifndef NAIVEAPPROACHKMEANSCSEG_PIXEUCCALCULATOR_HPP
#define NAIVEAPPROACHKMEANSCSEG_PIXEUCCALCULATOR_HPP

#include "Calculator.hpp"

class PixEucCalculator : public Calculator{
public:
    PixEucCalculator(int maxX, int maxY);
    ~PixEucCalculator() {}
    float distance(Vec5f vec1, Vec5f vec2);
    float distance(Vec5f vec1, Vec6f vec2);
    float distance(Vec6f vec1, Vec6f vec2);
    float distance(Vec6f vec1, Vec5f vec2);

    Vec5f variance(Vec5f vec1, Vec6f vec2);
    Vec5f variance(Vec6f vec1, Vec5f vec2);

    /* combines the variances of each attribute into a single one
     * the problem is to mash the two metrices (collorSpace and pixelSpace) together
     * for each attribute I define the maximal possible Variance with regards to the mean as 100%
     * hence the single variance = sum of (variance[i]/(maxVariance[i]/100)) for all i
    */
    float singleVariance(Vec5f vector);

    int getMaxX() { return maxX; }
    int getMaxY() { return maxY; }
private:
    // the maximal possible distances for maxColorDis and maxPixelDis
    int maxColorDis = 255;
    int maxX;
    int maxY;
    float scaling = 0.04;

};

#endif //NAIVEAPPROACHKMEANSCSEG_PixEucCalculator_HPP