#ifndef ENERGYEXTERNALFIELD_H
#define ENERGYEXTERNALFIELD_H

#include <QMainWindow>
#include <QtGUI>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <qmath.h>
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

    //copy image into new instance
    void setVectorField(Mat inputImage){
        //inputImage->copyTo(this->vectorField)
        //inputImage = new Mat(inputImage->rows, inputImage->cols, inputImage->type(), inputImage->data);
        //this->vectorField =  Mat(*inputImage);
        this->vectorField = inputImage.clone();
        }

    //vector field can manipulate the input field
    void cloneVectorField(Mat inputField){this->vectorField=inputField.clone();}

    Mat getVectorField(){return this->vectorField;}

    int getTypeOfVectorField(){return this->typeOfVectorField;}

    float getGausianDeviation(){return this->gausianDeviation;}

    float getValueFromVectorField(int x, int y){return this->vectorField.at<float>(y, x);}

    //float getValueFromVectorField(int x, int y, int layer){return this->vectorField.at<Vec3b>[layer](y, x);}



    enum FieldType{
        GradientMagnitudes,
        GradientVectorFlow
    };
};

#endif // ENERGYEXTERNALFIELD_H
