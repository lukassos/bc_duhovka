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
    float weight_E_ext;
    float weight_E_int;
    float avgDist;
    float contourThreshold;
    float edgeStrenghtThreshold;
    cv::Point possiblePupilCenter;

    //constructors
    Snake();
    Snake(Mat image);
    Snake(Mat image, QList<SnakePoint*> points);

    // INTERNAL ENERGY

    //set`s coordinates to snakePoints and sets radius circle/polygon created by those points
    void initSnakeContour(Snake* snakeToInit, int numberOfPoints, int energy_int_type, int energy_ext_type,
                  float weight_Eext = 1, float weight_Eint = 1,
                  float baseAlpha = 0.7, float baseBeta = 0.4,
                  float contourThresh = 0.3, float edgeStrenghtThresh = 120,
                  float deviation = 100, float scale = 3.5,
                  int baseStep = 1,
                  int fastInitKernel = 17,
                  int offsetX = 0, int offsetY = 0);

    //functions called during initialization
    float fastCenterLocalizationAlgorithm(Mat image, cv::Point *fastCenter, int k  = 11);
    void setCirclePositions(QList <SnakePoint*> points, float centerX, float ceterY, float radius, int maxX, int maxY);
    void setRectanglePositions(QList <SnakePoint*> points, float offsetX, float offsetY, float side_a, float side_b, int maxX, int maxY);

    void setAlphaToAllPoints(float alpha);

    void setBetaToAllPoints(float beta);

    void setStepToAllPoints(float step);

    // EXTERNAL ENERGY

    //input is picture read into matrix in grayscale
    //set`s
    void initSnakeExtField(Snake *snake, int energy_ext_type, float deviation, float scale, cv::Point possibleCenter =  cv::Point(0,0));

    //this function sets external energy to all snake control points and counts total external energy
    //same functionality is included in moveSnakeContour
    void countTotalEnergyExt(Snake *snake);

    // ITERATIONS OF SNAKE
    void moveSnakeContour(Snake *snake);

    // preconfigured function for finding iris, output is image map
    Mat Snake::iris_snake_function();

    // visual/output interface functions
    void showMatrix(Snake *snake);
    Mat showMatrix(Snake *snake_pupil, Snake *snake_corona, int type);

    bool saveSnakeToTextFile(Snake *snake);
    bool saveMatToTextFile(Mat output);


    Mat getImageOriginal(){return this->originalImage;}
    Mat* getImageOriginalPointer(){return &this->originalImage;}
    Mat getMatrixOfPoints(){return this->matrixOfPoints;}


    enum showType{
        Original_with_points,
        Original_with_lines,
        Original_with_polygons,
        Gradient_with_points,
        Gradient_with_lines,
        Gradient_with_polygons,
        Blank_with_points,
        Blank_with_lines,
        Blank_with_polygons,
        IrisMap
    };
};



#endif // SNAKE_H
