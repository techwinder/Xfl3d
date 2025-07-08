/****************************************************************************

    Xfl3d application
    Copyright (C) Andre Deperrois 
    License: GPL v3

*****************************************************************************/

#include <QPainter>
#include <QVBoxLayout>


#include "legendwt.h"

#include <xflcore/displayoptions.h>
#include <xflgraph/graph/curve.h>
#include <xflgraph/graph/graph.h>
#include <xflwidgets/customdlg/newnamedlg.h>
#include <xflwidgets/line/legendbtn.h>
#include <xflwidgets/line/linemenu.h>
#include <xflwidgets/wt_globals.h>

LegendWt::LegendWt(QWidget *pParent) : QWidget(pParent)
{
    setCursor(Qt::ArrowCursor);
    setMouseTracking(true);
    setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);

    m_pGraph = nullptr;
    m_pActiveCurve = nullptr;

    setAutoFillBackground(false);
}


void LegendWt::makeGraphLegendBtns(bool bHighlight)
{
    if(!m_pGraph) return;

    QPalette s_Palette;
    s_Palette.setColor(QPalette::WindowText, DisplayOptions::textColor());
    s_Palette.setColor(QPalette::Window,     DisplayOptions::backgroundColor());
    s_Palette.setColor(QPalette::Base,       DisplayOptions::backgroundColor());
    setPalette(s_Palette);

    m_XflObjectMap.clear();
    m_CurveMap.clear();

    QVBoxLayout *pLegendLayout = dynamic_cast<QVBoxLayout*>(layout());
    if(!pLegendLayout)
    {
        pLegendLayout = new QVBoxLayout;
        setLayout(pLegendLayout);
    }
    else
    {
        clearLayout(pLegendLayout);
    }
    for (int j=0; j<m_pGraph->curveCount(); j++)
    {
        Curve *pCurve = m_pGraph->curve(j);

        LegendBtn *pLegendBtn = new LegendBtn;
        LineStyle ls(true, pCurve->stipple(), pCurve->width(),
                     pCurve->color(), pCurve->symbol(), pCurve->name());
        pLegendBtn->setStyle(ls);
        pLegendBtn->setBackground(true);

        pLegendBtn->setHighLight(m_pGraph->isCurveSelected(pCurve) && Graph::isHighLighting() && bHighlight);
        m_CurveMap.insert(pLegendBtn, pCurve);

        connect(pLegendBtn, SIGNAL(clickedLB(LineStyle)),      SLOT(onClickedCurveBtn()));
        connect(pLegendBtn, SIGNAL(clickedRightLB(LineStyle)), SLOT(onRightClickedCurveBtn(LineStyle)));
        connect(pLegendBtn, SIGNAL(clickedLine(LineStyle)),    SLOT(onClickedCurveLine(LineStyle)));
        pLegendLayout->addWidget(pLegendBtn);

    }
    pLegendLayout->addStretch();
}


void LegendWt::onClickedCurveBtn()
{
//    for(int i=0; i<m_CurveMap.size(); i++) m_CurveMap.keys().at(i)->setHighLight(false);
    QMap<LegendBtn*, Curve*>::iterator it;
    for (it=m_CurveMap.begin(); it!=m_CurveMap.end(); it++)
        it.key()->setHighLight(false);

    if(Graph::isHighLighting())
    {
        LegendBtn *pLegendBtn = dynamic_cast<LegendBtn*>(sender());
        if(pLegendBtn) pLegendBtn->setHighLight(Graph::isHighLighting());
    }

    update();
    emit updateGraphs();
}


void LegendWt::onRightClickedCurveBtn(LineStyle )
{
    LegendBtn *pLegendBtn = dynamic_cast<LegendBtn*>(sender());
    m_pActiveCurve = dynamic_cast<Curve*>(m_CurveMap[pLegendBtn]);

    QAction *pRenameCurve = new QAction("Rename", this);
    QAction *pDeleteCurve = new QAction("Delete", this);
    connect(pRenameCurve, SIGNAL(triggered(bool)), SLOT(onRenameActiveCurve()));
    connect(pDeleteCurve, SIGNAL(triggered(bool)), SLOT(onDeleteActiveCurve()));
    QMenu *pCtxMenu = new QMenu;
    pCtxMenu->addAction(pRenameCurve);
    pCtxMenu->addAction(pDeleteCurve);
    pCtxMenu->exec(QCursor::pos());

    update();
    emit updateGraphs();
}


void LegendWt::onClickedCurveLine(LineStyle ls)
{
    LegendBtn *pLegendBtn = dynamic_cast<LegendBtn*>(sender());

    Curve* pCurve = dynamic_cast<Curve*>(m_CurveMap[pLegendBtn]);
    LineMenu *pLineMenu = new LineMenu(nullptr);
    pLineMenu->initMenu(ls);
    pLineMenu->exec(QCursor::pos());
    ls = pLineMenu->theStyle();
    pCurve->setTheStyle(ls);
    pLegendBtn->setStyle(ls);
    update();
    emit updateGraphs();
 }


void LegendWt::onDeleteActiveCurve()
{
    if(!m_pGraph || !m_pActiveCurve) return;

    m_pGraph->deleteCurve(m_pActiveCurve);
    makeLegend(false);
    update();
    emit updateGraphs();
}


void LegendWt::onRenameActiveCurve()
{
    if(!m_pGraph || !m_pActiveCurve) return;
    NewNameDlg dlg(m_pActiveCurve->name(), this);

    if(dlg.exec()==QDialog::Accepted)
    {
        m_pActiveCurve->setName(dlg.newName());
        LegendBtn *pLegendBtn = m_CurveMap.key(m_pActiveCurve);
        if(pLegendBtn)
        {
            pLegendBtn->setTag(m_pActiveCurve->name());
            pLegendBtn->update();
            emit updateGraphs();
        }
    }
}

/** default legend using curve names */
void LegendWt::makeLegend(bool bHighlight)
{
    setStyleSheet(QString::asprintf("background: %s;", DisplayOptions::backgroundColor().name(QColor::HexRgb).toStdString().c_str()));
    setAutoFillBackground(true);

    if(!m_pGraph) return;

    m_XflObjectMap.clear();
    m_CurveMap.clear();

    QVBoxLayout *pLegendLayout = dynamic_cast<QVBoxLayout*>(layout());
    if(!pLegendLayout)
    {
        pLegendLayout = new QVBoxLayout;
        setLayout(pLegendLayout);
    }
    else
    {
        clearLayout(pLegendLayout);
    }

    for (int j=0; j<m_pGraph->curveCount(); j++)
    {
        Curve *pCurve = m_pGraph->curve(j);

        LegendBtn *pLegendBtn = new LegendBtn;
        LineStyle ls(true, pCurve->stipple(), pCurve->width(),
                     pCurve->color(), pCurve->symbol(), pCurve->name());
        pLegendBtn->setStyle(ls);
        pLegendBtn->setBackground(true);

        pLegendBtn->setHighLight(m_pGraph->isCurveSelected(pCurve) && Graph::isHighLighting() && bHighlight);
        m_CurveMap.insert(pLegendBtn, pCurve);

        connect(pLegendBtn, SIGNAL(clickedLB(LineStyle)),      SLOT(onClickedCurveBtn()));
        connect(pLegendBtn, SIGNAL(clickedRightLB(LineStyle)), SLOT(onRightClickedCurveBtn(LineStyle)));
        connect(pLegendBtn, SIGNAL(clickedLine(LineStyle)),    SLOT(onClickedCurveLine(LineStyle)));
        pLegendLayout->addWidget(pLegendBtn);

    }
    pLegendLayout->addStretch();
}



