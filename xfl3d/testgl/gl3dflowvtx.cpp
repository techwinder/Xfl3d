/****************************************************************************

    flow5 application
    Copyright (C) Andre Deperrois
    All rights reserved.

*****************************************************************************/

#include <QDebug>
#include <QApplication>
#include <QtConcurrent/QtConcurrent>
#include <QGuiApplication>
#include <QScreen>
#include <QImage>
#include <QRandomGenerator>
#include <QHBoxLayout>
#include <QPushButton>
#include <QStandardPaths>


#include "gl3dflowvtx.h"
#include <xflwidgets/wt_globals.h>
#include <xflwidgets/customwts/floatedit.h>
#include <xflwidgets/customwts/intedit.h>
#include <xflcore/displayoptions.h>
#include <xflcore/trace.h>
#include <xfl3d/globals/gl_globals.h>
#include <xfl3d/controls/w3dprefs.h>

#define GROUP_SIZE 64
#define REFLENGTH 1.0f

int gl3dFlowVtx::s_NGroups(64);
float gl3dFlowVtx::s_dt(0.001f);
float gl3dFlowVtx::s_VInf(10.0f);
float gl3dFlowVtx::s_Gamma(1.0f);

gl3dFlowVtx::gl3dFlowVtx(QWidget *pParent) : gl3dTestGLView(pParent)
{
    setWindowTitle("Flow");

    QPalette palette;
    palette.setColor(QPalette::WindowText, DisplayOptions::textColor());
    palette.setColor(QPalette::Text, DisplayOptions::textColor());

    QColor clr = DisplayOptions::backgroundColor();
    clr.setAlpha(0);
    palette.setColor(QPalette::Window, clr);
    palette.setColor(QPalette::Base, clr);

    QFrame *pFrame = new QFrame(this);
    {
        pFrame->setCursor(Qt::ArrowCursor);

        pFrame->setFrameShape(QFrame::NoFrame);
        pFrame->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
//        pFrame->setMinimumWidth(550);

        QGridLayout*pMainLayout = new QGridLayout;
        {            
            QLabel *plabTitle = new QLabel("<p>Using OpenGL's compute shader to compute<br>the flow around a horseshoe vortex</p>");

            QLabel *plabGroupSize = new QLabel(QString::asprintf("Group size = %d", GROUP_SIZE));
            m_plabNMaxGroups = new QLabel("Max. number of groups");
            QLabel *plabNGroups = new QLabel("Number of groups =");
            m_pieNGroups = new IntEdit(s_NGroups);
            m_pieNGroups->setToolTip("The number of groups should be less than the max. number of groups accepted by the GPU.<br>"
                                     "The GroupSize is hard-coded in the compute shader.<br>"
                                     "The number of particles is NGroups x GroupSize.<br>"
                                     "Note that in the present case the main limitations to the number of groups are:"
                                     "<ul>"
                                     "<li>the processing time in the compute shader</li>"
                                     "<li>the number of particles that can be rendered without loss of frame rate</li>"
                                     "</ul>");

            m_plabNParticles = new QLabel;

            QLabel *plabGamma = new QLabel("<p>&Gamma;=</p>");
            m_pfeGamma = new FloatEdit(s_Gamma);
            m_pfeGamma->setToolTip("The vortex's circulation (i.e. strength)");

            QLabel *plabVInf = new QLabel("V<sub>&infin;</sub>=");
            m_pfeVInf = new FloatEdit(s_VInf);
            m_pfeVInf->setToolTip("The velocity of the incoming flow");

            QLabel *plabDt = new QLabel("dt=");
            m_pfeDt = new FloatEdit(s_dt);
            m_pfeDt->setToolTip("<p>The time step of the Runge-Kutta scheme.<br>"
                                "The boids (particles) are moved every 1/60 seconds by an increment of V.dt</p>");

            QPushButton *ppbPause = new QPushButton("Pause/Resume");
            connect(ppbPause, SIGNAL(clicked()), SLOT(onPause()));

            pMainLayout->addWidget(plabTitle,           1, 1, 1, 2);
            pMainLayout->addWidget(plabGroupSize,       2, 1);
            pMainLayout->addWidget(plabNGroups,         3, 1);
            pMainLayout->addWidget(m_pieNGroups,        3, 2);
            pMainLayout->addWidget(m_plabNParticles,    4,1,1,2);
            pMainLayout->addWidget(plabGamma,           5, 1);
            pMainLayout->addWidget(m_pfeGamma,          5, 2);
            pMainLayout->addWidget(plabVInf,            8, 1);
            pMainLayout->addWidget(m_pfeVInf,           8, 2);
            pMainLayout->addWidget(plabDt,              9, 1);
            pMainLayout->addWidget(m_pfeDt,             9, 2);

            pMainLayout->addWidget(ppbPause,            12,1,1,2);

            pMainLayout->setColumnStretch(3,1);
            pMainLayout->setRowStretch(11,1);
        }
        pFrame->setLayout(pMainLayout);
        pFrame->setStyleSheet("QFrame{background-color: transparent;}");
        setWidgetStyle(pFrame, palette);
    }

    m_bResetBoids = true;
    m_bResetVortices = true;

    m_locGamma = -1;
    m_locVInf = -1;
    m_locNVortices = -1;
    m_locDt = -1;
    m_locRandSeed = -1;


    m_Period = 17;
//    m_Period = int(1000.0/QGuiApplication::primaryScreen()->refreshRate());
//    qDebug()<<"refreshrate="<<QGuiApplication::primaryScreen()->refreshRate()<<period;
    connect(&m_Timer, SIGNAL(timeout()), SLOT(moveThem()));


    connect(m_pieNGroups, SIGNAL(intChanged(int)), SLOT(onRestart()));

    onRestart();
}


