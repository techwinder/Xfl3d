#include "mainframe.h"

#include <QFontDatabase>
#include <QFileInfo>
#include <QMenu>
#include <QMenuBar>
#include <QAction>
#include <QDir>
#include <QToolTip>


#include <globals/prefsdlg.h>
#include <xflgraph/controls/graphoptions.h>
#include <xflcore/displayoptions.h>
#include <xfl3d/globals/opengldlg.h>
#include <xfl3d/controls/w3dprefs.h>
#include <xfl3d/controls/gllightdlg.h>
#include <xfl3d/views/gl2dview.h>
#include <xfl3d/testgl/gl3dtestglview.h>
#include <xfl3d/testgl/gl2dfractal.h>
#include <xfl3d/testgl/gl2dnewton.h>
#include <xfl3d/testgl/gl2drm.h>
#include <xfl3d/testgl/gl3dattractors.h>
#include <xfl3d/testgl/gl3dflightview.h>
#include <xfl3d/testgl/gl3dhydrogen.h>
#include <xfl3d/testgl/gl3dsagittarius.h>
#include <xfl3d/testgl/gl3dsolarsys.h>
#include <xfl3d/testgl/gl3dspace.h>
#include <xfl3d/testgl/gl3dboids.h>
#include <xfl3d/testgl/gl2dquat.h>
#include <xfl3d/testgl/gl3dboids2.h>
#include <xfl3d/testgl/gl3dlorenz.h>
#include <xfl3d/testgl/gl3dlorenz2.h>
#include <xfl3d/testgl/gl3dshadow.h>
#include <xfl3d/testgl/gl3doptim2d.h>
#include <xfl3d/testgl/gl3dflowvtx.h>
#include <xflgraph/graph/curvemodel.h>
#include <xflwidgets/line/linepicker.h>


#include <xflwidgets/customwts/plaintextoutput.h>


QByteArray MainFrame::s_Geometry;

MainFrame::MainFrame(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowIcon(QIcon(":/icons/Xfl3d.png"));

    setDefaultStaticFonts();
    loadSettings();
    createMenu();

    setColorListFromFile();

    GraphOptions::setDefaults(DisplayOptions::isDarkTheme());

    m_pCentralWt = new gl3dTestGLView;

    setCentralWidget(m_pCentralWt);
}


MainFrame::~MainFrame()
{
}


