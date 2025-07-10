
#include <QApplication>
#include <QSettings>
#include <QFile>

#include <globals/mainframe.h>
#include <xfl3d/views/gl3dview.h>



/** OpenGL Default format must be set prior to app construction if Qt::AA_ShareOpenGLContexts is set */
void setOGLDefaultFormat(int version)
{
#if defined Q_OS_MAC
    QSettings settings(QSettings::IniFormat,QSettings::UserScope,"Xfl3d", );
#elif defined Q_OS_LINUX
    QSettings settings(QSettings::NativeFormat,QSettings::UserScope,"Xfl3d");
#else
    QSettings settings(QSettings::IniFormat,QSettings::UserScope,"Xfl3d");
#endif

    // Load preferred OpenGL version
    // and set the default format before any 3d view is created
    int OGLMajor = 3;
    int OGLMinor = 3;
    if(QFile(settings.fileName()).exists())
    {
        gl3dView::loadSettings(settings);
        OGLMajor = gl3dView::oglMajor();
        OGLMinor = gl3dView::oglMinor();
    }

    if(version>0)
    {
        OGLMajor = int(version/10);
        OGLMinor = version - 10*OGLMajor;
        qDebug()<<"setting context"<<OGLMajor<<OGLMinor;
    }

    // choose between the version passed as option if valid and the saved setting

    if(OGLMajor<=2 || (OGLMajor==3 && OGLMinor<3))
    {
        // Systems (may? commonly?) respond with the latest 4.x context,
        // so force deprecated functions and compatibility profile
        // Will also force v120 style shaders in GL initialization
        gl3dView::setProfile(QSurfaceFormat::NoProfile);
        gl3dView::setDeprecatedFuncs(true);
    }
    else
    {
        gl3dView::setProfile(QSurfaceFormat::CoreProfile);
        gl3dView::setDeprecatedFuncs(false);
    }

    gl3dView::setRenderableType(QSurfaceFormat::OpenGL);
    gl3dView::setOGLVersion(OGLMajor, OGLMinor);

    if(gl3dView::defaultXflSurfaceFormat().samples()<0) gl3dView::setDefaultSamples(4);

    QSurfaceFormat::setDefaultFormat(gl3dView::defaultXflSurfaceFormat()); // for all QOpenGLWidgets
//    qDebug()<<gl3dView::defaultXflSurfaceFormat();
}


/**
 * The app's point of entry !
 */
int main(int argc, char *argv[])
{
#ifdef Q_OS_LINUX
/*    struct rlimit limit ;
    getrlimit (RLIMIT_STACK, &limit); // The maximum size of the process stack, in bytes.
    qDebug("\nStack Limit = %.2fMb soft limit and %ld bytes hard limit\n", (float)limit.rlim_cur/1024./1024., limit.rlim_max);*/
#endif



#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
    //    qDebug()<<QCoreApplication::testAttribute(Qt::AA_EnableHighDpiScaling);
    QCoreApplication::setAttribute(Qt::AA_DisableHighDpiScaling);
#endif


    /*To set up sharing between QOpenGLWidget instances belonging to different windows,
     * set the Qt::AA_ShareOpenGLContexts application attribute before instantiating QApplication.
     * This will trigger sharing between all QOpenGLWidget instances without any further steps.*/
    QCoreApplication::setAttribute(Qt::AA_ShareOpenGLContexts);
    QCoreApplication::setAttribute(Qt::AA_UseDesktopOpenGL);

    int version = -1;
    for(int i=0; i<argc; i++)
    {
        QString strange = argv[i];
        if(strange.compare("-o", Qt::CaseSensitive)==0 && i<argc-1)
        {
            version = QString(argv[i+1]).toInt();
            qDebug()<<"OGL version" << version << "requested";
        }
    }
    setOGLDefaultFormat(version);

    QApplication a(argc, argv);
    MainFrame w;
    w.show();
    return a.exec();
}






