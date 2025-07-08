/****************************************************************************

    Xfl3d application
    Copyright (C) Andre Deperrois 
    License: GPL v3

*****************************************************************************/



#pragma once

#include <QFile>
#include <QPainter>
#include <QVector>


#include <xflgeom/geom2d/vector2d.h>
#include <xflgeom/geom3d/vector3d.h>


class Segment2d;
class Segment3d;
class Triangle3d;
class Spline;
class BSpline;
class NURBSSurface;

bool intersectXYPlane(Vector3d const &A,  Vector3d const &U,  Vector3d &I);

bool intersectQuad3d(Vector3d const &LA, Vector3d const &LB, Vector3d const &TA, Vector3d const &TB,
                      Vector3d const &A,  Vector3d const &U,  Vector3d &I, bool bDirOnly);

bool intersect(Vector2d const &A, Vector2d const &B, Vector2d const &C, Vector2d const &D, Vector2d &M);
bool intersectSegment(Vector2d const &A0, Vector2d const &A1, Vector2d const &B0, Vector2d const &B1, Vector2d &IPt, bool bPointsInside, double vtx_precision=1.0e-5);
bool intersectRay(Vector2d const &A0, Vector2d const &A1, Vector2d const &O, Vector2d const &U, Vector2d &IPt);

bool intersectTriangles3d(Triangle3d const &t0, Triangle3d const &t1, Segment3d &seg, double precision);
bool overlapSegments3d(Segment3d const &s0, Segment3d const &s1, Segment3d &seg, double precision, bool bCheckColinearity=false);

double distanceToLine2d(const Vector2d &A, const Vector2d &B, const Vector2d &P);

double distanceToLine3d(Vector3d const &A, Vector3d const &B, const Vector3d &P);


void splitTriangle(QVector<Segment3d> const & raylist,
                   QVector<Segment3d> &polygon3d, QVector<Segment3d>&PSLG3d,
                   QVector<Triangle3d> &trianglelist, bool bLeftSide) ;

void makeSphere(double radius, int nSplit, QVector<Triangle3d> &triangles);

void rotateOnPlane(Vector3d const &PlaneNormal, Vector3d const &Vin, Vector3d &VOut);
void rotateOnPlane(Vector3d const &Origin, Vector3d const &PlaneNormal, Vector3d const &Ptin, Vector3d &PtOut);

double basis(int i, int deg, double t, const double *knots);
double basisDerivative(int i, int deg, double t, const double *knots);

int isVector3d(QVector<Vector3d> const &m_Node, Vector3d &Pt);


