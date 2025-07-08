/****************************************************************************

    Xfl3d application
    Copyright (C) Andre Deperrois 
    License: GPL v3

*****************************************************************************/

#include <cmath>


#include "axis.h"

int Axis::s_UnitRatio = 5;

Axis::Axis()
{
    m_bAuto = true;

    m_min = 0.0;
    m_max = 1.0;

    m_O = 0.0;
    m_unit=0.1;
    m_scale = 1.0;
    m_MinorUnit = 0.02;

    m_theStyle.m_Color = Qt::white;
    m_theStyle.m_Stipple = Line::SOLID;
    m_theStyle.m_Width = 1;

    m_bInverted = false;
    m_bLogScale = false;

    m_Axis = AXIS::XAXIS;

    m_exponent = 1;
    m_UnitFactor = 1.0;
}


/**
 * copies everything except the title and bXAxis property
 */
void Axis::copySettings(Axis const & axis)
{
    m_bAuto     = axis.m_bAuto;

    m_min       = axis.m_min;
    m_max       = axis.m_max;

    m_O         = axis.m_O;
    m_unit      = axis.m_unit;
    m_scale     = axis.m_scale;
    m_MinorUnit = axis.m_MinorUnit;

    m_theStyle  = axis.m_theStyle;

    m_bInverted = axis.m_bInverted;
    m_bLogScale = axis.m_bLogScale;
}


void Axis::loadSettings(QSettings &settings)
{
    QString name;
    switch(m_Axis)
    {
        case AXIS::XAXIS:      name="x-axis";    break;
        case AXIS::LEFTYAXIS:  name="y0-axis";   break;
        case AXIS::RIGHTYAXIS: name="y1-axis";   break;
    }
    settings.beginGroup(name);
    {
        m_unit  = settings.value("Unit",    m_unit).toDouble();
        m_scale = settings.value("Scale",   m_scale).toDouble();
        m_O     = settings.value("Origin", m_O).toDouble();
        m_min   = settings.value("Min",     m_min).toDouble();
        m_max   = settings.value("Max",     m_max).toDouble();

        m_bAuto = settings.value("bAuto", m_bAuto).toBool();

        if(settings.contains("Inverted")) m_bInverted   = settings.value("Inverted", false).toBool();
        if(settings.contains("LogScale")) m_bLogScale   = settings.value("LogScale", false).toBool();
        m_theStyle.loadSettings(settings, "AxisStyle");
    }
    settings.endGroup();
}


void Axis::saveSettings(QSettings &settings) const
{
    QString name;
    switch(m_Axis)
    {
        case AXIS::XAXIS:      name="x-axis";    break;
        case AXIS::LEFTYAXIS:  name="y0-axis";   break;
        case AXIS::RIGHTYAXIS: name="y1-axis";   break;
    }

    settings.beginGroup(name);
    {
        settings.setValue("Unit",   m_unit);
        settings.setValue("Scale",  m_scale);
        settings.setValue("Origin", m_O);
        settings.setValue("Min",    m_min);
        settings.setValue("Max",    m_max);

        settings.setValue("bAuto", m_bAuto);
        settings.setValue("Inverted", m_bInverted);
        settings.setValue("LogScale", m_bLogScale);

        m_theStyle.saveSettings(settings, "AxisStyle");
    }
    settings.endGroup();
}


void Axis::setAutoUnit()
{
    m_unit = fabs(m_max-m_min)/3.0;

    if (m_unit<1.0) m_exponent = int(log10(m_unit*1.00001))-1;
    else            m_exponent = int(log10(m_unit*1.00001));

    int main = int(m_unit/pow(10.0, m_exponent)*1.000001);

    if(main<2)
        m_unit = pow(10.0,m_exponent);
    else if (main<5)
        m_unit = 2.0*pow(10.0,m_exponent);
    else
        m_unit = 5.0*pow(10.0,m_exponent);
}


void Axis::listAxis() const
{
//    qDebug(m_Title.toStdString().c_str());
    qDebug(" origin=%7.3g  min=%7.3g  max=%7.3g  unit=%7.3g  scale=%7.3g", m_O, m_min, m_max, m_unit, m_scale);
}
