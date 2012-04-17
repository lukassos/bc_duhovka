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
    Mat gaus = Mat(this->vectorField.rows, this->vectorField.cols, CV_32FC1);
    Mat dx = Mat(this->vectorField.rows, this->vectorField.cols, CV_32FC1);
    Mat dy = Mat(this->vectorField.rows, this->vectorField.cols, CV_32FC1);
    switch(type){
        case EnergyExternalField::GradientMagnitudes:

        //remove noise and scale image for furher range of gradient
        GaussianBlur(this->vectorField, gaus, Size(3,3), this->gausianDeviation, this->gausianDeviation);
        this->vectorField = Mat(this->vectorField.rows, this->vectorField.cols, CV_32FC3);

        //derivates of matrices by sobel operator

        Sobel(gaus, dx, CV_32F, 1, 0, 3);
        Sobel(gaus, dy, CV_32F, 0, 1, 3);

        //gradient magnitude = sqrt(power_2(derivation by x) + power_2(derivation by y))
        for(int i = 0; i < this->vectorField.rows; i++)
            for(int j = 0; j < this->vectorField.cols; j++){
                this->vectorField.at<Vec3b>(i,j)[0] = dx.at<float>(i,j);
                this->vectorField.at<Vec3b>(i,j)[1] = dy.at<float>(i,j);
                this->vectorField.at<Vec3b>(i,j)[2] = sqrt(sqr(dx.at<float>(i,j)) + sqr(dy.at<float>(i,j))) ;
            }

        //        cv::pow(this->vectorField.at[0], 2, this->vectorField.at[0]);
        //        cv::pow(this->vectorField.at[1], 2, this->vectorField.at[1]);
        //        cv::sqrt(this->vectorField.at[2], this->vectorField.at[2])
        //        || equals ||
        //        cv::magnitude(this->vectorField[0],this->vectorField[1],this->vectorField.channels()[3])

            break;
    default:;
    }
}

Mat EnergyExternalField::getOneLayerFromVectorField(int layer){
    Mat ret = Mat(this->vectorField.rows, this->vectorField.cols, CV_32FC1);
    for(int i = 0; i < this->vectorField.rows; i++)
        for(int j = 0; j < this->vectorField.cols; j++)
            ret.at<float>(i,j) = this->vectorField.at<Vec3b>(i,j)[layer];
    return ret;
}
