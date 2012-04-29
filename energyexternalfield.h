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
    QList <Mat> vectorField;
    int typeOfVectorField;
    float gausianDeviation;
    float sobelScale;

public:

    EnergyExternalField();
    EnergyExternalField(Mat inputImage, int energy_ext_type, float deviation, float scale);

    void countVectorField(int type, int centerX, int centerY);

    //copy image into new instance to 1 layered 2 dimensional field of float
    void setVectorField(Mat inputImage){
        this->vectorField.insert(0, inputImage.clone());
        //conversion of vector field is done inn next fuctions this just clone first original matrix
        //Mat(inputImage.rows, inputImage.cols, CV_32FC1, 0);
        //cv::conver(inputImage, this->vectorField);
    }

    //vector field cloned from input field, this is risky,
    //the input has to be CV_32F1 or CV_32F3 to manipulate well with other functions
    void cloneVectorField(QList <Mat> inputField){this->vectorField = inputField;}

    Mat getVectorField(int at){return this->vectorField.at(at);}

    int getTypeOfVectorField(){return this->typeOfVectorField;}

    float getGausianDeviation(){return this->gausianDeviation;}

    float getSobelScale(){return this->sobelScale;}

    //use only if vectorField is CV_32F1
    float getValueFromVectorField(int at, int x, int y){return this->vectorField.at(at).at<float>(y, x);}

    Mat getNeighborhoodExtE(int x, int y, int step, int at);

    int getCV_typeOfVectorField(int at){return this->vectorField.at(at).type();}

    Mat getScaledVectorField(int at);

    Mat getConvertedVectorField(int at);

    void scaleVectorField(Mat in, Mat out);

    void scaleConvertMat(Mat in, Mat out);

    bool saveMatToTextFile(Mat output, QString path = "");

    enum FieldType{
        GradientMagnitudes,
        GradientMagnitudes_pupil,
        GradientMagnitudes_corona
    };
};

#endif // ENERGYEXTERNALFIELD_H
