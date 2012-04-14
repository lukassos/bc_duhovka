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
    CvMat
}
