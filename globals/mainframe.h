
#pragma once

#include <QMainWindow>

class MainFrame : public QMainWindow
{
    Q_OBJECT

    public:
        MainFrame(QWidget *parent = nullptr);
        ~MainFrame();


    private:
        void createMenu();
        void loadSettings();
        void saveSettings();

        void setColorListFromFile();
        void setDefaultStaticFonts();

        void closeEvent(QCloseEvent *pEvent) override;

    private slots:
        void onAbout();
        void onPreferences();
        void onLight();
        void onOpenGLInfo();

        void onLoadBackImage();
        void onClearBackImage();
        void onSettingsBackImage();

        void onTestGL();
        void onFractal();
        void onNewton();
        void onJuliaQuat();
        void onShadow();
        void onFlight();
        void onHydrogen();
        void onLorenz();
        void onLorenzGPU();
        void onAttractors();
        void onBoids();
        void onBoidsGPU();
        void onOptim2d();
        void onSolarSys();
        void onSagittarius();
        void onFlow();
        void onSpace();


    private:

        QWidget m_VoidWidget;
        QWidget *m_pCentralWt;
        static QByteArray s_Geometry;
};
