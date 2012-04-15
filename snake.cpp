#include "snake.h"

Snake::Snake()
{

}


void Snake::initSnakeContour(Snake snake, int numberOfPoints, EnergyInternalTemplate::ContourType decision){
    snake.contour.clear();

    snake.typeOfContour = decision;
    for(int i=0; i<numberOfPoints; i++){
        SnakePoint temp;
        temp.alpha=0,2;
        temp.beta=0;
        temp.step=1;
        snake.contour.append(temp);
    }

    switch(decision){
        case EnergyInternalTemplate::ClosedContour_Circle:

        break;
        default:;
    }
}

void Snake::initSnakeContour(Snake snake, int numberOfPoints, EnergyInternalTemplate::ContourType decision,
                             float offsetX, float offsetY){
   // snake.contourTemplate.type=decision;
    snake.typeOfContour = decision;

    snake.contour.clear();
    if(snake.contour.isEmpty() || numberOfPoints!=snake.contour.size())
        for(int i=0; i<numberOfPoints; i++){
            SnakePoint temp;
            temp.alpha=0,2;
            temp.beta=0;
            temp.step=1;
            snake.contour.append(temp);
        }
    cv::Point fastCenter;
    float centerX ;
    float centerY ;

    switch(decision){
    case EnergyInternalTemplate::ClosedContour_Circle:

        float radius;
        //snake.contourTemplate.type = EnergyInternalTemplate::ClosedContour_Circle;
        snake.contourTemplate.fastCenterLocalizationAlgorithm(snake.getImageOriginal(), fastCenter, radius);
        centerX = fastCenter.x + offsetX;
        centerY = fastCenter.y + offsetY;
        snake.contourTemplate.setCirclePositions(snake.contour, centerX, centerY, radius);
        snake.showMatrix(snake.getImageOriginal(), snake, "Snake After Init");
        break;
    default:;
    }
}

void Snake::showMatrix(Mat image, Snake snake, QString windowName){

    Mat imageToShow = image.clone();
    for(int i=0; i<snake.contour.size(); i++){
//        const float* ptr = (const float*)(image.data.ptr + snake.contour.at(i).y*image.step + snake.contour.at(i).y);
//        ptr* =
        imageToShow.at<float>(snake.contour.at(i).x, snake.contour.at(i).y) = 255 - imageToShow.at<float>(snake.contour.at(i).x, snake.contour.at(i).y);
    }
    namedWindow( windowName.toStdString().c_str(), CV_WINDOW_AUTOSIZE );
    imshow( windowName.toStdString().c_str(), imageToShow );
    waitKey(0);
    destroyWindow( windowName.toStdString().c_str() );
}
