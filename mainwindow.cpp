#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    updater = new QTimer();
    readSettings();
    //ui->actualTime->setStyle(QStyle::PE_FrameButtonBevel);

    //ui->actualTime->setStyleSheet("QLCDNumber {background: green; text: red;}");

    //ui->centralWidget->setMouseTracking(true);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    //treba zavriet vsetko co inicializovala kniznica opencv
    //zatial sa zda ze staci zlikvidovat len vsetky okna z highgui
    //pozor tieto okna museli byt inicializovane aspon raz inac hrozi RTERR
    destroyAllCVWindows();
    writeSettings();
    QMainWindow::closeEvent(event);
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    //relative counting of main window items size
    ui->tabWidget->setGeometry(170,40,width()-180,height()-105);
    ui->graphicsView_orig->setGeometry(0,0,width()-186,height()-126);
    ui->graphicsView_gray->setGeometry(0,0,width()-186,height()-126);
    ui->graphicsView_hist->setGeometry(0,0,width()-186,height()-126);
    ui->graphicsView_active->setGeometry(0,0,width()-186,height()-126);
    ui->textEdit_setup->setGeometry(0,0,width()-186,height()-126);
    ui->toolBox_menu->setGeometry(5,10,150,height()-40);
    //restoreImage();
    updateGraphics_orig();
    updateGraphics_gray();
    updateGraphics_active();
    updateGraphics_hist();
    on_lineEdit_refreshRate_editingFinished();
    QWidget::resizeEvent(event);
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    //in case of human interaction
    //QString::number(QCursor::pos().ry())
//    ui->xposition->setNumDigits(QCursor::pos().x());
//    ui->yposition->setNumDigits(QCursor::pos().y());
//    ui->vypis->setText("Mouse at pos:\tx: "+QString::number(QCursor::pos().x())+"\n\ty: "+QString::number(QCursor::pos().y()));
    //ui->statusBar->showMessage("Mouse at pos:\tx: "+QString::number(QCursor::pos().x())+"\n\t\ty: "+QString::number(QCursor::pos().y()));
    QWidget::mouseMoveEvent(event);
}

void MainWindow::writeSettings()
{
    QSettings settings("MyCompany", "MyApp");
    settings.setValue("geometry", saveGeometry());
    settings.setValue("windowState", saveState());
    settings.setValue("pos", pos());
}

void MainWindow::readSettings()
{
    QSettings settings("MyCompany", "MyApp");
    restoreGeometry(settings.value("geometry").toByteArray());
    restoreState(settings.value("windowState").toByteArray());
    move(settings.value("pos", QPoint(200, 200)).toPoint());
}

void MainWindow::update()
 {
    //deprecated
    //ak nebola ziadna zmena na obrazku mozem volat
    restoreImage();
    //ked sa bude vykraslovat do obrazka treba to prerobit na podmienecne

 }

 ////////////////////////////////////////
//   LOAD PICTURE AND GRAPHICS INIT   //


void MainWindow::on_pushButton_setFilesList_clicked()
{
    FindFiles f(this);
    QObject::connect( &f, SIGNAL(signalFiles(QStringList,QStringList)), this, SLOT(setListOfFiles(QStringList,QStringList)));
    f.init(listOfFiles);
    f.exec();
    //multithread should be here
    QStringList temp = listOfFiles;
    ui->textEdit_setup->append("---------------\nFor processing: ");
    while(!temp.isEmpty()){
        ui->textEdit_setup->append(temp.first());
        temp.removeFirst();
    }
    ui->textEdit_setup->append("---------------");
}

bool MainWindow::on_actionSet_Mass_Save_Directory_triggered()
{
    QString filePath = QFileDialog::getExistingDirectory(this, tr("Set Save Directory"), QDir::currentPath());
    if (!filePath.isEmpty()){
        imageSaveDir = filePath;
        workImg = imread(filePath.toStdString(),0);
    }else{
        QMessageBox msg;
        msg.setText("Failed opening directory");
        msg.exec();
        return false;
    }
    return true;
}

void MainWindow::on_actionLoad_picture_triggered()
{
    QString filePath = QFileDialog::getOpenFileName(this, tr("Load Picture"), QDir::currentPath(), tr("JPEG(*.jpg)"));
    if (!filePath.isEmpty()){
        imagePath = filePath;
        workImg = imread(filePath.toStdString(),0);
        initGraphicsImages(filePath);
    }else{
        QMessageBox msg;
        msg.setText("Failed opening picture");
        msg.exec();
    }
}

QPixmap* MainWindow::loadPictureToPixmap(QString filePath)
{
    Mat image_greyscaled = imread(filePath.toStdString(),0);
    //workImg =  new Mat(640,480,CV_8U);
    //workImg = imread(filePath.toStdString(),0);
    //workImg = imread(filePath.toStdString(),0);
    QPixmap* back = new QPixmap("tempImage.jpg");
    //back.load("tempImage.jpg")
    //back.loadFromData(image_greyscaled.data,
//                      (*image_greyscaled.dataend-*image_greyscaled.datastart),
//                      0,
//                      Qt::AutoColor);
    return back;
}

void MainWindow::initGraphicsImages(QString filePath)
{
    image.load(filePath);
    //imageManipulation = new ImageManip(&image);

//here comes auto setup of gui
    //ui->spinBox_houghTransform_minRadius->setMaximum((Qt::qMin(workImg.cols, workImg.rows))/2);
    //ui->spinBox_houghTransform_maxRadius->setMaximum((Qt::qMin(workImg.cols, workImg.rows))/2);

    sc_orig = new QGraphicsScene;
    sc_gray = new QGraphicsScene;
    sc_hist = new QGraphicsScene;
    sc_active = new QGraphicsScene;

    pxi_orig = new QGraphicsPixmapItem;
    pxi_gray = new QGraphicsPixmapItem;
    pxi_hist = new QGraphicsPixmapItem;
    pxi_active = new QGraphicsPixmapItem;

    sc_orig->addItem(pxi_orig);
    sc_gray->addItem(pxi_gray);
    sc_hist->addItem(pxi_hist);
    sc_active->addItem(pxi_active);

    ui->graphicsView_orig->setScene(sc_orig);
    ui->graphicsView_gray->setScene(sc_gray);
    ui->graphicsView_hist->setScene(sc_hist);
    ui->graphicsView_active->setScene(sc_active);

//    ui->graphicsView_orig->show();
//    ui->graphicsView_gray->show();
    imageRasters.clear();


    QPixmap *tempPixmap_orig = new QPixmap(filePath);
    //*tempPixmap_orig = QPixmap::load(,0,Qt::ColorOnly);
    imageRasters.append(tempPixmap_orig);
    pxi_orig->setPixmap(*imageRasters.at(0));

    //ui->vypis->setGeometry(0,0,ui->tab->width(),ui->tab->height());
    //ui->vypis->setPixmap(*tempPixmap_orig);

    QPixmap *tempPixmap_gray = tempPixmap_orig;
    //*tempPixmap_gray = loadPictureToPixmap(filePath);//QPixmap::fromImage(image,Qt::ColorOnly);
    //*tempPixmap_gray = QPixmap::fromImage(image,Qt::ColorOnly);
    imageRasters.append(tempPixmap_gray);
    pxi_gray->setPixmap(*imageRasters.at(1));
//    histImg=ImageManip::doHistogram(workImg);
//    openedCVWindowNames.append("H-S Histogram");
//    namedWindow( "H-S Histogram", 1 );
//    imshow( "H-S Histogram", histImg );
    QPixmap *tempPixmap_hist = tempPixmap_orig;
    imageRasters.append(tempPixmap_hist);
    pxi_hist->setPixmap(*imageRasters.at(2));

    QPixmap *tempPixmap_active = loadPictureToPixmap(filePath);
    imageRasters.append(tempPixmap_active);
    pxi_active->setPixmap(*imageRasters.at(3));

    //tempPixmap_orig = tempPixmap_orig.scaled(ui->graphicsView_orig->width()-3, ui->graphicsView_orig->height()-3, Qt::KeepAspectRatio);
    //tempPixmap_gray = tempPixmap_gray.scaled(ui->graphicsView_gray->width()-3, ui->graphicsView_gray->height()-3, Qt::KeepAspectRatio);

    //pxi_gray->setPixmap(tempPixmap_gray);
    //pxi_orig->setPixmap(imageRaster.scaled(ui->graphicsView_orig->width()-3, ui->graphicsView_orig->height()-3, Qt::KeepAspectRatio));
    // pxi_gray->setPixmap(tempPixmap_gray.scaled(ui->graphicsView_gray->width()-3, ui->graphicsView_gray->height()-3, Qt::KeepAspectRatio));
    startUpdater(ui->lineEdit_refreshRate->text().toInt());
}

 /////////////////////////
