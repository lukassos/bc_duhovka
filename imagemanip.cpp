#include "imagemanip.h"

ImageManip::ImageManip(QImage *image)
{
    matTemp = bindIplImage2Mat(qImage2IplImage(*image));
    for (int i=0; i<20; i++)
        flags[i] = false;
}

ImageManip::~ImageManip()
{

}
    //treba spravit vypis *Mat do okna huigui
    //treba spravit obdobu tejto funkcie pre kazdy operato vracat bude *Mat
//void ImageManip::showImgThoughOpenCV(QString pathToImage)
//{
//    //destroyAllCVWindows();
//    while(pathToImage.isEmpty()){
//        pathToImage = QFileDialog::getOpenFileName(this, tr("Open File"), QDir::currentPath());
//        imagePath = pathToImage;
//    }
//    Mat image = imread(pathToImage.toStdString(),1);
//    Mat image_greyscaled = imread(pathToImage.toStdString(),0);
//    //    namedWindow("My Image");
//    createCVWindow("My Image");
//    createCVWindow("My Grey Image");
//    imshow("My Image", image);
//    //imwrite();
//    imshow("My Grey Image", image_greyscaled);
//    createCVWindow("Blur + Canny edge detection");
//    //tempMat = image_greyscaled;
//    imshow("Blur + Canny edge detection", image_greyscaled);
//    blur(image_greyscaled, tempMat, Size(3,3));
//    Mat blured = tempMat;
//    createCVWindow("Blur");
//    imshow("Blur", blured);
//    //createTrackbar("Canny threshold", "Blur + Canny edge detection", &tempInt, 100, NULL);
//    // Show the image
//    //onTrackbar(0, 0);
//    Canny(tempMat, edgeMat, ui->sliderValue->value(), ui->sliderValue->value()*3, 3);
//    imshow("Blur + Canny edge detection", edgeMat);
//    waitKey(-1);
//}

Mat* ImageManip::bindIplImage2Mat(const IplImage *iplImage)
{
    Mat *newMat = new Mat;
    *newMat = cvarrToMat(iplImage);
    //    ak sa data nekopiruju zdielaju sa z povodnym obrazkom a tym sa da vyhnut konverzii spat
    //    *newMat = cvarrToMat(iplImage).clone();
    return newMat;
}

//tato konverzia je zbytocne zlozita na vykonavanie real time
//IplImage* MainWindow:: Mat2IplImage(const Mat *mat)
//{
//    IplImage *newIplImage = new IplImage;
//    *mat->
//    *newIplImage =
//    return newIplImage;
//}

IplImage* ImageManip::qImage2IplImage(const QImage& qImage)
{
    int width = qImage.width();
    int height = qImage.height();

    // Creates a iplImage with 3 channels
    IplImage *img = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 3);
    char * imgBuffer = img->imageData;

    //Remove alpha channel
    int jump = (qImage.hasAlphaChannel()) ? 4 : 3;

    for (int y=0;y<img->height;y++){
        QByteArray a((const char*)qImage.scanLine(y), qImage.bytesPerLine());
        for (int i=0; i<a.size(); i+=jump){
            //Swap from RGB to BGR
            imgBuffer[2] = a[i];
            imgBuffer[1] = a[i+1];
            imgBuffer[0] = a[i+2];
            imgBuffer+=3;
        }
    }

    return img;
}

//conversion from iplimage

//unsigned char* qImageBuffer = (unsigned char*)(ipl_img->imageData);
//QImage* temp = new QImage((const unsigned char*)qImageBuffer, ipl_img->width, ipl_img->height, ipl_img->widthStep, QImage::Format_RGB888);
//QPixmap TEMP_pixmap;
//TEMP_pixmap.convertFromImage( *temp );
//this->setPixmap( TEMP_pixmap );
QImage ImageManip::IplImage2QImage(const IplImage *iplImage)
{
    int height = iplImage->height;
    int width = iplImage->width;

    if  (iplImage->depth == IPL_DEPTH_8U && iplImage->nChannels == 3)
    {
        const uchar *qImageBuffer = (const uchar*)iplImage->imageData;
        QImage img(qImageBuffer, width, height, QImage::Format_RGB888);
        return img.rgbSwapped();
    } else if  (iplImage->depth == IPL_DEPTH_8U && iplImage->nChannels == 1){
        const uchar *qImageBuffer = (const uchar*)iplImage->imageData;
        QImage img(qImageBuffer, width, height, QImage::Format_Indexed8);

        QVector<QRgb> colorTable;
        for (int i = 0; i < 256; i++){
            colorTable.push_back(qRgb(i, i, i));
        }
        img.setColorTable(colorTable);
        return img;
    }else{
        qWarning() << "Image cannot be converted.";
        return QImage();
    }
}


void ImageManip::onTrackbar(int, void*) {
    blur(tempMat, tempMat, Size(3,3));
    // Run the edge detector on grayscale
    Canny(tempMat, tempMat, tempInt, tempInt*3, 3);
    //    Mat edge = Scalar::all(0);
    Mat edge;
    Mat().copyTo(edge, tempMat);
    imshow("Blur + Canny edge detection", edge);
}

