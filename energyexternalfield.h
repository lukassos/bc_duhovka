#ifndef ENERGYEXTERNALFIELD_H
#define ENERGYEXTERNALFIELD_H

#include <QMainWindow>
#include <QtGUI>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <qmath.h>

#include "crossInclude.h"

class EnergyExternalField
{
protected:
    CvMat vectorField;
public:
    EnergyExternalField();

};

#endif // ENERGYEXTERNALFIELD_H