//  TIMER AND UPDATES  //
void MainWindow::on_actionRefresh_interval_triggered()
{
    bool ok;
    QString text = QInputDialog::getText(this, tr("Refresh time"),
                                         tr("Actual time:"), QLineEdit::Normal,
                                         ui->lineEdit_refreshRate->text(), &ok);
    if (ok && !text.isEmpty()){
        ui->lineEdit_refreshRate->text() = text;
    }else{
        ui->lineEdit_refreshRate->text() = "0";
    }

}

void MainWindow::on_lineEdit_refreshRate_editingFinished()
{
    if(image.isNull()){
        startUpdater(ui->lineEdit_refreshRate->text().toInt());
    }else{
        restartUpdater(ui->lineEdit_refreshRate->text().toInt());
    }
}

void MainWindow::startUpdater(int refreshRate){
    if(refreshRate>0){
        updater = new QTimer(this);
//        QObject::connect(updater,SIGNAL(timeout()),
//                         this,SLOT(updateGraphics_orig()));

//        QObject::connect(updater,SIGNAL(timeout()),
//                         this,SLOT(updateGraphics_gray()));

//        QObject::connect(updater,SIGNAL(timeout()),
//                         this,SLOT(updateGraphics_hist()));

        QObject::connect(updater,SIGNAL(timeout()),
                         this,SLOT(updateGraphics_active()));

        updater->start(ui->lineEdit_refreshRate->text().toInt());
    }
}

void MainWindow::restartUpdater(int refreshRate){
    if(refreshRate>0){
        updater->stop();
        updater->start(ui->lineEdit_refreshRate->text().toInt());
    }else{
        if(updater->isActive())
        {
            updater->stop();
        }
    }
}

void MainWindow::updateGraphics_orig(){
    if(imageRasters.count()>=1){
        if(ui->actionScale_pictures->isChecked()){
            QPixmap *temp0 = imageRasters.at(0);
            QPixmap temp1 = *temp0;
            QPixmap temp2;
            temp2 = temp1.scaled(ui->graphicsView_orig->width()-3, ui->graphicsView_orig->height()-3, Qt::KeepAspectRatio);
            pxi_orig->setPixmap(temp2);
            //resize of graphics scene
//            pxi_orig->setOffset(0,0);
//            sc_orig->update(0,0,ui->graphicsView_orig->width()-3,ui->graphicsView_orig->height()-3);
//            pxi_orig->update(0,0,ui->graphicsView_orig->width()-3,ui->graphicsView_orig->height()-3);
        }else{
            pxi_orig->setPixmap(*imageRasters.at(0));
        }
    }

    //++framecounter;
}

void MainWindow::updateGraphics_active(){
    if(imageRasters.count()>=3){
        if(ui->actionScale_pictures->isChecked()){
            imageRasters.removeAt(3);
            imageRasters.insert(2,new QPixmap("tempImage.jpg"));
            QPixmap *temp0 = imageRasters.at(3);
            QPixmap temp1 = *temp0;
            QPixmap temp2;
            temp2 = temp1.scaled(ui->graphicsView_active->width()-3, ui->graphicsView_active->height()-3, Qt::KeepAspectRatio);
            pxi_active->setPixmap(temp2);
            //resize of graphics scene
//            pxi_orig->setOffset(0,0);
//            sc_orig->update(0,0,ui->graphicsView_orig->width()-3,ui->graphicsView_orig->height()-3);
//            pxi_orig->update(0,0,ui->graphicsView_orig->width()-3,ui->graphicsView_orig->height()-3);
        }else{
            imageRasters.removeAt(3);
            imageRasters.insert(2,new QPixmap("tempImage.jpg"));
            pxi_active->setPixmap(*imageRasters.at(3));
        }
    }

    //++framecounter;
}

void MainWindow::updateGraphics_hist(){
    if(imageRasters.count()>=2){
        if(ui->actionScale_pictures->isChecked()){
            QPixmap *temp0 = imageRasters.at(2);
            QPixmap temp1 = *temp0;
            QPixmap temp2;
            temp2 = temp1.scaled(ui->graphicsView_hist->width()-3, ui->graphicsView_hist->height()-3, Qt::KeepAspectRatio);
            pxi_hist->setPixmap(temp2);
            //resize of graphics scene
//            pxi_orig->setOffset(0,0);
//            sc_orig->update(0,0,ui->graphicsView_orig->width()-3,ui->graphicsView_orig->height()-3);
//            pxi_orig->update(0,0,ui->graphicsView_orig->width()-3,ui->graphicsView_orig->height()-3);
        }else{
            pxi_hist->setPixmap(*imageRasters.at(2));
        }
    }

    //++framecounter;
}

void MainWindow::updateGraphics_gray(){
    if(imageRasters.count()>=2){
        if(ui->actionScale_pictures->isChecked()){
            QPixmap *temp0 = imageRasters.at(1);
            QPixmap temp1 = *temp0;
            QPixmap temp2;
            temp2 = temp1.scaled(ui->graphicsView_gray->width()-3, ui->graphicsView_gray->height()-3, Qt::KeepAspectRatio);
            pxi_gray->setPixmap(temp2);
        }else{
            pxi_gray->setPixmap(*imageRasters.at(1));
        }

    }

    //++framecounter;
}

