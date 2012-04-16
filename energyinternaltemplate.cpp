#define sqr(a) ((a)*(a))

#include "energyinternaltemplate.h"

EnergyInternalTemplate::EnergyInternalTemplate()
{

}

float EnergyInternalTemplate::countLocalEnergyInt(Snake snake, int pointOfSnake, int newX, int newY){
    //this function counts local energy of snake point in future location [newX, newY]
    //if snake.contour.at(pointOfSnake) is future location, counts actual local minima

    float newLocalEnergy = 0;
    int n = snake.contour.size();

    //indexes of deformable contour, has to be moduled to n
    //s_i_p1 = s with index (i + 1) // s_i = s with index i at new possiblepoint// s_i_m1 = s with index (i - 1)
    int s_i_p1_x, s_i_x, s_i_m1_x, s_i_p1_y, s_i_y, s_i_m1_y;

    s_i_p1_x = snake.contour.at((pointOfSnake+1)%n)->x;
    s_i_x = newX;
    s_i_m1_x = snake.contour.at((pointOfSnake==0) ? 299 : (pointOfSnake-1))->x;
    s_i_p1_y = snake.contour.at((pointOfSnake+1)%n)->y;
    s_i_y = newY;
    s_i_m1_y = snake.contour.at((pointOfSnake==0) ? 299 : (pointOfSnake-1))->y;

    newLocalEnergy = (snake.contour.at(pointOfSnake)->alpha*(sqr( qAbs((int)( s_i_x - s_i_m1_x )))
                                                            +sqr( qAbs((int)( s_i_y - s_i_m1_y )))
                   +
                   snake.contour.at(pointOfSnake)->beta*(sqr( qAbs((int)( s_i_m1_x - ( 2*s_i_x ) + s_i_p1_x )))
                                                        +sqr( qAbs((int)( s_i_m1_y - ( 2*s_i_y ) + s_i_p1_y ))))));

    return newLocalEnergy;
}

void EnergyInternalTemplate::countTotalEnergyInt(Snake snake){
    //this function counts total energy for snake contour at actual coordinates of points
    float totalEnergy = 0;

    switch (snake.typeOfContour){
    case EnergyInternalTemplate::ClosedContour_Circle:
        for(int i=0; i<snake.contour.size(); i++){
            snake.contour.at(i)->E_int = countLocalEnergyInt(snake, i, snake.contour.at(i)->x, snake.contour.at(i)->y);
            totalEnergy += snake.contour.at(i)->E_int;
        }
        snake.total_E_int = totalEnergy;
        break;
    default:;
    }
    snake.total_E_int = totalEnergy;
}
