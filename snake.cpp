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
                             int baseStep,
                             int fastInitKernel)
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
    float centerX;
    float centerY;
    float radius;
    snake->showImage = snake->originalImage.clone();
    switch(energy_int_type){
    case EnergyInternalTemplate::ClosedContour_Circle:
        //finds center of pupil and its radius
        radius = snake->fastCenterLocalizationAlgorithm(snake->originalImage, fastCenter, fastInitKernel);
        centerX = fastCenter->x + offsetX;
        centerY = fastCenter->y + offsetY;
        //set coordinates to circle
        snake->setCirclePositions(snake->contour,  centerX, centerY, radius, snake->getImageOriginal().cols, snake->getImageOriginal().rows);

        EnergyInternalTemplate().countTotalEnergyInt(snake);
        break;

    default:;
    }
    //creation of external energy and control class
    initSnakeExtField(snake, energy_ext_type, deviation);
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
        snake->contour.at(i)->E_ext=snake->vectorField->getValueFromVectorField(0, snake->contour.at(i)->x,snake->contour.at(i)->y);
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
    //saveSnakeToTextFile(snake);
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
                                actual_local_ext_E = snake->vectorField->getValueFromVectorField( 0, actual_x, actual_y)/255;
                                actual_local_int_E = EnergyInternalTemplate().countLocalEnergyInt(*snake, i, actual_x, actual_y);
                            }
//                            //if not found better point before and can move to another point with same energy
//                            if(!foundBetter){
//                                if(abs(best_local_int_E - best_local_ext_E) >= abs(actual_local_int_E - actual_local_ext_E)){
//                                    best_x = actual_x;
//                                    best_y = actual_y;
//                                    best_local_ext_E = actual_local_ext_E;
//                                    best_local_int_E = actual_local_int_E;
//                                    foundBetter = true;
//                                }
//                            //if found better point before then can move to another point only with lower energy
//                            }else{
                                if(abs(best_local_int_E - best_local_ext_E) > abs(actual_local_int_E - actual_local_ext_E)){
                                    best_x = actual_x;
                                    best_y = actual_y;
                                    best_local_ext_E = actual_local_ext_E;
                                    best_local_int_E = actual_local_int_E;
                                    foundBetter = true;
                                }
//                            }
                        }
                    }
                }
                if(foundBetter){
                    snake->total_E_int -= snake->contour.at(i)->E_ext;
                    snake->contour.at(i)->x = best_x;
                    snake->contour.at(i)->y = best_y;
                    snake->contour.at(i)->E_ext = best_local_ext_E;
                    snake->contour.at(i)->E_int = best_local_int_E;
                    snake->contour.at(i)->E_ext = snake->vectorField->getValueFromVectorField(0, best_x, best_y);
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
    showMatrix(snake);
    //saveSnakeToTextFile(snake);
}


void Snake::showMatrix(Snake *snake){
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
            snake->matrixOfPoints.at<Vec3b>((int)(snake->contour.at(i)->y), (int)(snake->contour.at(i)->x))[0] = 0;//255 - imageToShow.at<float>(snake->contour.at(i)->y, snake->contour.at(i)->x);
            snake->matrixOfPoints.at<Vec3b>((int)(snake->contour.at(i)->y), (int)(snake->contour.at(i)->x))[1] = 255;
            snake->matrixOfPoints.at<Vec3b>((int)(snake->contour.at(i)->y), (int)(snake->contour.at(i)->x))[2] = 255;
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