void MainWindow::restoreImage()
{
    //    QPixmap tempPixmap_orig = QPixmap::fromImage(image,Qt::ColorOnly);
    //    QPixmap tempPixmap_gray = QPixmap::fromImage(image,Qt::MonoOnly);
    //    tempPixmap_orig = tempPixmap_orig.scaled(ui->graphicsView_orig->width()-3, ui->graphicsView_orig->height()-3, Qt::KeepAspectRatio);
    //    tempPixmap_gray = tempPixmap_gray.scaled(ui->graphicsView_gray->width()-3, ui->graphicsView_gray->height()-3, Qt::KeepAspectRatio);
 //        QPixmap *tempPixmap_orig = new QPixmap(10,10) ; // QPixmap::fromImage(image,Qt::ColorOnly);
 //        tempPixmap_orig->fill(Qt::red);
//         pxi_orig = new QGraphicsPixmapItem;
 //        pxi_gray = new QGraphicsPixmapItem;
   // pxi_orig->pixmap().fromImage(image,Qt::ColorOnly);
//    tempPixmap_orig.scaled(ui->graphicsView_orig->width()-3, ui->graphicsView_orig->height()-3, Qt::KeepAspectRatio);
//    pxi_orig->setPixmap(*tempPixmap_orig);
// pxi_orig->setPixmap(imageRaster.scaled(ui->graphicsView_orig->width()-3, ui->graphicsView_orig->height()-3, Qt::KeepAspectRatio));
    //pxi_gray->setPixmap(tempPixmap_gray);
//    QPixmap tempPixmap_orig = QPixmap::fromImage(image,Qt::ColorOnly);
    //QPixmap tempPixmap_gray = QPixmap::fromImage(image,Qt::MonoOnly);
//
   // pxi_orig->setPixmap(tempPixmap_orig.scaled(ui->graphicsView_orig->width()-3, ui->graphicsView_orig->height()-3, Qt::KeepAspectRatio));
   // pxi_gray->setPixmap(tempPixmap_gray.scaled(ui->graphicsView_gray->width()-3, ui->graphicsView_gray->height()-3, Qt::KeepAspectRatio));

    //QPixmap pxm(filePath);
//    QPixmap pxm(filePath);
   // QSize picSize = ;
//    pxm.scaled(picSize);

}

 ///////////////////////////////
//     OPEN CV OPERATIONS    //
void MainWindow::destroyAllCVWindows()
{
    //treba volat aj pred kazdym novym inicializovanim obrazkov
    openedCVWindowNames.removeDuplicates();
    for(int i=0; i<openedCVWindowNames.count(); i++){
        destroyWindow(openedCVWindowNames.at(i).toStdString());
    }
    openedCVWindowNames.clear();
}

