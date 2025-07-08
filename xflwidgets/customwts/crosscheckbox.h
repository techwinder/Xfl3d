/****************************************************************************

    Xfl3d application
    Copyright (C) Andre Deperrois 
    License: GPL v3

*****************************************************************************/

#pragma once

#include <QCheckBox>

class CrossCheckBox : public QWidget
{
    Q_OBJECT

    public:
        CrossCheckBox(QWidget *pParent = nullptr);
        void setWidthHint(int w) {m_WidthHint = w;}
        void setCheckState(Qt::CheckState state) {m_State=state;}
        Qt::CheckState checkState() const {return m_State;}

    signals:
        void clicked(bool);

    private:
        QSize sizeHint() const override;
        QSize minimumSizeHint() const override;
        void paintEvent(QPaintEvent *) override;
        void mouseReleaseEvent(QMouseEvent *pEvent) override;



    private:
        Qt::CheckState m_State;
        int m_WidthHint;
};