void gl3dFlowVtx::loadSettings(QSettings &settings)
{
    settings.beginGroup("gl3dFlowVtx");
    {
        s_NGroups    = settings.value("NGroups",      s_NGroups).toInt();
        s_Gamma      = settings.value("Gamma",        s_Gamma).toFloat();
        s_VInf       = settings.value("VInf",         s_VInf).toFloat();
        s_dt         = settings.value("dt",           s_dt).toFloat();
    }
    settings.endGroup();
}


void gl3dFlowVtx::saveSettings(QSettings &settings)
{
    settings.beginGroup("gl3dFlowVtx");
    {
        settings.setValue("NGroups",      s_NGroups);
        settings.setValue("Gamma",        s_Gamma);
        settings.setValue("VInf",         s_VInf);
        settings.setValue("dt",           s_dt);
    }
    settings.endGroup();
}


void gl3dFlowVtx::readParams()
{
    s_NGroups   = m_pieNGroups->value();
}


void gl3dFlowVtx::makeVortices()
{
    m_Vortex.vertex(0).x = 0.0;
    m_Vortex.vertex(0).y = -1.0;
    m_Vortex.vertex(0).z = 0.5;

    m_Vortex.vertex(1).x = 0.0;
    m_Vortex.vertex(1).y = 1.0;
    m_Vortex.vertex(1).z = 0.5;

    m_Vortex.setCirculation(s_Gamma);
}


void gl3dFlowVtx::makeBoids()
{
    int NBoids = s_NGroups * GROUP_SIZE;
     m_Boid.resize(NBoids);

/*    float inc = REFLENGTH / float(GROUP_SIZE) * 3.0f;
    for(int i=0; i<s_NGroups; i++)
    {
        float y = -REFLENGTH*1.5;
        for(int j=0; j<GROUP_SIZE; j++)
        {
            int index = i*GROUP_SIZE+j;
            Boid & boid = m_Boid[index];
            boid.Index = index;
            boid.m_Position.set(-REFLENGTH/2.0, y, -REFLENGTH/5 + float(i)/float(s_NGroups)*REFLENGTH*1.5);
            y += inc;
            boid.m_Velocity.set(0.1,0,0);
        }
    }*/

/*    for(int inboid=0; inboid<NBoids; inboid++)
    {
        float row = float(inboid)/float(GROUP_SIZE);
        float col = float(inboid%GROUP_SIZE);
        Boid & boid = m_Boid[inboid];
        boid.m_Position.x = -REFLENGTH/2.0f;
        boid.m_Position.y = -REFLENGTH*1.5f + col * REFLENGTH / float(GROUP_SIZE) * 3.0f;
        boid.m_Position.z = -REFLENGTH/5.0f + row/float(s_NGroups)*REFLENGTH*1.5;
        boid.Index = inboid;
        boid.m_Velocity.set(0.1,0,0);
    }*/
     for(int inboid=0; inboid<NBoids; inboid++)
     {
         Boid & boid = m_Boid[inboid];
         boid.m_Position.x = -REFLENGTH/2.0f+QRandomGenerator::global()->generateDouble()*REFLENGTH*3.0;
         boid.m_Position.y = (QRandomGenerator::global()->generateDouble()*2.0-1.0)*REFLENGTH;
         boid.m_Position.z = -REFLENGTH/5.0f+QRandomGenerator::global()->generateDouble()*REFLENGTH*1.5;
         boid.Index = inboid;
         boid.m_Velocity.set(0.1,0,0);
     }
}