void MainWindow::callCVoperation(ImageManip::SetupFlags operation){
    int algTime = 0;

    switch(operation)
    {
    case ImageManip::Blur_flag:
        algTime= ImageManip::doBlur(workImg, intBuffer.at(0));
        if(!massRun){
            namedWindow("Blur");
            openedCVWindowNames.append("Blur");
            imshow("Blur",workImg);

            ui->textEdit_setup->append(QString::number(algTime)+"ms : Blur { "+QString::number(intBuffer.at(0))+"; }");
        }
        intBuffer.pop_front();
        break;

    case ImageManip::MedianBlur_flag:
        algTime = ImageManip::doMedianBlur(workImg, intBuffer.at(0));
        if(!massRun){
            namedWindow("MedianBlur");
            openedCVWindowNames.append("MedianBlur");
            imshow("MedianBlur",workImg);
            ui->textEdit_setup->append(QString::number(algTime)+"ms : Median Blur { "+QString::number(intBuffer.at(0))+"; }");

        }
        intBuffer.pop_front();
        break;

    case ImageManip::GausianBlur_flag:
        algTime = ImageManip::doGausianBlur(workImg, intBuffer.at(0), doubleBuffer.at(0), doubleBuffer.at(1));
        if(!massRun){
            namedWindow("GausianBlur");
            openedCVWindowNames.append("GausianBlur");
            imshow("GausianBlur",workImg);

            ui->textEdit_setup->append(QString::number(algTime)+"ms : Gausian Blur { "+QString::number(intBuffer.at(0))+"; "+QString::number(doubleBuffer.at(0))+"; "+QString::number(doubleBuffer.at(1))+"; }");

        }
        intBuffer.pop_front();
        doubleBuffer.pop_front();
        doubleBuffer.pop_front();
        break;

    case ImageManip::BilateralSmoothing_flag:
        algTime = ImageManip::doBilateralFiltering(workImg, intBuffer.at(0), doubleBuffer.at(0), doubleBuffer.at(1));
        if(!massRun){
            namedWindow("BilateralFiltering");
            openedCVWindowNames.append("BilateralFiltering");
            imshow("BilateralFiltering",workImg);

            ui->textEdit_setup->append(QString::number(algTime)+"ms : Bilateral Filtering { "+QString::number(intBuffer.at(0))+"; "+QString::number(doubleBuffer.at(0))+"; "+QString::number(doubleBuffer.at(1))+"; }");

        }
        intBuffer.pop_front();
        doubleBuffer.pop_front();
        doubleBuffer.pop_front();
        break;

    case ImageManip::ThresholdUnder_flag:
        algTime = ImageManip::doThresholdUnder(workImg, intBuffer.at(0));
        if(!massRun){
            namedWindow("ThresholdUnder");
            openedCVWindowNames.append("ThresholdUnder");
            imshow("ThresholdUnder",workImg);

            ui->textEdit_setup->append(QString::number(algTime)+"ms : Threshold Under { "+QString::number(intBuffer.at(0))+"; }");

        }
        intBuffer.pop_front();
        break;

    case ImageManip::ThresholdAbove_flag:
        algTime = ImageManip::doThresholdAbove(workImg,  intBuffer.at(0));
        if(!massRun){
            namedWindow("ThresholdAbove");
            openedCVWindowNames.append("ThresholdAbove");
            imshow("ThresholdAbove",workImg);

            ui->textEdit_setup->append(QString::number(algTime)+"ms : Threshold Above { "+QString::number(intBuffer.at(0))+"; }");

        }intBuffer.pop_front();
        break;

    case ImageManip::Canny_flag:
        algTime = ImageManip::doCanny(workImg,  intBuffer.at(0), intBuffer.at(1),intBuffer.at(2) ,boolBuffer.at(0));
        if(!massRun){
            namedWindow("Canny");
            openedCVWindowNames.append("Canny");
            imshow("Canny",workImg);
                if(boolBuffer.at(0))
                {
                    ui->textEdit_setup->append(QString::number(algTime)+"ms : Canny edge dettection {"+QString::number(intBuffer.at(0))+"; "+QString::number(intBuffer.at(1))+"; "+QString::number(intBuffer.at(2))+"; "+"true"+"; }");
                }else{
                    ui->textEdit_setup->append(QString::number(algTime)+"ms : Canny edge dettection {"+QString::number(intBuffer.at(0))+"; "+QString::number(intBuffer.at(1))+"; "+QString::number(intBuffer.at(2))+"; "+"false"+"; }");
                }

        }
        intBuffer.pop_front();
        intBuffer.pop_front();
        intBuffer.pop_front();
        boolBuffer.pop_front();
        break;

    case ImageManip::Laplacian_flag:
        algTime = ImageManip::doLaplacian(workImg, intBuffer.at(0), doubleBuffer.at(0), doubleBuffer.at(1));
    if(!massRun){
        namedWindow("Laplacian");
        openedCVWindowNames.append("Laplacian");
        imshow("Laplacian",workImg);

        ui->textEdit_setup->append(QString::number(algTime)+"ms : Laplacian { "+QString::number(intBuffer.at(0))+"; "+QString::number(doubleBuffer.at(0))+"; "+QString::number(doubleBuffer.at(1))+"; }");

    }
    intBuffer.pop_front();
        doubleBuffer.pop_front();
        doubleBuffer.pop_front();
        break;

    case ImageManip::Scharr_flag:
        algTime = ImageManip::doScharr(workImg, intBuffer.at(0), intBuffer.at(1), doubleBuffer.at(0), doubleBuffer.at(1));
    if(!massRun){
        namedWindow("Scharr");
        openedCVWindowNames.append("Scharr");
        imshow("Scharr",workImg);
        ui->textEdit_setup->append(QString::number(algTime)+"ms : Scharr { "+QString::number(intBuffer.at(0))+"; "+QString::number(intBuffer.at(1))+"; "+QString::number(doubleBuffer.at(0))+"; "+QString::number(doubleBuffer.at(1))+"; }");

    }
        intBuffer.pop_front();
        intBuffer.pop_front();
        doubleBuffer.pop_front();
        doubleBuffer.pop_front();
        break;

    case ImageManip::Sobel_flag:
        algTime = ImageManip::doSobel(workImg, intBuffer.at(0), intBuffer.at(1), intBuffer.at(2), doubleBuffer.at(0), doubleBuffer.at(1));
        if(!massRun){
            namedWindow("Sobel");
        openedCVWindowNames.append("Sobel");
        imshow("Sobel",workImg);

        ui->textEdit_setup->append(QString::number(algTime)+"ms : Sobel { "+QString::number(intBuffer.at(0))+"; "+QString::number(intBuffer.at(1))+"; "+QString::number(intBuffer.at(2))+"; "+QString::number(doubleBuffer.at(0))+"; "+QString::number(doubleBuffer.at(1))+"; }");
        }
        intBuffer.pop_front();
        intBuffer.pop_front();
        intBuffer.pop_front();
        doubleBuffer.pop_front();
        doubleBuffer.pop_front();
        break;

    case ImageManip::CornerHarris_flag:
        algTime = ImageManip::doCornerHarris(workImg, intBuffer.at(0), intBuffer.at(1), doubleBuffer.at(0));
    if(!massRun){
        namedWindow("Harris");
        openedCVWindowNames.append("Harris");
        imshow("Harris",workImg);

        ui->textEdit_setup->append(QString::number(algTime)+"ms : Harris { "+ui->spinBox_harris_blockSize->text()+"; "+ui->comboBox_harris_appertureSize->currentText()+"; "+ui->doubleSpinBox_harris_kFreeParam->text()+"; }");

    }
        intBuffer.pop_front();
        intBuffer.pop_front();
        doubleBuffer.pop_front();
        break;

    case ImageManip::PreCornerDetect_flag:
        algTime = ImageManip::doPreCornerDetect(workImg, intBuffer.at(0));
        if(!massRun){
            namedWindow("Pre-corner Detection");
        openedCVWindowNames.append("Pre-corner Detection");
        imshow("Pre-corner Detection",workImg);
            ui->textEdit_setup->append(QString::number(algTime)+"ms : Pre-corner Detection { "+ui->comboBox_preCornerDetect_appertureSize->currentText()+"; }");
        }
            intBuffer.pop_front();
        break;

    case ImageManip::HoughCircles_flag:

        algTime = ImageManip::doHoughCircles(workImg, circles, doubleBuffer.at(0), doubleBuffer.at(1), doubleBuffer.at(2), doubleBuffer.at(3), intBuffer.at(0), intBuffer.at(1));
        if(!massRun){
            namedWindow("Hough Transform");
        openedCVWindowNames.append("Hough Transform");
        imshow("Hough Transform",workImg);
//        origImg = imread(imagePath.toStdString(),1);
//        ImageManip::drawCircles(origImg, circles);
//        namedWindow("Hough Transform2");
//        openedCVWindowNames.append("Hough Transform2");
//        imshow("Hough Transform2",origImg);

            ui->textEdit_setup->append(QString::number(algTime)+"ms : Hough Transform { "+QString::number(doubleBuffer.at(0))+"; "+QString::number(doubleBuffer.at(1))+"; "+QString::number(doubleBuffer.at(2))+"; "+QString::number(doubleBuffer.at(3))+"; "+QString::number(intBuffer.at(0))+"; "+QString::number(intBuffer.at(1))+"; }\nFound Circles: ");
            for(uint i=0; i<circles.size(); i++){
                ui->textEdit_setup->append("("+QString::number(circles[i][0])+","+QString::number(circles[i][1])+","+QString::number(circles[i][2])+")");
            }
        }
        doubleBuffer.pop_front();
        doubleBuffer.pop_front();
        doubleBuffer.pop_front();
        doubleBuffer.pop_front();
        intBuffer.pop_front();
        intBuffer.pop_front();
        break;

    case ImageManip::Inverse_flag:
        algTime = ImageManip::doInvert(workImg);
        if(!massRun){
            namedWindow("Inverse intensity");
            openedCVWindowNames.append("Inverse intensity");
            imshow("Inverse intensity",workImg);
            ui->textEdit_setup->append(QString::number(algTime)+"ms : Inverse intensity { ; }");
        }
        break;

    case ImageManip::PlusIntensity_flag:
        algTime = ImageManip::doPlusIntensity(workImg, intBuffer.at(0));
        if(!massRun){
            namedWindow("Intensity addition");
            openedCVWindowNames.append("Intensity addition");
            imshow("Intensity addition",workImg);

            ui->textEdit_setup->append(QString::number(algTime)+"ms : Intensity addition { "+QString::number(intBuffer.at(0))+"; }");
        }
        intBuffer.pop_front();
        break;

    case ImageManip::PseudoContrast_flag:
        algTime = ImageManip::doPseudoContrast(workImg, intBuffer.at(0));
        if(!massRun){
            namedWindow("Pseudo-Contrast");
        openedCVWindowNames.append("Pseudo-Contrast");
        imshow("Pseudo-Contrast",workImg);
            ui->textEdit_setup->append(QString::number(algTime)+"ms : Pseudo-Contrast { "+QString::number(intBuffer.at(0))+"; }");
        }
            intBuffer.pop_front();
        break;

    default:;
    }
    ui->actualTime->setText(QString().number(ui->actualTime->text().toFloat()+algTime));

}