void MainFrame::createMenu()
{
    QMenu *pFileMenu = menuBar()->addMenu("File");
    {
        QAction*pAboutAct = new QAction("About Xfl3d", this);
        connect(pAboutAct, SIGNAL(triggered(bool)), SLOT(onAbout()));
        pFileMenu->addAction(pAboutAct);

        pFileMenu->addSeparator();

        QAction*pExitAct = new QAction("Exit", this);
        pExitAct->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_Q));
        connect(pExitAct, SIGNAL(triggered(bool)), SLOT(close()));
        pFileMenu->addAction(pExitAct);
    }

    QMenu *pViewMenu = menuBar()->addMenu("View");
    {
        QAction *pTestGL    = new QAction("Test", this);
        pTestGL->setShortcut(QKeySequence(Qt::SHIFT | Qt::Key_F1));

        QAction *pFractal   = new QAction("Fractal", this);
        pFractal->setShortcut(Qt::Key_F1);

        QAction *pNewton   = new QAction("Newton", this);
        pNewton->setShortcut(Qt::Key_F2);

        QAction *pJuliaQuat= new QAction("Julia quaternions", this);
        pJuliaQuat->setShortcut(QKeySequence(Qt::SHIFT | Qt::Key_F2));

        QAction *pShadow   = new QAction("Shadow", this);
        pShadow->setShortcut(Qt::Key_F3);

        QAction *pFlight   = new QAction("Flight", this);
        pFlight->setShortcut(Qt::Key_F4);

        QAction *pHydrogen   = new QAction("Hydrogen", this);
        pHydrogen->setShortcut(Qt::Key_F5);

        QAction *pLorenz  = new QAction("Lorenz (CPU)", this);
        pLorenz->setShortcut(QKeySequence(Qt::SHIFT | Qt::Key_F6));

        QAction *pLorenz2  = new QAction("Lorenz (GPU)", this);
        pLorenz2->setShortcut(Qt::Key_F6);

        QAction *pAttractors = new QAction("Attractors", this);
        pAttractors->setShortcut(Qt::Key_F7);

        QAction *pSolarSys   = new QAction("Solar system", this);
        pSolarSys->setShortcut(Qt::Key_F8);

        QAction *pSagittarius   = new QAction("Sagittarius A*", this);
        pSagittarius->setShortcut(Qt::Key_F9);

        QAction *pSpace      = new QAction("The final frontier", this);
        pSpace->setShortcut(Qt::Key_F10);

        QAction *pOptim2d    = new QAction("Optim. 2d", this);
        pOptim2d->setShortcut(Qt::Key_F11);

        QAction *pBoids      = new QAction("Boids (CPU)", this);
        pBoids->setShortcut(QKeySequence(Qt::SHIFT | Qt::Key_F12));

        QAction *pBoidsGPU   = new QAction("Boids (GPU)", this);
        pBoidsGPU->setShortcut(Qt::Key_F12);

        QAction *pFlow      = new QAction("Flow", this);
        pFlow->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_F));


        connect(pTestGL,      SIGNAL(triggered(bool)), SLOT(onTestGL()));
        connect(pFractal,     SIGNAL(triggered(bool)), SLOT(onFractal()));
        connect(pNewton,      SIGNAL(triggered(bool)), SLOT(onNewton()));
        connect(pJuliaQuat,   SIGNAL(triggered(bool)), SLOT(onJuliaQuat()));
        connect(pShadow,      SIGNAL(triggered(bool)), SLOT(onShadow()));
        connect(pFlight,      SIGNAL(triggered(bool)), SLOT(onFlight()));
        connect(pHydrogen,    SIGNAL(triggered(bool)), SLOT(onHydrogen()));
        connect(pLorenz,      SIGNAL(triggered(bool)), SLOT(onLorenz()));
        connect(pLorenz2,     SIGNAL(triggered(bool)), SLOT(onLorenzGPU()));
        connect(pAttractors,  SIGNAL(triggered(bool)), SLOT(onAttractors()));
        connect(pSolarSys,    SIGNAL(triggered(bool)), SLOT(onSolarSys()));
        connect(pSagittarius, SIGNAL(triggered(bool)), SLOT(onSagittarius()));
        connect(pSpace,       SIGNAL(triggered(bool)), SLOT(onSpace()));
        connect(pOptim2d,     SIGNAL(triggered(bool)), SLOT(onOptim2d()));
        connect(pBoids,       SIGNAL(triggered(bool)), SLOT(onBoids()));
        connect(pBoidsGPU,    SIGNAL(triggered(bool)), SLOT(onBoidsGPU()));
        connect(pFlow,        SIGNAL(triggered(bool)), SLOT(onFlow()));

        pViewMenu->addAction(pTestGL);
        pViewMenu->addAction(pFractal);
        pViewMenu->addAction(pNewton);
        pViewMenu->addAction(pJuliaQuat);
        pViewMenu->addAction(pShadow);
        pViewMenu->addAction(pFlight);
        pViewMenu->addAction(pHydrogen);
        pViewMenu->addAction(pLorenz);
        pViewMenu->addAction(pLorenz2);
        pViewMenu->addAction(pAttractors);
        pViewMenu->addAction(pSolarSys);
        pViewMenu->addAction(pSagittarius);
        pViewMenu->addAction(pSpace);
        pViewMenu->addAction(pOptim2d);
        pViewMenu->addAction(pBoids);
        pViewMenu->addAction(pBoidsGPU);
        pViewMenu->addAction(pFlow);
    }

    QMenu *pOptionsMenu =  menuBar()->addMenu("Settings");
    {
        QAction *pPreferences   = new QAction("Preferences", this);

        QAction *pOpenGLOpt   = new QAction("OpenGL settings", this);
        pOpenGLOpt->setShortcut(QKeySequence(Qt::CTRL | Qt::ALT | Qt::Key_O));

        QAction *pLightOpt    = new QAction("Light settings", this);
        pLightOpt->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_L));


        connect(pPreferences, SIGNAL(triggered(bool)), SLOT(onPreferences()));
        connect(pOpenGLOpt,   SIGNAL(triggered(bool)), SLOT(onOpenGLInfo()));
        connect(pLightOpt,    SIGNAL(triggered(bool)), SLOT(onLight()));

        pOptionsMenu->addAction(pPreferences);
        pOptionsMenu->addAction(pOpenGLOpt);
        pOptionsMenu->addAction(pLightOpt);

        pOptionsMenu->addSeparator();
        QMenu *pImageMenu = pOptionsMenu->addMenu("Background image");
        {
            QAction *pLoad     = new QAction("Load",     this);
            pLoad->setShortcut(QKeySequence(Qt::ALT | Qt::Key_I));
            QAction *pClear    = new QAction("Clear",    this);
            QAction *pSettings = new QAction("Settings", this);

            connect(pLoad,      SIGNAL(triggered(bool)), SLOT(onLoadBackImage()));
            connect(pClear,     SIGNAL(triggered(bool)), SLOT(onClearBackImage()));
            connect(pSettings,  SIGNAL(triggered(bool)), SLOT(onSettingsBackImage()));

            pImageMenu->addAction(pLoad);
            pImageMenu->addAction(pClear);
            pImageMenu->addAction(pSettings);
        }
    }
}