int ImageManip::toGrayscale(Mat processedImage)
{
    QElapsedTimer algTime;
    algTime.start();
    cvtColor(processedImage, processedImage, CV_RGB2GRAY);
    return algTime.elapsed();
}

int ImageManip::doBlur(Mat processedImage, int size)
{
    QElapsedTimer algTime;
    algTime.start();
    //Mat temp1, temp2;
    //temp1 = processedImage;

    blur(processedImage, processedImage, Size(size,size));
    //processedImage = temp2;
    return algTime.elapsed();
}

int ImageManip::doMedianBlur(Mat processedImage, int size)
{
    QElapsedTimer algTime;
    algTime.start();
    medianBlur(processedImage, processedImage, size);
    return algTime.elapsed();
}

int ImageManip::doGausianBlur(Mat processedImage, int size, double sigma1, double sigma2)
{
    QElapsedTimer algTime;
    algTime.start();
    GaussianBlur(processedImage, processedImage, Size(size,size), sigma1, sigma2);
    return algTime.elapsed();
}

int ImageManip::doGausianBlur(Mat processedImage, int size)
{
    QElapsedTimer algTime;
    algTime.start();
    GaussianBlur(processedImage, processedImage, Size(size,size), 0, 0);
    return algTime.elapsed();
}

int ImageManip::doLaplacian(Mat processedImage, int ksize, double scale, double delta)
{
    QElapsedTimer algTime;
    algTime.start();
    Laplacian(processedImage, processedImage, CV_8U, ksize, scale, delta, BORDER_DEFAULT);
    return algTime.elapsed();
}

int ImageManip::doSobel(Mat processedImage, int dx, int dy, int ksize, double scale, double delta )
{
    QElapsedTimer algTime;
    algTime.start();
    Sobel(processedImage, processedImage, CV_8U, dx, dy, ksize, scale, delta, BORDER_DEFAULT);
    return algTime.elapsed();
}

int ImageManip::doScharr(Mat processedImage, int dx, int dy, double scale, double delta )
{
    QElapsedTimer algTime;
    algTime.start();
    Scharr(processedImage, processedImage, CV_8U, dx, dy, scale, delta, BORDER_DEFAULT);
    return algTime.elapsed();
}

int ImageManip::doThresholdUnder(Mat processedImage, int threshVal)
{
    QElapsedTimer algTime;
    algTime.start();
    threshold(processedImage, processedImage, threshVal, 0, THRESH_TOZERO);
    return algTime.elapsed();
}

int ImageManip::doThresholdAbove(Mat processedImage, int threshVal)
{
    QElapsedTimer algTime;
    algTime.start();
    threshold(processedImage, processedImage, threshVal, 0, THRESH_TOZERO_INV);
    return algTime.elapsed();
}

int ImageManip::doCanny(Mat processedImage, int threshLower, int threshUpper, int size, bool l2grad)
{
    QElapsedTimer algTime;
    algTime.start();
    Canny(processedImage, processedImage, threshLower, threshUpper, 3, l2grad);
    return algTime.elapsed();
}

int ImageManip::doCornerHarris(Mat processedImage, int blockSize, int apertureSize, double k )
{
    QElapsedTimer algTime;
    algTime.start();
    Mat showka;
    namedWindow("showka2");
    cornerHarris( processedImage, showka, blockSize, apertureSize, k, BORDER_DEFAULT);
    imshow("showka2",showka);
    return algTime.elapsed();
}

int ImageManip::doHoughCircles(Mat processedImage, vector<Vec3f> circles, double dp_invAcum, double minDist, double param1, double param2, int minRadius, int maxRadius)
{
   QElapsedTimer algTime;
   algTime.start();
   HoughCircles(processedImage, circles, CV_HOUGH_GRADIENT, dp_invAcum, minDist, param1, param2, minRadius, maxRadius );
   drawCircles(processedImage, circles);

   return algTime.elapsed();
}

void ImageManip::drawCircles(Mat processedImage, vector<Vec3f> circles)
{
for( size_t i = 0; i < circles.size(); i++ )
   {
       Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
       int radius = cvRound(circles[i][2]);
       // draw the circle center
       circle( processedImage, center, 3, Scalar(255,0,0), -1, 8, 0 );
       // draw the circle outline
       //circle( processedImage, center, radius, Scalar(0,0,0), -radius, 8, 0 );
       circle( processedImage, center, radius, Scalar(255,0,0), 1, 8, 0 );
   }
}

int ImageManip::doGoodFeatureToTrack(Mat processedImage, int maxCorners, double qualityLevel, double minDistance, int blockSize, bool useHarris, double k)
{
    QElapsedTimer algTime;
    algTime.start();
    vector<Point2f> corners;
    //goodFeaturesToTrack( processedImage, vector<Point2f>& corners, maxCorners, qualityLevel, minDistance, const Mat& roiMask=Mat(), blockSize=3, useHarris, k=0.44);
    return algTime.elapsed();
}

