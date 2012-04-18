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

void Snake::initSnakeContour(Snake* snake, int numberOfPoints,
                             int energy_int_type, int energy_ext_type,
                             float offsetX, float offsetY,
                             float baseAlpha, float baseBeta,/*energy internal parameters*/
                             float deviation,/*energy external parameters*/
                             int baseStep)
{

   // snake.contourTemplate.type=decision;
    snake->typeOfContour = energy_int_type;

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
    cv::Point *fastCenter = new cv::Point();
    float centerX ;
    float centerY ;
    float radius;
    switch(energy_int_type){
    case EnergyInternalTemplate::ClosedContour_Circle:
        //snake.contourTemplate.type = EnergyInternalTemplate::ClosedContour_Circle;
        snake->fastCenterLocalizationAlgorithm(snake->getImageOriginal(), fastCenter, radius);
        centerX = fastCenter->x + offsetX;
        centerY = fastCenter->y + offsetY;
        snake->setCirclePositions(snake->contour,  centerX, centerY, 100, snake->getImageOriginal().rows, snake->getImageOriginal().cols);
        EnergyInternalTemplate().countTotalEnergyInt(snake);
        break;

    default:;
    }
    //creation of external energy and control class
    initSnakeExtField(snake, energy_ext_type, deviation);
    snake->showMatrix(snake);
}

void Snake::setCirclePositions(QList <SnakePoint*> points, float centerX, float centerY, float radius, int maxX, int maxY){
    int count = points.size();
    float angle = (2*M_PI)/count;
    QFile outputFile("c:\\Temp\\testovacibodykruhu.xls");
    outputFile.open(QIODevice::WriteOnly | QFile::Text);
    QTextStream outText(&outputFile);
    for(int i=0; i<count ;i++){
        //count and set coordinate X
        points.at(i)->x = (centerX+(radius*cos(i*angle)));
        if(points.at(i)->x < 0){
            points.at(i)->x = 0;
        }else if(points.at(i)->x > maxX){
            points.at(i)->x = maxX;
        }
        //count and set coordinate Y
        points.at(i)->y = (centerY+(radius*sin(i*angle)));
        if(points.at(i)->y < 0){
            points.at(i)->y = 0;
        }else if(points.at(i)->y > maxY){
            points.at(i)->y = maxY;
        }
        outText << "x\t" << QString().number(points.at(i)->x) << "\ty\t" << QString().number(points.at(i)->y) << "\n";
    }

        outputFile.close();
}

void Snake::initSnakeExtField(Snake *snake, int energy_ext_type, float deviation){

    switch(energy_ext_type){
    case EnergyExternalField::GradientMagnitudes:
        snake->vectorField = new EnergyExternalField(snake->getImageOriginal(), energy_ext_type, deviation);
        snake->vectorField->countVectorField(EnergyExternalField::GradientMagnitudes);

        break;

    default:;
    }
    countTotalEnergyExt(snake);
}

void Snake::countTotalEnergyExt(Snake *snake){
    snake->total_E_ext = 0;
    for (int i=0; i<snake->contour.size(); i++){
        snake->contour.at(i)->E_ext=snake->vectorField->getValueFromVectorField(snake->contour.at(i)->x,snake->contour.at(i)->y, 2);
        snake->total_E_ext += snake->contour.at(i)->E_ext;
    }
}


