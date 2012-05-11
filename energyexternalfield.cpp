#include "energyexternalfield.h"

EnergyExternalField::EnergyExternalField()
{
}


EnergyExternalField::EnergyExternalField(Mat inputImage, int energy_ext_type, float deviation, float scale)
{
    this->setVectorField(inputImage);
    this->gausianDeviation = deviation;
    this->sobelScale = scale;
    this->typeOfVectorField = energy_ext_type;
}


void EnergyExternalField::countVectorField(int type, int centerX, int centerY){
    Mat gaus = Mat(this->vectorField.at(0).rows, this->vectorField.at(0).cols, CV_8UC1);
    Mat dx = Mat(this->vectorField.at(0).rows, this->vectorField.at(0).cols, CV_8UC1);
    Mat dy = Mat(this->vectorField.at(0).rows, this->vectorField.at(0).cols, CV_8UC1);
    Mat canny = Mat(this->vectorField.at(0).rows, this->vectorField.at(0).cols, CV_8UC1);
    Mat gradient = Mat(this->vectorField.at(0).rows, this->vectorField.at(0).cols, CV_32FC1);
    Mat gradient_bg = Mat(this->vectorField.at(0).rows, this->vectorField.at(0).cols, CV_32FC1);
     switch(type){
    case EnergyExternalField::GradientMagnitudes:

        //remove noise and scale image for furher range of gradient
        GaussianBlur(this->vectorField.at(0), gaus, Size(5, 5), this->gausianDeviation, 0);
        Canny(gaus, canny, 30, 90, 5, false);
        this->vectorField.insert(0, gaus.clone());

        Sobel(gaus, dx,  CV_8UC1, 1, 0, 3, this->sobelScale);
        Sobel(gaus, dy,  CV_8UC1, 0, 1, 3, this->sobelScale);
    medianBlur(dx, dx, 7);medianBlur(dx, dx, 3);
        medianBlur(dy, dy, 7);medianBlur(dy, dy, 3);

        for(int j = 0; j < gradient.rows; j++)
            for(int i = 0; i < gradient.cols; i++){
                gradient.at<float>(j,i) = sqrt( pow(dx.at<unsigned char>(j,i),2) + pow(dy.at<unsigned char>(j,i),2) );
                 float dist = (abs(i-centerX+1)+abs(j-centerY+1))/2;
                 if(dist < 10){
                     dist = 500;
                 }
                 if(gradient.at<float>(j,i) < 70){//70-good
                     gradient.at<float>(j,i) =0;

                }else if(canny.at<unsigned char>(j,i) == 255){
                    gradient.at<float>(j,i) = 255;
                }
                 if(dist > 150)
                    ;
            }


        this->vectorField.insert(0, gradient);

        this->vectorField.insert(1, dx);
        this->vectorField.insert(2, dy);

        break;
    case EnergyExternalField::GradientMagnitudes_pupil:

        //remove noise and scale image for further range of gradient
        GaussianBlur(this->vectorField.at(0), gaus, Size(5, 5), this->gausianDeviation, 0);
        Canny(gaus, canny, 30, 90, 5, false);
        this->vectorField.insert(0, gaus.clone());
        //derivates of matrices by sobel operator
        Sobel(gaus, dx,  CV_8UC1, 1, 0, 3, this->sobelScale);
        Sobel(gaus, dy,  CV_8UC1, 0, 1, 3, this->sobelScale);
        medianBlur(dx, dx, 7);medianBlur(dx, dx, 3);
        medianBlur(dy, dy, 7);medianBlur(dy, dy, 3);


///////////////////////////////////////////////////////////////
        //set zeros to gradient vector addition
        gradient_bg = Mat().zeros(gradient_bg.rows, gradient_bg.cols, gradient_bg.type());

        //counting gradient vector field from gradient maximas
        for(int j = 0; j < gradient_bg.rows; j++){
            for(int i = 0; i < gradient_bg.cols; i++){
                if(70 >= sqrt( pow(dx.at<unsigned char>(j,i),2) + pow(dy.at<unsigned char>(j,i),2) )
                        &&
                    canny.at<unsigned char>(j,i) == 255){

                    //when canny found then it is 255 = maximal
                    gradient_bg.at<float>(j,i) += 255;

                    //count distances from borderso of matrix
                    int dist_l = i;
                    int dist_r =  abs(i-gradient.cols+1);
                    int dist_u = j;
                    int dist_d = abs(j-gradient.rows+1);
                    //l, r, d, u == left, right, up, down

                    //in four direction make gradient pointing to this point
                    //short distance term
                    for(int d = 3; d > 0; d--){
                        if(d<=dist_l)
                            gradient.at<float>(j,i-d) += (150 /d);
                        if(d<=dist_r)
                            gradient.at<float>(j,i+d) += (150 /d);
                        if(d<=dist_u)
                            gradient.at<float>(j-d,i) += (150 /d);
                        if(d<=dist_d)
                            gradient.at<float>(j+d,i) += (150 /d);
                    }
                    //mid distance term
                    for(int d = 7; d > 0; d--){
                        if(d<=dist_l)
                            gradient.at<float>(j,i-d) += (50 /d);
                        if(d<=dist_r)
                            gradient.at<float>(j,i+d) += (50 /d);
                        if(d<=dist_u)
                            gradient.at<float>(j-d,i) += (50 /d);
                        if(d<=dist_d)
                            gradient.at<float>(j+d,i) += (50 /d);
                    }
                    //long distance term
                    for(int d = dist_l; d > 0; d--){
                        gradient.at<float>(j,i-d) += (16 / d);
                    }
                    for(int d = dist_r; d > 0; d--){
                        gradient.at<float>(j,i+d) += (16 / d);
                    }
                    for(int d = dist_u; d > 0; d--){
                        gradient.at<float>(j-d,i) += (16 / d);
                    }
                    for(int d = dist_d; d > 0; d--){
                        gradient.at<float>(j+d,i) += (16 / d);
                    }
                };
            }
        }
      if(true){
            double maxGradient_bg = 0;
            cv::minMaxIdx(gradient_bg, 0, &maxGradient_bg);
            gradient_bg = ((255 * gradient_bg) / maxGradient_bg);

        }
//////////////////////////////////////////////

        for(int j = 0; j < gradient.rows; j++)
            for(int i = 0; i < gradient.cols; i++){
                gradient.at<float>(j,i) = sqrt( pow(dx.at<unsigned char>(j,i),2) + pow(dy.at<unsigned char>(j,i),2) );
                 float dist = (abs(i-centerX+1)+abs(j-centerY+1))/2;
                 if(dist < 10){
                     dist = 500;
                 }
                 if(gradient.at<float>(j,i) < 70){

                     gradient.at<float>(j,i) = 0;

                }else if(canny.at<unsigned char>(j,i) == 255){
                    gradient.at<float>(j,i) = 255;
                }
                 if(dist > 150)
                    ;
            }

        this->vectorField.insert(0, gradient);
        this->vectorField.insert(1, dx);
        this->vectorField.insert(2, dy);
       break;

    case EnergyExternalField::GradientMagnitudes_corona:
        //input of corona is not the original but modified image then it is the same as pupil gradient
        //center of pupil is used the same as in pupil therefore is needed to be handled outside
        gradient_bg = this->vectorField.at(0).clone();
        threshold(gradient_bg, gradient_bg, 50, 0, THRESH_TOZERO);
        threshold(gradient_bg, gradient_bg, 195, 0, THRESH_TOZERO_INV);
        gradient_bg+=170;
        threshold(gradient_bg, gradient_bg, 250, 0, THRESH_TOZERO);
        medianBlur(gradient_bg, gradient_bg, 5);
        medianBlur(gradient_bg, gradient_bg, 3);

        //remove noise and scale image for further range of gradient
        GaussianBlur(gradient_bg, gaus, Size(5, 5), this->gausianDeviation, 0);
        Canny(gaus, canny, 30, 90, 5, false);
        //derivates of matrices by sobel operator
        Sobel(gaus, dx,  CV_8UC1, 1, 0, 3, this->sobelScale);
        Sobel(gaus, dy,  CV_8UC1, 0, 1, 3, this->sobelScale);
       medianBlur(dx, dx, 7);medianBlur(dx, dx, 3);
        medianBlur(dy, dy, 7);medianBlur(dy, dy, 3);

     gradient_bg = Mat().zeros(gradient_bg.rows, gradient_bg.cols, gradient_bg.type());

        //counting gradient vector field from gradient maximas
        for(int j = 0; j < gradient_bg.rows; j++){
            for(int i = 0; i < gradient_bg.cols; i++){
                if(70 >= sqrt( pow(dx.at<unsigned char>(j,i),2) + pow(dy.at<unsigned char>(j,i),2) )
                        &&
                    canny.at<unsigned char>(j,i) == 255){

                    //when canny found then it is 255 = maximal
                    gradient_bg.at<float>(j,i) += 255;

                    //count distances from borderso of matrix
                    int dist_l = i;
                    int dist_r =  abs(i-gradient.cols+1);
                    int dist_u = j;
                    int dist_d = abs(j-gradient.rows+1);
                    //l, r, d, u == left, right, up, down

                    //in four direction make gradient pointing to this point
                    //short distance term
                    for(int d = 3; d > 0; d--){
                        if(d<=dist_l)
                            gradient.at<float>(j,i-d) += (150 /d);
                        if(d<=dist_r)
                            gradient.at<float>(j,i+d) += (150 /d);
                        if(d<=dist_u)
                            gradient.at<float>(j-d,i) += (150 /d);
                        if(d<=dist_d)
                            gradient.at<float>(j+d,i) += (150 /d);
                    }
                    //mid distance term
                    for(int d = 7; d > 0; d--){
                        if(d<=dist_l)
                            gradient.at<float>(j,i-d) += (50 /d);
                        if(d<=dist_r)
                            gradient.at<float>(j,i+d) += (50 /d);
                        if(d<=dist_u)
                            gradient.at<float>(j-d,i) += (50 /d);
                        if(d<=dist_d)
                            gradient.at<float>(j+d,i) += (50 /d);
                    }
                    //long distance term
                    for(int d = dist_l; d > 0; d--){
                        gradient.at<float>(j,i-d) += (16 / d);
                    }
                    for(int d = dist_r; d > 0; d--){
                        gradient.at<float>(j,i+d) += (16 / d);
                    }
                    for(int d = dist_u; d > 0; d--){
                        gradient.at<float>(j-d,i) += (16 / d);
                    }
                    for(int d = dist_d; d > 0; d--){
                        gradient.at<float>(j+d,i) += (16 / d);
                    }
                };
            }
        }
        if(true){
            double maxGradient_bg = 0;
            cv::minMaxIdx(gradient_bg, 0, &maxGradient_bg);
            gradient_bg = ((255 * gradient_bg) / maxGradient_bg);

        }
       for(int j = 0; j < gradient.rows; j++)
            for(int i = 0; i < gradient.cols; i++){
                gradient.at<float>(j,i) = sqrt( pow(dx.at<unsigned char>(j,i),2) + pow(dy.at<unsigned char>(j,i),2) );

                if(gradient.at<float>(j,i) < 70){
                    gradient.at<float>(j,i) = gradient_bg.at<float>(j,i);
                }else if(canny.at<unsigned char>(j,i) == 255){
                    gradient.at<float>(j,i) = 255;
                }
            }

        this->vectorField.insert(0, gradient);
        this->vectorField.insert(1, dx);
        this->vectorField.insert(2, dy);
        this->vectorField.insert(3, gradient_bg);
        break;
        gaus.release();
        dx.release();
        dy.release();
        canny.release();
        gradient.release();
        gradient_bg.release();

    default:;
    }
}

