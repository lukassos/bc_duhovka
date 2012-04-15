#ifndef ENERGYINTERNALTEMPLATE_H
#define ENERGYINTERNALTEMPLATE_H

#include <QMainWindow>
#include <QtGUI>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <qmath.h>

#include "crossInclude.h"


using namespace cv;


class EnergyInternalTemplate
{
protected:
    int type;

public:
    EnergyInternalTemplate();

    void setCirclePositions(QList<SnakePoint> points, float centerX, float ceterY, float radius);
    void fastCenterLocalizationAlgorithm(Mat image, cv::Point fastCenter, float radius);
    float countPotentialPositionX(int pointNumber, Snake snake);
    float countPotentialPositionY(int pointNumber, Snake snake);

    float countTotalEnergyInt(Snake snake);
    enum ContourType{
        ClosedContour_Circle,
        ClosedContour_Polygon,
        OpenContou_Line,
        OpenContour_Arc
    };

};

#endif // ENERGYINTERNALTEMPLATE_H