void Snake::moveSnakeContour(Snake *snake)
{
    int best_x, best_y;
    float actual_local_ext_E, actual_local_int_E,
            best_local_ext_E, best_local_int_E;
    float lastRun_tot_ext_E = 0;
    float lastRun_tot_int_E = 0;
    int border_x = snake->originalImage.cols;
    int border_y = snake->originalImage.rows;
    bool equilibrium = false;
    int test_equilibrium = 10;
    bool foundBetter = false;
    int movedCount = 0;
    //int n = snake->contour.size();
    saveSnakeToTextFile(snake);
    switch(snake->vectorField->getTypeOfVectorField()){
    case EnergyExternalField::GradientMagnitudes:

        for (int cycles = 0; cycles < 200; cycles++){
        //while(!equilibrium){
            //for all points count locally external and internal energy and look if is in neighborhood point witl lower energy

            for (int i=0; i<snake->contour.size(); i++){
                best_x = snake->contour.at(i)->x;
                best_y = snake->contour.at(i)->y;
                actual_local_ext_E = snake->contour.at(i)->E_ext;
                actual_local_int_E = snake->contour.at(i)->E_int;
                best_local_ext_E = actual_local_ext_E;
                best_local_int_E = actual_local_int_E;
                //for steps*steps points around snake->contour.at(i)
                int hint_x1 = snake->contour.at(i)->x - snake->contour.at(i)->step;
                int hint_x2 = snake->contour.at(i)->x + snake->contour.at(i)->step;
                foundBetter = false;
                for (int actual_x = (snake->contour.at(i)->x -  snake->contour.at(i)->step); actual_x <= (snake->contour.at(i)->x + snake->contour.at(i)->step); actual_x++)
                {
                    for (int actual_y = (snake->contour.at(i)->y - snake->contour.at(i)->step); actual_y <= (snake->contour.at(i)->y + snake->contour.at(i)->step); actual_y++)
                    {

                        if( (0 <= actual_x) && (actual_x < border_x) && (0 <= actual_y) && (actual_y < border_y) )
                        {
                            if(actual_x != snake->contour.at(i)->x && actual_y != snake->contour.at(i)->y){
                                actual_local_ext_E = snake->vectorField->getValueFromVectorField(actual_x, actual_y, 2);
                                actual_local_int_E = EnergyInternalTemplate().countLocalEnergyInt(*snake, i, actual_x, actual_y);
                            }
                            //if not found better point before and can move to another point with same energy
                            if(!foundBetter){
                                if(abs(best_local_int_E - best_local_ext_E) >= abs(actual_local_int_E - actual_local_ext_E)){
                                    best_x = actual_x;
                                    best_y = actual_y;
                                    best_local_ext_E = actual_local_ext_E;
                                    best_local_int_E = actual_local_int_E;
                                    foundBetter = true;
                                }
                            //if found better point before then can move to another point only with lower energy
                            }else{
                                if(abs(best_local_int_E - best_local_ext_E) > abs(actual_local_int_E - actual_local_ext_E)){
                                    best_x = actual_x;
                                    best_y = actual_y;
                                    best_local_ext_E = actual_local_ext_E;
                                    best_local_int_E = actual_local_int_E;
                                    foundBetter = true;
                                }
                            }
                        }
                    }
                }
                if(foundBetter){
                    snake->total_E_int -= snake->contour.at(i)->E_ext;
                    snake->contour.at(i)->x = best_x;
                    snake->contour.at(i)->y = best_y;
                    snake->contour.at(i)->E_ext = best_local_ext_E;
                    snake->contour.at(i)->E_int = best_local_int_E;
                    snake->contour.at(i)->E_ext = snake->vectorField->getValueFromVectorField(best_x, best_y);
                    snake->total_E_int += best_local_ext_E;
                    movedCount++;
                }
            }
            EnergyInternalTemplate().countTotalEnergyInt(snake);

            if((lastRun_tot_ext_E+lastRun_tot_int_E)==(snake->total_E_ext+snake->total_E_int)){
                test_equilibrium--;
                if(test_equilibrium < 0)
                    equilibrium = true;
            }else{
                test_equilibrium = 10;
                lastRun_tot_ext_E=snake->total_E_ext;
                lastRun_tot_int_E=snake->total_E_int;
            }
        }

        break;

    default:;
    }
    saveSnakeToTextFile(snake);
}


void Snake::showMatrix(Snake *snake){
    saveSnakeToTextFile(this);
    saveSnakeToTextFile(snake);
    Mat zeromat = Mat().zeros(snake->originalImage.rows,snake->originalImage.cols, CV_8U);
    snake->matrixOfPoints.release();
    snake->matrixOfPoints = Mat(snake->originalImage.rows,snake->originalImage.cols, CV_32FC3, 0);
    cvtColor(zeromat, snake->matrixOfPoints, CV_GRAY2RGBA);
    snake->showImage.release();
    snake->showImage = Mat(snake->originalImage.rows,snake->originalImage.cols, CV_32FC3, 0);
    cvtColor(snake->originalImage, snake->showImage, CV_GRAY2RGBA);

    //    for(int i = 0; i < imageToShow.rows; i++)
    //        for(int j = 0; j < imageToShow.cols; j++)
    //            imageToShow.at<Vec3b>(i,j)[2]=155;

    for(int i=0; i<snake->contour.size(); i++){
        //        const float* ptr = (const float*)(image.data.ptr + snake->contour.at(i).y*image.step + snake->contour.at(i).y);
        //        ptr* =
        snake->matrixOfPoints.at<Vec3b>(snake->contour.at(i)->y, snake->contour.at(i)->x)[0] = 0;//255 - imageToShow.at<float>(snake->contour.at(i)->y, snake->contour.at(i)->x);
        snake->matrixOfPoints.at<Vec3b>(snake->contour.at(i)->y, snake->contour.at(i)->x)[1] = 255;
        snake->matrixOfPoints.at<Vec3b>(snake->contour.at(i)->y, snake->contour.at(i)->x)[2] = 255;
        snake->showImage.at<Vec3b>(snake->contour.at(i)->y, snake->contour.at(i)->x)[0] = 0;//255 - imageToShow.at<float>(snake->contour.at(i)->y, snake->contour.at(i)->x);
        snake->showImage.at<Vec3b>(snake->contour.at(i)->y, snake->contour.at(i)->x)[1] = 0;
        snake->showImage.at<Vec3b>(snake->contour.at(i)->y, snake->contour.at(i)->x)[2] = 255;

    }
}

