#ifndef ENERGYEXTERNALFIELD_H
#define ENERGYEXTERNALFIELD_H

#include <QMainWindow>
#include <QtGUI>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <qmath.h>
#define sqr(a) ((a)*(a))
using namespace cv;


class EnergyExternalField
{
protected:
    Mat vectorField;
    int typeOfVectorField;
    float gausianDeviation;

public:

    EnergyExternalField();
    EnergyExternalField(Mat inputImage, int energy_ext_type, float deviation);

    void countVectorField(int type);

    //copy image into new instance to 1 layered 2 dimensional field of float
    void setVectorField(Mat inputImage){
        this->vectorField = Mat(inputImage.rows,inputImage.cols, CV_32FC1, 0);
        cv::convertScaleAbs(inputImage, this->vectorField);
    }

    //vector field cloned from input field, this is risky,
    //the input has to be CV_32F1 or CV_32F3 to manipulate well with other functions
    void cloneVectorField(Mat inputField){this->vectorField=inputField.clone();}

    Mat getVectorField(){return this->vectorField;}

    int getTypeOfVectorField(){return this->typeOfVectorField;}

    float getGausianDeviation(){return this->gausianDeviation;}

    //use only if vectorField is CV_32F1
    float getValueFromVectorField(int x, int y){return this->vectorField.at<float>(y, x);}

    //use only if vectorField is CV_32F1
    float getValueFromVectorField(int x, int y, int layer){return this->vectorField.at<Vec3b>(y, x)[layer];}

    int getCV_typeOfVectorField(){return this->vectorField.type();}

    enum FieldType{
        GradientMagnitudes,
        GradientVectorFlow
    };
};

#endif // ENERGYEXTERNALFIELD_H
