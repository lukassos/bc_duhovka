#include "snake.h"

Snake::snake()
{

}


void Snake::initSnakeContour(Snake snake, int numberOfPoints, EnergyInternalTemplate::ContourType decision){
    snake.contour.clear();
    for(int i=0; i<numberOfPoints; i++){
        SnakePoint temp = new SnakePoint();
        snake.contour.append(temp);
    }

    switch(decision){
        case energyInternalTemplate::ClosedContour_Circle:
            snake.contourTemplate.;
        break;
        default:
    }



}

void Snake::initSnakeContour(Snake snake, int numberOfPoints, EnergyInternalTemplate::ContourType decision,
                             float offsetX, float offsetY){
    snake.contourTemplate.type=decision;
    snake.typeOfContour = decision;

    if(snake.contour.isEmpty() || numberOfPoints!=snake.contour.size())
        for(int i=0; i<numberOfPoints; i++){
            SnakePoint temp = new SnakePoint();
            snake.contour.append(temp);
        }

    switch(decision){
    case EnergyInternalTemplate::ClosedContour_Circle:
        cv::Point fastCenter;
        EnergyInternalTemplate::fastCenterLocalizationAlgorithm(snake.fastCenter, radius);
        float centerX = fastCenter.x + offsetX;
        float centerY = fastCenter.y + offsetY;
        EnergyInternalTemplate::setCirclePositions(snake.contour, centerX, centerY, radius);
        break;
    default:
    }


}