void MainFrame::setDefaultStaticFonts()
{
    //"Qt does not support style hints on X11 since this information is not provided by the window system."
    QFont generalfnt(QFontDatabase::systemFont(QFontDatabase::GeneralFont));
    QFont fixedfnt(QFontDatabase::systemFont(QFontDatabase::FixedFont));

    DisplayOptions::setTextFont(fixedfnt);
    DisplayOptions::setTableFont(fixedfnt);
    DisplayOptions::setTreeFont(generalfnt);
    DisplayOptions::setToolTipFont(generalfnt);

    QToolTip::setFont(DisplayOptions::toolTipFont());

    GraphOptions::setTitleFontStruct(DisplayOptions::treeFontStruct());// the general font to avoid fixed width by default
    GraphOptions::setLabelFontStruct(DisplayOptions::treeFontStruct());
    GraphOptions::setLegendFontStruct(DisplayOptions::treeFontStruct());
}


void MainFrame::closeEvent(QCloseEvent *pEvent)
{
    saveSettings();
    QMainWindow::closeEvent(pEvent);
}


void MainFrame::loadSettings()
{
#if defined Q_OS_MAC
    QSettings settings(QSettings::IniFormat,QSettings::UserScope,"Xfl3d", );
#elif defined Q_OS_LINUX
    QSettings settings(QSettings::NativeFormat,QSettings::UserScope,"Xfl3d");
#else
    QSettings settings(QSettings::IniFormat,QSettings::UserScope,"Xfl3d");
#endif
    if(QFile(settings.fileName()).exists())
    {
        settings.beginGroup("Mainframe");
        {
            s_Geometry = settings.value("WindowGeom").toByteArray();
        }
        settings.endGroup();

        DisplayOptions::loadSettings(settings);
        GLLightDlg::loadSettings(settings);
        OpenGlDlg::loadSettings(settings);
        W3dPrefs::loadSettings(settings);
        gl2dFractal::loadSettings(settings);
        gl2dNewton::loadSettings(settings);
        gl2dQuat::loadSettings(settings);
        gl3dAttractors::loadSettings(settings);
        gl3dBoids2::loadSettings(settings);
        gl3dBoids::loadSettings(settings);
        gl3dFlightView::loadSettings(settings);
        gl3dFlowVtx::loadSettings(settings);
        gl3dHydrogen::loadSettings(settings);
        gl3dLorenz2::loadSettings(settings);
        gl3dLorenz::loadSettings(settings);
        gl3dOptim2d::loadSettings(settings);
        gl3dSagittarius::loadSettings(settings);
        gl3dShadow::loadSettings(settings);
        gl3dSolarSys::loadSettings(settings);
        gl3dSpace::loadSettings(settings);
        gl3dView::loadSettings(settings);
    }
}