//void MainWindow::on_horizontalSlider_sliderMoved(int position)
//{
//    ui->sliderValue->display(position+1);
//    ui->sliderValue->setFrameStyle(QFrame::Box | QFrame::Raised);
//    //ui->sliderValue->setWidth(1);
//    ui->sliderValue->setLineWidth(2);
//    ui->sliderValue->setStyleSheet("QLCDNumber {background: green; color: red;}");
////    if(openedCVWindowNames.indexOf("Blur + Canny edge detection")!=-1){
////        Canny(tempMat, edgeMat, position+1, (position+1)*3, 3);
////        imshow("Blur + Canny edge detection", edgeMat);
////    }
//}

//void MainWindow::on_horizontalSlider_valueChanged(int value)
//{
//    on_horizontalSlider_sliderMoved(value);
//}

void MainWindow::messageOutStringList(const QString listName, QStringList list){
    QMessageBox msg;
    msg.setText(listName);
    QString info;
    for(int i = 0; i< list.size(); i++){
        info.append(QString().number(i)+" - "+list.at(i)+"\n");
    }
    msg.setInformativeText(info);
    msg.exec();
}

void MainWindow::messageOutBuffersLists(const QString listName, QList<int> listInt, QList<double> listDouble, QList<bool> listBool){
    QMessageBox msg;
    msg.setText(listName);
    QString info;
    info.append(" ---- int buffer ---- \n");
    for(int i = 0; i< listInt.size(); i++){
        info.append(QString().number(i)+" - "+QString().number(listInt.at(i))+"\n");
    }
    info.append(" ---- double buffer ---- \n");
    for(int i = 0; i< listDouble.size(); i++){
        info.append(QString().number(i)+" - "+QString().number(listDouble.at(i))+"\n");
    }
    info.append(" ---- bool buffer ---- \n");
    for(int i = 0; i< listBool.size(); i++){
        if(listBool.at(i)){
            info.append(QString().number(i)+" - true\n");
        }else{
            info.append(QString().number(i)+" - false\n");
        }
    }
    msg.setInformativeText(info);
    msg.exec();
}


void MainWindow::debugWindow(const QString text){
    QMessageBox mess;
    mess.setText(text);
    mess.exec();

}

 //////////////////////////
//     SETUP BUTTONS    //
/////////////////////////

 /////////////////////////
//      RUN  SETUP     //
void MainWindow::on_pushButton_massRun_clicked()
{
    massRun=true;
    destroyAllCVWindows();
    while(listOfFiles.isEmpty()){
        on_pushButton_setFilesList_clicked();
    }
    //QStringList temp = listOfFiles;
    if(listOfFiles.size()<50)
        messageOutStringList("listOFiles",listOfFiles);
    //messageOutStringList("activeSetup",QStringList(activeSetup));
    //init setup
    if(!activeSetup.isEmpty()){
        lastSetup = activeSetup;
        lastIntBuffer = intBuffer;
        lastDoubleBuffer = doubleBuffer;
        lastBoolBuffer = boolBuffer;
    }
    int processedFiles = 0;
    if(activeSetup.size()>0){
        if(imageSaveDir.isEmpty()){
            while(!on_actionSet_Mass_Save_Directory_triggered());
        }else{
            QMessageBox msg;
            msg.setText("Actual save directory is:");
            msg.setInformativeText(imageSaveDir);
            QPushButton *okButton = msg.addButton(QMessageBox::Ok);
            QPushButton *changeButton = msg.addButton(tr("Change"),QMessageBox::ActionRole);
            msg.exec();
            if (msg.clickedButton() == changeButton)
            {
                // change the directory
                while(!on_actionSet_Mass_Save_Directory_triggered());
            } else if (msg.clickedButton() == okButton){
                // it`s ok nothing done
            }
        }
        int proc_number = 0;
        QString subdir_name = "processed_images_run_0000";
        QDir dir;
        dir.setCurrent(imageSaveDir);
        while(!dir.mkdir(subdir_name))
        {
            proc_number++;
            if(proc_number<10){
                subdir_name = "processed_images_run_000"+QString().number(proc_number);
            }else if(proc_number<100){
                subdir_name = "processed_images_run_00"+QString().number(proc_number);
            }else if(proc_number<1000){
                subdir_name = "processed_images_run_0"+QString().number(proc_number);
            }else{
                subdir_name = "processed_images_run_"+QString().number(proc_number);
            }
        }

        //debugWindow("directory created");
        //QList <ImageManip::SetupFlags> temp_setup = activeSetup;
        QString actual_file_prefix = imageSaveDir+"\\"+subdir_name+"\\processedImg_";

        //for all is time counted together
        ui->actualTime->setText("0");
        QProgressDialog progressDialog(this);
        progressDialog.setWindowModality(Qt::WindowModal);
        progressDialog.setCancelButtonText(tr("&Cancel"));
        progressDialog.setRange(0, listOfFiles.size());
        progressDialog.setWindowTitle(tr("Mass Run"));

        //debugWindow("starting for cycle");
        for(int files=0; files< listOfFiles.size(); files++){
            progressDialog.setValue(files);
            progressDialog.setLabelText(tr("Processing file number %1 of %2...").arg(files).arg(listOfFiles.size()));

            qApp->processEvents();

            if (progressDialog.wasCanceled())
                break;


            //begin - processing of image from list
            //debugWindow("work image will be : "+QString().fromStdString(listOfFiles.at(files).toStdString()));
            //tempPath = listOfFilePaths.at(files);
            workImg = imread(listOfFilePaths.at(files).toStdString(),0);
            origImg = workImg;
            //debugWindow("starting OpenCV functions");
            intBuffer = lastIntBuffer;
            doubleBuffer = lastDoubleBuffer;
            boolBuffer = lastBoolBuffer;
            for(int i=0; i<activeSetup.size(); i++){
                //messageOutBuffersLists(QString().number(i)+" run "+QString().number(activeSetup.at(i)), intBuffer, doubleBuffer, boolBuffer);

                if (progressDialog.wasCanceled())
                    break;

                callCVoperation(activeSetup.at(i));
            }

            QString actual_file = actual_file_prefix+listOfFiles.at(files)+".jpg";
            QString actual_window = "Processed image: "+listOfFiles.at(files);
            std::string actual_w = actual_window.toStdString();

            imwrite(actual_file.toStdString(),workImg);
            //show the resulting image processed
            if(listOfFiles.size()<50){
                ui->textEdit_setup->append("Processed: "+listOfFiles.at(files)+"\n------------");
                //debugWindow("starting creation of window");
                namedWindow(actual_w);
                openedCVWindowNames.append(actual_window);
                imshow(actual_w,workImg);
            }else{
                processedFiles++;
            }
            //end - processing of image from list end
        }
    }else{
        ui->textEdit_setup->append("Processed nothing as no setup found\n------------");
    }
    if(listOfFiles.size()>50){
        ui->textEdit_setup->append("Processed "+QString().number(processedFiles)+"number of files.\n------------");
    }
    ui->textEdit_setup->append("\n-------------------------------\nSetup File\n-------------------------------");
    updateGraphics_active();
}

