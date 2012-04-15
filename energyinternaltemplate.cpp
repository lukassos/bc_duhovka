#define sqr(a) ((a)*(a))

#include "energyinternaltemplate.h"

EnergyInternalTemplate::EnergyInternalTemplate()
{

}

void EnergyInternalTemplate::count
void EnergyInternalTemplate::countTotalEnergyInt(Snake snake){
    float totalEnergy = 0;
    float localEnergy = 0;
    //indexes of deformable contour, has to be moduled to n
    //s_i_p1 = s with index (i + 1) // s_i = s with index i // s_i_m1 = s with index (i - 1)
    int s_i_p1_x, s_i_x, s_i_m1_x, s_i_p1_y, s_i_y, s_i_m1_y;
    int n = snake.contour.size();
    switch (snake.typeOfContour){
    case EnergyInternalTemplate::ClosedContour_Circle:
        for(int i=0; i<n; i++){
            //alpha*
            s_i_p1_x = snake.contour.at((i+1)%n)->x;
            s_i_x = snake.contour.at(i)->x;
            s_i_m1_x = snake.contour.at((i-1)%n)->x;
            s_i_p1_y = snake.contour.at((i+1)%n)->y;
            s_i_y = snake.contour.at(i)->y;
            s_i_m1_y = snake.contour.at((i-1)%n)->y;
            localEnergy = (snake.contour.at(i)->alpha*(sqr( abs((int)( s_i_x - s_i_m1_x )))
                                                      +sqr( abs((int)( s_i_y - s_i_m1_y )))
                           +
                           snake.contour.at(i)->beta*(sqr( abs((int)( s_i_m1_x - ( 2*s_i_x ) + s_i_p1_x )))
                                                     + sqr( abs((int)( s_i_m1_y - ( 2*s_i_y ) + s_i_p1_y ))))));
            snake.contour.at(i)->setE_int(localEnergy);
            totalEnergy += localEnergy;
        }
        break;
    default:;
    }
    snake.total_E_int = totalEnergy;
}
