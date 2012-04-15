#include "snake.h"

Snake::Snake()
{
}

Snake::Snake(Mat image)
{
    this->originalImage = image.clone();
}

Snake::Snake(Mat image, QList<SnakePoint*> points)
{
    this->originalImage = image.clone();
    this->contour = points;
}


//void Snake::initSnakeContour(Snake snake, int numberOfPoints, EnergyInternalTemplate::ContourType decision){
//    snake.contour.clear();

//    snake.typeOfContour = decision;
//    for(int i=0; i<numberOfPoints; i++){
//        SnakePoint temp;
//        temp.setAlpha(0.2);
//        temp.setBeta(0);
//        temp.setStep(1);
//        snake.contour.append(&temp);
//    }

//    switch(decision){
//        case EnergyInternalTemplate::ClosedContour_Circle:

//        break;
//        default:;
//    }
//}

void Snake::initSnakeContour(Snake* snake, int numberOfPoints, int decision,
                             float offsetX, float offsetY,
                             float baseAlpha, float baseBeta,
                             int baseStep)
{

   // snake.contourTemplate.type=decision;
    snake->typeOfContour = decision;

    snake->contour.clear();
    if(snake->contour.isEmpty() || numberOfPoints!=snake->contour.size()){
        for(int i=0; i<numberOfPoints; i++){
            SnakePoint* temp = new SnakePoint();
            temp->setAlpha(baseAlpha);
            temp->setBeta(baseBeta);
            temp->setStep(baseStep);
            snake->contour.append(temp);
        }
    }
    cv::Point fastCenter;
    float centerX ;
    float centerY ;
    float radius;
    switch(decision){
    case EnergyInternalTemplate::ClosedContour_Circle:


        //snake.contourTemplate.type = EnergyInternalTemplate::ClosedContour_Circle;
        snake->fastCenterLocalizationAlgorithm(snake->getImageOriginal(), fastCenter, radius);
        centerX = fastCenter.x + offsetX;
        centerY = fastCenter.y + offsetY;
        snake->setCirclePositions(snake->contour,  centerX, centerY, 100);
        snake->showMatrix(snake->getImageOriginal(), *snake, "Snake After Init");
        break;
    default:;
    }
}

void Snake::showMatrix(Mat image, Snake snake, QString windowName){

    Mat imageToShow = Mat(image.rows,image.cols, CV_32FC3, 0);
    cvtColor(image,imageToShow,CV_GRAY2RGBA);
//    for(int i = 0; i < imageToShow.rows; i++)
//        for(int j = 0; j < imageToShow.cols; j++)
//            imageToShow.at<Vec3b>(i,j)[2]=155;

    for(int i=0; i<snake.contour.size(); i++){
//        const float* ptr = (const float*)(image.data.ptr + snake.contour.at(i).y*image.step + snake.contour.at(i).y);
//        ptr* =
        imageToShow.at<Vec3b>(snake.contour.at(i)->y, snake.contour.at(i)->x)[0] = 155;//255 - imageToShow.at<float>(snake.contour.at(i)->y, snake.contour.at(i)->x);
        imageToShow.at<Vec3b>(snake.contour.at(i)->y, snake.contour.at(i)->x)[1] = 200;
        imageToShow.at<Vec3b>(snake.contour.at(i)->y, snake.contour.at(i)->x)[2] = 155;

    }
    namedWindow( windowName.toStdString().c_str(), CV_WINDOW_AUTOSIZE );
    imshow( windowName.toStdString().c_str(), imageToShow );
    waitKey(0);
    destroyWindow( windowName.toStdString().c_str() );
}