void MainFrame::saveSettings()
{
#if defined Q_OS_MAC
    QSettings settings(QSettings::IniFormat,QSettings::UserScope,"Xfl3d", );
#elif defined Q_OS_LINUX
    QSettings settings(QSettings::NativeFormat,QSettings::UserScope,"Xfl3d");
#else
    QSettings settings(QSettings::IniFormat,QSettings::UserScope,"Xfl3d");
#endif

    settings.beginGroup("Mainframe");
    {
        settings.setValue("WindowGeom", s_Geometry);
    }
    settings.endGroup();


    DisplayOptions::saveSettings(settings);
    GLLightDlg::saveSettings(settings);
    OpenGlDlg::saveSettings(settings);
    W3dPrefs::saveSettings(settings);
    gl2dFractal::saveSettings(settings);
    gl2dNewton::saveSettings(settings);
    gl3dAttractors::saveSettings(settings);
    gl3dBoids2::saveSettings(settings);
    gl3dBoids::saveSettings(settings);
    gl3dFlightView::saveSettings(settings);
    gl3dFlowVtx::saveSettings(settings);
    gl3dHydrogen::saveSettings(settings);
    gl3dLorenz2::saveSettings(settings);
    gl3dLorenz::saveSettings(settings);
    gl3dOptim2d::saveSettings(settings);
    gl3dSagittarius::saveSettings(settings);
    gl3dShadow::saveSettings(settings);
    gl3dSolarSys::saveSettings(settings);
    gl3dSpace::saveSettings(settings);
    gl3dView::saveSettings(settings);
    gl2dQuat::saveSettings(settings);
}


void MainFrame::setColorListFromFile()
{
    QString appdir = qApp->applicationDirPath();
    QString ColorPathName = appdir + QDir::separator() +"/colorlist.txt";

    QFileInfo fi(ColorPathName);
    if(!fi.exists()) ColorPathName = ":/textfiles/colorlist.txt";

    QFile ColorFile(ColorPathName);
    ColorFile.open(QIODevice::ReadOnly);

    QStringList LineColorList;
    QStringList LineColorNames;
    QVector<QColor> colors;
    QTextStream stream(&ColorFile);
    while(!stream.atEnd())
    {
        QString colorline = stream.readLine().simplified();
        QStringList colorpair = colorline.split(" ");
        if(colorpair.size()>=2)
        {
            LineColorList.append(colorpair.at(0));
            LineColorNames.append(colorpair.at(1));
            colors.append(LineColorList.last());
        }
    }
    for(int i=LineColorList.size(); i<NCOLORROWS*NCOLORCOLS; i++)
    {
        LineColorList.append(QString("#000000"));
        LineColorNames.append(QString("#000000"));
        colors.append(Qt::black);
    }

    CurveModel::setColorList(colors);
    LinePicker::setColorList(LineColorList, LineColorNames);
//    ColorPicker::setColorList(LineColorList, LineColorNames);
}


void MainFrame::onAbout()
{
}


void MainFrame::onLoadBackImage()
{
    gl3dView *pglView = dynamic_cast<gl3dView*>(m_pCentralWt);
    if(!pglView) return;
    pglView->onLoadBackImage();
}


void MainFrame::onClearBackImage()
{
    gl3dView *pglView = dynamic_cast<gl3dView*>(m_pCentralWt);
    if(!pglView) return;
    pglView->onClearBackImage();
}


void MainFrame::onSettingsBackImage()
{
    gl3dView *pglView = dynamic_cast<gl3dView*>(m_pCentralWt);
    if(!pglView) return;
    pglView->onBackImageSettings();
}


void MainFrame::onPreferences()
{
    PrefsDlg dlg(this);
    dlg.initWidgets();
    dlg.exec();

    if(GraphOptions::isGraphModified())
    {
    }


    QPalette palette;
    palette.setColor(QPalette::WindowText, DisplayOptions::textColor());
    palette.setColor(QPalette::Window,     DisplayOptions::backgroundColor());
    m_VoidWidget.setPalette(palette); // to update its palette
    m_VoidWidget.update();

    saveSettings();

}


void MainFrame::onOpenGLInfo()
{
    OpenGlDlg w(this);
    w.initDialog();
    w.exec();
}