void gl3dFlowVtx::onPause()
{
    if(m_Timer.isActive())
    {
        m_Timer.stop();
    }
    else
    {
        m_Timer.start(m_Period); //  As a special case, a QTimer with a timeout of 0 will time out as soon as possible
    }
}


void gl3dFlowVtx::onRestart()
{
    m_Timer.stop();
    readParams();
    makeVortices();
    makeBoids();
    m_bResetBoids = true;
    m_bResetVortices = true;

    m_Timer.start(m_Period); //  As a special case, a QTimer with a timeout of 0 will time out as soon as possible
}


void gl3dFlowVtx::initializeGL()
{
    gl3dTestGLView::initializeGL();
    QString csrc, strange;

    csrc = ":/shaders/flow/flowVtx_CS.glsl";
    m_shadCompute.addShaderFromSourceFile(QOpenGLShader::Compute, csrc);
    if(m_shadCompute.log().length())
    {
        strange = QString::asprintf("%s", QString("Compute shader log:"+m_shadCompute.log()).toStdString().c_str());
        trace(strange);
    }

    if(!m_shadCompute.link())
    {
        trace("Compute shader is not linked");
    }
    m_shadCompute.bind();
    {
        m_locGamma     = m_shadCompute.uniformLocation("gamma");
        m_locVInf      = m_shadCompute.uniformLocation("vinf");
        m_locNVortices = m_shadCompute.uniformLocation("nvortices");
        m_locDt        = m_shadCompute.uniformLocation("dt");
        m_locRandSeed  = m_shadCompute.uniformLocation("randseed");
        m_shadCompute.setUniformValue(m_locRandSeed, QRandomGenerator::global()->generate());
    }
    m_shadCompute.release();


    int MaxInvocations = 0;
    glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &MaxInvocations);

    int workGroupCounts[3] = { 0 };
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &workGroupCounts[0]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &workGroupCounts[1]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &workGroupCounts[2]);

    int workGroupSizes[3] = { 0 };
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &workGroupSizes[0]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &workGroupSizes[1]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &workGroupSizes[2]);

/*    qDebug("Max invocations = %5d", MaxInvocations);
    qDebug("Work Group Count= %5d %5d %5d", workGroupCounts[0], workGroupCounts[1], workGroupCounts[2]);
    qDebug("Work Group Size = %5d %5d %5d", workGroupSizes[0], workGroupSizes[1], workGroupSizes[2]);*/

    int n = workGroupCounts[0];
    int pow = 1;
    do
    {
        pow++;
        n=n/2;
    }while(n>1);

    m_plabNMaxGroups->setText(QString("Max. number of groups = 2<sup>")+QString::asprintf("%d", pow)+QString("</sup>"));
}


