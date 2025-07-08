/****************************************************************************

    Xfl3d application
    Copyright (C) Andre Deperrois 
    License: GPL v3

*****************************************************************************/


#pragma once

#include <QDialog>
#include <QDialogButtonBox>
#include <QDir>
#include <QGroupBox>
#include <QSettings>
#include <QTreeWidget>
#include <QStackedWidget>
#include <QCheckBox>
#include <QRadioButton>
#include <QComboBox>
#include <QListWidget>
#include <QLabel>
#include <QSplitter>

#include <xflwidgets/customwts/intedit.h>
#include <xflwidgets/color/textclrbtn.h>
#include <xflwidgets/color/colorbtn.h>

class FloatEdit;
class GraphOptions;
class SaveOptionsWt;
class UnitsWt;
class W3dPrefs;

class PrefsDlg : public QDialog
{
    Q_OBJECT

    public:
        PrefsDlg(QWidget *pParent=nullptr);

        void reject();
        void showEvent(QShowEvent *pEvent);
        void hideEvent(QHideEvent *pEvent);
        QSize sizeHint() const {return QSize(1000,800);}

        void initWidgets();


        static void loadSettings(QSettings &settings);
        static void saveSettings(QSettings &settings);

        static void setStyleName(QString const &name){s_StyleName=name;}
        static QString const &styleName()  {return s_StyleName;}



    private:
        void setupLayout();
        void connectSignals();
        void readData();
        void fillTreeWidget();
        void showStyleBox(int iBox);
        void setButtonFonts();

    private slots:
        void onBackgroundColor();
        void onButton(QAbstractButton *pButton);
        void onItemChanged(QTreeWidgetItem*pNewItem, QTreeWidgetItem *pPreviousItem);
        void onMultiThreading();
        void onOK();
        void onStyleChanged(const QString &StyleName);
        void onStyleSheet(bool);
        void onTableFont();
        void onTextColor();
        void onTextFont();
        void onTheme();
        void onToolTipFont();
        void onTreeFont();

    private:

        QSplitter *m_pHSplitter;

        QTreeWidget *m_ptwOptionsTree;
        QTreeWidgetItem *m_pStyleItem, *m_pDisplayItem;
        QTreeWidgetItem *m_p2dItem, *m_p3dItem;

        QStackedWidget *m_pPageStack;

        QVector<QWidget*>m_pStyleWidgets;
        QRadioButton *m_prbDark, *m_prbLight, *m_prbCustom;
        ColorBtn *m_pcbBackColor;
        TextClrBtn *m_ptcbTextClr;
        QPushButton *m_ppbTextFont, *m_ppbTableFont, *m_ppbTreeFont, *m_ppbToolTipFont;

        QWidget *m_pStyleOptions;
        QWidget *m_pMultiThreadOptions;
        GraphOptions *m_pGraphOptionsWt;
        W3dPrefs *m_p3dPrefsWt;

        QCheckBox *m_pchMultiThreading;
        IntEdit *m_pieMaxThreadCount;
        QComboBox *m_pcbThreadPriority;

        QDialogButtonBox *m_pButtonBox;

        QCheckBox *m_pchDontUseNativeMacDlg;

        FloatEdit *m_pdeScaleFactor;
        IntEdit *m_pieIconSize;

        QComboBox *m_pcbStyles;
        QCheckBox *m_pchStyleSheetOverride;

        static QString s_StyleName;



        static int s_CurrentIndex;

        static int s_ExitRow;
        static int s_ExitChildRow;

        static QByteArray s_HSplitterSizes;
        static QByteArray s_WindowGeometry;
};


