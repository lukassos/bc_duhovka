#ifndef SNAKE_H
#define SNAKE_H

#include <QMainWindow>
#include <QtGUI>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <qmath.h>
#include "energyexternalfield.h"
#ifndef ENERGYINTERNALTEMPLATE_H
    #include "energyinternaltemplate.h"
#endif

using namespace std;
using namespace cv;

namespace snakeNames{
    class Snake;
    class SnakePoint;
}


#ifndef SNAKEPOINT_H
#define SNAKEPOINT_H
class SnakePoint
{//one control point of entire contour
protected:/*until getre and setre created commented
    float x, y; // coordnates
    float alpha, beta, step;
    float E_int, E_ext, E_snake;*/

public:
    SnakePoint();
    float getAlpha(){return this->alpha;}
    float getBeta(){return this->beta;}
    float getStep(){return this->step;}

    float x, y; // coordnates
    float alpha, beta, step;
    float E_int, E_ext, E_snake;
};
#endif

class Snake
{//controller of all snake parts and fuctions


protected:
    QList<SnakePoint> contour;
    EnergyInternalTemplate contourTemplate;
    EnergyExternalField vectorField;
    float total_E_ext;
    float total_E_int;
    int typeOfContour;
    Mat originalImage;
    Mat matrixOfPoints;


public:
    Snake();
    Mat getImageOriginal(){return this->originalImage;}
    Mat getMatrixOfPoints(){return this->matrixOfPoints;}
            // set`s linked list of snakePoints and sets contour this.template
    void initSnakeContour(Snake snakeToInit, int numberOfPoints, EnergyInternalTemplate::ContourType decision);

            //set`s coordinates to snakePoints and sets radius circle/polygon created by those points
    void initSnakeContour(Snake snakeToInit, int numberOfPoints, EnergyInternalTemplate::ContourType decision,
                          float offsetX, float offsety);

            //input is picture read into matrix in grayscale
            //set`s
    void initSnakeExtField(Mat pictureMatrix);

    void showMatrix(Mat image, Snake snake, QString windowName);



};



#endif // SNAKE_H