void gl3dFlowVtx::glMake3dObjects()
{
    if(m_bResetVortices)
    {
        // Create a VBO and an SSBO for the vortices
        // VBO is used for display and SSBO is used in the compute shader

        // create the minimal SSBO
        //need to use 4 components for positions due to std140/430 padding constraints for vec3
        int buffersize =  2*(3+1); // (2 vortices * (3 components+ 1 float padding) )
        QVector<float>BufferArray(buffersize);
        int iv=0;


        BufferArray[iv++] = m_Vortex.vertexAt(0).xf();
        BufferArray[iv++] = m_Vortex.vertexAt(0).yf();
        BufferArray[iv++] = m_Vortex.vertexAt(0).zf();
        BufferArray[iv++] = 1.0f;

        BufferArray[iv++] = m_Vortex.vertexAt(1).xf();
        BufferArray[iv++] = m_Vortex.vertexAt(1).yf();
        BufferArray[iv++] = m_Vortex.vertexAt(1).zf();
        BufferArray[iv++] = 1.0f;


        Q_ASSERT(iv==buffersize);

        if(m_ssboVortices.isCreated()) m_ssboVortices.destroy();
        m_ssboVortices.create();
        m_ssboVortices.bind();
        {
            m_ssboVortices.allocate(BufferArray.data(), buffersize * sizeof(GLfloat));
        }
        m_ssboVortices.release();


        //Create the VBO
        buffersize = 1*3*2*3; //3 segments * 2 vortices * 3 components

        BufferArray.resize(buffersize);
        iv=0;

            Vortex const &vortex = m_Vortex;

            BufferArray[iv++] = vortex.vertexAt(0).xf()+5.0f;
            BufferArray[iv++] = vortex.vertexAt(0).yf();
            BufferArray[iv++] = vortex.vertexAt(0).zf();

            BufferArray[iv++] = vortex.vertexAt(0).xf();
            BufferArray[iv++] = vortex.vertexAt(0).yf();
            BufferArray[iv++] = vortex.vertexAt(0).zf();

            BufferArray[iv++] = vortex.vertexAt(0).xf();
            BufferArray[iv++] = vortex.vertexAt(0).yf();
            BufferArray[iv++] = vortex.vertexAt(0).zf();

            BufferArray[iv++] = vortex.vertexAt(1).xf();
            BufferArray[iv++] = vortex.vertexAt(1).yf();
            BufferArray[iv++] = vortex.vertexAt(1).zf();

            BufferArray[iv++] = vortex.vertexAt(1).xf();
            BufferArray[iv++] = vortex.vertexAt(1).yf();
            BufferArray[iv++] = vortex.vertexAt(1).zf();

            BufferArray[iv++] = vortex.vertexAt(1).xf()+5.0f;
            BufferArray[iv++] = vortex.vertexAt(1).yf();
            BufferArray[iv++] = vortex.vertexAt(1).zf();

        Q_ASSERT(iv==buffersize);

        if(m_vboVortices.isCreated()) m_vboVortices.destroy();
        m_vboVortices.create();
        m_vboVortices.bind();
        {
            m_vboVortices.allocate(BufferArray.data(), buffersize * sizeof(GLfloat));
        }
        m_vboVortices.release();

        m_bResetVortices = false;
    }

    if(m_bResetBoids)
    {
        // use only one buffer object used both as VBO and SSBO
        int NBoids = s_NGroups * GROUP_SIZE;
        m_plabNParticles->setText(QString::asprintf("Number of particles = %d", NBoids));
        //need to use v4 vertices for velocity due to std140/430 padding constraints for vec3:
        int buffersize = NBoids*(4+4+4); //4 vertices + 4 velocity + 4 color components for each boid

        QColor clr(Qt::darkCyan);
        QVector<float>BufferArray(buffersize);
        int iv=0;
        for(int i=0; i<NBoids; i++)
        {
            Boid const &boid = m_Boid.at(i);
            BufferArray[iv++] = boid.m_Position.xf();
            BufferArray[iv++] = boid.m_Position.yf();
            BufferArray[iv++] = boid.m_Position.zf();
            BufferArray[iv++] = 1.0f;

            BufferArray[iv++] = boid.m_Velocity.xf();
            BufferArray[iv++] = boid.m_Velocity.xf();
            BufferArray[iv++] = boid.m_Velocity.xf();
            BufferArray[iv++] = 0.0f;

            BufferArray[iv++] = clr.redF();
            BufferArray[iv++] = clr.greenF();
            BufferArray[iv++] = clr.blueF();
            BufferArray[iv++] = 1.0f;
        }
        Q_ASSERT(iv==buffersize);

        if(m_vboBoids.isCreated()) m_vboBoids.destroy();
        m_vboBoids.create();
        m_vboBoids.bind();
        {
            m_vboBoids.allocate(BufferArray.data(), buffersize * sizeof(GLfloat));
        }
        m_vboBoids.release();



        buffersize = NBoids*TRACESEGS*2*(4+4); //TRACESEGS segments x 2 pts x (4 vertices + 4 color components)
        BufferArray.resize(buffersize);
        iv=0;
        for(int i=0; i<NBoids; i++)
        {
            Boid const &boid = m_Boid.at(i);
            //  write TRACESEGS segments x 2 vertices, with all vertices set to the boid's position as a starting point
            for(int j=0; j<TRACESEGS; j++)
            {
                for(int k=0; k<2; k++)
                {
                    BufferArray[iv++] = boid.m_Position.xf();
                    BufferArray[iv++] = boid.m_Position.yf();
                    BufferArray[iv++] = boid.m_Position.zf();
                    BufferArray[iv++] = 1.0f;
                    BufferArray[iv++] = clr.redF();
                    BufferArray[iv++] = clr.greenF();
                    BufferArray[iv++] = clr.blueF();
                    BufferArray[iv++] = 1.0f;
                }
            }
        }
        Q_ASSERT(iv==buffersize);
        if(m_vboTraces.isCreated()) m_vboTraces.destroy();
        m_vboTraces.create();
        m_vboTraces.bind();
        {
            m_vboTraces.allocate(BufferArray.data(), buffersize * sizeof(GLfloat));
//            qDebug("Boids trace size = %.2f MB", float(m_vboTraces.size())/1024.0f/1024.0f);
        }
        m_vboTraces.release();

        m_bResetBoids = false;
    }
}


