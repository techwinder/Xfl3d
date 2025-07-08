/****************************************************************************

    Xfl3d application
    Copyright (C) Andre Deperrois 
    License: GPL v3

*****************************************************************************/


#pragma once

#include <QVector>

#include <complex>
#include <vector>


/** Data structure for multithreaded matrix multiplication */
struct MatMultData
{
    MatMultData(double* _A, double* _B, double* _AB, int _nRows, int _n, int _nCols)
    {
        A     = _A;
        B     = _B;
        AB    = _AB;
        nRows = _nRows;
        n     = _n;
        nCols = _nCols;
    }

    double* A=nullptr;
    double* B=nullptr;
    double* AB=nullptr;
    int nRows=0;
    int n=0;
    int nCols=0;
};

/** Inverts in place a 2x2 matrix */
bool  invert22(double *l);
/** Inverts a 2x2 matrix */
bool  invert22(double *in, double *out);
/** multiplies two 2x2 matrices */
void  matMult22(double *a, double *b, double *ab);

/** Transposes in place a 3x3 matrix */
void  transpose33(double *l);
/** Inverts in place a 3x3 matrix */
bool  invert33(double *l);
/** Inverts a 3x3 matrix */
bool  invert33(double *in, double *out);

void  AV33(double *A, double *v, double *p);

/** multiplies two 3x3 matrices */
void  matMult33(double *a, double *b, double *ab);

float   det33(float *M);
double  det33(double *aij);
std::complex<double>  det33(std::complex<double> *aij);


/** Transposes in place a 4x4 matrix */
void  transpose44f(float *l);

/** Transposes a 4x4 matrix */
void  transpose44f(const float *in, float *out);


double  det44(double *aij);


std::complex<double>  det44(std::complex<double> *aij);

float   cofactor44(float *aij, int i, int j);
double  cofactor44(double *aij, int i, int j);
std::complex<double>  cofactor44(std::complex<double> *aij, int i, int j);

bool  invert44(double *l);
bool  invert44(double *ain, double *aout);
bool  invert44(std::complex<double> *ain, std::complex<double> *aout);


bool  Gauss(double *A, int n, double *B, int m, bool &bCancel);
bool  GaussNoPivot(double* A, int n, double* RHS, double * x);

bool  eigenVector(double a[][4], std::complex<double> lambda, std::complex<double> *V);

void  display_vec(double *vec, int n);
void  display_vec(float *vec, int n, bool bHorizontal=false);

void  display_mat(double const *mat, int rows, int cols=-1);
void  display_mat(float const*mat, int rows, int cols=-1);
void  display_mat(std::vector<std::vector<double>> const &mat);
void  display_mat(QVector<QVector<double>> const &mat);
void  display_mat(QVector<double> const &mat, int size);

void  saveMatrixtoFile(std::vector<double> const &mat, int N);

void  listArray(double *array, int size);

void  listArray(std::vector<double> const &array);
void  listArrays(std::vector<double> const &array1, std::vector<double> const &array2);
void  listArrays(std::vector<double> const &array1, std::vector<double> const &array2, std::vector<double> const &array3);

void  listArray(QVector<double> const &array);
void  listArrays(QVector<double> const &array1, QVector<double> const &array2);
void  listArrays(QVector<float> const &array1, QVector<float> const &array2);

void  matMult_SingleThread(double * const A, double * const B, double* AB, int m, int n, int q);
void  matMult(double* const A, double* const B, double* AB, int n, int nThreads=-1);
void  matMult(double* const A, double* const B, double* AB, int nRows, int n, int nCols, int nThreads=-1);
void  matMultBlock(int iBlock, MatMultData &mmd, int nThreads);

void  CholevskiFactor(double* A, double* L, int n);
bool  CholevskiSolve(const double *A, double* b, int n);

void  matVecMult(double const *A, double const *V, double *res, int nRows, int nCols);
void  matVecMult2x2(const double *A, const double *X, double* Y);
void  matVecMult3x3(const double *A, const double *X, double* Y);
void  matVecMult4x4(const double *A, const double *X, double* Y);

void  rowToColumnMajorIndex(int n, double *aij, double  *taij);
void  columnToRowMajorIndex(int n, double *aij, double  *taij);
bool  inverseMatrix_old(int n, double *aij, double *invAij);

void  print_matrix_colmajor(const char* desc, int m, int n, double* a, int lda );


double  determinant(double *mat, int n);

bool  solve2x2(double *M, double *rhs, int nrhs);
bool  solve3x3(double *M, double *rhs, int nrhs);
bool  solve4x4(double *M, double *rhs, int nrhs);


void setIdentityMatrix(double *M, int n);

bool makeILUC(double const*A, double *ILU, int n, int p);
bool makeSGS(double const *A, double *SGSLU, int n);
void ILUC(double const*A, double *ILU, int n, int p);