int ImageManip::doPreCornerDetect(Mat processedImage, int apertureSize){
    QElapsedTimer algTime;
    algTime.start();
    Mat showka;
    namedWindow("showka");
    preCornerDetect( processedImage, showka, apertureSize, BORDER_DEFAULT );
    imshow("showka",showka);
     return algTime.elapsed();
//Mat corners, dilated_corners;
//preCornerDetect(image, corners, 3);
//// dilation with 3x3 rectangular structuring element
//dilate(corners, dilated_corners, Mat(), 1);
//Mat corner_mask = corners == dilated_corners;
}

int fastCenterAlgorithm(Mat processedImage)
{
    QElapsedTimer algTime;
    algTime.start();
    uchar vertical[processedImage.rows];
    uchar horizontal[processedImage.cols];
    //only a scetch of fast locenter alg

//    for(int i=1; i<processedImage.cols; i++){
//        //uchar intensity of actual cell processedImage[i][cvRound(processedImage.rows/2)]
//        if( processedImage[i][cvRound(processedImage.rows/2)] == processedImage[i-1][cvRound(processedImage.rows/2)]){
//            horizontal[i] = 0;
//        }else if( processedImage[i][cvRound(processedImage.rows/2)] < processedImage[i-1][cvRound(processedImage.rows/2)] )
//        {
//            horizontal[i] = processedImage[i-1][cvRound(processedImage.rows/2)] - processedImage[i][cvRound(processedImage.rows/2)];
//        }else{
//            horizontal[i] = processedImage[i][cvRound(processedImage.rows/2)] - processedImage[i-1][cvRound(processedImage.rows/2)];
//        }
//        if(i==1){
//            horizontal[0] = horizontal[1];
//        }
//    }
    //derivate one row in the middle then find range between two maximas
    //derivate one col in the middle of range found by previous row then find range between two maximas
    //derivate one row in the middle of the range found by previous column and then

    return algTime.elapsed();
}

int ImageManip::doPseudoContrast(Mat processedImage, int amount)
{
    QElapsedTimer algTime;
    algTime.start();
    //    for(int y=0; y<processedImage.rows; y++)
    //        for(int x=0; x<processedImage.cols; x++)
    //        {
    //            if(processedImage.at(y,x)>125){
    //                processedImage.at(y,x)-=amount;
    //            }else{
    //                processedImage.at(y,x)+=amount;
    //            }
    //        }
        return algTime.elapsed();
}

int ImageManip::doPlusIntensity(Mat processedImage, int amount)
{
    QElapsedTimer algTime;
    algTime.start();
    processedImage+=amount;
    return algTime.elapsed();
}

int ImageManip::doInvert(Mat processedImage)
{
    QElapsedTimer algTime;
    algTime.start();
    processedImage=255-processedImage;
    return algTime.elapsed();
}

Mat ImageManip::doHistogram(Mat processedImage){
    Mat hsv;
    cvtColor(processedImage, hsv, CV_GRAY2BGR);
    cvtColor(hsv, hsv, CV_BGR2HSV);//here is bad conv
    // let’s quantize the hue to 30 levels and the saturation to 32 levels
    int hbins = 30, sbins = 32;
    int histSize[] = {hbins, sbins};
    // hue varies from 0 to 179, see cvtColor
    float hranges[] = { 0, 180 };
    // saturation varies from 0 (black-gray-white) to 255 (pure spectrum color)
    float sranges[] = { 0, 256 };
    const float* ranges[] = { hranges, sranges };
    MatND hist;
    // we compute the histogram from the 0-th and 1-st channels
    int channels[] = {0};//0,1
    calcHist(&hsv, 1, channels, Mat(), /* do not use mask*/ hist, 1, histSize, ranges, true, /* the histogram is uniform*/ false );
//    calcHist(&hsv, 1, channels, Mat(), /* do not use mask*/ hist, 2, histSize, ranges, true, /* the histogram is uniform*/ false );
    double maxVal=255;
    minMaxLoc(hist, 0, &maxVal, 0, 0);
    int scale = 10;
    Mat histImg = Mat::zeros(sbins*scale, hbins*10, CV_8U);
    for( int h = 0; h < hbins; h++ )
        for( int s = 0; s < sbins; s++ )
        {
            float binVal = hist.at<float>(h, s);
            int intensity = cvRound(binVal*255/maxVal);
            rectangle( histImg, Point(h*scale, s*scale), Point( (h+1)*scale - 1, (s+1)*scale - 1), Scalar::all(intensity), CV_FILLED );
        }

    //namedWindow( "H-S Histograms", 1 );
    //imshow( "H-S Histograms", histImg );
    return histImg;
}

int ImageManip::doBilateralFiltering(Mat processedImage, double kernel, double sigmaColor, double sigmaSpace )
{
    QElapsedTimer algTime;
    algTime.start();
    Mat showka;
    //namedWindow("showka2");
    bilateralFilter(processedImage, showka, kernel, sigmaColor,  sigmaSpace, BORDER_DEFAULT);
    //imshow("showka2",showka);
    processedImage = showka;
    return algTime.elapsed();
}

//int downsize(){

//}