void MainFrame::onLight()
{
    if(!m_pCentralWt) return;
    gl3dView *pgl3dView = dynamic_cast<gl3dView*>(m_pCentralWt);
    if(!pgl3dView) return;
    GLLightDlg  * m_pglLightDlg = new GLLightDlg;
    m_pglLightDlg->setgl3dView(pgl3dView);
    m_pglLightDlg->show();
}


void MainFrame::onTestGL()
{
    if(m_pCentralWt) delete m_pCentralWt;
    m_pCentralWt = new gl3dTestGLView;
    setCentralWidget(m_pCentralWt);
    update();
}


void MainFrame::onFractal()
{
    if(m_pCentralWt) delete m_pCentralWt;
    m_pCentralWt = new gl2dFractal;
    setCentralWidget(m_pCentralWt);
    update();
}


void MainFrame::onNewton()
{
    if(m_pCentralWt) delete m_pCentralWt;
    m_pCentralWt = new gl2dNewton;
    setCentralWidget(m_pCentralWt);
    update();
}


void MainFrame::onJuliaQuat()
{
    if(m_pCentralWt) delete m_pCentralWt;
    m_pCentralWt = new gl2dQuat;
    setCentralWidget(m_pCentralWt);
    update();
}


void MainFrame::onShadow()
{
    if(m_pCentralWt) delete m_pCentralWt;
    m_pCentralWt = new gl3dShadow;
    setCentralWidget(m_pCentralWt);
    update();
}


void MainFrame::onFlight()
{
    if(m_pCentralWt) delete m_pCentralWt;
    m_pCentralWt = new gl3dFlightView;
    setCentralWidget(m_pCentralWt);
    update();
}


void MainFrame::onHydrogen()
{
    if(m_pCentralWt) delete m_pCentralWt;
    m_pCentralWt = new gl3dHydrogen;
    setCentralWidget(m_pCentralWt);
    update();
}


void MainFrame::onLorenz()
{
    if(m_pCentralWt) delete m_pCentralWt;
    m_pCentralWt = new gl3dLorenz;
    setCentralWidget(m_pCentralWt);
    update();
}


void MainFrame::onLorenzGPU()
{
    if(m_pCentralWt) delete m_pCentralWt;
    m_pCentralWt = new gl3dLorenz2;
    setCentralWidget(m_pCentralWt);
    update();
}


void MainFrame::onAttractors()
{
    if(m_pCentralWt) delete m_pCentralWt;
    m_pCentralWt = new gl3dAttractors;
    setCentralWidget(m_pCentralWt);
    update();
}



void MainFrame::onSolarSys()
{
    if(m_pCentralWt) delete m_pCentralWt;
    m_pCentralWt = new gl3dSolarSys;
    setCentralWidget(m_pCentralWt);
    update();
}


void MainFrame::onSagittarius()
{
    if(m_pCentralWt) delete m_pCentralWt;
    m_pCentralWt = new gl3dSagittarius;
    setCentralWidget(m_pCentralWt);
    update();
}


void MainFrame::onFlow()
{
    if(m_pCentralWt) delete m_pCentralWt;
    m_pCentralWt = new gl3dFlowVtx;
    setCentralWidget(m_pCentralWt);
    update();
}


void MainFrame::onSpace()
{
    if(m_pCentralWt) delete m_pCentralWt;
    m_pCentralWt = new gl3dSpace;
    setCentralWidget(m_pCentralWt);
    update();
}


void MainFrame::onOptim2d()
{
    if(m_pCentralWt) delete m_pCentralWt;
    m_pCentralWt = new gl3dOptim2d;
    setCentralWidget(m_pCentralWt);
    update();
}


void MainFrame::onBoids()
{
    if(m_pCentralWt) delete m_pCentralWt;
    m_pCentralWt = new gl3dBoids;
    setCentralWidget(m_pCentralWt);
    update();
}


void MainFrame::onBoidsGPU()
{
    if(m_pCentralWt) delete m_pCentralWt;
    m_pCentralWt = new gl3dBoids2;
    setCentralWidget(m_pCentralWt);
    update();
}