bool Snake::saveSnakeToTextFile(Snake *snake)
{
    QString path;
    QWidget *dialog = new QWidget();
    //while(path.isEmpty()){
    path = QFileDialog::getSaveFileName(dialog,  "Save Snake header and SnakePoints to text file" , QDir::currentPath(), "Text files(*.txt)");
    //}
    QFile outputFile(path);
    if(outputFile.open(QIODevice::WriteOnly | QFile::Text)){
        QTextStream outText(&outputFile);
        outText << "Header of Snake - \t"<< path << endl
                << "Total Energy External =\t" << snake->total_E_ext << endl
                << "Total Energy Internal =\t" << snake->total_E_int << endl
                << "Type of Contour =\t" << snake->typeOfContour << endl
                << "Type of Vector Field =\t" << snake->vectorField->getTypeOfVectorField() << endl
                << "Type of Vector Field (OpenCV) =\t" << snake->vectorField->getCV_typeOfVectorField() << endl
                << "Type of Gausian Deviation =\t" << snake->vectorField->getGausianDeviation() << endl
                << "Type of Vector Field =\t" << snake->vectorField->getTypeOfVectorField() << endl
                << "Number of SnakePoints =\t" << snake->contour.size() << endl;
        for(int i=0; i<snake->contour.size(); i++){
            outText << "---" << endl
                    << "Point Number =\t" << i+1 << endl
                    << "\tX =\t" << snake->contour.at(i)->x << endl
                    << "\tY =\t" << snake->contour.at(i)->y << endl
                    << "\tEnergy External =\t" << snake->contour.at(i)->E_ext << endl
                    << "\tEnergy Internal =\t" << snake->contour.at(i)->E_int << endl
                    << "\tEnergy Snake =\t" << snake->contour.at(i)->E_snake << endl
                    << "\tAlpha =\t" << snake->contour.at(i)->alpha << endl
                    << "\tBeta =\t" << snake->contour.at(i)->beta << endl
                    << "\tStep =\t" << snake->contour.at(i)->step << endl
                    << "---"<< endl;
        }
        outText << "Show Matrix" <<  endl
                << "---"<< endl
                << snake->matrixOfPoints.data << snake->matrixOfPoints.datastart << snake->matrixOfPoints.dataend << endl
                << "---"<< endl ;
//            for(int i = 0; i < snake->matrixOfPoints.rows; i++)
//                for(int j = 0; j < snake->matrixOfPoints.cols; j++){
//                    outText << snake->matrixOfPoints.at<Vec3b>(i,j)[0] <<  endl
//                            << snake->matrixOfPoints.at<Vec3b>(i,j)[1] <<  endl
//                            << snake->matrixOfPoints.at<Vec3b>(i,j)[2] <<  endl;

//                }

        outputFile.close();
        return true;
    }else{
        QMessageBox mess;
        mess.setText("Could not save file");
        mess.exec();
    }
    return false;
}


void Snake::fastCenterLocalizationAlgorithm(Mat image, cv::Point *fastCenter, float radius){
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
        int randomN = qrand()+time.msec();
        //intensity of random pixel in 30% * 30% rectangle in the middle of image
        //(image.data.ptr + ((yOffset + randomN)%yMax)*image.step + ((xOffset+randomN) % xMax))
        //(image.data.ptr + actY*image.step + actX)

        actX = (yOffset + (randomN % yMax));
        actY = (xOffset + (randomN % xMax));

        if( image.at<float>(actY, actX) == 0 )
        {
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

    fastCenter->x = borders[3]-borders[2]+xOffset;
    fastCenter->y = borders[1]-borders[0]+yOffset;
    radius = 0;
    radius = min((borders[3]-borders[2])/2,(borders[1]-borders[0])/2);

}


