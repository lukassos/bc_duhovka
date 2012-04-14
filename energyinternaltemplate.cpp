#include "energyinternaltemplate.h"

EnergyInternalTemplate::EnergyInternalTemplate()
{

}

void EnergyInternalTemplate::setCirclePositions(QList <SnakePoint> points, float centerX, float centerY, float radius){
    int count = points.size();
    float angle = (2*M_PI)/count;
    for(int i=0; i<count ;i++){
        points.at(i).x = centerX+(radius*cos(angle));
        points.at(i).y = centerY+(radius*sin(angle));
    }
}

void EnergyInternalTemplate::fastCenterLocalizationAlgorithm(CvMat image, cv::Point fastCenter, float radius){

    float borders[4] = {0,0,0,0}; //top,bottom,left,right
    QSystemLocale time;

    qsrand(131*QSystemLocale.TimeFormatShort);
    float xMax = image.cols*0.3;
    float yMax = image.rows*0.3;
    float xOffset = image.cols*0.35;
    float yOffset = image.rows*0.35;

    for(int i=0; i<maxSize; i++){
        int randomN = qrand();
        //intensity of random pixel in 30% * 30% rectangle in the middle of image
        //(image.data.ptr + ((yOffset + randomN)%yMax)*image.step + ((xOffset+randomN) % xMax))
        if((image.data.ptr + ((yOffset + randomN)%yMax)*image.step + ((xOffset+randomN) % xMax)) == 0){
            if(borders[0]>


        }
    }


}
