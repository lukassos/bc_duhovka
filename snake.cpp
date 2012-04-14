#include "snake.h"

snake::snake()
{

}


void snake::initSnakeContour(snake snakeToInit, int numberOfPoints, energyInternalTemplate::ContourType decision){
    snakeToInit.contour.clear();
    for(int i=0; i<numberOfPoints; i++){
        snakePoint temp = new snakePoint();
        snakeToInit.contour.append(temp);
    }

    switch(){
        case energyInternalTemplate::ClosedContour_Circle:
            snakeToInit.contourTemplate->
        break;
        default
    }



}

void snake::initSnakeContour(snake snakeToInit, int numberOfPoints, energyInternalTemplate::ContourType decision,
                             float offsetX, float offsety, float radius){



    switch(){
        case energyInternalTemplate::ClosedContour_Circle:

        break;
    }



}
