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
                             float weight_Eext, float weight_Eint,
                             float baseAlpha, float baseBeta,/*energy internal parameters*/
                             float contourThresh, float edgeStrenghtThresh,
                             float deviation, float scale,/*energy external parameters*/
                             int baseStep,
                             cv::Point *fastCenter, float radius,
                             int fastInitKernelSize,
                             int offsetX, int offsetY
                             )
{

   // snake.contourTemplate.type=decision;
    snake->typeOfContour = energy_int_type;
    snake->weight_E_ext = weight_Eext;
    snake->weight_E_int = weight_Eint;
    snake->contourThreshold = contourThresh;
    snake->edgeStrenghtThreshold = edgeStrenghtThresh;
    snake->contour.clear();

    if(energy_ext_type == EnergyExternalField::GradientMagnitudes_corona){
        QMessageBox gsgd;
        gsgd.setText(" 1");
        gsgd.exec();

    snake->showImage = snake->originalImage.clone();
    imshow(" 1",snake->originalImage);

    imshow(" 2", snake->showImage);
            gsgd.setText(" 2");
            gsgd.exec();
    }

    if(snake->contour.isEmpty() || numberOfPoints!=snake->contour.size()){
        for(int i=0; i<numberOfPoints; i++){
            SnakePoint* temp = new SnakePoint();
            temp->setAlpha(baseAlpha);
            temp->setBeta(baseBeta);
            temp->setStep(baseStep);
            snake->contour.append(temp);
        }
    }
    float centerX;
    float centerY;

    switch(energy_int_type){
    case EnergyInternalTemplate::ClosedContour_Circle:
        if(energy_ext_type == EnergyExternalField::GradientMagnitudes_pupil){
            //finds center of pupil and its radius
            radius = 1.25 * snake->fastCenterLocalizationAlgorithm(snake->originalImage, fastCenter, fastInitKernelSize);
            centerX = fastCenter->x;
            centerY = fastCenter->y;
        }else if(energy_ext_type == EnergyExternalField::GradientMagnitudes_corona){
            centerX = fastCenter->x;
            centerY = fastCenter->y;
            radius = 1.1 * (snake->originalImage.cols /6);
        }else{
            centerX = fastCenter->x;
            centerY = fastCenter->y;
        }

        snake->possiblePupilCenter = *fastCenter;
        //set coordinates to circle
        snake->setCirclePositions(snake->contour,  centerX, centerY, radius, snake->getImageOriginal().cols, snake->getImageOriginal().rows);
        //EnergyInternalTemplate().countTotalEnergyInt(snake);
        break;
    case EnergyInternalTemplate::ClosedContour_Rectangle:
        //set appropriate rectangle, as corona is mostly taking about 1/3 of picure it is forced meassure
        snake->setRectanglePositions(snake->contour,  offsetX, offsetY, 0.33 * snake->originalImage.cols, 0.33 * snake->originalImage.rows, snake->getImageOriginal().cols, snake->getImageOriginal().rows);
        snake->possiblePupilCenter = *fastCenter;
        //EnergyInternalTemplate().countTotalEnergyInt(snake);
        break;
    default:;
    }
    //creation of external energy and control class
    if(energy_ext_type == EnergyExternalField::GradientMagnitudes_pupil || energy_ext_type == EnergyExternalField::GradientMagnitudes_corona){
        initSnakeExtField(snake, energy_ext_type, deviation, scale, *fastCenter);
    }else{
        initSnakeExtField(snake, energy_ext_type, deviation, scale);
    }

    snake->showMatrix(snake);//writes points into snake->showImage
}

void Snake::setCirclePositions(QList <SnakePoint*> points, float centerX, float centerY, float radius, int maxX, int maxY){
    int count = points.size();
    float angle = (2*M_PI)/count;
//    QFile outputFile("c:\\Temp\\testovaciebodykruhu.xls");
//    outputFile.open(QIODevice::WriteOnly | QFile::Text);
//    QTextStream outText(&outputFile);
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
//        outText << "x\t" << QString().number(points.at(i)->x) << "\ty\t" << QString().number(points.at(i)->y) << "\n";
    }

//        outputFile.close();
}

void Snake::setRectanglePositions(QList <SnakePoint*> points, float offsetX, float offsetY, float side_a, float side_b, int maxX, int maxY){
    int count = points.size();
    float periphery = ((2 * side_a) + (2 * side_b));
    float distance = periphery/count;
    float i = 0;
    for(int n = count - 1; n >= 0; n--){
         if( i <= side_a ){
            points.at(n)->x = (offsetX + i);
            points.at(n)->y = offsetY;
        }else if( i > side_a  && i <= (side_a + side_b) ){
            points.at(n)->x = (offsetX + side_a);
            points.at(n)->y = (offsetY + i - side_a);
        }else if( i > (side_a + side_b)  && i <= ((2 * side_a) + side_b) ){
            points.at(n)->x = (offsetX + side_a - i + side_a + side_b);
            points.at(n)->y = (offsetY + side_b);
        }else if( i > ((2 * side_a) + side_b)  && i < periphery ){
            points.at(n)->x = offsetX;
            points.at(n)->y = (offsetY + side_b - i + (2 * side_a) + side_b);
        }

        if(points.at(n)->x >= maxX)
            points.at(n)->x = maxX - 1;

        if(points.at(n)->y >= maxY)
            points.at(n)->y = maxY - 1;

        i += distance;
    }
}


void Snake::initSnakeExtField(Snake *snake, int energy_ext_type, float deviation, float scale, cv::Point possibleCenter){
    cv::Point *fastCenter = new cv::Point();
    switch(energy_ext_type){
    case EnergyExternalField::GradientMagnitudes:
        snake->vectorField = new EnergyExternalField(snake->getImageOriginal(), energy_ext_type, deviation, scale);
        fastCenterLocalizationAlgorithm(snake->getImageOriginal(), fastCenter);
        snake->vectorField->countVectorField(energy_ext_type, floor((double)fastCenter->x -1), floor((double)fastCenter->y -1));

        break;
    case EnergyExternalField::GradientMagnitudes_pupil:
        snake->vectorField = new EnergyExternalField(snake->getImageOriginal(), energy_ext_type, deviation, scale);
        snake->vectorField->countVectorField(energy_ext_type, floor((double)possibleCenter.x -1), floor((double)possibleCenter.y -1));

        break;
    case EnergyExternalField::GradientMagnitudes_corona:
        snake->vectorField = new EnergyExternalField(snake->getImageOriginal(), energy_ext_type, deviation, scale);
        snake->vectorField->countVectorField(energy_ext_type, floor((double)possibleCenter.x -1), floor((double)possibleCenter.y -1));

        break;

    default:;
    }
    //countTotalEnergyExt(snake);
}

void Snake::setAlphaToAllPoints(float alpha){
    for(int i=0; i<this->contour.size(); i++)
        this->contour.at(i)->setAlpha(alpha);
}

void Snake::setBetaToAllPoints(float beta){
    for(int i=0; i<this->contour.size(); i++)
        this->contour.at(i)->setBeta(beta);
}

void Snake::setStepToAllPoints(float step){
    for(int i=0; i<this->contour.size(); i++)
        this->contour.at(i)->setStep(step);
}

void Snake::countTotalEnergyExt(Snake *snake){
    snake->total_E_ext = 0;
    for (int i=0; i<snake->contour.size(); i++){
        snake->contour.at(i)->E_ext=snake->weight_E_ext * snake->vectorField->getValueFromVectorField(0, snake->contour.at(i)->x,snake->contour.at(i)->y);
        snake->total_E_ext += snake->contour.at(i)->E_ext;
    }
}

Mat Snake::iris_snake_function(Mat image)
{
    imshow("test",image);
    QMessageBox gsgd;
    gsgd.setText("test");
    gsgd.exec();
    Mat irisMap;
    Snake *snake_pupil = new Snake(image);
    Snake *snake_corona = new Snake(image);
    cv::Point *possiblePupilCenter = new cv::Point();
    initSnakeContour(snake_pupil, 50,
                                    EnergyInternalTemplate::ClosedContour_Circle,
                                    EnergyExternalField::GradientMagnitudes_pupil,
                                    //weights of internal and external energies
                                    1.2, 0.6,
                                    //alpha, beta,
                                    0.7, 0.55,
                                    //thresholds for curvature estimation
                                    1.3, 120,
                                    //gausian deviation, sobel scale factor
                                    97, 3.5,
                                    //step or neighborhood size where can control point move
                                    3,
                                    possiblePupilCenter);
    moveSnakeContour(snake_pupil);

    imshow("Iris Function after moving snake_pupil orig", showMatrix(snake_pupil, snake_pupil, Original_with_lines));
    imshow("Iris Function after moving snake_pupil grad", showMatrix(snake_pupil, snake_pupil, Gradient_with_lines));

    initSnakeContour(snake_corona, 100,
                                    EnergyInternalTemplate::ClosedContour_Circle,
                                    EnergyExternalField::GradientMagnitudes_corona,
                                    //weights of internal and external energies
                                    1.2, 0.6,
                                    //alpha, beta,
                                    0.75, 0.65,
                                    //thresholds for curvature estimation
                                    1.3, 150,
                                    //gausian deviation, sobel scale factor
                                    97, 3.5,
                                    //step or neighborhood size where can control point move
                                    3,
                                    possiblePupilCenter);
    moveSnakeContour(snake_corona);

    imshow("Iris Function after moving snake_corona orig", showMatrix(snake_pupil, snake_corona, Original_with_lines));
    imshow("Iris Function after moving snake_corona grad", showMatrix(snake_pupil, snake_corona, Gradient_with_lines));

    irisMap = showMatrix(snake_pupil, snake_corona, IrisMap);
    return irisMap;
}

