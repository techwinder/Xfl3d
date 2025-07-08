/****************************************************************************

    Xfl3d application
    Copyright (C) Andre Deperrois
    License: GPL v3

*****************************************************************************/

#pragma once

#include <QFileDialog>
#include <QSettings>

class XflFileDialog : public QFileDialog
{
    public:
        XflFileDialog();

        void showEvent(QShowEvent*) override;
        void hideEvent(QHideEvent*) override;

        static void loadSettings(QSettings &settings);
        static void saveSettings(QSettings &settings);

    private:
        static QByteArray s_Geometry;
};

