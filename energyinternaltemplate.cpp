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

void EnergyInternalTemplate::fastCenterLocalizationAlgorithm(Mat image, cv::Point fastCenter, float radius){
    //pre
    threshold(image, image, 60, 0, THRESH_TOZERO);
    threshold(image, image, 190, 0, THRESH_TOZERO_INV);
    medianBlur(image, image, 3);
    image+=150;
    threshold(image, image, 160, 0, THRESH_TOZERO);



    //rectangle of boundaries for circle found in the middle of image
    float borders[4] = {0,0,0,0}; //top,bottom,left,right

    //seed of random number depends on system time
    QTime time;
    qsrand(131*time.currentTime().msec);

    //30% * 30% rectangle in the middle of image
    float xMax = image.cols*0.3;
    float yMax = image.rows*0.3;
    float maxSize = (xMax*(yMax*0,1));
    float xOffset = image.cols*0.35;
    float yOffset = image.rows*0.35;
    borders[3] = xOffset+(xMax/2);
    borders[2] = xOffset+(xMax/2);
    borders[1] = yOffset+(yMax/2);
    borders[0] = yOffset+(yMax/2);
    for(int i=0; i<maxSize; i++){
        int randomN = qrand();
        //intensity of random pixel in 30% * 30% rectangle in the middle of image
        //(image.data.ptr + ((yOffset + randomN)%yMax)*image.step + ((xOffset+randomN) % xMax))
        float actX = (yOffset + randomN) % yMax;
        float actY = (xOffset + randomN) % xMax;
        if((image.data.ptr + actY*image.step + actX) == 0){
            if(borders[0] > actY)
                borders[0] = actY;
            if(borders[1] < actY)
                borders[1] = actY;
            if(borders[2] > actX)
                borders[2] = actX;
            if(borders[3] < actX)
                borders[3] = actX;
        }

    }
    fastCenter.x = borders[3]-borders[2];
    fastCenter.y = borders[1]-borders[0];
    radius = qMin((borders[3]-borders[2])/2,(borders[1]-borders[0])/2);
}
