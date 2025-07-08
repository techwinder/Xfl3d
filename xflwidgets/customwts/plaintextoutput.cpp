/****************************************************************************

    Xfl3d application
    Copyright (C) Andre Deperrois 
    License: GPL v3

*****************************************************************************/

#include "plaintextoutput.h"

#include <xflcore/displayoptions.h>

QColor PlainTextOutput::s_TableColor;

PlainTextOutput::PlainTextOutput(QWidget *pWidget) : QPlainTextEdit(pWidget)
{
    m_nHChar = 15;
    m_nVChar = 5;


    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::MinimumExpanding);
    setReadOnly(true);
    setLineWrapMode(QPlainTextEdit::WidgetWidth);

    setOpaque(false);

    setMinimumHeight(2*DisplayOptions::tableFontStruct().height());

}


void PlainTextOutput::setOpaque(bool bOpaque)
{
    QPalette palette;
    palette.setColor(QPalette::WindowText, s_TableColor);
    if(bOpaque)
    {
        palette.setColor(QPalette::Window, QColor(225,125,125));
        palette.setColor(QPalette::Base, QColor(125,155,225));
    }
    else
    {
        palette.setColor(QPalette::Window, QColor(225,125,125,0));
        palette.setColor(QPalette::Base, QColor(125,155,225,25));
    }

    setBackgroundVisible(false);
    setPalette(palette);
    viewport()->setPalette(palette);
    viewport()->setAutoFillBackground(true);
}


void PlainTextOutput::keyPressEvent(QKeyEvent *pEvent)
{
    bool bAlt = false;
    if(pEvent->modifiers() & Qt::AltModifier)   bAlt =true;

    switch (pEvent->key())
    {
        case Qt::Key_L:
        {
            if(bAlt)
            {
                clear();
            }
            break;
        }
    }
    QPlainTextEdit::keyPressEvent(pEvent);
}


void PlainTextOutput::showEvent(QShowEvent *pEvent)
{
    QPlainTextEdit::showEvent(pEvent);
    setFont(DisplayOptions::tableFontStruct().font());
}


void PlainTextOutput::setCharDimensions(int nHChar, int nVChar)
{
    m_nHChar = nHChar;
    m_nVChar = nVChar;
}


QSize PlainTextOutput::sizeHint() const
{
    return QSize(DisplayOptions::tableFontStruct().averageCharWidth()*m_nHChar, DisplayOptions::tableFontStruct().height()*m_nVChar);
}


void PlainTextOutput::onAppendThisPlainText(QString const &sometext)
{
    moveCursor(QTextCursor::End);
    insertPlainText(sometext);
    moveCursor(QTextCursor::End);
    ensureCursorVisible();
}