Mat EnergyExternalField::getNeighborhoodExtE(int x, int y, int step, int at){
    Mat temp = Mat(((step * 2) + 1 ), ((step * 2) + 1 ), CV_32FC1);
    int index_neighbor_x = 0;
    int index_neighbor_y = 0;
    if(this->vectorField.at(at).type() == CV_8UC1){
        for (int actual_x = (x - step); actual_x <= (x + step); actual_x++)
        {
            index_neighbor_y=0;
            for (int actual_y = (y - step); actual_y <= (y + step); actual_y++)
            {
                if( actual_x < 0 || actual_x >= this->vectorField.at(at).cols
                        || actual_y < 0 || actual_y >= this->vectorField.at(at).rows)
                {
                    temp.at<float>(index_neighbor_y, index_neighbor_x) = 0;
                }else{
                    temp.at<float>(index_neighbor_y, index_neighbor_x)= this->vectorField.at(at).at<unsigned char>(actual_y, actual_x);
                }

                index_neighbor_y++;

            }
            index_neighbor_x++;
        }
    }else if (this->vectorField.at(at).type() == CV_32FC1){
        for (int actual_x = (x - step); actual_x <= (x + step); actual_x++)
        {
            index_neighbor_y=0;
            for (int actual_y = (y - step); actual_y <= (y + step); actual_y++)
            {
                if( actual_x < 0 || actual_x >= this->vectorField.at(at).cols
                        || actual_y < 0 || actual_y >= this->vectorField.at(at).rows)
                {
                    temp.at<float>(index_neighbor_y, index_neighbor_x) = 0;
                }else{
                    temp.at<float>(index_neighbor_y, index_neighbor_x)= (unsigned char)this->vectorField.at(at).at<float>(actual_y, actual_x);
                }
                index_neighbor_y++;

            }
            index_neighbor_x++;
        }
    }
    return temp;
}


