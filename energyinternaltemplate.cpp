#define sqr(a) ((a)*(a))

#include "energyinternaltemplate.h"

EnergyInternalTemplate::EnergyInternalTemplate()
{

}

float EnergyInternalTemplate::countLocalEnergyInt(Snake snake, int pointOfSnake, int newX, int newY){
    //this function counts local energy of snake point in future location [newX, newY]


    float newLocalEnergy = 0;
    int n = snake.contour.size();

    //indexes of deformable contour, has to be moduled to n
    //s_i_p1 = s with index (i + 1) // s_i = s with index i at new possiblepoint// s_i_m1 = s with index (i - 1)
    int s_i_p1_x, s_i_x, s_i_m1_x, s_i_p1_y, s_i_y, s_i_m1_y;

    s_i_p1_x = snake.contour.at((pointOfSnake+1)%n)->x;
    s_i_x = newX;
    s_i_m1_x = snake.contour.at((pointOfSnake==0) ? (n-1) : (pointOfSnake-1))->x;
    s_i_p1_y = snake.contour.at((pointOfSnake+1)%n)->y;
    s_i_y = newY;
    s_i_m1_y = snake.contour.at((pointOfSnake==0) ? (n-1) : (pointOfSnake-1))->y;

    newLocalEnergy = (snake.contour.at(pointOfSnake)->alpha*(sqr( qAbs((int)( s_i_x - s_i_m1_x )))
                                                            +sqr( qAbs((int)( s_i_y - s_i_m1_y )))
                   +
                   snake.contour.at(pointOfSnake)->beta*(sqr( qAbs((int)( s_i_m1_x - ( 2*s_i_x ) + s_i_p1_x )))
                                                        +sqr( qAbs((int)( s_i_m1_y - ( 2*s_i_y ) + s_i_p1_y ))))));

    return newLocalEnergy;
}


float EnergyInternalTemplate::countLocalEnergyInt1stage(Snake snake, int pointOfSnake, int newX, int newY){
    //this function counts 1 part of local energy of snake point in future location [newX, newY]

    float newLocalEnergy = 0;
    int n = snake.contour.size();

    //indexes of deformable contour, has to be moduled to n
    //s_i_p1 = s with index (i + 1) // s_i = s with index i at new possiblepoint// s_i_m1 = s with index (i - 1)
    int s_i_x, s_i_m1_x, s_i_y, s_i_m1_y;

    s_i_x = newX;
    s_i_m1_x = snake.contour.at((pointOfSnake==0) ? (n-1) : (pointOfSnake-1))->x;
    s_i_y = newY;
    s_i_m1_y = snake.contour.at((pointOfSnake==0) ? (n-1) : (pointOfSnake-1))->y;

    newLocalEnergy = abs(
                      snake.avgDist
                        -
                          (sqr( qAbs((int)( s_i_x - s_i_m1_x )))
                           +
                           sqr( qAbs((int)( s_i_y - s_i_m1_y )))
                           )
                     );

    return newLocalEnergy;
}


float EnergyInternalTemplate::countLocalEnergyInt2stage(Snake snake, int pointOfSnake, int newX, int newY){
    //this function counts local energy of snake point in future location [newX, newY]

    float newLocalEnergy = 0;
    int n = snake.contour.size();

    //indexes of deformable contour, has to be moduled to n
    //s_i_p1 = s with index (i + 1) // s_i = s with index i at new possiblepoint// s_i_m1 = s with index (i - 1)
    int s_i_p1_x, s_i_x, s_i_m1_x, s_i_p1_y, s_i_y, s_i_m1_y;

    s_i_p1_x = snake.contour.at((pointOfSnake+1)%n)->x;
    s_i_x = newX;
    s_i_m1_x = snake.contour.at((pointOfSnake==0) ? (n-1) : (pointOfSnake-1))->x;
    s_i_p1_y = snake.contour.at((pointOfSnake+1)%n)->y;
    s_i_y = newY;
    s_i_m1_y = snake.contour.at((pointOfSnake==0) ? (n-1) : (pointOfSnake-1))->y;

    newLocalEnergy = ( sqr( qAbs((int)( s_i_m1_x - ( 2*s_i_x ) + s_i_p1_x )))
                       +
                       sqr( qAbs((int)( s_i_m1_y - ( 2*s_i_y ) + s_i_p1_y )))  );

    return newLocalEnergy;
}

float EnergyInternalTemplate::getAverageDistance(Snake snake){
    //this function counts average distance for further counting of 1st stage equation of internal energy
    float summary = 0;
    int s_i_x, s_i_m1_x, s_i_y, s_i_m1_y;
    int n = snake.contour.size();

    for(int i = 0; i < n; i++){
        s_i_x = snake.contour.at(i)->x;
        s_i_m1_x = snake.contour.at((i==0) ? (n-1) : (i-1))->x;
        s_i_y = snake.contour.at(i)->y;
        s_i_m1_y = snake.contour.at((i==0) ? (n-1) : (i-1))->y;

        summary = ( sqr( qAbs((int)( s_i_x - s_i_m1_x )))
                     +
                    sqr( qAbs((int)( s_i_y - s_i_m1_y ))) );
    }
    float average = summary / snake.contour.size();

    return average;
}

void EnergyInternalTemplate::countContourEstimation(Snake *snake){
    //this function counts total energy for snake contour at actual coordinates of points
    int n = snake->contour.size();
    int s_i_p1_x, s_i_x, s_i_m1_x, s_i_p1_y, s_i_y, s_i_m1_y;

    for(int i = 0; i < n; i++){
        s_i_p1_x = snake->contour.at((i+1)%n)->x;
        s_i_x = snake->contour.at(i)->x;
        s_i_m1_x = snake->contour.at((i==0) ? (n-1) : (i-1))->x;
        s_i_p1_y = snake->contour.at((i+1)%n)->y;
        s_i_y = snake->contour.at(i)->y;
        s_i_m1_y = snake->contour.at((i==0) ? (n-1) : (i-1))->y;

        snake->contour.at(i)->E_int = sqr( abs((s_i_x-s_i_m1_x)/abs(s_i_x-s_i_m1_x)
                                               -
                                               (s_i_p1_x-s_i_x)/abs(s_i_p1_x-s_i_x)
                                              ))
                                        +
                                      sqr( abs((s_i_y-s_i_m1_y)/abs(s_i_y-s_i_m1_y)
                                               -
                                              (s_i_p1_y-s_i_y)/abs(s_i_p1_y-s_i_y)
                                              )
                                      );
    }

}



void EnergyInternalTemplate::countTotalEnergyInt(Snake *snake){
    //this function counts total energy for snake contour at actual coordinates of points
    float totalEnergy = 0;

    switch (snake->typeOfContour){
    case EnergyInternalTemplate::ClosedContour_Circle:
        for(int i=0; i<snake->contour.size(); i++){
            snake->contour.at(i)->E_int = snake->weight_E_int * countLocalEnergyInt(*snake, i, snake->contour.at(i)->x, snake->contour.at(i)->y);
            totalEnergy += snake->contour.at(i)->E_int;
        }
        snake->total_E_int = totalEnergy;
        break;
    default:;
    }
    snake->total_E_int = totalEnergy;
}