int Snake::moveSnakeContour(Snake *snake)
{
    //int n = snake->contour.size();
    //saveSnakeToTextFile(snake);
    bool equilibrium = false;
    int cycles = 0;
    switch(snake->vectorField->getTypeOfVectorField()){
    //this switch is only for future improvement of code
    case EnergyExternalField::GradientMagnitudes:
        //while(!equilibrium) iterate through all points in cycles
        while(!equilibrium && (cycles < 200)){
            int movedCount = 0;
            if (!equilibrium){
            //for all points count locally external and internal energy and look if is in neighborhood point witl lower energy

                for (int n=0; n <= snake->contour.size(); n++){
                    int i = n%snake->contour.size();
                    int neighborhoodSize = (snake->contour.at(i)->step * 2) + 1 ;
                    Mat actual_x_coordinate =  Mat(neighborhoodSize, neighborhoodSize, CV_32FC1);
                    Mat actual_y_coordinate =  Mat(neighborhoodSize, neighborhoodSize, CV_32FC1);

                    Mat actual_local_int_E_1stg = Mat(neighborhoodSize, neighborhoodSize, CV_32FC1);
                    Mat actual_local_int_E_2stg = Mat(neighborhoodSize, neighborhoodSize, CV_32FC1);

                    Mat actual_local_ext_E = Mat(snake->vectorField->getNeighborhoodExtE(snake->contour.at(i)->x, snake->contour.at(i)->y, snake->contour.at(i)->step, 0).clone());
                    Mat actual_local_ext_E_norm = Mat(neighborhoodSize, neighborhoodSize, CV_32FC1);
//                    snake->vectorField->saveMatToTextFile(actual_local_ext_E, "C:\\Users\\lukassos\\Documents\\kodenie\\duhovecka-build-desktop-Qt_4_7_4_for_Desktop_-_MinGW_4_4__Qt_SDK__Debug\\_debugimg\\moving_extE.txt");
                    double minExtE = 0;
                    double maxExtE = 0;
                    cv::minMaxIdx(actual_local_ext_E, &minExtE, &maxExtE);

                    if((maxExtE - minExtE) < 5){
                        minExtE = maxExtE - 5;
                    }

                    snake->avgDist = EnergyInternalTemplate().getAverageDistance(*snake);


                    //for steps*steps points around snake->contour.at(i)
                    int index_neighbor_x = 0;
                    int index_neighbor_y = 0;
                    for (int actual_y = (snake->contour.at(i)->y - snake->contour.at(i)->step); actual_y <= (snake->contour.at(i)->y + snake->contour.at(i)->step); actual_y++)
                    {
                        index_neighbor_x = 0;
                        for (int actual_x = (snake->contour.at(i)->x -  snake->contour.at(i)->step); actual_x <= (snake->contour.at(i)->x + snake->contour.at(i)->step); actual_x++)
                        {
                            //1st Stage of Internal Energy

                            //actual_local_int_E_1stg.at<float>(index_neighbor_y, index_neighbor_x) = 4;
                            actual_local_int_E_1stg.at<float>(index_neighbor_y, index_neighbor_x) =
                                    EnergyInternalTemplate().countLocalEnergyInt1stage(*snake, i, actual_x, actual_y);
                            //2nd Stage of Internal Energy
                            actual_local_int_E_2stg.at<float>(index_neighbor_y, index_neighbor_x) =
                                    EnergyInternalTemplate().countLocalEnergyInt2stage(*snake, i, actual_x, actual_y);
                            //Normalized External Energy
                            actual_local_ext_E_norm.at<float>(index_neighbor_y, index_neighbor_x) =
                                    ( (float) minExtE - actual_local_ext_E.at<unsigned char>(index_neighbor_y, index_neighbor_x)) / ( (float)maxExtE - (float)minExtE );
                            //fill coordinates to matrixes
                            if(actual_x < 0){
                                actual_x_coordinate.at<float>(index_neighbor_y, index_neighbor_x) = 0;
                            }else if(actual_x >= snake->originalImage.cols){
                                actual_x_coordinate.at<float>(index_neighbor_y, index_neighbor_x) = snake->originalImage.cols - 1;
                            }else{
                                actual_x_coordinate.at<float>(index_neighbor_y, index_neighbor_x) = actual_x;
                            }
                            if(actual_y < 0){
                                actual_y_coordinate.at<float>(index_neighbor_y, index_neighbor_x) = 0;
                            }else if(actual_y >= snake->originalImage.cols){
                                actual_y_coordinate.at<float>(index_neighbor_y, index_neighbor_x) = snake->originalImage.rows - 1;
                            }else{
                                actual_y_coordinate.at<float>(index_neighbor_y, index_neighbor_x) = actual_y;
                            }
                            //constrain energy of picture border
                            if( actual_x < 0 || actual_x >= snake->originalImage.cols
                                    || actual_y < 0 || actual_y >= snake->originalImage.rows)
                            {
                                actual_local_ext_E_norm.at<float>(index_neighbor_y, index_neighbor_x) = 1;
                            }
                            index_neighbor_x++;
                        }
                        index_neighbor_y++;
                    }
//                    snake->vectorField->saveMatToTextFile(actual_local_int_E_1stg, "C:\\Users\\lukassos\\Documents\\kodenie\\duhovecka-build-desktop-Qt_4_7_4_for_Desktop_-_MinGW_4_4__Qt_SDK__Debug\\_debugimg\\moving_1stage.txt");
//                    snake->vectorField->saveMatToTextFile(actual_local_int_E_2stg, "C:\\Users\\lukassos\\Documents\\kodenie\\duhovecka-build-desktop-Qt_4_7_4_for_Desktop_-_MinGW_4_4__Qt_SDK__Debug\\_debugimg\\moving_2stage.txt");
//                    snake->vectorField->saveMatToTextFile(actual_local_ext_E_norm, "C:\\Users\\lukassos\\Documents\\kodenie\\duhovecka-build-desktop-Qt_4_7_4_for_Desktop_-_MinGW_4_4__Qt_SDK__Debug\\_debugimg\\moving_extNorm.txt");
//                    snake->vectorField->saveMatToTextFile(actual_x_coordinate, "C:\\Users\\lukassos\\Documents\\kodenie\\duhovecka-build-desktop-Qt_4_7_4_for_Desktop_-_MinGW_4_4__Qt_SDK__Debug\\_debugimg\\moving_coordinatesX.txt");
//                    snake->vectorField->saveMatToTextFile(actual_y_coordinate, "C:\\Users\\lukassos\\Documents\\kodenie\\duhovecka-build-desktop-Qt_4_7_4_for_Desktop_-_MinGW_4_4__Qt_SDK__Debug\\_debugimg\\moving_coordinatesY.txt");

                    double maxIntE_1stg = 0;
                    double maxIntE_2stg = 0;
                    cv::minMaxIdx(actual_local_int_E_1stg, 0, &maxIntE_1stg);
                    cv::minMaxIdx(actual_local_int_E_2stg, 0, &maxIntE_2stg);
                    //normalization of internal energy
                    actual_local_int_E_1stg = actual_local_int_E_1stg/maxIntE_1stg;
                    actual_local_int_E_2stg = actual_local_int_E_2stg/maxIntE_2stg;
//                    snake->vectorField->saveMatToTextFile(actual_local_int_E_1stg, "C:\\Users\\lukassos\\Documents\\kodenie\\duhovecka-build-desktop-Qt_4_7_4_for_Desktop_-_MinGW_4_4__Qt_SDK__Debug\\_debugimg\\moving_1stageNorm.txt");
//                    snake->vectorField->saveMatToTextFile(actual_local_int_E_2stg, "C:\\Users\\lukassos\\Documents\\kodenie\\duhovecka-build-desktop-Qt_4_7_4_for_Desktop_-_MinGW_4_4__Qt_SDK__Debug\\_debugimg\\moving_2stageNorm.txt");


                    int best_x = -1;
                    int best_intex_x = snake->contour.at(i)->step;
                    int best_y = -1;
                    int best_intex_y = snake->contour.at(i)->step;
                    float actual_local_snake_E;
                    //set big value for further comparision with local minima
                    snake->contour.at(i)->E_snake = snake->weight_E_int * (snake->contour.at(i)->alpha + snake->contour.at(i)->beta)
                            +
                            snake->weight_E_ext * (255);

                    //sum of all Energies actual_local_snake_E = weightInt * ( alpha * 1stage + beta * 2stage) + weightExt * nomalizedExt
                    for (index_neighbor_x = 0; index_neighbor_x < neighborhoodSize; index_neighbor_x++){
                        for(index_neighbor_y = 0; index_neighbor_y < neighborhoodSize; index_neighbor_y++){
                            actual_local_snake_E=
                                    ( snake->weight_E_int *
                                      ( snake->contour.at(i)->alpha * ( actual_local_int_E_1stg.at<float>(index_neighbor_y, index_neighbor_x) )
                                        +
                                        snake->contour.at(i)->beta * ( actual_local_int_E_2stg.at<float>(index_neighbor_y, index_neighbor_x) )
                                        )
                                      +
                                      snake->weight_E_ext * actual_local_ext_E_norm.at<float>(index_neighbor_y, index_neighbor_x)
                                      );

                            if(actual_local_snake_E < snake->contour.at(i)->E_snake){
                                snake->contour.at(i)->E_snake = actual_local_snake_E;
                                best_x = actual_x_coordinate.at<float>(index_neighbor_y, index_neighbor_x);
                                best_y = actual_y_coordinate.at<float>(index_neighbor_y, index_neighbor_x);
                                if( best_x < 0 ){
                                    best_x = 0;
                                }else if(best_x >= snake->originalImage.cols){
                                    best_x = snake->originalImage.cols - 1;
                                }else{
                                    best_intex_x = index_neighbor_x;
                                }

                                if( best_y < 0 ){
                                    best_y = 0;
                                }else if(best_y >= snake->originalImage.rows){
                                    best_y = snake->originalImage.rows - 1;
                                }else{
                                    best_intex_y = index_neighbor_y;
                                }

                            }
                        }
                    }

                    //if coordinates changed then movedCount increase and move point
                    if (best_x != snake->contour.at(i)->x && best_y != snake->contour.at(i)->y){
                        snake->contour.at(i)->x = best_x;
                        snake->contour.at(i)->y = best_y;
                        movedCount++;
                    }
                    snake->contour.at(i)->E_ext = actual_local_ext_E.at<float>(best_intex_y, best_intex_x);
                }

                //here goes energy curvature estimation for better beta parameter setting
                //curvature is counted for all snake points
                EnergyInternalTemplate().countContourEstimation(snake);
                //if curvature is maximal then set beta
                for( int i=0; i < snake->contour.size(); i++){
                    if(  EnergyInternalTemplate().largerThanContourOfNeighbors(*snake, i, true)
                         &&
                         EnergyInternalTemplate().largerThanContourOfNeighbors(*snake, i, false)
                         &&
                         snake->contour.at(i)->C_int > snake->contourThreshold
                         &&
                         snake->contour.at(i)->E_ext > snake->edgeStrenghtThreshold )

                        snake->contour.at(i)->beta = 0;
                }


                //if only 3 points moved then stop iteration of snake
                if (movedCount < 3){
                    equilibrium = true;
                }
            }

            cycles++;
        }

        break;
    case EnergyExternalField::GradientMagnitudes_pupil:
        //while(!equilibrium) iterate through all points in cycles
        while(!equilibrium && (cycles < 200)){
            int movedCount = 0;
            if (!equilibrium){
            //for all points count locally external and internal energy and look if is in neighborhood point witl lower energy

                for (int n=0; n <= snake->contour.size(); n++){
                    int i = n%snake->contour.size();
                    int neighborhoodSize = (snake->contour.at(i)->step * 2) + 1 ;
                    Mat actual_x_coordinate =  Mat(neighborhoodSize, neighborhoodSize, CV_32FC1);
                    Mat actual_y_coordinate =  Mat(neighborhoodSize, neighborhoodSize, CV_32FC1);

                    Mat actual_local_int_E_1stg = Mat(neighborhoodSize, neighborhoodSize, CV_32FC1);
                    Mat actual_local_int_E_2stg = Mat(neighborhoodSize, neighborhoodSize, CV_32FC1);

                    Mat actual_local_ext_E = Mat(snake->vectorField->getNeighborhoodExtE(snake->contour.at(i)->x, snake->contour.at(i)->y, snake->contour.at(i)->step, 0).clone());
                    Mat actual_local_ext_E_norm = Mat(neighborhoodSize, neighborhoodSize, CV_32FC1);
//                    snake->vectorField->saveMatToTextFile(actual_local_ext_E, "C:\\Users\\lukassos\\Documents\\kodenie\\duhovecka-build-desktop-Qt_4_7_4_for_Desktop_-_MinGW_4_4__Qt_SDK__Debug\\_debugimg\\moving_extE.txt");
                    double minExtE = 0;
                    double maxExtE = 0;
                    cv::minMaxIdx(actual_local_ext_E, &minExtE, &maxExtE);

                    if((maxExtE - minExtE) < 5){
                        minExtE = maxExtE - 5;
                    }

                    snake->avgDist = EnergyInternalTemplate().getAverageDistance(*snake);


                    //for steps*steps points around snake->contour.at(i)
                    int index_neighbor_x = 0;
                    int index_neighbor_y = 0;
                    for (int actual_y = (snake->contour.at(i)->y - snake->contour.at(i)->step); actual_y <= (snake->contour.at(i)->y + snake->contour.at(i)->step); actual_y++)
                    {
                        index_neighbor_x = 0;
                        for (int actual_x = (snake->contour.at(i)->x -  snake->contour.at(i)->step); actual_x <= (snake->contour.at(i)->x + snake->contour.at(i)->step); actual_x++)
                        {
                            //1st Stage of Internal Energy

                            //actual_local_int_E_1stg.at<float>(index_neighbor_y, index_neighbor_x) = 4;
                            actual_local_int_E_1stg.at<float>(index_neighbor_y, index_neighbor_x) =
                                    EnergyInternalTemplate().countLocalEnergyInt1stage(*snake, i, actual_x, actual_y);
                            //2nd Stage of Internal Energy
                            actual_local_int_E_2stg.at<float>(index_neighbor_y, index_neighbor_x) =
                                    EnergyInternalTemplate().countLocalEnergyInt2stage(*snake, i, actual_x, actual_y);
                            //Normalized External Energy
                            actual_local_ext_E_norm.at<float>(index_neighbor_y, index_neighbor_x) =
                                    ( (float) minExtE - actual_local_ext_E.at<unsigned char>(index_neighbor_y, index_neighbor_x)) / ( (float)maxExtE - (float)minExtE );
                            //fill coordinates to matrixes
                            if(actual_x < 0){
                                actual_x_coordinate.at<float>(index_neighbor_y, index_neighbor_x) = 0;
                            }else if(actual_x >= snake->originalImage.cols){
                                actual_x_coordinate.at<float>(index_neighbor_y, index_neighbor_x) = snake->originalImage.cols - 1;
                            }else{
                                actual_x_coordinate.at<float>(index_neighbor_y, index_neighbor_x) = actual_x;
                            }
                            if(actual_y < 0){
                                actual_y_coordinate.at<float>(index_neighbor_y, index_neighbor_x) = 0;
                            }else if(actual_y >= snake->originalImage.cols){
                                actual_y_coordinate.at<float>(index_neighbor_y, index_neighbor_x) = snake->originalImage.rows - 1;
                            }else{
                                actual_y_coordinate.at<float>(index_neighbor_y, index_neighbor_x) = actual_y;
                            }
                            //constrain energy of picture border
                            if( actual_x < 0 || actual_x >= snake->originalImage.cols
                                    || actual_y < 0 || actual_y >= snake->originalImage.rows)
                            {
                                actual_local_ext_E_norm.at<float>(index_neighbor_y, index_neighbor_x) = 1;
                            }
                            index_neighbor_x++;
                        }
                        index_neighbor_y++;
                    }
//                    snake->vectorField->saveMatToTextFile(actual_local_int_E_1stg, "C:\\Users\\lukassos\\Documents\\kodenie\\duhovecka-build-desktop-Qt_4_7_4_for_Desktop_-_MinGW_4_4__Qt_SDK__Debug\\_debugimg\\moving_1stage.txt");
//                    snake->vectorField->saveMatToTextFile(actual_local_int_E_2stg, "C:\\Users\\lukassos\\Documents\\kodenie\\duhovecka-build-desktop-Qt_4_7_4_for_Desktop_-_MinGW_4_4__Qt_SDK__Debug\\_debugimg\\moving_2stage.txt");
//                    snake->vectorField->saveMatToTextFile(actual_local_ext_E_norm, "C:\\Users\\lukassos\\Documents\\kodenie\\duhovecka-build-desktop-Qt_4_7_4_for_Desktop_-_MinGW_4_4__Qt_SDK__Debug\\_debugimg\\moving_extNorm.txt");
//                    snake->vectorField->saveMatToTextFile(actual_x_coordinate, "C:\\Users\\lukassos\\Documents\\kodenie\\duhovecka-build-desktop-Qt_4_7_4_for_Desktop_-_MinGW_4_4__Qt_SDK__Debug\\_debugimg\\moving_coordinatesX.txt");
//                    snake->vectorField->saveMatToTextFile(actual_y_coordinate, "C:\\Users\\lukassos\\Documents\\kodenie\\duhovecka-build-desktop-Qt_4_7_4_for_Desktop_-_MinGW_4_4__Qt_SDK__Debug\\_debugimg\\moving_coordinatesY.txt");

                    double maxIntE_1stg = 0;
                    double maxIntE_2stg = 0;
                    cv::minMaxIdx(actual_local_int_E_1stg, 0, &maxIntE_1stg);
                    cv::minMaxIdx(actual_local_int_E_2stg, 0, &maxIntE_2stg);
                    //normalization of internal energy
                    actual_local_int_E_1stg = actual_local_int_E_1stg/maxIntE_1stg;
                    actual_local_int_E_2stg = actual_local_int_E_2stg/maxIntE_2stg;
//                    snake->vectorField->saveMatToTextFile(actual_local_int_E_1stg, "C:\\Users\\lukassos\\Documents\\kodenie\\duhovecka-build-desktop-Qt_4_7_4_for_Desktop_-_MinGW_4_4__Qt_SDK__Debug\\_debugimg\\moving_1stageNorm.txt");
//                    snake->vectorField->saveMatToTextFile(actual_local_int_E_2stg, "C:\\Users\\lukassos\\Documents\\kodenie\\duhovecka-build-desktop-Qt_4_7_4_for_Desktop_-_MinGW_4_4__Qt_SDK__Debug\\_debugimg\\moving_2stageNorm.txt");


                    int best_x = -1;
                    int best_intex_x = snake->contour.at(i)->step;
                    int best_y = -1;
                    int best_intex_y = snake->contour.at(i)->step;
                    float actual_local_snake_E;
                    //set big value for further comparision with local minima
                    snake->contour.at(i)->E_snake = snake->weight_E_int * (snake->contour.at(i)->alpha + snake->contour.at(i)->beta)
                            +
                            snake->weight_E_ext * (255);

                    //sum of all Energies actual_local_snake_E = weightInt * ( alpha * 1stage + beta * 2stage) + weightExt * nomalizedExt
                    for (index_neighbor_x = 0; index_neighbor_x < neighborhoodSize; index_neighbor_x++){
                        for(index_neighbor_y = 0; index_neighbor_y < neighborhoodSize; index_neighbor_y++){
                            actual_local_snake_E=
                                    ( snake->weight_E_int *
                                      ( snake->contour.at(i)->alpha * ( actual_local_int_E_1stg.at<float>(index_neighbor_y, index_neighbor_x) )
                                        +
                                        snake->contour.at(i)->beta * ( actual_local_int_E_2stg.at<float>(index_neighbor_y, index_neighbor_x) )
                                        )
                                      +
                                      snake->weight_E_ext * actual_local_ext_E_norm.at<float>(index_neighbor_y, index_neighbor_x)
                                      );

                            if(actual_local_snake_E < snake->contour.at(i)->E_snake){
                                snake->contour.at(i)->E_snake = actual_local_snake_E;
                                best_x = actual_x_coordinate.at<float>(index_neighbor_y, index_neighbor_x);
                                best_y = actual_y_coordinate.at<float>(index_neighbor_y, index_neighbor_x);
                                if( best_x < 0 ){
                                    best_x = 0;
                                }else if(best_x >= snake->originalImage.cols){
                                    best_x = snake->originalImage.cols - 1;
                                }else{
                                    best_intex_x = index_neighbor_x;
                                }

                                if( best_y < 0 ){
                                    best_y = 0;
                                }else if(best_y >= snake->originalImage.rows){
                                    best_y = snake->originalImage.rows - 1;
                                }else{
                                    best_intex_y = index_neighbor_y;
                                }

                            }
                        }
                    }

                    //if coordinates changed then movedCount increase and move point
                    if (best_x != snake->contour.at(i)->x && best_y != snake->contour.at(i)->y){
                        snake->contour.at(i)->x = best_x;
                        snake->contour.at(i)->y = best_y;
                        movedCount++;
                    }
                    snake->contour.at(i)->E_ext = actual_local_ext_E.at<float>(best_intex_y, best_intex_x);
                }

                //here goes energy curvature estimation for better beta parameter setting
                //curvature is counted for all snake points
                EnergyInternalTemplate().countContourEstimation(snake);
                //if curvature is maximal then set beta
                for( int i=0; i < snake->contour.size(); i++){
                    if(  EnergyInternalTemplate().largerThanContourOfNeighbors(*snake, i, true)
                         &&
                         EnergyInternalTemplate().largerThanContourOfNeighbors(*snake, i, false)
                         &&
                         snake->contour.at(i)->C_int > snake->contourThreshold
                         &&
                         snake->contour.at(i)->E_ext > snake->edgeStrenghtThreshold )

                        snake->contour.at(i)->beta = 0;
                }


                //if only 3 points moved then stop iteration of snake
                if (movedCount < 3){
                    equilibrium = true;
                }
            }

            cycles++;
        }

        break;
    case EnergyExternalField::GradientMagnitudes_corona:
        //while(!equilibrium) iterate through all points in cycles
        while(!equilibrium && (cycles < 200)){
            int movedCount = 0;
            if (!equilibrium){
            //for all points count locally external and internal energy and look if is in neighborhood point witl lower energy

                for (int n=0; n <= snake->contour.size(); n++){
                    int i = n%snake->contour.size();
                    int neighborhoodSize = (snake->contour.at(i)->step * 2) + 1 ;
                    Mat actual_x_coordinate =  Mat(neighborhoodSize, neighborhoodSize, CV_32FC1);
                    Mat actual_y_coordinate =  Mat(neighborhoodSize, neighborhoodSize, CV_32FC1);

                    Mat actual_local_int_E_1stg = Mat(neighborhoodSize, neighborhoodSize, CV_32FC1);
                    Mat actual_local_int_E_2stg = Mat(neighborhoodSize, neighborhoodSize, CV_32FC1);

                    Mat actual_local_ext_E = Mat(snake->vectorField->getNeighborhoodExtE(snake->contour.at(i)->x, snake->contour.at(i)->y, snake->contour.at(i)->step, 0).clone());
                    Mat actual_local_ext_E_norm = Mat(neighborhoodSize, neighborhoodSize, CV_32FC1);
//                    snake->vectorField->saveMatToTextFile(actual_local_ext_E, "C:\\Users\\lukassos\\Documents\\kodenie\\duhovecka-build-desktop-Qt_4_7_4_for_Desktop_-_MinGW_4_4__Qt_SDK__Debug\\_debugimg\\moving_extE.txt");
                    double minExtE = 0;
                    double maxExtE = 0;
                    cv::minMaxIdx(actual_local_ext_E, &minExtE, &maxExtE);

                    if((maxExtE - minExtE) < 5){
                        minExtE = maxExtE - 5;
                    }

                    snake->avgDist = EnergyInternalTemplate().getAverageDistance(*snake);


                    //for steps*steps points around snake->contour.at(i)
                    int index_neighbor_x = 0;
                    int index_neighbor_y = 0;
                    for (int actual_y = (snake->contour.at(i)->y - snake->contour.at(i)->step); actual_y <= (snake->contour.at(i)->y + snake->contour.at(i)->step); actual_y++)
                    {
                        index_neighbor_x = 0;
                        for (int actual_x = (snake->contour.at(i)->x -  snake->contour.at(i)->step); actual_x <= (snake->contour.at(i)->x + snake->contour.at(i)->step); actual_x++)
                        {
                            //1st Stage of Internal Energy

                            //actual_local_int_E_1stg.at<float>(index_neighbor_y, index_neighbor_x) = 4;
                            actual_local_int_E_1stg.at<float>(index_neighbor_y, index_neighbor_x) =
                                    EnergyInternalTemplate().countLocalEnergyInt1stage(*snake, i, actual_x, actual_y);
                            //2nd Stage of Internal Energy
                            actual_local_int_E_2stg.at<float>(index_neighbor_y, index_neighbor_x) =
                                    EnergyInternalTemplate().countLocalEnergyInt2stage(*snake, i, actual_x, actual_y);
                            //Normalized External Energy
                            actual_local_ext_E_norm.at<float>(index_neighbor_y, index_neighbor_x) =
                                    ( (float) minExtE - actual_local_ext_E.at<unsigned char>(index_neighbor_y, index_neighbor_x)) / ( (float)maxExtE - (float)minExtE );
                            //fill coordinates to matrixes
                            if(actual_x < 0){
                                actual_x_coordinate.at<float>(index_neighbor_y, index_neighbor_x) = 0;
                            }else if(actual_x >= snake->originalImage.cols){
                                actual_x_coordinate.at<float>(index_neighbor_y, index_neighbor_x) = snake->originalImage.cols - 1;
                            }else{
                                actual_x_coordinate.at<float>(index_neighbor_y, index_neighbor_x) = actual_x;
                            }
                            if(actual_y < 0){
                                actual_y_coordinate.at<float>(index_neighbor_y, index_neighbor_x) = 0;
                            }else if(actual_y >= snake->originalImage.cols){
                                actual_y_coordinate.at<float>(index_neighbor_y, index_neighbor_x) = snake->originalImage.rows - 1;
                            }else{
                                actual_y_coordinate.at<float>(index_neighbor_y, index_neighbor_x) = actual_y;
                            }
                            //constrain energy of picture border
                            if( actual_x < 0 || actual_x >= snake->originalImage.cols
                                    || actual_y < 0 || actual_y >= snake->originalImage.rows)
                            {
                                actual_local_ext_E_norm.at<float>(index_neighbor_y, index_neighbor_x) = 1;
                            }
                            index_neighbor_x++;
                        }
                        index_neighbor_y++;
                    }
//                    snake->vectorField->saveMatToTextFile(actual_local_int_E_1stg, "C:\\Users\\lukassos\\Documents\\kodenie\\duhovecka-build-desktop-Qt_4_7_4_for_Desktop_-_MinGW_4_4__Qt_SDK__Debug\\_debugimg\\moving_1stage.txt");
//                    snake->vectorField->saveMatToTextFile(actual_local_int_E_2stg, "C:\\Users\\lukassos\\Documents\\kodenie\\duhovecka-build-desktop-Qt_4_7_4_for_Desktop_-_MinGW_4_4__Qt_SDK__Debug\\_debugimg\\moving_2stage.txt");
//                    snake->vectorField->saveMatToTextFile(actual_local_ext_E_norm, "C:\\Users\\lukassos\\Documents\\kodenie\\duhovecka-build-desktop-Qt_4_7_4_for_Desktop_-_MinGW_4_4__Qt_SDK__Debug\\_debugimg\\moving_extNorm.txt");
//                    snake->vectorField->saveMatToTextFile(actual_x_coordinate, "C:\\Users\\lukassos\\Documents\\kodenie\\duhovecka-build-desktop-Qt_4_7_4_for_Desktop_-_MinGW_4_4__Qt_SDK__Debug\\_debugimg\\moving_coordinatesX.txt");
//                    snake->vectorField->saveMatToTextFile(actual_y_coordinate, "C:\\Users\\lukassos\\Documents\\kodenie\\duhovecka-build-desktop-Qt_4_7_4_for_Desktop_-_MinGW_4_4__Qt_SDK__Debug\\_debugimg\\moving_coordinatesY.txt");

                    double maxIntE_1stg = 0;
                    double maxIntE_2stg = 0;
                    cv::minMaxIdx(actual_local_int_E_1stg, 0, &maxIntE_1stg);
                    cv::minMaxIdx(actual_local_int_E_2stg, 0, &maxIntE_2stg);
                    //normalization of internal energy
                    actual_local_int_E_1stg = actual_local_int_E_1stg/maxIntE_1stg;
                    actual_local_int_E_2stg = actual_local_int_E_2stg/maxIntE_2stg;
//                    snake->vectorField->saveMatToTextFile(actual_local_int_E_1stg, "C:\\Users\\lukassos\\Documents\\kodenie\\duhovecka-build-desktop-Qt_4_7_4_for_Desktop_-_MinGW_4_4__Qt_SDK__Debug\\_debugimg\\moving_1stageNorm.txt");
//                    snake->vectorField->saveMatToTextFile(actual_local_int_E_2stg, "C:\\Users\\lukassos\\Documents\\kodenie\\duhovecka-build-desktop-Qt_4_7_4_for_Desktop_-_MinGW_4_4__Qt_SDK__Debug\\_debugimg\\moving_2stageNorm.txt");


                    int best_x = -1;
                    int best_intex_x = snake->contour.at(i)->step;
                    int best_y = -1;
                    int best_intex_y = snake->contour.at(i)->step;
                    float actual_local_snake_E;
                    //set big value for further comparision with local minima
                    snake->contour.at(i)->E_snake = snake->weight_E_int * (snake->contour.at(i)->alpha + snake->contour.at(i)->beta)
                            +
                            snake->weight_E_ext * (255);

                    //sum of all Energies actual_local_snake_E = weightInt * ( alpha * 1stage + beta * 2stage) + weightExt * nomalizedExt
                    for (index_neighbor_x = 0; index_neighbor_x < neighborhoodSize; index_neighbor_x++){
                        for(index_neighbor_y = 0; index_neighbor_y < neighborhoodSize; index_neighbor_y++){
                            actual_local_snake_E=
                                    ( snake->weight_E_int *
                                      ( snake->contour.at(i)->alpha * ( actual_local_int_E_1stg.at<float>(index_neighbor_y, index_neighbor_x) )
                                        +
                                        snake->contour.at(i)->beta * ( actual_local_int_E_2stg.at<float>(index_neighbor_y, index_neighbor_x) )
                                        )
                                      +
                                      snake->weight_E_ext * actual_local_ext_E_norm.at<float>(index_neighbor_y, index_neighbor_x)
                                      );

                            if(actual_local_snake_E < snake->contour.at(i)->E_snake){
                                snake->contour.at(i)->E_snake = actual_local_snake_E;
                                best_x = actual_x_coordinate.at<float>(index_neighbor_y, index_neighbor_x);
                                best_y = actual_y_coordinate.at<float>(index_neighbor_y, index_neighbor_x);
                                if( best_x < 0 ){
                                    best_x = 0;
                                }else if(best_x >= snake->originalImage.cols){
                                    best_x = snake->originalImage.cols - 1;
                                }else{
                                    best_intex_x = index_neighbor_x;
                                }

                                if( best_y < 0 ){
                                    best_y = 0;
                                }else if(best_y >= snake->originalImage.rows){
                                    best_y = snake->originalImage.rows - 1;
                                }else{
                                    best_intex_y = index_neighbor_y;
                                }

                            }
                        }
                    }

                    //if coordinates changed then movedCount increase and move point
                    if (best_x != snake->contour.at(i)->x && best_y != snake->contour.at(i)->y){
                        snake->contour.at(i)->x = best_x;
                        snake->contour.at(i)->y = best_y;
                        movedCount++;
                    }
                    snake->contour.at(i)->E_ext = actual_local_ext_E.at<float>(best_intex_y, best_intex_x);
                }

                //here goes energy curvature estimation for better beta parameter setting
                //curvature is counted for all snake points
                EnergyInternalTemplate().countContourEstimation(snake);
                //if curvature is maximal then set beta
                for( int i=0; i < snake->contour.size(); i++){
                    if(  EnergyInternalTemplate().largerThanContourOfNeighbors(*snake, i, true)
                         &&
                         EnergyInternalTemplate().largerThanContourOfNeighbors(*snake, i, false)
                         &&
                         snake->contour.at(i)->C_int > snake->contourThreshold
                         &&
                         snake->contour.at(i)->E_ext > snake->edgeStrenghtThreshold )

                        snake->contour.at(i)->beta = 0;
                }


                //if only 3 points moved then stop iteration of snake
                if (movedCount < 3){
                    equilibrium = true;
                }
            }

            cycles++;
        }

        break;

    default:;
    }
    //write snake points into image matrix for showing
    showMatrix(snake);
    //saveSnakeToTextFile(snake);
    return cycles;
}


