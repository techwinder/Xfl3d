/****************************************************************************

    Xfl3d application
    Copyright (C) Andre Deperrois 
    License: GPL v3

*****************************************************************************/

#pragma once

#include <QDialog>
#include <QDialogButtonBox>
#include <QTabWidget>
#include <QListWidget>
#include <QLabel>
#include <QCheckBox>
#include <QPushButton>
#include <QRadioButton>

#include <xflgraph/graph/graph.h>

class LineBtn;
class TextClrBtn;
class ColorBtn;
class FloatEdit;
class IntEdit;

class CPTableView;
class ActionItemModel;
class XflDelegate;

class GraphDlg : public QDialog
{
    Q_OBJECT
    friend class Graph;

    public:
        GraphDlg(QWidget *pParent);
        ~GraphDlg();
        void setControls();
        void setGraph(Graph* pGraph);
        Graph* graph() {return m_pGraph;}
        bool bVariableChanged() const {return m_bVariableChanged;}

        void showCurvePage(bool bShow);

        int XSel() const {return m_XSel;}
        int YSel(int iy) const {return m_YSel[iy];}
        void setXVariable(int XVar) {m_XSel=XVar;}
        void setYVariable(int iy, int YVar) {m_YSel[iy]=YVar;}

        static void setActivePage(int iPage);
        static void setWindowGeometry(QByteArray geom) {s_Geometry=geom;}
        static QByteArray windowGeometry() {return s_Geometry;}

    private slots:
        void onActivePage(int index);
        void onApply();
        void onAutoX();
        void onAutoY();
        void onBorderStyle(LineStyle ls);
        void onButton(QAbstractButton *pButton);
        void onGraphBackColor();
        void onGraphBorder(int state);
        void onLabelColor();
        void onLabelFont();
        void onLegendColor();
        void onLegendFont();
        void onMargin();
        void onOK();
        void onRestoreParams();
        void onRightAxis(bool bChecked);
        void onShowLegend(bool bShow);
        void onTitleColor();
        void onTitleFont();
        void onVariableChanged();
        void onXAxisStyle();
        void onXMajGridShow();
        void onXMajGridStyle();
        void onXMinGridShow();
        void onXMinGridStyle();
        void onYAxisStyle();
        void onYInverted();
        void onYMajGridShow();
        void onYMajGridStyle();
        void onYMinGridShow();
        void onYMinGridStyle();

        void onCurveTableClicked(QModelIndex index);
    private:
        void keyPressEvent(QKeyEvent *pEvent) override;
        void hideEvent(QHideEvent *pEvent) override;
        void showEvent(QShowEvent *pEvent) override;

        void reject() override;

        void resizeTableColumns();

        void setupLayout();
        void setButtonColors();
        void setApplied(bool bApplied);
        void connectSignals();
        void applyChanges();       
        void fillVariableList();

        void fillCurvePage();


    private:
        QWidget *m_pParent;

        QTabWidget *m_pTabWidget;
/*        QWidget *m_pScalePage, *m_pFontPage, *m_pGridPage;
        QWidget *m_pVariablePage, *m_pLegendPage;*/

        QCheckBox *m_pchRightAxis;
        QListWidget *m_plwXSel;
        QListWidget *m_plwYSel[2];

        QCheckBox *m_pchShowLegend;
        QRadioButton *m_prbRight, *m_prbLeft, *m_prbTop, *m_prbBottom;
        QRadioButton *m_prbTopRight, *m_prbTopLeft, *m_prbBottomLeft, *m_prbBottomRight;


        QDialogButtonBox *m_pButtonBox;

        QPushButton *m_ppbTitleButton, *m_ppbLabelButton, *m_ppbLegendButton;
        TextClrBtn*m_ptcbTitleClr, *m_ptcbLabelClr, *m_ptcbLegendClr;

        QCheckBox *m_pchXLog;
        FloatEdit *m_pdeXMin, *m_pdeXMax, *m_pdeXUnit;

        QCheckBox *m_pchXAuto;

        QCheckBox *m_pchYAuto[2], *m_pchYInverted[2];
        FloatEdit *m_pdeYMin[2], *m_pdeYMax[2], *m_pdeYUnit[2];
        QCheckBox *m_pchYLog[2];

        QRadioButton *m_prbExpanding, *m_prbResetting;

        QCheckBox *m_pchXMajGridShow, *m_pchXMinGridShow;
        QCheckBox *m_plabYMajGridShow[2], *m_pchMinGridShow[2];
        LineBtn *m_plbXAxisStyle, *m_plbYAxisStyle[2];
        LineBtn *m_plbXMajGridStyle, *m_plbXMinGridStyle;
        LineBtn *m_plbYMajGridStyle[2], *m_plbYMinGridStyle[2];

        QCheckBox *m_pchGraphBorder;
        ColorBtn *m_pcobGraphBack;
        LineBtn *m_plbBorderStyle;
        IntEdit *m_pieLMargin, *m_pieRMargin, *m_pieTMargin, *m_pieBMargin;

        CPTableView *m_pcpCurveTable;
        ActionItemModel *m_pCurveModel;

        QFont *m_pTitleFont, *m_pLabelFont;

        bool m_bApplied;

        Graph *m_pGraph;
        Graph m_SaveGraph;
        int m_XSel, m_YSel[2];

        bool m_bVariableChanged;
        bool m_bCurveStylePage;

        static int s_iActivePage;
        static QByteArray s_Geometry;
};




