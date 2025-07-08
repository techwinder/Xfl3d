/****************************************************************************

    Xfl3d application
    Copyright (C) Andre Deperrois
    License: GPL v3

*****************************************************************************/

#include "xflfiledialog.h"


QByteArray XflFileDialog::s_Geometry;


XflFileDialog::XflFileDialog()
{
}


void XflFileDialog::loadSettings(QSettings &settings)
{
    settings.beginGroup("XflFileDialog");
    {
        s_Geometry = settings.value("Geometry").toByteArray();
    }
    settings.endGroup();
}


void XflFileDialog::saveSettings(QSettings &settings)
{
    settings.beginGroup("XflFileDialog");
    {
        settings.setValue("Geometry", s_Geometry);
    }
    settings.endGroup();
}


void XflFileDialog::showEvent(QShowEvent*)
{
    restoreGeometry(s_Geometry);
}


void XflFileDialog::hideEvent(QHideEvent*)
{
    s_Geometry = saveGeometry();
}

