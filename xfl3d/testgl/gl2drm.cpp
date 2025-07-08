/****************************************************************************

    Xfl3d
    Copyright (C) André Deperrois
    GNU General Public License v3

*****************************************************************************/

#include <QApplication>
#include <QGroupBox>
#include <QGridLayout>
#include <QLabel>

#include "gl2drm.h"

#include <xflcore/displayoptions.h>
#include <xflcore/trace.h>
#include <xfl3d/globals/gl_globals.h>
#include <xfl3d/views/gl3dview.h> // for the static variables
#include <xflwidgets/customwts/intedit.h>
#include <xflwidgets/customwts/exponentialslider.h>
#include <xflwidgets/wt_globals.h>

#define REFLENGTH 50.0

QVector3D gl2dRM::s_LightPos(2.5f,2.5f,5.0f);
QVector3D gl2dRM::s_EyePos(0.0f,0.0f,-10.0f);


gl2dRM::gl2dRM(QWidget *pParent) : gl2dView(pParent)
{
    setWindowTitle("Ray marching");
    setFocusPolicy(Qt::WheelFocus);
    setCursor(Qt::CrossCursor);

    m_bAxes = false;

    m_attrXY = -1;
    m_locViewTrans = -1;
    m_locViewScale = -1;
    m_locViewRatio = -1;
    m_locLightPos = m_locEyePos = -1;

    m_rectView = QRectF(-1., -1.0, 2.0, 2.0);

    QFrame *pFrame = new QFrame(this);
    {
        QPalette palette;
        palette.setColor(QPalette::WindowText, DisplayOptions::textColor());
        palette.setColor(QPalette::Text,  DisplayOptions::textColor());
        QColor clr = DisplayOptions::backgroundColor();
        clr.setAlpha(0);
        palette.setColor(QPalette::Window, clr);
        palette.setColor(QPalette::Base, clr);
        pFrame->setCursor(Qt::ArrowCursor);

        pFrame->setFrameShape(QFrame::NoFrame);
        pFrame->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);

        QVBoxLayout*pFrameLayout = new QVBoxLayout;
        {
            QLabel *pLabTitle = new QLabel("Simulating ray marching in OpenGL's fragment shader");

            QGroupBox *pEyePositionBox = new QGroupBox("Eye position, view-space");
            {
                QGridLayout *pEyePosition = new QGridLayout;
                {
                    QLabel *pLab21 = new QLabel("x");
                    QLabel *pLab22 = new QLabel("y");
                    QLabel *pLab23 = new QLabel("z");

                    m_peslXEye = new ExponentialSlider(true, 2.0, Qt::Horizontal);
                    m_peslYEye = new ExponentialSlider(true, 2.0, Qt::Horizontal);
                    m_peslZEye = new ExponentialSlider(true, 2.0, Qt::Horizontal);
                    m_peslXEye->setTickPosition(QSlider::TicksBelow);
                    m_peslYEye->setTickPosition(QSlider::TicksBelow);
                    m_peslZEye->setTickPosition(QSlider::TicksBelow);

                    pEyePosition->addWidget(pLab21,        1,1);
                    pEyePosition->addWidget(pLab22,        2,1);
                    pEyePosition->addWidget(pLab23,        3,1);
                    pEyePosition->addWidget(m_peslXEye,    1,2);
                    pEyePosition->addWidget(m_peslYEye,    2,2);
                    pEyePosition->addWidget(m_peslZEye,    3,2);
                    pEyePositionBox->setLayout(pEyePosition);
                }
            }
            QGroupBox *pLightPositionBox = new QGroupBox("Light position, view-space");
            {
                QGridLayout *pLightPosition = new QGridLayout;
                {
                    QLabel *pLab21 = new QLabel("x");
                    QLabel *pLab22 = new QLabel("y");
                    QLabel *pLab23 = new QLabel("z");

                    m_peslXLight = new ExponentialSlider(true, 2.0, Qt::Horizontal);
                    m_peslYLight = new ExponentialSlider(true, 2.0, Qt::Horizontal);
                    m_peslZLight = new ExponentialSlider(true, 2.0, Qt::Horizontal);
                    m_peslXLight->setTickPosition(QSlider::TicksBelow);
                    m_peslYLight->setTickPosition(QSlider::TicksBelow);
                    m_peslZLight->setTickPosition(QSlider::TicksBelow);

                    pLightPosition->addWidget(pLab21,          1,1);
                    pLightPosition->addWidget(pLab22,          2,1);
                    pLightPosition->addWidget(pLab23,          3,1);
                    pLightPosition->addWidget(m_peslXLight,    1,2);
                    pLightPosition->addWidget(m_peslYLight,    2,2);
                    pLightPosition->addWidget(m_peslZLight,    3,2);
                    pLightPositionBox->setLayout(pLightPosition);
                }
            }
            pFrameLayout->addWidget(pLabTitle);
            pFrameLayout->addWidget(pEyePositionBox);
            pFrameLayout->addWidget(pLightPositionBox);
        }

        pFrame->setLayout(pFrameLayout);
        pFrame->setStyleSheet("QFrame{background-color: transparent;}");
        setWidgetStyle(pFrame, palette);
    }

    // adding a x100 factor to enable small move intervals
    m_peslXEye->setRange(-int(REFLENGTH*100), int(REFLENGTH*100));
    m_peslYEye->setRange(-int(REFLENGTH*100), int(REFLENGTH*100));
    m_peslZEye->setRange(-int(REFLENGTH*100), int(REFLENGTH*100));
    m_peslXEye->setTickInterval(int(REFLENGTH*10.0));
    m_peslYEye->setTickInterval(int(REFLENGTH*10.0));
    m_peslZEye->setTickInterval(int(REFLENGTH*10.0));
    m_peslXEye->setExpValuef( 0.0*100.0f);
    m_peslYEye->setExpValuef( 0.0*100.0f);
    m_peslZEye->setExpValuef(-5.0*100.0f);

    m_peslXLight->setRange(-int(REFLENGTH*100), int(REFLENGTH*100));
    m_peslYLight->setRange(-int(REFLENGTH*100), int(REFLENGTH*100));
    m_peslZLight->setRange(-int(REFLENGTH*100), int(REFLENGTH*100));
    m_peslXLight->setTickInterval(int(REFLENGTH*10.0));
    m_peslYLight->setTickInterval(int(REFLENGTH*10.0));
    m_peslZLight->setTickInterval(int(REFLENGTH*10.0));
    m_peslXLight->setExpValuef( 2.5*100.0f);
    m_peslYLight->setExpValuef( 2.5*100.0f);
    m_peslZLight->setExpValuef(-2.5*100.0f);

    connect(m_peslXEye,        SIGNAL(valueChanged(int)), SLOT(onParamChanged()));
    connect(m_peslYEye,        SIGNAL(valueChanged(int)), SLOT(onParamChanged()));
    connect(m_peslZEye,        SIGNAL(valueChanged(int)), SLOT(onParamChanged()));

    connect(m_peslXLight,      SIGNAL(valueChanged(int)), SLOT(onParamChanged()));
    connect(m_peslYLight,      SIGNAL(valueChanged(int)), SLOT(onParamChanged()));
    connect(m_peslZLight,      SIGNAL(valueChanged(int)), SLOT(onParamChanged()));
}


