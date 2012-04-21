#ifndef SNAKE_H
#define SNAKE_H

#include <QMainWindow>
#include <QtGUI>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <qmath.h>

#include "crossInclude.h"
#include "snakepoint.h"

using namespace std;
using namespace cv;

namespace snakeNames{
    class Snake;
}


class Snake
{//controller of all snake parts and fuctions

public:
    QList<SnakePoint*> contour;
//    EnergyInternalTemplate contourTemplate;
    EnergyExternalField *vectorField;
    float total_E_ext;
    float total_E_int;
    int typeOfContour;
    Mat originalImage;
    Mat showImage;
    Mat matrixOfPoints;



    //constructors
    Snake();
    Snake(Mat image);
    Snake(Mat image, QList<SnakePoint*> points);

    // INTERNAL ENERGY

    //set`s coordinates to snakePoints and sets radius circle/polygon created by those points
    void initSnakeContour(Snake* snakeToInit, int numberOfPoints, int energy_int_type, int energy_ext_type,
                  float offsetX=0, float offsetY=0,
                  float baseAlpha = 0.2, float baseBeta = 0.5,
                  float deviation = 24, int baseStep = 1,
                  int fastInitKernel = 17);

    //functions called during initialization
    float fastCenterLocalizationAlgorithm(Mat image, cv::Point *fastCenter, int k  = 11);
    void setCirclePositions(QList <SnakePoint*> points, float centerX, float ceterY, float radius, int maxX, int maxY);

    // EXTERNAL ENERGY

    //input is picture read into matrix in grayscale
    //set`s
    void initSnakeExtField(Snake *snake, int type, float deviation);

    //this function sets external energy to all snake control points and counts total external energy
    //same functionality is included in moveSnakeContour
    void countTotalEnergyExt(Snake *snake);

    // ITERATIONS OF SNAKE
    void moveSnakeContour(Snake *snake);

    // demo
    void showMatrix(Snake *snake);
    bool saveSnakeToTextFile(Snake *snake);

    Mat getImageOriginal(){return this->originalImage;}
    Mat* getImageOriginalPointer(){return &this->originalImage;}
    Mat getMatrixOfPoints(){return this->matrixOfPoints;}

};



#endif // SNAKE_H
