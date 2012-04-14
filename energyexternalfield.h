#ifndef EXTERNALENERGYFIELD_H
#define EXTERNALENERGYFIELD_H

#include <QMainWindow>
#include <QtGUI>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <qmath.h>

class ExternalEnergyField
{
protected:
    CvMat vectorField;
public:
    ExternalEnergyField();

};

#endif // EXTERNALENERGYFIELD_H