void gl2dRM::loadSettings(QSettings &settings)
{
    settings.beginGroup("gl3dRM");
    {
        s_EyePos.setX(settings.value("EyePosX", s_EyePos.x()).toFloat());
        s_EyePos.setY(settings.value("EyePosY", s_EyePos.y()).toFloat());
        s_EyePos.setZ(settings.value("EyePosZ", s_EyePos.z()).toFloat());
        s_LightPos.setX(settings.value("LightPosX", s_LightPos.x()).toFloat());
        s_LightPos.setY(settings.value("LightPosY", s_LightPos.y()).toFloat());
        s_LightPos.setZ(settings.value("LightPosZ", s_LightPos.z()).toFloat());
    }
    settings.endGroup();
}


void gl2dRM::saveSettings(QSettings &settings)
{
    settings.beginGroup("gl3dRM");
    {
        settings.setValue("EyePosX", s_EyePos.x());
        settings.setValue("EyePosY", s_EyePos.y());
        settings.setValue("EyePosZ", s_EyePos.z());
        settings.setValue("LightPosX", s_LightPos.x());
        settings.setValue("LightPosY", s_LightPos.y());
        settings.setValue("LightPosZ", s_LightPos.z());
    }
    settings.endGroup();
}


void gl2dRM::onParamChanged()
{
    s_EyePos.setX(m_peslXEye->expValuef()/100.0f);
    s_EyePos.setY(m_peslYEye->expValuef()/100.0f);
    s_EyePos.setZ(m_peslZEye->expValuef()/100.0f);
    s_LightPos.setX(m_peslXLight->expValuef()/100.0f);
    s_LightPos.setY(m_peslYLight->expValuef()/100.0f);
    s_LightPos.setZ(m_peslZLight->expValuef()/100.0f);
    update();
}


