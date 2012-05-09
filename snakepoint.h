#ifndef SNAKEPOINT_H
#define SNAKEPOINT_H

class SnakePoint
{//one control point of entire contour

public:
    SnakePoint();
    float getAlpha(){return this->alpha;}
    float getBeta(){return this->beta;}
    float getStep(){return this->step;}
    float getX(){return this->x;}
    float getY(){return this->y;}
    float getE_int(){return this->E_int;}
    float getE_ext(){return this->E_ext;}
    float getE_snake(){return this->E_snake;}


    void setAlpha(float value){this->alpha = value;}
    void setBeta(float value){this->beta = value;}
    void setStep(float value){this->step = value;}
    void setX(float value){this->x = value;}
    void setY(float value){this->y = value;}
    void setE_int(float value){this->E_int = value;}
    void setE_ext(float value){this->E_ext = value;}
    void setE_snake(float value){this->E_snake = value;}


    float x, y; // coordinates
    float alpha, beta, step;
    float E_int, E_ext, E_snake, C_int;
};

#endif // SNAKEPOINT_H
