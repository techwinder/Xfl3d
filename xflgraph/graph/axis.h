/****************************************************************************

    Xfl3d application
    Copyright (C) Andre Deperrois 
    License: GPL v3

*****************************************************************************/

#pragma once

#include <QString>
#include <QColor>
#include <QSettings>

#include <xflcore/linestyle.h>

namespace AXIS
{
    /** @enum The different applications available to the user */
    enum enumAxis {XAXIS, LEFTYAXIS, RIGHTYAXIS};
}


class Axis
{
//    friend class Graph;

    public:
        Axis();

        AXIS::enumAxis const & axis() const {return m_Axis;}
        void setAxis(AXIS::enumAxis ax) {m_Axis=ax;}

        bool isHorizontal() const {return m_Axis==AXIS::XAXIS;}

        void set0(double f)         {m_O=f;}
        void setMax(double f)       {m_max=f;}
        void setMin(double f)       {m_min=f;}
        void setMinorUnit(double f) {m_MinorUnit = f;}
        void setUnit(double f)      {m_unit = f;}
        void setScale(double sc)    {m_scale=sc;}

        double origin()  const {return m_O;}
        double axmin()   const {return m_min;}
        double axmax()   const {return m_max;}
        double minUnit() const {return m_MinorUnit;}
        double unit()    const {return m_unit;}
        double scale()   const {return m_scale;}

        void setExponent(int expo) {m_exponent=expo;}
        int exponent()   const {return m_exponent;}

        bool bAuto() const {return m_bAuto;}
        void setAutoUnit(bool bAuto) {m_bAuto=bAuto;}

        void setLogScale(bool bLog) {m_bLogScale=bLog;}
        bool bLogScale() const {return m_bLogScale;}

        void setInverted(bool bInv) {m_bInverted=bInv;}
        bool bInverted() const  {return m_bInverted;}

        void setUnitFactor(double unitfactor) {m_UnitFactor=unitfactor;}
        double unitFactor() const {return m_UnitFactor;}

        void setAutoUnit();
        void expand(double zoomfactor) {m_scale*=zoomfactor;}

        void copySettings(Axis const & axis);

        QColor colour() const {return m_theStyle.m_Color;}
        void setColour(QColor clr){m_theStyle.m_Color=clr;}

        Line::enumLineStipple stipple() const {return m_theStyle.m_Stipple;}
        void setStipple(Line::enumLineStipple iStyle)  {m_theStyle.m_Stipple=iStyle;}

        int width() const {return m_theStyle.m_Width;}
        void setWidth(int iWidth) {m_theStyle.m_Width=iWidth;}

        bool isVisible() const {return m_theStyle.m_bIsVisible;}
        void setVisible(bool bVisible) {m_theStyle.m_bIsVisible = bVisible;}

        Line::enumPointStyle pointStyle() const {return m_theStyle.m_Symbol;}
        void setPointStyle(Line::enumPointStyle iPointStyle)  {m_theStyle.m_Symbol=iPointStyle;}

        void setTheStyle(LineStyle ls)  {m_theStyle=ls;}
        LineStyle theStyle() const {return m_theStyle;}

        void loadSettings(QSettings &settings);
        void saveSettings(QSettings &settings) const;

        void listAxis() const;


        static int unitRatio() {return s_UnitRatio;}

    private:
        AXIS::enumAxis m_Axis;

        double m_O;
        double m_unit;
        double m_scale;
        double m_min, m_max;
        double m_MinorUnit;

        bool m_bAuto;
        bool m_bInverted;
        bool m_bLogScale;

        LineStyle m_theStyle;

        int m_exponent;

        double m_UnitFactor; /** used to display labels with units, e.g. meters */

        static int s_UnitRatio; /** =m_Unit/m_MinorUnit */
};