void MainWindow::on_pushButton_run_clicked()
{
    massRun=false;
    destroyAllCVWindows();
    if(!activeSetup.isEmpty()){
        lastSetup = activeSetup;
        lastIntBuffer = intBuffer;
        lastDoubleBuffer = doubleBuffer;
        lastBoolBuffer = boolBuffer;
    }
    ui->actualTime->setText("0");
    if(imagePath.isEmpty()){
       on_actionLoad_picture_triggered();
    }else{
       workImg = imread(imagePath.toStdString(),0);
       origImg = workImg;
    }

    namedWindow("Original");
    openedCVWindowNames.append("Original");
    imshow("Original",workImg);

    ui->textEdit_setup->append("\n-------------------------------\nProcesed\n-------------------------------");

    while(!activeSetup.empty()){
        callCVoperation(activeSetup.at(0));
        activeSetup.pop_front();
        //wait for slow motion, maybe not here but in algorithm of HT or ACM
    }

    ui->textEdit_setup->append("\n-------------------------------\nSetup File\n-------------------------------");

    imwrite("tempImage.jpg",workImg);
    updateGraphics_active();
    namedWindow("Active");
    openedCVWindowNames.append("Active");
    imshow("Active",workImg);

    //namedWindow("Histogram");
    //openedCVWindowNames.append("Histogram");
    //imshow("Histogram",calcHist(workImg,1,workImg[0].channels(),););

//    ui->graphicsView->setMouseTracking(true);
//    showImgThoughOpenCV(imagePath);
    //createCVWindow();
}

void MainWindow::on_pushButton_clear_clicked()
{
    activeSetup.clear();
    intBuffer.clear();
    boolBuffer.clear();
    doubleBuffer.clear();
    ui->textEdit_setup->setText("Setup File\n-----------------------------------------------------");
}

void MainWindow::on_pushButton_undo_clicked()
{
//    switch( activeSetup.end()):
      //sem prijde kopia hotoveho funkcneho volania operacie CV osekaneho aby si nemusel dva krat pisat
//    activeSetup.pop_back();
}

 ////////////////////////////
//    SETUP CV BUTTONS    //
void MainWindow::on_pushButton_thresholdUnder_clicked()
{
    activeSetup.append(ImageManip::ThresholdUnder_flag);
    ui->textEdit_setup->append("Threshold Under { "+ui->spinBox_thresholdUnder_value->text()+"; }");
    intBuffer.append(ui->spinBox_thresholdUnder_value->value());
//    callCVoperation(ImageManip::ThresholdUnder_flag);
}


void MainWindow::on_pushButton_thresholdAbove_clicked()
{
    activeSetup.append(ImageManip::ThresholdAbove_flag);
    ui->textEdit_setup->append("Threshold Above { "+ui->spinBox_thresholdAbove_value->text()+"; }");
    intBuffer.append(ui->spinBox_thresholdAbove_value->value());
//    callCVoperation(ImageManip::ThresholdAbove_flag);
}

void MainWindow::on_pushButton_blur_clicked()
{
    activeSetup.append(ImageManip::Blur_flag);
    ui->textEdit_setup->append("Blur { "+ui->comboBox_blur_ksize->currentText()+"; }");
    intBuffer.append(ui->comboBox_blur_ksize->currentText().toInt());
    //   callCVoperation(ImageManip::Blur_flag);
}


void MainWindow::on_pushButton_medianBlur_clicked()
{
    activeSetup.append(ImageManip::MedianBlur_flag);
    ui->textEdit_setup->append("Median Blur { "+ui->comboBox_medianBlur_ksize->currentText()+"; }");
    intBuffer.append(ui->comboBox_medianBlur_ksize->currentText().toInt());
//    callCVoperation(ImageManip::MedianBlur_flag);
}

void MainWindow::on_pushButton_gausBlur_clicked()
{
    activeSetup.append(ImageManip::GausianBlur_flag);
    ui->textEdit_setup->append("Gausian Blur { "+ui->comboBox_gausianBlur_ksize->currentText()+"; "+ui->doubleSpinBox_gausianSigmaX->text()+"; "+ui->doubleSpinBox_gausianSigmaY->text()+"; }");
    intBuffer.append(ui->comboBox_gausianBlur_ksize->currentText().toInt());
    doubleBuffer.append(ui->doubleSpinBox_gausianSigmaX->value());
    doubleBuffer.append(ui->doubleSpinBox_gausianSigmaY->value());
    //    callCVoperation(ImageManip::GausianBlur_flag);
}

void MainWindow::on_pushButton_laplacian_clicked()
{
    activeSetup.append(ImageManip::Laplacian_flag);
    ui->textEdit_setup->append("Laplacian { "+ui->comboBox_laplacian_ksize->currentText()+"; "+ui->doubleSpinBox_laplacianScale->text()+"; "+ui->doubleSpinBox_laplacianDeltaIntensityOffset->text()+"; }");
    intBuffer.append(ui->comboBox_laplacian_ksize->currentText().toInt());
    doubleBuffer.append(ui->doubleSpinBox_laplacianScale->value());
    doubleBuffer.append(ui->doubleSpinBox_laplacianDeltaIntensityOffset->value());
}

void MainWindow::on_pushButton_scharr_clicked()
{
    activeSetup.append(ImageManip::Scharr_flag);
    ui->textEdit_setup->append("Scharr { "+ui->comboBox_scharr_xDerivOrder->currentText()+"; "+ui->comboBox_scharr_yDerivOrder->currentText()+"; "+ui->doubleSpinBox_scharr_scale->text()+"; "+ui->doubleSpinBox_scharr_DeltaIntensityOffset->text()+"; }");
    intBuffer.append(ui->comboBox_scharr_xDerivOrder->currentText().toInt());
    intBuffer.append(ui->comboBox_scharr_yDerivOrder->currentText().toInt());
    doubleBuffer.append(ui->doubleSpinBox_scharr_scale->value());
    doubleBuffer.append(ui->doubleSpinBox_scharr_DeltaIntensityOffset->value());
}

void MainWindow::on_comboBox_scharr_xDerivOrder_activated(const QString &arg1)
{
    if(arg1.toInt()==1){
        ui->comboBox_scharr_yDerivOrder->setCurrentIndex(0);
    }else{
        ui->comboBox_scharr_yDerivOrder->setCurrentIndex(1);
    }
}

void MainWindow::on_comboBox_scharr_yDerivOrder_activated(const QString &arg1)
{
    if(arg1.toInt()==1){
        ui->comboBox_scharr_xDerivOrder->setCurrentIndex(0);
    }else{
        ui->comboBox_scharr_xDerivOrder->setCurrentIndex(1);
    }
}

