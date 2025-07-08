/****************************************************************************

    Xfl3d application
    Copyright (C) Andre Deperrois 
    License: GPL v3

*****************************************************************************/

#pragma once


#include <QApplication>
#include <QPlainTextEdit>


class PlainTextOutput : public QPlainTextEdit
{
    Q_OBJECT
    public:
        PlainTextOutput(QWidget *pWidget=nullptr);
        void setCharDimensions(int nHChar, int nVChar);

        void setOpaque(bool bopaque);

        static void setTableColor(QColor textcolor) {s_TableColor=textcolor;}

    public slots:
        void onAppendThisPlainText(QString const &sometext);

    protected:
        QSize sizeHint() const override;
        void showEvent(QShowEvent *pEvent) override;
        void keyPressEvent(QKeyEvent *pEvent) override;

    private:
        int m_nHChar;
        int m_nVChar;


        static QColor s_TableColor;
};

