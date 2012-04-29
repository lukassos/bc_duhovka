#ifndef ENERGYINTERNALTEMPLATE_H
#define ENERGYINTERNALTEMPLATE_H

#include <QMainWindow>
#include <QtGUI>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <qmath.h>

#include "crossInclude.h"

class SnakePoint;
class Snake;

using namespace cv;


class EnergyInternalTemplate
{
protected:
    int type;

public:
    EnergyInternalTemplate();

    float countPotentialPositionX(int pointNumber, Snake snake);
    float countPotentialPositionY(int pointNumber, Snake snake);

    float countLocalEnergyInt(Snake snake, int point, int newX, int newY);
    float countLocalEnergyInt1stage(Snake snake, int point, int newX, int newY);
    float countLocalEnergyInt2stage(Snake snake, int point, int newX, int newY);
    float getAverageDistance(Snake snake);
    void countContourEstimation(Snake *snake);
    bool largerThanContourOfNeighbors(Snake snake, int pointOfSnake, bool plus_not_minus);

    void countTotalEnergyInt(Snake *snake);

    enum ContourType{
        ClosedContour_Circle,
        ClosedContour_Polygon,
        ClosedContour_Rectangle,
        OpenContou_Line,
        OpenContour_Arc
    };

};

#endif // ENERGYINTERNALTEMPLATE_H