void MainWindow::on_pushButton_sobel_clicked()
{
    if(1<=(ui->comboBox_sobel_ksize->currentText().toInt()*ui->doubleSpinBox_sobel_scale->value()) && (ui->comboBox_sobel_ksize->currentText().toInt()*ui->doubleSpinBox_sobel_scale->value())<31){
        activeSetup.append(ImageManip::Sobel_flag);
        ui->textEdit_setup->append("Sobel { "+ui->comboBox_sobel_ksize->currentText()+"; "+ui->comboBox_sobel_xDerivOrder->currentText()+"; "+ui->comboBox_sobel_yDerivOrder->currentText()+"; "+ui->doubleSpinBox_sobel_scale->text()+"; "+ui->doubleSpinBox_sobel_DeltaIntensityOffset->text()+"; }");
        intBuffer.append(ui->comboBox_sobel_ksize->currentText().toInt());
        intBuffer.append(ui->comboBox_sobel_xDerivOrder->currentText().toInt());
        intBuffer.append(ui->comboBox_sobel_yDerivOrder->currentText().toInt());
        doubleBuffer.append(ui->doubleSpinBox_sobel_scale->value());
        doubleBuffer.append(ui->doubleSpinBox_sobel_DeltaIntensityOffset->value());
    }else{
        QMessageBox mess;
        mess.setText("Sobel cannot be processed\nSetup 1 <= Ksize x Scale < 31");
        mess.exec();
    }
}

void MainWindow::on_pushButton_canny_clicked()
{
    if (ui->spinBox_canny_threshold1->value()<ui->spinBox_canny_threshold2->value()){
        activeSetup.append(ImageManip::Canny_flag);
        if(ui->checkBox_canny_L2gradient->isChecked())
        {
            ui->textEdit_setup->append("Canny edge dettection {"+ui->comboBox_canny_appertureSize->currentText()+"; "+ui->spinBox_canny_threshold1->text()+"; "+ui->spinBox_canny_threshold2->text()+"; "+"true"+"; }");
        }else{
            ui->textEdit_setup->append("Canny edge dettection {"+ui->comboBox_canny_appertureSize->currentText()+"; "+ui->spinBox_canny_threshold1->text()+"; "+ui->spinBox_canny_threshold2->text()+"; "+"false"+"; }");
        }
        intBuffer.append(ui->comboBox_canny_appertureSize->currentText().toInt());
        intBuffer.append(ui->spinBox_canny_threshold1->value());
        intBuffer.append(ui->spinBox_canny_threshold2->value());
        boolBuffer.append(ui->checkBox_canny_L2gradient->isChecked());
    }else{
        QMessageBox mess;
        mess.setText("Canny cannot be processed\nSetup Threshold Upper > Threshold Lower");
        mess.exec();
    }
}

void MainWindow::on_pushButton_harris_clicked()
{
    activeSetup.append(ImageManip::CornerHarris_flag);
    ui->textEdit_setup->append("Harris { "+ui->spinBox_harris_blockSize->text()+"; "+ui->comboBox_harris_appertureSize->currentText()+"; "+ui->doubleSpinBox_harris_kFreeParam->text()+"; }");
    intBuffer.append(ui->spinBox_harris_blockSize->value());
    intBuffer.append(ui->comboBox_harris_appertureSize->currentText().toInt());
    doubleBuffer.append(ui->doubleSpinBox_harris_kFreeParam->value());

}

void MainWindow::on_pushButton_preCornerDetect_clicked()
{
    activeSetup.append(ImageManip::PreCornerDetect_flag);
    ui->textEdit_setup->append("Pre-corner Detection { "+ui->comboBox_preCornerDetect_appertureSize->currentText()+"; }");
    intBuffer.append(ui->comboBox_preCornerDetect_appertureSize->currentText().toInt());
}

void MainWindow::on_pushButton_houghTransform_clicked()
{
    activeSetup.append(ImageManip::HoughCircles_flag);
    ui->textEdit_setup->append("Hough Transformation { "+ui->doubleSpinBox_houghTransform_dp_invAcum->text()+"; "+ui->doubleSpinBox_houghTransform_minDist->text()+"; "+ui->doubleSpinBox_houghTransform_param1->text()+"; "+ui->doubleSpinBox_houghTransform_param2->text()+"; "+ui->spinBox_houghTransform_minRadius->text()+"; "+ui->spinBox_houghTransform_maxRadius->text()+"; }");
    doubleBuffer.append(ui->doubleSpinBox_houghTransform_dp_invAcum->value());
    doubleBuffer.append(ui->doubleSpinBox_houghTransform_minDist->value());
    doubleBuffer.append(ui->doubleSpinBox_houghTransform_param1->value());
    doubleBuffer.append(ui->doubleSpinBox_houghTransform_param2->value());
    intBuffer.append(ui->spinBox_houghTransform_minRadius->value());
    intBuffer.append(ui->spinBox_houghTransform_maxRadius->value());
}

void MainWindow::on_pushButton_inverseIntensity_clicked()
{
    activeSetup.append(ImageManip::Inverse_flag);
    ui->textEdit_setup->append("Inverse intensity { ; }");
}

void MainWindow::on_pushButton_intensityAdd_clicked()
{
    activeSetup.append(ImageManip::PlusIntensity_flag);
    ui->textEdit_setup->append("Intensity Addition { "+ui->spinBox_intensityAdd->text()+"; }");
    intBuffer.append(ui->spinBox_intensityAdd->value());
}

void MainWindow::on_pushButton_pseudoContrast_clicked()
{
    activeSetup.append(ImageManip::PseudoContrast_flag);
    ui->textEdit_setup->append("Pseudo Contrast { "+ui->spinBox_pseudoContrast->text()+"; }");
    intBuffer.append(ui->spinBox_pseudoContrast->value());
}

void MainWindow::on_pushButton_bilateralSmoothing_clicked()
{
    activeSetup.append(ImageManip::BilateralSmoothing_flag);
    ui->textEdit_setup->append("Bilateral Filtering { "+ui->comboBox_bilateralFilter_ksize->currentText()+"; "+ui->doubleSpinBox_bilateralFilterSigmaColor->text()+"; "+ui->doubleSpinBox_bilateralFilterSigmaSpace->text()+"; }");
    intBuffer.append(ui->comboBox_bilateralFilter_ksize->currentText().toInt());
    doubleBuffer.append(ui->doubleSpinBox_bilateralFilterSigmaColor->value());
    doubleBuffer.append(ui->doubleSpinBox_bilateralFilterSigmaSpace->value());
}

void MainWindow::on_pushButton_pupilFilterSetup_clicked()
{
    int setup[5]={60, 190, 3, 150, 160};
    activeSetup.append(ImageManip::ThresholdUnder_flag);
    activeSetup.append(ImageManip::ThresholdAbove_flag);
    activeSetup.append(ImageManip::MedianBlur_flag);
    activeSetup.append(ImageManip::PlusIntensity_flag);
    activeSetup.append(ImageManip::ThresholdUnder_flag);
    intBuffer.append(setup[0]);
    intBuffer.append(setup[1]);
    intBuffer.append(3);
    intBuffer.append(setup[3]);
    intBuffer.append(setup[4]);
    ui->textEdit_setup->append("Threshold Under { "+QString().number(setup[0])+"; }");
    ui->textEdit_setup->append("Threshold Above { "+QString().number(setup[1])+"; }");
    ui->textEdit_setup->append("Median Blur { "+QString().number(setup[2])+"; }");
    ui->textEdit_setup->append("Intensity Addition { "+QString().number(setup[3])+"; }");
    ui->textEdit_setup->append("Threshold Under { "+QString().number(setup[4])+"; }");
}

 //////////////////////////////
