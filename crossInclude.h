#ifndef CROSSINCLUDE_H
#define CROSSINCLUDE_H

#include "energyexternalfield.h"
#include "energyinternaltemplate.h"
#include "snake.h"

class Snake;
class SnakePoint;
class EnergyInternalTemplate;
    void setCirclePositions(QList<SnakePoint> points, float centerX, float ceterY, float radius);
    void fastCenterLocalizationAlgorithm(Mat image, cv::Point fastCenter, float radius);

class EnergyExternalField;


#endif // CROSSINCLUDE_H
