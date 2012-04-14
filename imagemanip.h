#ifndef IMAGEMANIP_H
#define IMAGEMANIP_H

#include <QObject>
#include <QtGUI>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>

using namespace cv;

class ImageManip : public QObject
{

public:
    ImageManip(QImage *image);
    ~ImageManip();
    //treba spravit vypis *CvMat do okna huigui
    //treba spravit obdobu tejto funkcie pre kazdy operator vracat bude *CvMat
    void onTrackbar(int, void*);
    void showImgThoughOpenCV(QString pathToImage);
    CvMat* bindIplImage2Mat(const IplImage *iplImage);
    IplImage Mat2IplImage(const CvMat *CvMat);
    IplImage* qImage2IplImage(const QImage& qImage);
    QImage IplImage2QImage(const IplImage *iplImage);

    //CvMat manipulation functions
    static int toGrayscale(CvMat processedImage);

    static void doOneClickProcessing(CvMat processedImage);
    static int doBlur(CvMat processedImage, int size);
    static int doGausianBlur(CvMat processedImage, int size);
    static int doGausianBlur(CvMat processedImage, int size, double sigma1, double sigma2);
    static int doMedianBlur(CvMat processedImage, int size);
    static int doBilateralFiltering(CvMat processedImage, double kernel, double sigmaColor, double sigmaSpace );

    static int doLaplacian(CvMat processedImage, int ksize, double scale, double delta);
    static int doSobel(CvMat processedImage, int dx, int dy, int ksize, double scale, double delta );
    static int doScharr(CvMat processedImage, int dx, int dy, double scale, double delta );

    static int doThresholdUnder(CvMat processedImage, int threshVal);
    static int doThresholdAbove(CvMat processedImage, int threshVal);
    static int doImageMap(CvMat processedImage, int threshVal);
    static int doAdaptiveThreshold(CvMat processedImage);

    static int doGrabCut(CvMat processedImage);
    static int doIntegral(CvMat processedImage);

    static int doCanny(CvMat processedImage, int thresh1, int thresh2, int size, bool l2grad);
    static int doCornerHarris(CvMat processedImage, int blockSize, int apertureSize, double k);
    static int doHoughCircles(CvMat processedImage, vector<Vec3f> circles, double dp_invAcum, double minDist, double param1, double param2, int minRadius, int maxRadius);

    static int doPreCornerDetect(CvMat processedImage, int apertureSize);
    static int doGoodFeatureToTrack(CvMat processedImage, int maxCorners, double qualityLevel, double minDistance, int blockSize, bool useHarris, double k);

    static int doInvert(CvMat processedImage);
    static int doPlusIntensity(CvMat processedImage, int amount);
    static int doPseudoContrast(CvMat processedImage, int amount);

    static void drawCircles(CvMat processedImage, vector<Vec3f> circles);
    static CvMat doHistogram(CvMat processedImage);

    enum SetupFlags{
        //cv::Image Preprocessing
            //Image Filterring
            PseudoContrast_flag,
            Inverse_flag,
            PlusIntensity_flag,
            Blur_flag,
            GausianBlur_flag,
            MedianBlur_flag,
            Laplacian_flag,
            Sobel_flag,
            Scharr_flag,
            BilateralSmoothing_flag,
        //cv::Miscellaneous Image Transformations
            //Thresholds
            ThresholdUnder_flag,
            ThresholdAbove_flag,
            AdaptiveThreshold_flag,
            //Segmentation
            GrabCut_flag,
            //Other Transformation
            Integral_flag,
        //cv:Feature Detection
            //Edge Detection
            Canny_flag,
            CornerHarris_flag,
            HoughCircles_flag,
            //Corner Pre-Detection
            PreCornerDetect_flag,
            GoodFeatureToTrack_flag
        };
private:


    bool flags[20];
    CvMat *matTemp;
    CvMat tempMat, edgeMat;
    int tempInt;
};

#endif // IMAGEMANIP_H