void Snake::showMatrix(Snake *snake){
    //Mat zeromat = Mat().zeros(snake->originalImage.rows,snake->originalImage.cols, CV_8U);
    snake->matrixOfPoints.release();
    snake->matrixOfPoints = Mat(snake->originalImage.rows,snake->originalImage.cols, CV_32FC3, 0);
    cvtColor(snake->vectorField->getConvertedVectorField(0), snake->matrixOfPoints, CV_GRAY2RGBA);
    //cvtColor(zeromat, snake->matrixOfPoints, CV_GRAY2RGBA);
    snake->showImage.release();
    snake->showImage = Mat(snake->originalImage.rows,snake->originalImage.cols, CV_32FC3, 0);
    cvtColor(snake->originalImage, snake->showImage, CV_GRAY2RGBA);

    //    for(int i = 0; i < imageToShow.rows; i++)
    //        for(int j = 0; j < imageToShow.cols; j++)
    //            imageToShow.at<Vec3b>(i,j)[2]=155;
        for(int i=0; i<snake->contour.size(); i++){
            if(i < (snake->contour.size() - 1) ){
                cv::line(snake->matrixOfPoints,cv::Point((int)(snake->contour.at(i)->x), (int)(snake->contour.at(i)->y)),cv::Point((int)(snake->contour.at(i+1)->x), (int)(snake->contour.at(i+1)->y)), cv::Scalar(0,105,230));
            }else{
                cv::line(snake->matrixOfPoints,cv::Point((int)(snake->contour.at(i)->x), (int)(snake->contour.at(i)->y)),cv::Point((int)(snake->contour.at(0)->x), (int)(snake->contour.at(0)->y)), cv::Scalar(0,105,230));
            }

            snake->matrixOfPoints.at<Vec3b>((int)(snake->contour.at(i)->y), (int)(snake->contour.at(i)->x))[0] = 0;//255 - imageToShow.at<float>(snake->contour.at(i)->y, snake->contour.at(i)->x);
            snake->matrixOfPoints.at<Vec3b>((int)(snake->contour.at(i)->y), (int)(snake->contour.at(i)->x))[1] = 255;
            snake->matrixOfPoints.at<Vec3b>((int)(snake->contour.at(i)->y), (int)(snake->contour.at(i)->x))[2] = 0;

            snake->showImage.at<Vec3b>(snake->contour.at(i)->y, snake->contour.at(i)->x)[0] = 0;//255 - imageToShow.at<float>(snake->contour.at(i)->y, snake->contour.at(i)->x);
            snake->showImage.at<Vec3b>(snake->contour.at(i)->y, snake->contour.at(i)->x)[1] = 0;
            snake->showImage.at<Vec3b>(snake->contour.at(i)->y, snake->contour.at(i)->x)[2] = 255;

    }
}

