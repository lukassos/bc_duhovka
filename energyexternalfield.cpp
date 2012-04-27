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


void EnergyExternalField::countVectorField(int type){
    Mat gaus = Mat(this->vectorField.at(0).rows, this->vectorField.at(0).cols, CV_8UC1);
    Mat dx = Mat(this->vectorField.at(0).rows, this->vectorField.at(0).cols, CV_8UC1);
    Mat dy = Mat(this->vectorField.at(0).rows, this->vectorField.at(0).cols, CV_8UC1);
    Mat gradient = Mat(this->vectorField.at(0).rows, this->vectorField.at(0).cols, CV_32FC1);;

    //this->vectorField.at(0).convertTo(gaus, gaus.depth(), 1, 15);
    //scaleConvertMat(this->vectorField.at(0), gaus);

    //imwrite("C:\\Users\\lukassos\\Documents\\kodenie\\duhovecka-build-desktop-Qt_4_7_4_for_Desktop_-_MinGW_4_4__Qt_SDK__Debug\\_debugimg\\input_origina.png",this->vectorField.at(0));
    //imwrite("C:\\Users\\lukassos\\Documents\\kodenie\\duhovecka-build-desktop-Qt_4_7_4_for_Desktop_-_MinGW_4_4__Qt_SDK__Debug\\_debugimg\\input_scaled.png",gaus);
    //cv::convertScaleAbs(this->vectorField.at(0),  gaus, 1, 15); did not work - uses the same type

    cv::imshow("Input for E EXT counting", this->vectorField.at(0));

    //cv::imshow("EXTERNAL first gausian", gaus);

    //saveMatToTextFile(this->vectorField.at(0), "C:\\Users\\lukassos\\Documents\\kodenie\\duhovecka-build-desktop-Qt_4_7_4_for_Desktop_-_MinGW_4_4__Qt_SDK__Debug\\_debugimg\\input_origina.txt");
    //saveMatToTextFile(gaus, "C:\\Users\\lukassos\\Documents\\kodenie\\duhovecka-build-desktop-Qt_4_7_4_for_Desktop_-_MinGW_4_4__Qt_SDK__Debug\\_debugimg\\input_scaled.txt");
    switch(type){
    case EnergyExternalField::GradientMagnitudes:

        //remove noise and scale image for furher range of gradient
        GaussianBlur(this->vectorField.at(0), gaus, Size(7, 7), this->gausianDeviation, 0);
        saveMatToTextFile(gaus, "C:\\Users\\lukassos\\Documents\\kodenie\\duhovecka-build-desktop-Qt_4_7_4_for_Desktop_-_MinGW_4_4__Qt_SDK__Debug\\_debugimg\\gausianBlur.txt");
        this->vectorField.insert(0, gaus.clone());
        cv::imshow("EXTERNAL GAUSIAN", gaus);
        //medianBlur(gaus, gaus, 3);
        //imwrite("C:\\Users\\lukassos\\Documents\\kodenie\\duhovecka-build-desktop-Qt_4_7_4_for_Desktop_-_MinGW_4_4__Qt_SDK__Debug\\_debugimg\\gausianBlur.png",gaus);
        //derivates of matrices by sobel operator
        //Laplacian(gaus, gradient, gradient.depth(), 3, this->sobelScale);

        Sobel(gaus, dx,  CV_8UC1, 1, 0, 3, this->sobelScale);
        Sobel(gaus, dy,  CV_8UC1, 0, 1, 3, this->sobelScale);
        cv::imshow("EXTERNAL sobel DX", dx);
        cv::imshow("EXTERNAL sobel DY", dy);
        //gradient magnitude = sqrt(power_2(derivation by x) + power_2(derivation by y))
        //        for(int i = 0; i < this->vectorField.rows; i++)
        //            for(int j = 0; j < this->vectorField.cols; j++){
        //                this->vectorField.at<Vec3b>(i,j)[0] = dx.at<float>(i,j);
        //                this->vectorField.at<Vec3b>(i,j)[1] = dy.at<float>(i,j);
        //                this->vectorField.at<Vec3b>(i,j)[2] = dx.at<float>(i,j) + dy.at<float>(i,j);
        //                this->vectorField.at<Vec3b>(i,j)[2] = sqrt(sqr(dx.at<float>(i,j)) + sqr(dy.at<float>(i,j))) ;
        //            }

        saveMatToTextFile(dx, "C:\\Users\\lukassos\\Documents\\kodenie\\duhovecka-build-desktop-Qt_4_7_4_for_Desktop_-_MinGW_4_4__Qt_SDK__Debug\\_debugimg\\dx_scale_0.01.txt");
        //imwrite("C:\\Users\\lukassos\\Documents\\kodenie\\duhovecka-build-desktop-Qt_4_7_4_for_Desktop_-_MinGW_4_4__Qt_SDK__Debug\\_debugimg\\dx_scale_0.01.png",dx);
        saveMatToTextFile(dy, "C:\\Users\\lukassos\\Documents\\kodenie\\duhovecka-build-desktop-Qt_4_7_4_for_Desktop_-_MinGW_4_4__Qt_SDK__Debug\\_debugimg\\dy_scale_0.01.txt");
        //imwrite("C:\\Users\\lukassos\\Documents\\kodenie\\duhovecka-build-desktop-Qt_4_7_4_for_Desktop_-_MinGW_4_4__Qt_SDK__Debug\\_debugimg\\dx_scale_0.01.png",dy);
        cv::pow(dx, 2, dx);
        cv::pow(dy, 2, dy);

        //saveMatToTextFile(dx, "C:\\Users\\lukassos\\Documents\\kodenie\\duhovecka-build-desktop-Qt_4_7_4_for_Desktop_-_MinGW_4_4__Qt_SDK__Debug\\_debugimg\\dx_pow_2.txt");
        //imwrite("C:\\Users\\lukassos\\Documents\\kodenie\\duhovecka-build-desktop-Qt_4_7_4_for_Desktop_-_MinGW_4_4__Qt_SDK__Debug\\_debugimg\\dx_pow_2.png",dx);
        //saveMatToTextFile(dy, "C:\\Users\\lukassos\\Documents\\kodenie\\duhovecka-build-desktop-Qt_4_7_4_for_Desktop_-_MinGW_4_4__Qt_SDK__Debug\\_debugimg\\dy_pow_2.txt");
        //imwrite("C:\\Users\\lukassos\\Documents\\kodenie\\duhovecka-build-desktop-Qt_4_7_4_for_Desktop_-_MinGW_4_4__Qt_SDK__Debug\\_debugimg\\dy_pow_2.png",dy);
        gradient = dx + dy;
        saveMatToTextFile(gradient, "C:\\Users\\lukassos\\Documents\\kodenie\\duhovecka-build-desktop-Qt_4_7_4_for_Desktop_-_MinGW_4_4__Qt_SDK__Debug\\_debugimg\\dx_dy_sum.txt");
        imwrite("C:\\Users\\lukassos\\Documents\\kodenie\\duhovecka-build-desktop-Qt_4_7_4_for_Desktop_-_MinGW_4_4__Qt_SDK__Debug\\_debugimg\\dx_dy_sum.png",gradient);

        //cv::sqrt(gradient, gradient);
        //scaleVectorField(gaus);


        imshow("gradient field", gradient);
        //imwrite("C:\\Users\\lukassos\\Documents\\kodenie\\duhovecka-build-desktop-Qt_4_7_4_for_Desktop_-_MinGW_4_4__Qt_SDK__Debug\\_debugimg\\gradient.png",gaus);
        saveMatToTextFile(gradient, "C:\\Users\\lukassos\\Documents\\kodenie\\duhovecka-build-desktop-Qt_4_7_4_for_Desktop_-_MinGW_4_4__Qt_SDK__Debug\\_debugimg\\gradient.txt");

        //                for(int i = 0; i < this->vectorField.rows; i++)
        //                    for(int j = 0; j < this->vectorField.cols; j++){
        //                        this->vectorField.at<Vec3b>(i,j)[0] = dx.at<float>(i,j);
        //                        this->vectorField.at<Vec3b>(i,j)[1] = dy.at<float>(i,j);
        //                        this->vectorField.at<Vec3b>(i,j)[2] = gaus.at<float>(i,j);
        //                    }
        this->vectorField.insert(0, gradient);
        this->vectorField.insert(1, dx);
        this->vectorField.insert(2, dy);
        //        || equals ||
        //        cv::magnitude(this->vectorField[0],this->vectorField[1],this->vectorField.channels()[3])

        break;
    default:;
    }
}

Mat EnergyExternalField::getNeighborhoodExtE(int x, int y, int step, int at){
    Mat temp = Mat(((step * 2) + 1 ), ((step * 2) + 1 ), CV_8UC1);
    int index_neighbor_x = 0;
    int index_neighbor_y = 0;

    for (int actual_x = (x - step); actual_x <= (x + step); actual_x++)
    {
        index_neighbor_y=0;
        for (int actual_y = (y - step); actual_y <= (y + step); actual_y++)
        {
            temp.at<unsigned char>(actual_y, actual_x)= this->vectorField.at(at).at<unsigned char>(actual_y, actual_x);
            index_neighbor_y++;
        }
        index_neighbor_x++;
    }
    saveMatToTextFile(temp, "C:\\Users\\lukassos\\Documents\\kodenie\\duhovecka-build-desktop-Qt_4_7_4_for_Desktop_-_MinGW_4_4__Qt_SDK__Debug\\_debugimg\\moving_beforeextE.txt");
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
            temp.at<unsigned char>(i,j) = (unsigned char)(this->vectorField.at(at).at<float>(i,j) * 255);
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



