void EnergyExternalField::scaleConvertMat(Mat in, Mat out){
    //this function converts one matrix type to other with proportialy scaling colours
    if(out.type() == CV_32FC1 && in.type() == CV_8UC1){
        for(int i = 0; i < in.rows; i++){
            for(int j = 0; j < in.cols; j++){
                out.at<float>(i,j) = in.at<unsigned char>(i,j);
            }
        }
        out = out/255;
    }else if(out.type() == CV_8UC1 && in.type() == CV_32FC1){
        for(int i = 0; i < in.rows; i++){
            for(int j = 0; j < in.cols; j++){
                out.at<unsigned char>(i,j) = in.at<float>(i,j) * 255;
            }
        }

    }else{
        out = in.clone();
    }
}

Mat EnergyExternalField::getScaledVectorField(int at){
    //only for CV_32F
    //this function scales all values in field so no will be above maximum 1 that coresponds to CV_32F)
    Mat temp = Mat(this->vectorField.at(at).rows, this->vectorField.at(at).cols, CV_32FC1);
    double min, max;
    cv::minMaxLoc(this->vectorField.at(at), &min, &max);
    if (max > 1){
        temp = this->vectorField.at(at)/ max;
    }
    return temp;
}

Mat EnergyExternalField::getConvertedVectorField(int at){
    //only for CV_32F
    //this function scales all values in field so no will be above maximum 1 that coresponds to CV_32F)
    Mat temp = Mat(this->vectorField.at(at).rows, this->vectorField.at(at).cols, CV_8UC1);
    for(int i = 0; i < temp.rows; i++){
        for(int j = 0; j < temp.cols; j++){
            temp.at<unsigned char>(i,j) = (unsigned char)(this->vectorField.at(at).at<float>(i,j) ); //* 255
        }
    }
    return temp;
}

