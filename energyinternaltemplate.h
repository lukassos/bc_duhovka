#ifndef ENERGYINTERNALTEMPLATE_H
#define ENERGYINTERNALTEMPLATE_H

#include <QMainWindow>
#include <QtGUI>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <qmath.h>
#include "snake.h"

class EnergyInternalTemplate
{
protected:
    ContourType type;

public:
    EnergyInternalTemplate();

    void setCirclePositions(QList <snakePoint> points, float radius);

    float countPotentialPositionX(int pointNumber, Snake snake);
    float countPotentialPositionY(int pointNumber, Snake snake);

    countTotalEnergyInt(Snake snake);
    enum ContourType{
        ClosedContour_Circle,
        ClosedContour_Polygon,
        OpenContou_Line,
        OpenContour_Arc
    };

};

#endif // ENERGYINTERNALTEMPLATE_H