void gl2dRM::initializeGL()
{
    QString strange, vsrc, fsrc;
    vsrc = ":/shaders/raymarching/raymarching_VS.glsl";
    m_shadRM.addShaderFromSourceFile(QOpenGLShader::Vertex, vsrc);
    if(m_shadRM.log().length())
    {
        strange = QString::asprintf("%s", QString("raymarching vertex shader log:"+m_shadRM.log()).toStdString().c_str());
        trace(strange);
    }

    fsrc = ":/shaders/raymarching/raymarching_FS.glsl";
    m_shadRM.addShaderFromSourceFile(QOpenGLShader::Fragment, fsrc);
    if(m_shadRM.log().length())
    {
        strange = QString::asprintf("%s", QString("raymarching fragment shader log:"+m_shadRM.log()).toStdString().c_str());
        trace(strange);
    }

    m_shadRM.link();
    m_shadRM.bind();
    {
        m_attrXY       = m_shadRM.attributeLocation("vertexXY");
        m_locViewTrans = m_shadRM.uniformLocation("ViewTrans");
        m_locViewScale = m_shadRM.uniformLocation("ViewScale");
        m_locViewRatio = m_shadRM.uniformLocation("ViewRatio");
        m_locEyePos    = m_shadRM.uniformLocation("EyePosition");
        m_locLightPos  = m_shadRM.uniformLocation("LightPosition");
    }
    m_shadRM.release();

    gl::makeQuad2d(m_rectView, m_vboQuad);
}


void gl2dRM::glRenderView()
{
    QOpenGLVertexArrayObject::Binder vaoBinder(&m_vao);
    m_shadRM.bind();
    {
        m_shadRM.setUniformValue(m_locViewRatio, float(width())/float(height()));

        double w = 2.0;
        QVector2D off(-m_ptOffset.x()/width()*w, m_ptOffset.y()/width()*w);
        m_shadRM.setUniformValue(m_locViewTrans, off);
        m_shadRM.setUniformValue(m_locViewScale, m_fScale);

        m_shadRM.setUniformValue(m_locLightPos, -s_LightPos.x(), -s_LightPos.y(), -s_LightPos.z());
        m_shadRM.setUniformValue(m_locEyePos, s_EyePos.x(), s_EyePos.y(), s_EyePos.z());

        m_vboQuad.bind();
        {
            int stride = 2;
            m_shadRM.enableAttributeArray(m_attrXY);
            m_shadRM.setAttributeBuffer(m_attrXY, GL_FLOAT, 0*sizeof(GLfloat), 2, stride*sizeof(GLfloat));

            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            glDisable(GL_CULL_FACE);

            int nvtx = m_vboQuad.size()/stride/int(sizeof(float));
            glDrawArrays(GL_TRIANGLE_STRIP, 0, nvtx);
            m_shadRM.disableAttributeArray(m_attrXY);
        }
        m_vboQuad.release();
    }
    m_shadRM.release();
}