void EnergyExternalField::scaleVectorField(Mat in, Mat out){
    //only for CV_32F
    //this function scales all values in field so no will be above maximum 1 that coresponds to CV_32F1
    double max;
    double min;
    cv::minMaxLoc(in, &min, &max);
    if (max > 1){
        out = in / max;
    }else{
        out = in.clone();
    }
}


bool EnergyExternalField::saveMatToTextFile(Mat output, QString path)
{
    if(path == ""){
        QWidget *dialog = new QWidget();
        path = QFileDialog::getSaveFileName(dialog,  "Save Matrix to text file" , QDir::currentPath(), "Text files(*.txt)");
    }
    QFile outputFile(path);
    if(outputFile.open(QIODevice::WriteOnly | QFile::Text)){
        QTextStream outText(&outputFile);
        if(output.type() == CV_8UC1){
            outText << "Matrix of type "<< output.type() <<  endl
                    << "---"<< endl
                    << output.data << output.datastart << output.dataend << endl
                    << "---"<< endl ;
            for(int i = 0; i < output.rows; i++){
                outText <<  endl;
                for(int j = 0; j < output.cols; j++){
                    outText << output.at<unsigned char>(i,j) <<  "\t";
                }
            }
        }else if(output.type() == CV_32FC1){
            outText << "Matrix of type " << output.type() <<  endl
                    << "---"<< endl
                    << output.data << output.datastart << output.dataend << endl
                    << "---"<< endl ;
            for(int i = 0; i < output.rows; i++){
                outText <<  endl;
                for(int j = 0; j < output.cols; j++){
                    outText << output.at<float>(i,j) <<  "\t";
                }
            }
        }
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



