Mat Snake::showMatrix(Snake *snake_pupil, Snake *snake_corona, int type){
    Mat out =  Mat(snake_corona->originalImage.rows,snake_corona->originalImage.cols, CV_32FC3, 0);

    cv::Point countour_polygon[snake_corona->contour.size()];
    cv::Point pupil_polygon[snake_pupil->contour.size()];

    switch(type){
    case Snake::Original_with_lines:
        //set background
        cvtColor(snake_corona->originalImage, out, CV_GRAY2RGBA);

        //draw corona snake
        for(int i=0; i<snake_corona->contour.size(); i++){
            if(i < (snake_corona->contour.size() - 1) ){
                cv::line(out,cv::Point((int)(snake_corona->contour.at(i)->x), (int)(snake_corona->contour.at(i)->y)),cv::Point((int)(snake_corona->contour.at(i+1)->x), (int)(snake_corona->contour.at(i+1)->y)), cv::Scalar(0,105,230));
            }else{
                cv::line(out,cv::Point((int)(snake_corona->contour.at(i)->x), (int)(snake_corona->contour.at(i)->y)),cv::Point((int)(snake_corona->contour.at(0)->x), (int)(snake_corona->contour.at(0)->y)), cv::Scalar(0,105,230));
            }
            out.at<Vec3b>((int)(snake_corona->contour.at(i)->y), (int)(snake_corona->contour.at(i)->x))[0] = 255;
            out.at<Vec3b>((int)(snake_corona->contour.at(i)->y), (int)(snake_corona->contour.at(i)->x))[1] = 0;
            out.at<Vec3b>((int)(snake_corona->contour.at(i)->y), (int)(snake_corona->contour.at(i)->x))[2] = 0;
        }
        //draw pupil snake
        for(int i=0; i<snake_pupil->contour.size(); i++){
            if(i < (snake_pupil->contour.size() - 1) ){
                cv::line(snake_pupil->matrixOfPoints,cv::Point((int)(snake_pupil->contour.at(i)->x), (int)(snake_pupil->contour.at(i)->y)),cv::Point((int)(snake_pupil->contour.at(i+1)->x), (int)(snake_pupil->contour.at(i+1)->y)), cv::Scalar(0,105,230));
            }else{
                cv::line(snake_pupil->matrixOfPoints,cv::Point((int)(snake_pupil->contour.at(i)->x), (int)(snake_pupil->contour.at(i)->y)),cv::Point((int)(snake_pupil->contour.at(0)->x), (int)(snake_pupil->contour.at(0)->y)), cv::Scalar(0,105,230));
            }
            out.at<Vec3b>((int)(snake_pupil->contour.at(i)->y), (int)(snake_pupil->contour.at(i)->x))[0] = 255;
            out.at<Vec3b>((int)(snake_pupil->contour.at(i)->y), (int)(snake_pupil->contour.at(i)->x))[1] = 0;
            out.at<Vec3b>((int)(snake_pupil->contour.at(i)->y), (int)(snake_pupil->contour.at(i)->x))[2] = 0;
        }
        break;

    case Snake::Original_with_points:
        //set background
        cvtColor(snake_corona->originalImage, out, CV_GRAY2RGBA);

        //draw corona snake
        for(int i=0; i<snake_corona->contour.size(); i++){
            out.at<Vec3b>((int)(snake_corona->contour.at(i)->y), (int)(snake_corona->contour.at(i)->x))[0] = 50;
            out.at<Vec3b>((int)(snake_corona->contour.at(i)->y), (int)(snake_corona->contour.at(i)->x))[1] = 0;
            out.at<Vec3b>((int)(snake_corona->contour.at(i)->y), (int)(snake_corona->contour.at(i)->x))[2] = 255;
        }
        //draw pupil snake
        for(int i=0; i<snake_pupil->contour.size(); i++){
            out.at<Vec3b>((int)(snake_pupil->contour.at(i)->y), (int)(snake_pupil->contour.at(i)->x))[0] = 50;
            out.at<Vec3b>((int)(snake_pupil->contour.at(i)->y), (int)(snake_pupil->contour.at(i)->x))[1] = 0;
            out.at<Vec3b>((int)(snake_pupil->contour.at(i)->y), (int)(snake_pupil->contour.at(i)->x))[2] = 255;
        }
        break;

    case Snake::Original_with_polygons:
        //set background
        cvtColor(snake_corona->originalImage, out, CV_GRAY2RGBA);

        //set points to polygon of corona snake
        for(int i=0; i<snake_corona->contour.size(); i++){
            countour_polygon[i] = cv::Point((int)(snake_corona->contour.at(i)->x), (int)(snake_corona->contour.at(i)->y));
        }

        //set points to polygon of pupil snake
        for(int i=0; i<snake_pupil->contour.size(); i++){
            pupil_polygon[i] = cv::Point((int)(snake_pupil->contour.at(i)->x), (int)(snake_pupil->contour.at(i)->y));
        }
        //draw polygons first corona then pupil
        cv::fillConvexPoly(out, countour_polygon, snake_corona->contour.size(), cv::Scalar(0,255,0));
        cv::fillConvexPoly(out, pupil_polygon, snake_pupil->contour.size(), cv::Scalar(0,105,230));

        break;
    case Snake::Blank_with_lines:
        //set background
        cvtColor(Mat().zeros(out.rows, out.cols, out.type()), out, CV_GRAY2RGBA);

        //draw corona snake
        for(int i=0; i<snake_corona->contour.size(); i++){
            if(i < (snake_corona->contour.size() - 1) ){
                cv::line(out,cv::Point((int)(snake_corona->contour.at(i)->x), (int)(snake_corona->contour.at(i)->y)),cv::Point((int)(snake_corona->contour.at(i+1)->x), (int)(snake_corona->contour.at(i+1)->y)), cv::Scalar(0,105,230));
            }else{
                cv::line(out,cv::Point((int)(snake_corona->contour.at(i)->x), (int)(snake_corona->contour.at(i)->y)),cv::Point((int)(snake_corona->contour.at(0)->x), (int)(snake_corona->contour.at(0)->y)), cv::Scalar(0,105,230));
            }
            out.at<Vec3b>((int)(snake_corona->contour.at(i)->y), (int)(snake_corona->contour.at(i)->x))[0] = 255;
            out.at<Vec3b>((int)(snake_corona->contour.at(i)->y), (int)(snake_corona->contour.at(i)->x))[1] = 0;
            out.at<Vec3b>((int)(snake_corona->contour.at(i)->y), (int)(snake_corona->contour.at(i)->x))[2] = 0;
        }
        //draw pupil snake
        for(int i=0; i<snake_pupil->contour.size(); i++){
            if(i < (snake_pupil->contour.size() - 1) ){
                cv::line(out,cv::Point((int)(snake_pupil->contour.at(i)->x), (int)(snake_pupil->contour.at(i)->y)),cv::Point((int)(snake_pupil->contour.at(i+1)->x), (int)(snake_pupil->contour.at(i+1)->y)), cv::Scalar(0,105,230));
            }else{
                cv::line(out,cv::Point((int)(snake_pupil->contour.at(i)->x), (int)(snake_pupil->contour.at(i)->y)),cv::Point((int)(snake_pupil->contour.at(0)->x), (int)(snake_pupil->contour.at(0)->y)), cv::Scalar(0,105,230));
            }
            out.at<Vec3b>((int)(snake_pupil->contour.at(i)->y), (int)(snake_pupil->contour.at(i)->x))[0] = 255;
            out.at<Vec3b>((int)(snake_pupil->contour.at(i)->y), (int)(snake_pupil->contour.at(i)->x))[1] = 0;
            out.at<Vec3b>((int)(snake_pupil->contour.at(i)->y), (int)(snake_pupil->contour.at(i)->x))[2] = 0;
        }
        break;

    case Snake::Blank_with_points:
        //set background
        cvtColor(Mat().zeros(out.rows, out.cols, out.type()), out, CV_GRAY2RGBA);

        //draw corona snake
        for(int i=0; i<snake_corona->contour.size(); i++){
            out.at<Vec3b>((int)(snake_corona->contour.at(i)->y), (int)(snake_corona->contour.at(i)->x))[0] = 50;
            out.at<Vec3b>((int)(snake_corona->contour.at(i)->y), (int)(snake_corona->contour.at(i)->x))[1] = 0;
            out.at<Vec3b>((int)(snake_corona->contour.at(i)->y), (int)(snake_corona->contour.at(i)->x))[2] = 255;
        }
        //draw pupil snake
        for(int i=0; i<snake_pupil->contour.size(); i++){
            out.at<Vec3b>((int)(snake_pupil->contour.at(i)->y), (int)(snake_pupil->contour.at(i)->x))[0] = 50;
            out.at<Vec3b>((int)(snake_pupil->contour.at(i)->y), (int)(snake_pupil->contour.at(i)->x))[1] = 0;
            out.at<Vec3b>((int)(snake_pupil->contour.at(i)->y), (int)(snake_pupil->contour.at(i)->x))[2] = 255;
        }
        break;

    case Snake::Blank_with_polygons:
        //set background
        cvtColor(Mat().zeros(out.rows, out.cols, out.type()), out, CV_GRAY2RGBA);

        //set points to polygon of corona snake
        for(int i=0; i<snake_corona->contour.size(); i++){
            countour_polygon[i] = cv::Point((int)(snake_corona->contour.at(i)->x), (int)(snake_corona->contour.at(i)->y));
        }

        //set points to polygon of pupil snake
        for(int i=0; i<snake_pupil->contour.size(); i++){
            pupil_polygon[i] = cv::Point((int)(snake_pupil->contour.at(i)->x), (int)(snake_pupil->contour.at(i)->y));
        }
        //draw polygons first corona then pupil
        cv::fillConvexPoly(out, countour_polygon, snake_corona->contour.size(), cv::Scalar(0,255,0));
        cv::fillConvexPoly(out, pupil_polygon, snake_pupil->contour.size(), cv::Scalar(0,105,230));

        break;
    case Snake::Gradient_with_lines:
        //set background
        cvtColor(snake_corona->vectorField->getConvertedVectorField(0), out, CV_GRAY2RGBA);

        //draw corona snake
        for(int i=0; i<snake_corona->contour.size(); i++){
            if(i < (snake_corona->contour.size() - 1) ){
                cv::line(out,cv::Point((int)(snake_corona->contour.at(i)->x), (int)(snake_corona->contour.at(i)->y)),cv::Point((int)(snake_corona->contour.at(i+1)->x), (int)(snake_corona->contour.at(i+1)->y)), cv::Scalar(0,105,230));
            }else{
                cv::line(out,cv::Point((int)(snake_corona->contour.at(i)->x), (int)(snake_corona->contour.at(i)->y)),cv::Point((int)(snake_corona->contour.at(0)->x), (int)(snake_corona->contour.at(0)->y)), cv::Scalar(0,105,230));
            }
            out.at<Vec3b>((int)(snake_corona->contour.at(i)->y), (int)(snake_corona->contour.at(i)->x))[0] = 255;
            out.at<Vec3b>((int)(snake_corona->contour.at(i)->y), (int)(snake_corona->contour.at(i)->x))[1] = 0;
            out.at<Vec3b>((int)(snake_corona->contour.at(i)->y), (int)(snake_corona->contour.at(i)->x))[2] = 0;
        }
        //draw pupil snake
        for(int i=0; i<snake_pupil->contour.size(); i++){
            if(i < (snake_pupil->contour.size() - 1) ){
                cv::line(out,cv::Point((int)(snake_pupil->contour.at(i)->x), (int)(snake_pupil->contour.at(i)->y)),cv::Point((int)(snake_pupil->contour.at(i+1)->x), (int)(snake_pupil->contour.at(i+1)->y)), cv::Scalar(0,105,230));
            }else{
                cv::line(out,cv::Point((int)(snake_pupil->contour.at(i)->x), (int)(snake_pupil->contour.at(i)->y)),cv::Point((int)(snake_pupil->contour.at(0)->x), (int)(snake_pupil->contour.at(0)->y)), cv::Scalar(0,105,230));
            }
            out.at<Vec3b>((int)(snake_pupil->contour.at(i)->y), (int)(snake_pupil->contour.at(i)->x))[0] = 255;
            out.at<Vec3b>((int)(snake_pupil->contour.at(i)->y), (int)(snake_pupil->contour.at(i)->x))[1] = 0;
            out.at<Vec3b>((int)(snake_pupil->contour.at(i)->y), (int)(snake_pupil->contour.at(i)->x))[2] = 0;
        }
        break;

    case Snake::Gradient_with_points:
        //set background
        cvtColor(snake_corona->vectorField->getConvertedVectorField(0), out, CV_GRAY2RGBA);

        //draw corona snake
        for(int i=0; i<snake_corona->contour.size(); i++){
            out.at<Vec3b>((int)(snake_corona->contour.at(i)->y), (int)(snake_corona->contour.at(i)->x))[0] = 50;
            out.at<Vec3b>((int)(snake_corona->contour.at(i)->y), (int)(snake_corona->contour.at(i)->x))[1] = 0;
            out.at<Vec3b>((int)(snake_corona->contour.at(i)->y), (int)(snake_corona->contour.at(i)->x))[2] = 255;
        }
        //draw pupil snake
        for(int i=0; i<snake_pupil->contour.size(); i++){
            out.at<Vec3b>((int)(snake_pupil->contour.at(i)->y), (int)(snake_pupil->contour.at(i)->x))[0] = 50;
            out.at<Vec3b>((int)(snake_pupil->contour.at(i)->y), (int)(snake_pupil->contour.at(i)->x))[1] = 0;
            out.at<Vec3b>((int)(snake_pupil->contour.at(i)->y), (int)(snake_pupil->contour.at(i)->x))[2] = 255;
        }
        break;

    case Snake::Gradient_with_polygons:
        //set background
        cvtColor(snake_corona->vectorField->getConvertedVectorField(0), out, CV_GRAY2RGBA);

        //set points to polygon of corona snake
        for(int i=0; i<snake_corona->contour.size(); i++){
            countour_polygon[i] = cv::Point((int)(snake_corona->contour.at(i)->x), (int)(snake_corona->contour.at(i)->y));
        }

        //set points to polygon of pupil snake
        for(int i=0; i<snake_pupil->contour.size(); i++){
            pupil_polygon[i] = cv::Point((int)(snake_pupil->contour.at(i)->x), (int)(snake_pupil->contour.at(i)->y));
        }
        //draw polygons first corona then pupil
        cv::fillConvexPoly(out, countour_polygon, snake_corona->contour.size(), cv::Scalar(0,255,0));
        cv::fillConvexPoly(out, pupil_polygon, snake_pupil->contour.size(), cv::Scalar(0,105,230));

        break;
    case Snake::IrisMap:
        //set background
        out = Mat().zeros(out.rows, out.cols, CV_8UC1);

        //set points to polygon of corona snake
        for(int i=0; i<snake_corona->contour.size(); i++){
            countour_polygon[i] = cv::Point((int)(snake_corona->contour.at(i)->x), (int)(snake_corona->contour.at(i)->y));
        }

        //set points to polygon of pupil snake
        for(int i=0; i<snake_pupil->contour.size(); i++){
            pupil_polygon[i] = cv::Point((int)(snake_pupil->contour.at(i)->x), (int)(snake_pupil->contour.at(i)->y));
        }
        //draw polygons first corona then pupil
        cv::fillConvexPoly(out, countour_polygon, snake_corona->contour.size(), cv::Scalar(255));
        cv::fillConvexPoly(out, pupil_polygon, snake_pupil->contour.size(), cv::Scalar(0));

        break;
    default:
        out = Mat().zeros(out.rows, out.cols, out.type());
    }
    return out;
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
                << "Type of Vector Field (OpenCV) =\t" << snake->vectorField->getCV_typeOfVectorField(0) << endl
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
        QMessageBox msg;
        msg.setText("Something went wrong!");
        msg.setInformativeText("Could not open file "+path);
        msg.exec();
    }
    return false;
}




