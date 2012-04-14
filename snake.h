#ifndef SNAKE_H
#define SNAKE_H

#include <QMainWindow>
#include <QtGUI>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <qmath.h>
#include "energyexternalfield.h"
#include "energyinternaltemplate.h"

using namespace std;
namespace EI {
    class energyInternalTemplate;

}
namespace EE {
    class energyExternalField;

}

class snakePoint
{//one control point of entire contour
protected:
    float x, y; // coordnates
    float alpha, beta;
    float E_int, E_ext, E_snake;

public:
    snakePoint();

};


class snake
{//controller of all snake parts and fuctions


protected:
    QList<snakePoint> contour;
    energyInternalTemplate contourTemplate;
    energyExternalField vectorField;
    float total_E_ext;
    float total_E_int;

public:
    snake();

            // set`s linked list of snakePoints and sets contour this.template
    void initSnakeContour(snake snakeToInit, int numberOfPoints, energyInternalTemplate::ContourType decision);

            //set`s coordinates to snakePoints and sets radius circle/polygon created by those points
    void initSnakeContour(snake snakeToInit, int numberOfPoints, energyInternalTemplate::ContourType decision,
                          float offsetX, float offsety, float radius);

            //input is picture read into matrix in grayscale
            //set`s
    void initSnakeExtField(CvMat *pictureMatrix);


};



#endif // SNAKE_H
