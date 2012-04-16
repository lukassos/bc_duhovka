#include "energyexternalfield.h"

EnergyExternalField::EnergyExternalField()
{
}


EnergyExternalField::EnergyExternalField(Mat inputImage, int energy_ext_type, float deviation)
{
    this->setVectorField(inputImage);

    this->gausianDeviation = deviation;
    this->typeOfVectorField = energy_ext_type;
}


void EnergyExternalField::countVectorField(int type){

    switch(type){
        case EnergyExternalField::GradientMagnitudes:

            GaussianBlur(this->vectorField, this->vectorField, Size(3,3), this->gausianDeviation, this->gausianDeviation);

// here count gradient for x,y and together magnitude as cross product
//            Mat vectorField_3layered =  Mat(this->vectorField.rows,this->vectorField.cols, CV_32FC3, 0);
//            for(int i = 0; i < this->vectorField.rows; i++){
//                    for(int j = 0; j < this->vectorField.cols; j++){
//                        vectorField_3layered.at<Vec3b>(i,j)[2]=this->vectorField.at<float>(i,j);
//                    }
//            }

            break;
    default:;
    }
}