void gl3dFlowVtx::moveThem()
{
    s_dt        = m_pfeDt->valuef();
    s_Gamma     = m_pfeGamma->valuef();
    s_VInf      = m_pfeVInf->valuef();

    m_shadCompute.bind();
    {
        m_shadCompute.setUniformValue(m_locRandSeed,  QRandomGenerator::global()->bounded(1024));
        m_shadCompute.setUniformValue(m_locNVortices, 3);
        m_shadCompute.setUniformValue(m_locGamma,     s_Gamma);
        m_shadCompute.setUniformValue(m_locVInf,      s_VInf);
        m_shadCompute.setUniformValue(m_locDt,        s_dt);

        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_vboBoids.bufferId());
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_ssboVortices.bufferId());
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, m_vboTraces.bufferId());

        glDispatchCompute(s_NGroups, 1, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
        glFinish();
        getGLError();
    }
    m_shadCompute.release();
    update();
}


void gl3dFlowVtx::glRenderView()
{
    m_matModel.setToIdentity();
    QMatrix4x4 vmMat(m_matView*m_matModel);
    QMatrix4x4 pvmMat(m_matProj*vmMat);

    m_shadLine.bind();
    {
        m_shadLine.setUniformValue(m_locLine.m_vmMatrix,  vmMat);
        m_shadLine.setUniformValue(m_locLine.m_pvmMatrix, pvmMat);
    }


    // paint the particles
    m_vao.bind();
    {
/*        int stride = 12;
        m_shadPoint2.bind();
        {
            m_shadPoint2.setUniformValue(m_locPt2.m_vmMatrix,  vmMat);
            m_shadPoint2.setUniformValue(m_locPt2.m_pvmMatrix, pvmMat);

            m_shadPoint2.setUniformValue(m_locPt2.m_ClipPlane, m_ClipPlanePos);

            m_vboBoids.bind();
            {
                int nPts = m_vboBoids.size()/stride/int(sizeof(float)); // first half of the buffer only

                m_shadPoint2.enableAttributeArray(m_locPt2.m_attrVertex);
                m_shadPoint2.enableAttributeArray(m_locPt2.m_attrColor);

                m_shadPoint2.setAttributeBuffer(m_locPt2.m_attrVertex, GL_FLOAT, 0,                  4, stride * sizeof(GLfloat));
                m_shadPoint2.setAttributeBuffer(m_locPt2.m_attrColor,  GL_FLOAT, 8* sizeof(GLfloat), 4, stride * sizeof(GLfloat));

                glPointSize(5.0f);
                glDrawArrays(GL_POINTS, 0, nPts);

                m_shadPoint2.disableAttributeArray(m_locPt2.m_attrVertex);
                m_shadPoint2.disableAttributeArray(m_locPt2.m_attrColor);
            }
            m_shadPoint2.release();
        }
        m_shadPoint2.release();*/

        paintColourSegments8(m_vboTraces, float(W3dPrefs::s_FlowStyle.m_Width), W3dPrefs::s_FlowStyle.m_Stipple);

    }
    m_vao.release();


    paintSegments(m_vboVortices, Qt::darkYellow, 3.0f, Line::SOLID);
}


