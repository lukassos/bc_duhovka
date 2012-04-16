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
    void countTotalEnergyInt(Snake snake);
    enum ContourType{
        ClosedContour_Circle,
        ClosedContour_Polygon,
        OpenContou_Line,
        OpenContour_Arc
    };

};

#endif // ENERGYINTERNALTEMPLATE_H
