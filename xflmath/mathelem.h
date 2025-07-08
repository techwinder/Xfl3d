/****************************************************************************

  Xfl3d application
  Copyright (C) Andre Deperrois 
  License: GPL v3

*****************************************************************************/

#pragma once


#include <QString>

#include <complex>



namespace xfl {

/** @enum The different types of panel distribution on the wing */
    enum enumDistribution {UNIFORM, COSINE, SINE, INV_SINE, INV_SINH, TANH, EXP, INV_EXP};


    QStringList distributionTypes();
    QString distributionType(xfl::enumDistribution dist);
    xfl::enumDistribution distributionType(const QString &Dist);

    void getPointDistribution(QVector<double> &fraclist, int nPanels, xfl::enumDistribution DistType=xfl::COSINE);
    double getDistribFraction(double tau, xfl::enumDistribution DistType=xfl::COSINE);

}


void testEigen();
void Legendre(int n, double *a);
double LegendreAssociated( int m, int l, double x);
double Laguerre(int alpha, int k, double x);

std::complex<double> LaplaceHarmonic(int m, int l, double theta, double phi);

int factorial(int n);
int binomial(int n, int k);


bool cubicSplineInterpolation(int n, const double *x, const double *y, double *a, double *b, double *c, double *d);



void testPointDistribution();

double err_func(double x);
double erf_inv(double a);

double interpolateLine(double x, double x0, double y0, double x1, double y1);

double interpolatePolyLine(double x, const QVector<double> &xp, const QVector<double> &yp, bool bExtend);

inline bool isEven(int n) {return n%2==0;}
inline bool isOdd(int n) {return n%2==1;}

bool isBetween(int f, int f1, int f2);
bool isBetween(int f, double f1, double f2);

double bunchedParameter(double bunchdist, double bunchamp, double t);

double sigmoid(double amplitude, double t);
double doubleSigmoid(double amplitude, double t);

bool linearRegression(int n, double const *x, double const*y, double &a, double &b);

double HicksHenne(double x, double t1, double t2, double xmin, double xmax);
