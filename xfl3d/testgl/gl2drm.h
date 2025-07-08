/****************************************************************************

    Xfl3d
    Copyright (C) André Deperrois
    GNU General Public License v3

*****************************************************************************/

#pragma once

#include <QOpenGLShaderProgram>
#include <QSettings>
#include <QLabel>

#include <xfl3d/views/gl2dview.h>

class ExponentialSlider;

class gl2dRM : public gl2dView
{
    Q_OBJECT
    public:
        gl2dRM(QWidget *pParent = nullptr);

        void initializeGL() override;
        void glRenderView()  override;

        QPointF defaultOffset() override {return QPointF(0.0f,0.0f);}

        static void loadSettings(QSettings &settings);
        static void saveSettings(QSettings &settings);

    private slots:
        void onParamChanged();

    private:
        QOpenGLShaderProgram m_shadRM;

        // shader uniforms
        int m_locViewTrans;
        int m_locViewScale;
        int m_locViewRatio;
        int m_locEyePos, m_locLightPos;

        //shader attributes
        int m_attrXY;

        ExponentialSlider *m_peslXLight, *m_peslYLight, *m_peslZLight;
        ExponentialSlider *m_peslXEye, *m_peslYEye, *m_peslZEye;

        static QVector3D s_LightPos;
        static QVector3D s_EyePos;
};


