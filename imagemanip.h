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
    //treba spravit vypis *Mat do okna huigui
    //treba spravit obdobu tejto funkcie pre kazdy operator vracat bude *Mat
    void onTrackbar(int, void*);
    void showImgThoughOpenCV(QString pathToImage);
    Mat* bindIplImage2Mat(const IplImage *iplImage);
    IplImage Mat2IplImage(const Mat *mat);
    IplImage* qImage2IplImage(const QImage& qImage);
    QImage IplImage2QImage(const IplImage *iplImage);

    //Mat manipulation functions
    static int toGrayscale(Mat processedImage);

    static void doOneClickProcessing(Mat processedImage);
    static int doBlur(Mat processedImage, int size);
    static int doGausianBlur(Mat processedImage, int size);
    static int doGausianBlur(Mat processedImage, int size, double sigma1, double sigma2);
    static int doMedianBlur(Mat processedImage, int size);
    static int doBilateralFiltering(Mat processedImage, double kernel, double sigmaColor, double sigmaSpace );

    static int doLaplacian(Mat processedImage, int ksize, double scale, double delta);
    static int doSobel(Mat processedImage, int dx, int dy, int ksize, double scale, double delta );
    static int doScharr(Mat processedImage, int dx, int dy, double scale, double delta );

    static int doThresholdUnder(Mat processedImage, int threshVal);
    static int doThresholdAbove(Mat processedImage, int threshVal);
    static int doImageMap(Mat processedImage, int threshVal);
    static int doAdaptiveThreshold(Mat processedImage);

    static int doGrabCut(Mat processedImage);
    static int doIntegral(Mat processedImage);

    static int doCanny(Mat processedImage, int thresh1, int thresh2, int size, bool l2grad);
    static int doCornerHarris(Mat processedImage, int blockSize, int apertureSize, double k);
    static int doHoughCircles(Mat processedImage, vector<Vec3f> circles, double dp_invAcum, double minDist, double param1, double param2, int minRadius, int maxRadius);

    static int doPreCornerDetect(Mat processedImage, int apertureSize);
    static int doGoodFeatureToTrack(Mat processedImage, int maxCorners, double qualityLevel, double minDistance, int blockSize, bool useHarris, double k);

    static int doInvert(Mat processedImage);
    static int doPlusIntensity(Mat processedImage, int amount);
    static int doPseudoContrast(Mat processedImage, int amount);

    static void drawCircles(Mat processedImage, vector<Vec3f> circles);
    static Mat doHistogram(Mat processedImage);

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
    Mat *matTemp;
    cv::Mat tempMat, edgeMat;
    int tempInt;
};

#endif // IMAGEMANIP_H
