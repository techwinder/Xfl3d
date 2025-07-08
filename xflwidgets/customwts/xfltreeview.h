/****************************************************************************

    Xfl3d application
    Copyright (C) Andre Deperrois 
    License: GPL v3

*****************************************************************************/

#pragma once

#include <QTreeView>



class XflTreeView : public QTreeView
{
    public:
        XflTreeView(QWidget *pParent=nullptr);

    private:
         QModelIndex moveCursor(QAbstractItemView::CursorAction cursorAction, Qt::KeyboardModifiers modifiers) override;
};