void Snake::fastCenterLocalizationAlgorithm(Mat image, cv::Point fastCenter, float radius){
    //pre
    threshold(image, image, 60, 0, THRESH_TOZERO);
    threshold(image, image, 190, 0, THRESH_TOZERO_INV);
    medianBlur(image, image, 3);
    image+=150;
    threshold(image, image, 160, 0, THRESH_TOZERO);

//    namedWindow( "test", CV_WINDOW_AUTOSIZE );
//    imshow( "test", image );

//    destroyWindow( "test" );

    //rectangle of boundaries for circle found in the middle of image
    float borders[4] = {0,0,0,0}; //top,bottom,left,right

    //seed of random number depends on system time
    QTime time = QTime().currentTime();
    qsrand(131*time.msec());

    //30% * 30% rectangle in the middle of image
    int xMax = floor(image.cols*0.3);
    int yMax = floor(image.rows*0.3);
    int maxSize = floor(xMax*(yMax*0.1));
    int xOffset = floor(image.cols*0.35);
    int yOffset = floor(image.rows*0.35);
    borders[3] = floor(xOffset+(xMax/2));
    borders[2] = floor(xOffset+(xMax/2));
    borders[1] = floor(yOffset+(yMax/2));
    borders[0] = floor(yOffset+(yMax/2));
    int actX = 0;
    int actY = 0;
    QStringList test;
    test.append("maxsize:\t "+QString().number(maxSize));
    for(int i=0; i<maxSize; i++){
        int randomN = qrand();
        //intensity of random pixel in 30% * 30% rectangle in the middle of image
        //(image.data.ptr + ((yOffset + randomN)%yMax)*image.step + ((xOffset+randomN) % xMax))
        //(image.data.ptr + actY*image.step + actX)

        actX = (yOffset + (randomN % yMax));
        actY = (xOffset + (randomN % xMax));

        if( image.at<float>(actY, actX) == 0 )
        {

            image.at<float>(actY, actX) = 150;
            if(borders[0] > actY)
                borders[0] = actY;
            if(borders[1] < actY)
                borders[1] = actY;
            if(borders[2] > actX)
                borders[2] = actX;
            if(borders[3] < actX)
                borders[3] = actX;


        test.append("i:\t"+QString().number(i)+
                    "\timage value:\t"+QString().number(image.at<float>(actX, actY))+
                    "\tactx:\t"+QString().number(actX)+
                    "acty:\t"+QString().number(actY)+
                    "\ttop:\t"+QString().number(borders[0])+
                    "\tbottom:\t"+QString().number(borders[1])+
                    "\tleft:\t"+QString().number(borders[2])+
                    "\tright:\t"+QString().number(borders[3])+
                    "\n"
                    );
        }
    }

    fastCenter.x = borders[3]-borders[2];
    fastCenter.y = borders[1]-borders[0];
    radius = 0;
    radius = min((borders[3]-borders[2])/2,(borders[1]-borders[0])/2);
    test.append("fastCenterY:\t"+QString().number(fastCenter.x)+
                "\tfastCenterY:\t"+QString().number(fastCenter.y)+
                "\tradius:\t"+QString().number(radius)
                );
    QFile outputFile("c:\\Temp\\testovacisubor.xls");
    if(outputFile.open(QIODevice::WriteOnly | QFile::Text)){
        QTextStream outText(&outputFile);
        for(int i=0; i<test.size(); i++){
            outText << test.at(i);
        }
        outputFile.close();

    }
}

void Snake::setCirclePositions(QList <SnakePoint*> points, float centerX, float centerY, float radius){
    int count = points.size();
    float angle = (2*M_PI)/count;
    QFile outputFile("c:\\Temp\\testovacibodykruhu.xls");
    outputFile.open(QIODevice::WriteOnly | QFile::Text);
    QTextStream outText(&outputFile);
    for(int i=0; i<count ;i++){
        points.at(i)->x = (centerX+(radius*cos(i*angle)));
        points.at(i)->y = (centerY+(radius*sin(i*angle)));
        outText << "x\t" << QString().number(points.at(i)->x) << "\ty\t" << QString().number(points.at(i)->y) << "\n";
    }

        outputFile.close();


}

