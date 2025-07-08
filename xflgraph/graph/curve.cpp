/****************************************************************************

    Xfl3d application
    Copyright (C) Andre Deperrois 
    License: GPL v3

*****************************************************************************/


#include <xflgraph/graph/curve.h>

#include <xflgraph/graph/graph.h>

int Curve::s_DefaultLineWidth = 2;
bool Curve::s_bAlignChildren  = true;



Curve::Curve()
{
    m_theStyle.m_Color = QColor(255,0,0,127);
    m_Name = "";
    m_theStyle.m_bIsVisible = true;
    m_theStyle.m_Symbol = Line::NOSYMBOL;
    m_theStyle.m_Width = s_DefaultLineWidth;
    m_theStyle.m_Stipple = Line::SOLID;

    m_bLeftAxis = true;

    m_iSelectedPt = -1;
}


Curve::Curve(Curve const &curve)
{
    m_theStyle = curve.theStyle();
    m_Name = curve.name();
    m_bLeftAxis = curve.m_bLeftAxis;
    m_iSelectedPt = -1;
    m_pts = curve.m_pts;
}


int Curve::appendPoint(double xn, double yn)
{
    if(qIsNaN(xn) || qIsNaN(yn))
    {
        // don't append
    }
    else
        m_pts.append({xn,yn});
    return size();
}


int  Curve::appendPoint(double xn, double yn, QString const &tag)
{
    if(qIsNaN(xn) || qIsNaN(yn))
    {
        // don't append
    }
    else
    {
        m_pts.append({xn,yn});
        m_Tag.append(tag);
    }
    return size();
}


void Curve::popFront()
{
    if(!m_pts.isEmpty() && !m_Tag.isEmpty())
    {
        m_pts.pop_front();
        m_Tag.pop_front();
    }
}


void Curve::setPoints(QVector<double> const &xc, QVector<double> const&yc)
{
    m_pts.clear();
    for(int i=0; i<std::min(xc.size(), yc.size()); i++)
    {
        m_pts.append({xc.at(i), yc.at(i)});
    }
}


void Curve::duplicate(Curve const *pCurve)
{
    if(!pCurve) return;

    copyData(pCurve);

    m_theStyle = pCurve->m_theStyle;
    m_Name  = pCurve->m_Name;
}


void Curve::copyData(Curve const*pCurve)
{
    if(!pCurve) return;
    m_pts = pCurve->m_pts;
}


int Curve::closestPoint(double xs, double ys, double xScale, double yScale) const
{
    int ref = -1;
    double dist = 1.e10;
    if (size()<1) return -1;
    for(int i=0; i<size(); i++)
    {
        double d2 =    (xs-m_pts.at(i).x()) * (xs-m_pts.at(i).x())*xScale*xScale
                     + (ys-m_pts.at(i).y()) * (ys-m_pts.at(i).y())*yScale*yScale;
        if (d2<dist)
        {
            dist = d2;
            ref = i;
        }
    }
    return ref;
}


int Curve::closestPoint(double const &xs, double const &ys, double &xSel, double &ySel, double &dist) const
{
    int nSel=0;
    double d2=0;
    dist = 1.e40;

    for(int i=0; i<size(); i++)
    {
        d2 =   (xs-m_pts.at(i).x())*(xs-m_pts.at(i).x()) + (ys-m_pts.at(i).y())*(ys-m_pts.at(i).y());
        if (d2<dist)
        {
            dist = d2;
            xSel = m_pts.at(i).x();
            ySel = m_pts.at(i).y();
            nSel = i;
        }
    }
    return nSel;
}


double Curve::xMin() const
{
    double xMin = 99999999.0;
    //    if(n==0) xmin = .0;
    //    else
    for(int i=0; i<size();i++)
        xMin = std::min(xMin, m_pts.at(i).x());
    return xMin;
}


double Curve::xMax() const
{
    double xMax = -99999999.0;
    //    if(n==0) xmax = 1.0;
    //    else
    for(int i=0; i<size();i++)
    {
        xMax = std::max(xMax, m_pts.at(i).x());
    }
    return xMax;
}


/**
 * Returns the minimum y value of this curve
 *@return the y min value, or +99999999.0 if the curve has no points
 */
double Curve::yMin() const
{
    double yMin = 99999999.0;
    //    if(n==0) ymin = .0;
    //    else
    for(int i=0; i<m_pts.size();i++)
    {
        yMin = std::min(yMin, m_pts.at(i).y());
    }
    return yMin;
}


/**
 * Returns the maximum y value of this curve
 *@return the y max value, or -99999999.0 if the curve has no points
 */
double Curve::yMax() const
{
    double yMax = -99999999.0;
    //    if(n==0) ymax = 1.0;
    //    else
    for(int i=0; i<m_pts.size();i++)
    {
        yMax = std::max(yMax, m_pts.at(i).y());
    }
    return yMax;
}



void Curve::setTheStyle(Line::enumLineStipple Style, int Width, QColor color, Line::enumPointStyle PointStyle, bool bVisible)
{
    m_theStyle.m_Stipple = Style;
    m_theStyle.m_Width = Width;
    m_theStyle.m_Color = color;
    m_theStyle.m_Symbol = PointStyle;
    m_theStyle.m_bIsVisible = bVisible;
}














