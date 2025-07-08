/****************************************************************************

    Xfl3d application
    Copyright (C) Andre Deperrois 
    License: GPL v3

*****************************************************************************/



#include "lineaction.h"




LineAction::LineAction(QObject *pParent) : QWidgetAction(pParent)
{
    setDefaultWidget(&m_LineBtn);
    connect(&m_LineBtn, SIGNAL(clickedLB(LineStyle)), this, SLOT(onClickedLB(LineStyle)));
}



void LineAction::onClickedLB(LineStyle )
{
    QAction::trigger();
}