//     SETUP CV ACTIONS     //

void MainWindow::on_actionHarris_triggered()
{
    on_pushButton_harris_clicked();
}

void MainWindow::on_actionMedian_Blur_triggered()
{
    on_pushButton_medianBlur_clicked();
}

void MainWindow::on_actionBlur_triggered()
{
    on_pushButton_blur_clicked();
}

void MainWindow::on_actionGausian_Blur_triggered()
{
    on_pushButton_gausBlur_clicked();
}

void MainWindow::on_actionThreshold_Under_triggered()
{
    on_pushButton_thresholdUnder_clicked();
}

void MainWindow::on_actionThreshold_Above_triggered()
{
    on_pushButton_thresholdAbove_clicked();
}

void MainWindow::on_actionLaplacian_triggered()
{
    on_pushButton_laplacian_clicked();
}

void MainWindow::on_actionSobel_triggered()
{
    on_pushButton_sobel_clicked();
}

void MainWindow::on_actionScharr_triggered()
{
    on_pushButton_scharr_clicked();
}

void MainWindow::on_actionCanny_triggered()
{
    on_pushButton_canny_clicked();
}

void MainWindow::on_actionHough_Transformation_triggered()
{
    on_pushButton_houghTransform_clicked();
}

void MainWindow::on_actionPupil_Filter_Setup_triggered()
{
    on_pushButton_pupilFilterSetup_clicked();
}

void MainWindow::on_actionBilateral_Filtering_triggered()
{
    on_pushButton_bilateralSmoothing_clicked();
}

 //////////////////////////
//     GUI ACTIONS      //

//needs to be resized properely to take effect of firefox f11
//void MainWindow::on_actionFull_screen_mode_triggered()
//{
//    if(ui->actionFull_screen_mode->isChecked() || this->isFullScreen()){
//        this->setWindowState(this->windowState() ^ Qt::WindowMaximized);
//    }else{
//        if(ui->actionFull_screen_mode->isChecked())
//            this->setWindowState(this->windowState() ^ Qt::WindowMaximized);
//        if(!ui->actionFull_screen_mode->isChecked())
//            this->setWindowState(this->windowState() ^ Qt::WindowFullScreen);
//        if(this->isFullScreen())
//            this->setWindowState(this->windowState() ^ Qt::WindowMaximized);

//    }
//}
void MainWindow::on_actionExit_triggered()
{
    this->close();
}

void MainWindow::on_textEdit_setup_customContextMenuRequested(const QPoint &pos)
{
    int x = pos.x();
    int y = pos.y();
    x += this->pos().x() + 170;
    y += this->pos().y() + 100;
    QPoint *th = new QPoint(x,y);
    //*th.operator +=( pos);
    //th.setX(th.x()+170);
    //th.setY(th.y() + 60);
    ui->menuSetup->exec(*th);
}


void MainWindow::on_actionRun_Setup_triggered()
{
    on_pushButton_run_clicked();
}

void MainWindow::on_actionClear_Setup_triggered()
{
    on_pushButton_clear_clicked();
}

void MainWindow::on_actionSave_Setup_File_triggered()
{
    writeTextFile("Save Setup File", ui->textEdit_setup->toPlainText());
}

bool MainWindow::writeTextFile(QString windowName, QString text)
{
    QString path;
    //while(path.isEmpty()){
        path = QFileDialog::getSaveFileName(this, windowName, QDir::currentPath(), tr("Text files(*.txt)"));
    //}
    QFile outputFile(path);
    if(outputFile.open(QIODevice::WriteOnly | QFile::Text)){
        QTextStream outText(&outputFile);
        outText << text;
        outputFile.close();
        return true;
    }else{
        QMessageBox mess;
        mess.setText("Could not save file");
        mess.exec();
    }
    return false;
}

void MainWindow::on_actionLoad_Setup_File_triggered()
{
    QString loadedText = readTextFile("Load Setup File");
    if(!loadedText.isEmpty()){
        activeSetup.clear();
        intBuffer.clear();
        boolBuffer.clear();
        doubleBuffer.clear();
        ui->textEdit_setup->setText(loadedText);
    }
}

QString MainWindow::readTextFile(QString windowName)
{
    QString retVal;
    QString path;
    //while(path.isEmpty()){
        path = QFileDialog::getOpenFileName(this, windowName, QDir::currentPath(), tr("Text files(*.txt)"));
    //}
        QFile inputFile(path);
        if (!inputFile.open(QIODevice::ReadOnly | QIODevice::Text)){
            QMessageBox mess;
            mess.setText("Could not open file\n");
            mess.exec();
        }else{
            QTextStream in(&inputFile);
            while (!in.atEnd()) {
                retVal = in.readAll();
            }
            inputFile.close();
        }
    return retVal;
}

void MainWindow::on_actionLast_Setup_triggered()
{
    if(lastSetup.isEmpty()){
        QMessageBox mess;
        mess.setText("No Last Setup found");
        mess.exec();
    }else{
        ui->textEdit_setup->append("Last Setup Loaded (same as above)");
        activeSetup = lastSetup;
        intBuffer = lastIntBuffer;
        doubleBuffer = lastDoubleBuffer;
        boolBuffer = lastBoolBuffer;
    }
}


void MainWindow::on_pushButton_snakePromo_clicked()
{
    //runs snake
    if(imagePath.isEmpty()){
       on_actionLoad_picture_triggered();
    }else{
       workImg = imread(imagePath.toStdString(),0);
       origImg = workImg;
    }
    openedCVWindowNames.append("Snake After Init");
    Snake *activeContour = new Snake(workImg);
    activeContour->initSnakeContour(activeContour, 300,
                                    EnergyInternalTemplate::ClosedContour_Circle,
                                    EnergyExternalField::GradientMagnitudes,
                                    0, 0, 0.1, 0,
                                    50,
                                    5 );
    namedWindow("Snake Orig Matrix");
    openedCVWindowNames.append("Snake Orig Matrix");
    imshow("Snake Orig Matrix",activeContour->originalImage);

    namedWindow("VectorField");
    openedCVWindowNames.append("VectorField");
    imshow("VectorField",activeContour->vectorField->getVectorField());
    namedWindow("Snake Point Matrix");
    openedCVWindowNames.append("Snake Point Matrix");
    imshow("Snake Point Matrix",activeContour->matrixOfPoints);

    //activeContour->moveSnakeContour(activeContour);
    namedWindow("Snake Show Matrix");
    openedCVWindowNames.append("Snake Show Matrix");
    imshow("Snake Show Matrix",activeContour->showImage);

}
