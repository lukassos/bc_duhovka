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

Mat* ImageManip::bindIplImage2Mat(const IplImage *iplImage)
{
    Mat *newMat = new Mat;
    *newMat = cvarrToMat(iplImage);

    return newMat;
}


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
    Canny(tempMat, tempMat, tempInt, tempInt*3, 3);
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
    blur(processedImage, processedImage, Size(size,size));
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
       circle( processedImage, center, 3, Scalar(255,0,0), -1, 8, 0 );
       circle( processedImage, center, radius, Scalar(255,0,0), 1, 8, 0 );
   }
}

int ImageManip::doGoodFeatureToTrack(Mat processedImage, int maxCorners, double qualityLevel, double minDistance, int blockSize, bool useHarris, double k)
{
    QElapsedTimer algTime;
    algTime.start();
    vector<Point2f> corners;
    return algTime.elapsed();
}

int ImageManip::doPreCornerDetect(Mat processedImage, int apertureSize){
    QElapsedTimer algTime;
    algTime.start();
    Mat temp = processedImage.clone();
    preCornerDetect( processedImage, temp, apertureSize, BORDER_DEFAULT );
   processedImage = temp.clone();
     return algTime.elapsed();

}

int fastCenterAlgorithm(Mat processedImage)
{
    QElapsedTimer algTime;
    algTime.start();
    uchar vertical[processedImage.rows];
    uchar horizontal[processedImage.cols];

    return algTime.elapsed();
}

int ImageManip::doPseudoContrast(Mat processedImage, int amount)
{
    QElapsedTimer algTime;
    algTime.start();

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
    int channels[] = {0};//0,1
    calcHist(&hsv, 1, channels, Mat(), /* do not use mask*/ hist, 1, histSize, ranges, true, /* the histogram is uniform*/ false );
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


    return histImg;
}

int ImageManip::doBilateralFiltering(Mat processedImage, double kernel, double sigmaColor, double sigmaSpace )
{
    QElapsedTimer algTime;
    algTime.start();
    Mat showka;
    bilateralFilter(processedImage, showka, kernel, sigmaColor,  sigmaSpace, BORDER_DEFAULT);
   processedImage = showka;
    return algTime.elapsed();
}

