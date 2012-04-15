#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtGUI>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include "imagemanip.h"
#include "findFiles.h"
#include "crossInclude.h"


namespace Ui {
    class MainWindow;
}

using namespace cv;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    QString imagePath;
    QString tempPath;
    QString imageSaveDir;
    QStringList openedCVWindowNames;
    Mat origImg, workImg;
    Mat histImg;
    vector<Vec3f> circles;

    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    //events
    void closeEvent(QCloseEvent *event);
    void resizeEvent(QResizeEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

    //main window functions
    void writeSettings();
    void readSettings();
    void initGraphicsImages(QString fileName);
    void restoreImage();
    void update();
    void startUpdater(int refreshRate);
    void restartUpdater(int refreshRate);
    QString readTextFile(QString windowName);
    bool writeTextFile(QString windowName, QString text);

    //OpenCV manipulators
    void callCVoperation(ImageManip::SetupFlags);
    void destroyAllCVWindows();
    QPixmap* loadPictureToPixmap(QString filePath);

public slots:
    void setListOfFiles(){listOfFiles = QStringList();}
    void setListOfFiles(QStringList f,QStringList p){listOfFiles.operator =(f);listOfFilePaths.operator =(p);}
private slots:
    //buttons and QT gui item calls
    void on_actionLoad_picture_triggered();
    void updateGraphics_orig();
    void updateGraphics_gray();
    void updateGraphics_active();
    void updateGraphics_hist();
//    void on_horizontalSlider_sliderMoved(int position);
//    void on_horizontalSlider_valueChanged(int value);
    void on_actionRefresh_interval_triggered();
    void on_lineEdit_refreshRate_editingFinished();

    void on_pushButton_run_clicked();

    void on_pushButton_thresholdUnder_clicked();

    void on_pushButton_clear_clicked();

    void on_pushButton_thresholdAbove_clicked();

    void on_pushButton_blur_clicked();

    void on_pushButton_medianBlur_clicked();

    void on_pushButton_gausBlur_clicked();

    void on_pushButton_canny_clicked();

    void on_pushButton_undo_clicked();

    void on_pushButton_laplacian_clicked();

    void on_pushButton_scharr_clicked();

    void on_pushButton_sobel_clicked();

    void on_comboBox_scharr_xDerivOrder_activated(const QString &arg1);

    void on_comboBox_scharr_yDerivOrder_activated(const QString &arg1);

    void on_pushButton_harris_clicked();

    void on_pushButton_houghTransform_clicked();

//    void on_actionFull_screen_mode_triggered();

    void on_actionExit_triggered();

    void on_textEdit_setup_customContextMenuRequested(const QPoint &pos);

    void on_actionRun_Setup_triggered();

    void on_actionSave_Setup_File_triggered();

    void on_actionLoad_Setup_File_triggered();

    void on_actionHarris_triggered();

    void on_actionLast_Setup_triggered();

    void on_actionClear_Setup_triggered();

    void on_pushButton_preCornerDetect_clicked();

    void on_actionMedian_Blur_triggered();

    void on_actionBlur_triggered();

    void on_actionGausian_Blur_triggered();

    void on_actionThreshold_Under_triggered();

    void on_actionThreshold_Above_triggered();

    void on_actionLaplacian_triggered();

    void on_actionSobel_triggered();

    void on_actionScharr_triggered();

    void on_actionCanny_triggered();

    void on_actionHough_Transformation_triggered();

    void on_pushButton_inverseIntensity_clicked();

    void on_pushButton_intensityAdd_clicked();

    void on_pushButton_pseudoContrast_clicked();

    void on_pushButton_setFilesList_clicked();

    bool on_actionSet_Mass_Save_Directory_triggered();

    void on_pushButton_massRun_clicked();

    void messageOutStringList(const QString listName, QStringList list);

    void debugWindow(const QString text);

    void messageOutBuffersLists(const QString listName, QList<int> listInt, QList<double> listDouble, QList<bool> listBool);

    void on_pushButton_pupilFilterSetup_clicked();

    void on_actionPupil_Filter_Setup_triggered();

    void on_pushButton_bilateralSmoothing_clicked();

    void on_actionBilateral_Filtering_triggered();

    void on_pushButton_snakePromo_clicked();

private:
    Ui::MainWindow *ui;
    QGraphicsScene *sc_orig;
    QGraphicsPixmapItem *pxi_orig;
    QGraphicsScene *sc_gray;
    QGraphicsPixmapItem *pxi_gray;
    QGraphicsScene *sc_hist;
    QGraphicsPixmapItem *pxi_hist;
    QGraphicsScene *sc_active;
    QGraphicsPixmapItem *pxi_active;
    int refreshInterval;
    QImage image;
    QList <QPixmap *> imageRasters;
    ImageManip imageManipulation(QImage *);
    QTimer* updater;
    QList <ImageManip::SetupFlags> activeSetup, lastSetup;
    QList <int> intBuffer, lastIntBuffer;
    QList <double> doubleBuffer, lastDoubleBuffer;
    QList <bool> boolBuffer, lastBoolBuffer;
    QStringList listOfFiles;
    QStringList listOfFilePaths;
    bool massRun;
};

#endif // MAINWINDOW_H