float Snake::fastCenterLocalizationAlgorithm(Mat processedImage, cv::Point *fastCenter, int k){
    Mat sum = processedImage.clone();
    threshold(sum, sum, 60, 0, THRESH_TOZERO);
    threshold(sum, sum, 190, 0, THRESH_TOZERO_INV);
    medianBlur(sum, sum, 3);
    sum+=150;
    threshold(sum, sum, 160, 0, THRESH_TOZERO);
    Mat sumBackup = sum.clone();

    //30% * 30% rectangle in the middle of sum
    int xMax = (int)(sum.cols*0.3)+1;
    int yMax = (int)(sum.rows*0.3)+1;
    int xOffset = (int)(sum.cols*0.35);
    int yOffset = (int)(sum.rows*0.35);

    //2d list finder + multiple chcks if radius is right
    Mat kernel;
    QList<int> x,y;
    for(int kernelSize = k; kernelSize < k+11; kernelSize+=2){
        //reset picture matrix for summing
        sum = sumBackup.clone();
        //set new kernel
        kernel = Mat().ones(kernelSize, kernelSize, sum.type());
        for(int i = 0; i < kernelSize; i++){
            for(int j = 0; j < kernelSize; j++){
                if(j<4){
                    if(i<=(((kernelSize-1)/2)-j)){
                        kernel.at<unsigned char>(j, i)=0;
                    }else if(i>=(kernelSize-((kernelSize-1)/2)+j)){
                        kernel.at<unsigned char>(j, i)=0;
                    }
                }else if(j>kernelSize-((kernelSize-1)/2)-1){
                    if(i<=(kernelSize-1-j)){
                        kernel.at<unsigned char>(j, i)=0;
                    }else if(kernelSize-1+kernelSize-((kernelSize-1)/2)-j){
                        kernel.at<unsigned char>(j, i)=0;
                    }
                }
            }
        }
        kernel.at<float>((((kernelSize-1)/2)), (((kernelSize-1)/2)))=5;
        //apply new kernel
        filter2D(sum, sum, sum.type(), kernel);
        for(int j = yOffset; j < yOffset+yMax; j++){
            for(int i = xOffset; i < xOffset+xMax; i++){

                if(sum.at<unsigned char>(j,i) == 0){
                    x.append(i);
                    y.append(j);
                }
                if(i==xOffset || j == yOffset || i==(xOffset+xMax-1) || j == (yOffset+yMax-1))
                    sum.at<unsigned char>(j,i) =120;
            }
        }
        //uncomment this to see area of searching and vision of matrix sum after using this custom kernel

        //imshow("Sum Matrix", sum);
        //QMessageBox msg;
        //msg.setText("Custom kernel = "+QString().number(kernelSize));
        //msg.exec();
    }

    int probableXY[4]={0, (int)(sum.rows/2), 0, yOffset+yMax};

    for(int i= 0; i<x.size(); i++){
        int countOfAtI = 0;
        qCount(x.begin(), x.end(), x.at(i), countOfAtI);
        if(probableXY[1]<countOfAtI){
            probableXY[0] = x.at(i);
            probableXY[2] = countOfAtI;
        }
        probableXY[1]=y.at(i);
        i+=(countOfAtI-1);
    }

    sum = sumBackup.clone();
    probableXY[2] = probableXY[3];
    for(int j = probableXY[1]; j < probableXY[3]; j++){
        if(sum.at<unsigned char>(j,probableXY[0]) == 0){
            if(sum.at<unsigned char>(j,probableXY[0]) == sum.at<unsigned char>(j+1,probableXY[0])){
                probableXY[2] = j+1;
            }else{
                break;
            }
        }
    }
    //finding of top y coordinate
    bool notFound = true;
    while(notFound){
        if(sum.at<unsigned char>(probableXY[1],probableXY[0]) == sum.at<unsigned char>(probableXY[1]-1,probableXY[0])){
            probableXY[1]--;
        }else{
            notFound = false; //did not found black point above actual => edge of pupil
        }
    }
    //setting y center
    probableXY[1]= probableXY[1]+((probableXY[2] - probableXY[1])/2);
    //correction of x coordinate = find right x and left x then find center between then compate with old one x in probableXY[0]
    int correctionOfX[2] = {probableXY[0],probableXY[0]}; //{old x/ most left x, old x/most right x}
    notFound = true;
    while(notFound){
        if(sum.at<unsigned char>(probableXY[1],correctionOfX[1]) == sum.at<unsigned char>(probableXY[1],correctionOfX[1]+1)){
            correctionOfX[1]++;
        }else{
            notFound = false; //did not found black point at right of actual => edge of pupil
        }
    }
    notFound = true;
    while(notFound){
        if(sum.at<unsigned char>(probableXY[1],correctionOfX[0]) == sum.at<unsigned char>(probableXY[1],correctionOfX[0]-1)){
            correctionOfX[0]--;
        }else{
            notFound = false; //did not found black point at left of actual => edge of pupil
        }
    }
    //if old x not equal new x then set new x as x
    probableXY[0] = ((((correctionOfX[1]-correctionOfX[0])/2)+correctionOfX[0])!=probableXY[0] ? (((correctionOfX[1]-correctionOfX[0])/2)+correctionOfX[0]) : probableXY[0]);

    //    correction of y and setting radius
    float radius = 0;
    float radiusXY[2] = {((correctionOfX[1]-correctionOfX[0])/2),(probableXY[2] - probableXY[1])};
    if(radiusXY[0] < radiusXY[1]){
        //if there is big difference between radiuses then need to correct x, y coordinates
        while((radiusXY[1] - radiusXY[0])>2){
            if(radiusXY[0] < radiusXY[1]){
                radiusXY[1]= radiusXY[0];
                probableXY[1]=probableXY[2]-radiusXY[0];
                correctionOfX[0] = probableXY[0];
                correctionOfX[1] = probableXY[0];
                //int correctionOfX[2] = {probableXY[0],probableXY[0]}; //{old x/ most left x, old x/most right x}
                notFound = true;
                while(notFound){
                    if(sum.at<unsigned char>(probableXY[1],correctionOfX[1]) == sum.at<unsigned char>(probableXY[1],correctionOfX[1]+1)){
                        correctionOfX[1]++;
                    }else{
                        notFound = false; //did not found black point at right of actual => edge of pupil
                    }
                }
                notFound = true;
                while(notFound){
                    if(sum.at<unsigned char>(probableXY[1],correctionOfX[0]) == sum.at<unsigned char>(probableXY[1],correctionOfX[0]-1)){
                        correctionOfX[0]--;
                    }else{
                        notFound = false; //did not found black point at left of actual => edge of pupil
                    }
                }
                //if old x not equal new x then set new x as x
                probableXY[0] = ((((correctionOfX[1]-correctionOfX[0])/2)+correctionOfX[0])!=probableXY[0] ? (((correctionOfX[1]-correctionOfX[0])/2)+correctionOfX[0]) : probableXY[0]);

                probableXY[1]=probableXY[2]-radiusXY[0];
                radiusXY[0] = ((correctionOfX[1]-correctionOfX[0])/2);
            }
        }
        radius = radiusXY[0];
    }else{
        radius = radiusXY[1];
    }

    fastCenter->x = probableXY[0];
    fastCenter->y = probableXY[1];

    return radius;
}


