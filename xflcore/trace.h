/****************************************************************************

    Xfl3d application
    Copyright (C) Andre Deperrois 
    License: GPL v3

*****************************************************************************/

#include <QFile>
#include <QString>

extern bool g_bTrace;
extern QFile *g_pTraceFile;

void trace(int n);
void trace(QString const &msg);
void trace(QString const &msg, bool b);
void trace(QString const &msg, int n);
void trace(QString const &msg, double f);
void trace(QString const &msg, QString txt);


void startTrace(const QString &filename);
