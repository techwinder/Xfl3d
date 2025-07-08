/****************************************************************************

    Xfl3d application
    Copyright (C) Andre Deperrois 
    License: GPL v3

*****************************************************************************/


#include "quad2d.h"
#include <xflmath/constants.h>
#include <xflgeom/geom_globals/geom_global.h>
#include <xflgeom/geom2d/segment2d.h>
#include <xflgeom/geom2d/triangle2d.h>


Quad2d::Quad2d()
{
    m_Area = 0.0;
}

/*
Quad2d::Quad2d(Quad2d  const &quad2d)
{
    setPanelFrame(quad2d.S[0],quad2d.S[1],quad2d.S[2],quad2d.S[3]);
}*/


Quad2d::Quad2d(const Vector2d &S0, const Vector2d &S1, const Vector2d &S2, const Vector2d &S3)
{
    setPanelFrame(S0,S1,S2,S3);
}


void Quad2d::setPanelFrame(Vector2d const &S0, Vector2d const &S1, Vector2d const &S2, Vector2d const &S3)
{
    S[0]=S0;  S[1]=S1;  S[2]=S2;  S[3]=S3;
    setPanelFrame();
}


void Quad2d::setPanelFrame()
{
    CoG_G = (S[0]+S[1]+S[2]+S[3])/4.0;
    m_Area =0.0; /**@todo improve */
}


void Quad2d::initialize()
{

}


Segment2d Quad2d::edge(int i) const
{
    if     (i==0) return Segment2d(S[0], S[1]);
    else if(i==1) return Segment2d(S[1], S[2]);
    else if(i==2) return Segment2d(S[2], S[3]);
    else if(i==3) return Segment2d(S[3], S[0]);
    else
    {
        qDebug("Panel3::edge   Problem");
        Segment2d anEdge;
        return anEdge;
    }
}


bool Quad2d::contains(const Vector2d &pt) const
{
    return contains(pt.x, pt.y);
}


/**
 * Determines if the point with ccordinates (x,y) is inside the quad or not.
 * edges are considered to be inside the triangle
 * @return  true if the point is inside the triangle, false otherwise
 */
bool Quad2d::contains(double x, double y) const
{
    Vector2d S01, S02, S03;
    double vv1, vv2, v0v1, v0v2, v1v2, a, b;
    S01.set(S[1].x-S[0].x, S[1].y-S[0].y);
    S02.set(S[2].x-S[0].x, S[2].y-S[0].y);
    S03.set(S[3].x-S[0].x, S[3].y-S[0].y);

    // test if the point lies in either of the two half triangles
    vv1 =     x*S01.y -    y*S01.x;
    vv2 =     x*S02.y -    y*S02.x;
    v0v1 =  S[0].x*S01.y -  S[0].y*S01.x;
    v0v2 =  S[0].x*S02.y -  S[0].y*S02.x;
    v1v2 = S01.x*S02.y - S01.y*S02.x;
    a =  (vv2-v0v2)/v1v2;
    b = -(vv1-v0v1)/v1v2;
    if(a>=-LENGTHPRECISION && b>=-LENGTHPRECISION && a+b<=1.0+LENGTHPRECISION) return true;

    vv1 =     x*S02.y -    y*S02.x;
    vv2 =     x*S03.y -    y*S03.x;
    v0v1 =  S[0].x*S02.y -  S[0].y*S02.x;
    v0v2 =  S[0].x*S03.y -  S[0].y*S03.x;
    v1v2 = S02.x*S03.y - S02.y*S03.x;
    a =  (vv2-v0v2)/v1v2;
    b = -(vv1-v0v1)/v1v2;
    if(a>=-LENGTHPRECISION && b>=-LENGTHPRECISION && a+b<=1.0+LENGTHPRECISION) return true;

    return false;
}


/** returns a half triangle */
Triangle2d Quad2d::triangle(int iTriangle) const
{
    iTriangle = iTriangle%2;
    if(iTriangle==0) return Triangle2d(S[0], S[1], S[3]);
    else             return Triangle2d(S[1], S[2], S[3]);
}


void Quad2d::displayNodes(const QString &msg) const
{
    QString strong, str;
    str = QString::asprintf("Vector2d(%7.3g, %7.3g), Vector2d(%7.3g, %7.3g), Vector32d(%7.3g, %7.3g), Vector2d(%7.3g, %7.3g)",
                S[0].x, S[0].y, S[1].x, S[1].y, S[2].x, S[2].y, S[3].x, S[3].y);
    strong = msg+ " " +str;
    qDebug("%s", strong.toStdString().c_str());
}

