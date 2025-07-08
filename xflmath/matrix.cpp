/****************************************************************************

    Xfl3d application
    Copyright (C) Andre Deperrois 
    License: GPL v3

*****************************************************************************/

#include <cstring>
#include <QString>
#include <QFutureSynchronizer>
#include <QtConcurrent/QtConcurrent>


#include <complex>


#if defined ACCELERATE
  #include <Accelerate/Accelerate.h>
  #define lapack_int int
#elif defined INTEL_MKL
    #include <mkl.h>
#elif defined OPENBLAS
    #include <cblas.h>
    #include <openblas/lapacke.h>
#endif


#include "matrix.h"


#include <xflcore/trace.h>
#include <xflmath/constants.h>


/** Inverts in place a 2x2 matrix */
bool invert22(double *l)
{
    double mat[4];
    memcpy(mat,l,sizeof(mat));
    return invert22(mat, l);
}


/** Inverts a 2x2 matrix */
bool invert22(double *in, double *out)
{
    double det(0);

    det  = in[0]*in[3] - in[1]*in[2];
    if(fabs(det)<1.e-20) return false;

    * out     =  in[3]/det;
    *(out+1)  = -in[1]/det;
    *(out+2)  = -in[2]/det;
    *(out+3)  =  in[0]/det;

    return true;
}

/** multiplies two 2x2 matrices */
void matMult22(double *a, double *b, double *ab)
{
    ab[0] = a[0]*b[0] + a[1]*b[2];
    ab[1] = a[0]*b[1] + a[1]*b[3];
    ab[2] = a[2]*b[0] + a[3]*b[2];
    ab[3] = a[2]*b[1] + a[3]*b[3];
}


/** Transposes in place a 3x3 matrix */
void transpose33(double *l)
{
    double temp(0);
    temp=l[1];   l[1]=l[3];  l[3]=temp;
    temp=l[2];   l[2]=l[6];  l[6]=temp;
    temp=l[5];   l[5]=l[7];  l[7]=temp;
}


/** Inverts in place a 3x3 matrix */
bool invert33(double *l)
{
    double mat[9];
    memcpy(mat,l,sizeof(mat));
    return invert33(mat, l);
}


/** Inverts a 3x3 matrix */
bool invert33(double *in, double *out)
{
    double det;

    det  = in[0] *(in[4] * in[8] - in[5]* in[7]);
    det -= in[1] *(in[3] * in[8] - in[5]* in[6]);
    det += in[2] *(in[3] * in[7] - in[4]* in[6]);
    if(fabs(det)<1.e-20) return false;

    * out     = (in[4] * in[8] - in[5] * in[7])/det;
    *(out+1)  = (in[2] * in[7] - in[1] * in[8])/det;
    *(out+2)  = (in[1] * in[5] - in[2] * in[4])/det;

    *(out+3)  = (in[5] * in[6] - in[3] * in[8])/det;
    *(out+4)  = (in[0] * in[8] - in[2] * in[6])/det;
    *(out+5)  = (in[2] * in[3] - in[0] * in[5])/det;

    *(out+6)  = (in[3] * in[7] - in[4] * in[6])/det;
    *(out+7)  = (in[1] * in[6] - in[0] * in[7])/det;
    *(out+8)  = (in[0] * in[4] - in[1] * in[3])/det;

    return true;
}


/**
 * @brief Av33 performs the product of the 3x3 matrix A and vector v. Stores the result in array p.
 * @param A a pointer to the matrix
 * @param v the input vector;
 * @param p the output vector;
 */
void AV33(double *A, double *v, double *p)
{
    p[0] = A[0]*v[0] + A[1]*v[1] + A[2]*v[2];
    p[1] = A[3]*v[0] + A[4]*v[1] + A[5]*v[2];
    p[2] = A[6]*v[0] + A[7]*v[1] + A[8]*v[2];
}


/** multiplies two 3x3 matrices */
void matMult33(double *a, double *b, double *ab)
{
    ab[0] = a[0]*b[0] + a[1]*b[3] + a[2]*b[6];
    ab[1] = a[0]*b[1] + a[1]*b[4] + a[2]*b[7];
    ab[2] = a[0]*b[2] + a[1]*b[5] + a[2]*b[8];

    ab[3] = a[3]*b[0] + a[4]*b[3] + a[5]*b[6];
    ab[4] = a[3]*b[1] + a[4]*b[4] + a[5]*b[7];
    ab[5] = a[3]*b[2] + a[4]*b[5] + a[5]*b[8];

    ab[6] = a[6]*b[0] + a[7]*b[3] + a[8]*b[6];
    ab[7] = a[6]*b[1] + a[7]*b[4] + a[8]*b[7];
    ab[8] = a[6]*b[2] + a[7]*b[5] + a[8]*b[8];
}


/**
 * Solves a linear system using Gauss partial pivot method
 * @param A a pointer to the single dimensionnal array of double values. Size is n².
 * @param n the size of the square matrix
 * @param B a pointer to the array of m RHS
 * @param m the number of RHS arrays to solve
 * @param pbCancel a pointer to the boolean variable which holds true if the operation should be interrupted.
 * @return true if the problem was successfully solved.
 */
bool Gauss(double *A, int n, double *B, int m, bool &bCancel)
{
    int row=0, i=0, j=0, pivot_row=0, k=0;
    double max=0, dum=0;
    double *pa=nullptr, *pA=nullptr, *A_pivot_row=nullptr;

    // for each variable find pivot row and perform forward substitution
    pa = A;
    for (row=0; row<n-1; row++, pa+=n)
    {
        if(bCancel) return false;

        //  find the pivot row
        A_pivot_row = pa;
        max = fabs(*(pa + row));
        pA = pa + n;
        pivot_row = row;
        for (i=row+1; i<n; pA+=n, i++)
        {
            if ((dum = fabs(*(pA+row)))>max)
            {
                max = dum;
                A_pivot_row = pA;
                pivot_row = i;
            }
        }

        if (max <= PRECISION)
            return false; // the matrix A is singular

        // and if it differs from the current row, interchange the two rows.
        if (pivot_row != row)
        {
            for (i=row; i<n; i++)
            {
                dum = *(pa + i);
                *(pa + i) = *(A_pivot_row + i);
                *(A_pivot_row + i) = dum;
            }
            for(k=0; k<m; k++)
            {
                dum = B[row+k*n];
                B[row+k*n] = B[pivot_row+k*n];
                B[pivot_row+k*n] = dum;
            }
        }

        // Perform forward substitution
        for (i= row+1; i<n; i++)
        {
            pA = A + i * n;
            dum = - *(pA + row) / *(pa + row);
            *(pA + row) = 0.0;
            for (j=row+1; j<n; j++) *(pA+j) += dum * *(pa + j);
            for (k=0; k<m; k++)
                B[i+k*n] += dum * B[row+k*n];
        }
    }

    // Perform backward substitution
    pa = A + (n-1) * n;
    for (row = n-1; row >= 0; pa -= n, row--)
    {
        if(bCancel) return false;

        if ( fabs(*(pa + row)) <PRECISION)
            return false;           // matrix is singular

        dum = 1.0 / *(pa + row);
        for (i=row+1; i<n; i++) *(pa + i) *= dum;
        for(k=0; k<m; k++) B[row+k*n] *= dum;
        for (i=0, pA=A; i<row; pA+= n, i++)
        {
            dum = *(pA + row);
            for (j=row+1; j<n; j++) *(pA + j) -= dum * *(pa+j);
            for(k=0; k<m; k++)
                B[i+k*n] -= dum * B[row+k*n];
        }
    }
    return true;
}



// Gauss no pivot, assuming no zero diagonal element
bool GaussNoPivot(double* A, int n, double* RHS, double *x)
{
    //Forward substitution
    for(int col=0; col<n; col++)
    {
        for(int row=col+1; row<n; row++)
        {
            if(fabs(A[col*n+col])<PRECISION) return false;
            double pivot = 1.0 / A[col*n+col]  * A[row*n+col];
            for(int j=col; j<n; j++)
            {
                 A[row*n+j] += - pivot * A[col*n+j];
            }
            RHS[row] +=  - pivot * RHS[col];
        }
    }

    //Backward substitution
    for(int row=n-1; row>=0; row--)
    {
        if(fabs(A[row*n+row])<PRECISION) return false;
        double pivot = 1.0 / A[row*n+row];
        x[row] = RHS[row] * pivot;
        for(int j=row+1; j<n; j++)
        {
            x[row] -= pivot * A[row*n+j] * x[j];
        }
    }
    return true;
}


/** Transposes in place a 4x4 matrix */
void transpose44f(float *l)
{
    qDebug()<<"transpose in place";
    double temp;
    temp=l[1];     l[1] =l[4];     l[4] =temp;
    temp=l[2];     l[2] =l[8];     l[8] =temp;
    temp=l[3];     l[3] =l[12];    l[12]=temp;
    temp=l[6];     l[6] =l[9];     l[9] =temp;
    temp=l[7];     l[7] =l[13];    l[13]=temp;
    temp=l[11];    l[11]=l[14];    l[14]=temp;
}


void transpose44f(float const *in, float *out)
{
    qDebug()<<"transpose out of place";
    out[0]  = in[ 0];
    out[1]  = in[ 4];
    out[2]  = in[ 8];
    out[3]  = in[12];
    out[4]  = in[ 1];
    out[5]  = in[ 5];
    out[6]  = in[ 9];
    out[7]  = in[13];
    out[8]  = in[ 2];
    out[9]  = in[ 6];
    out[10] = in[13];
    out[11] = in[14];
    out[12] = in[ 3];
    out[13] = in[ 7];
    out[14] = in[11];
    out[15] = in[15];
}


/** Inverts in place a 4x4 matrix */
bool invert44(double *l)
{
    double mat[16];
    memcpy(mat,l,sizeof(mat));
    return invert44(mat, l);
}

/**
 * Inverts a real 4x4 matrix
 * @param ain in input, a pointer to a one-dimensional array holding the 16 complex values of the input matrix
 * @param aout in output, a pointer to a one-dimensional array holding the 16 complex values of the inverted matrix
 * @return if the inversion was successful
*/
bool invert44(double *ain, double *aout)
{
    //small size, use the direct method
    double det;
    double sign;

    det = det44(ain);

    if(fabs(det)<1.e-50) return false;

    for(int i=0; i<4; i++)
    {
        for(int j=0; j<4; j++)
        {
//            sign = pow(-1.0,i+j);
            sign = -(((i+j)%2)*2-1);
            aout[4*j+i] = sign * cofactor44(ain, i, j)/det;
        }
    }
    return true;
}


/**
 * Inverts a complex 4x4 matrix
 * @param ain in input, a pointer to a one-dimensional array holding the 16 complex values of the input matrix
 * @param aout in output, a pointer to a one-dimensional array holding the 16 complex values of the inverted matrix
 * @return if the inversion was successful
*/
bool invert44(std::complex<double> *ain, std::complex<double> *aout)
{
    //small size, use the direct method
    std::complex<double> det;
    double sign;

    det = det44(ain);

    if(abs(det)<PRECISION) return false;

    for(int i=0; i<4; i++)
    {
        for(int j=0; j<4; j++)
        {
//            sign = pow(-1.0,i+j);
            sign = -(((i+j)%2)*2-1);
            aout[4*j+i] = sign * cofactor44(ain, i, j)/det;
        }
    }
    return true;
}


/**
*Returns the determinant of a complex 3x3 matrix
*@param aij a pointer to a one-dimensional array holding the 9 complex values of the matrix
*@return the matrix's determinant
*/
std::complex<double> det33(std::complex<double> *aij)
{
    std::complex<double> det;

    det  = aij[0]*aij[4]*aij[8];
    det -= aij[0]*aij[5]*aij[7];

    det -= aij[1]*aij[3]*aij[8];
    det += aij[1]*aij[5]*aij[6];

    det += aij[2]*aij[3]*aij[7];
    det -= aij[2]*aij[4]*aij[6];

    return det;
}


/**
 * Returns the determinant of a 4x4 matrix
 * @param aij a pointer to a one-dimensional array holding the 16 double values of the matrix
 * @return the matrix's determinant
 */
double det44(double *aij)
{
    int p(0), q(0);
    double det(0), sign(0);
    double a33[9]{0};

    det = 0.0;

    for(int j=0; j<4; j++)
    {
        p = 0;
        for(int k=0; k<4; k++)
        {
            if(k!=0)
            {
                q = 0;
                for(int l=0; l<4; l++)
                {
                    if(l!=j)
                    {
                        *(a33+p*3+q) = *(aij+4*k+l);
                        q++;
                    }
                }
                Q_ASSERT(q==3);
                p++;
            }
        }
        Q_ASSERT(p==3);
//        sign = pow(-1.0,j);
        sign = -(((j)%2)*2-1);
        det += aij[j] * sign * det33(a33);
    }

    return det;
}


/** returns the cofactor of element i,j, in the 4x4 matrix aij */
float cofactor44(float *aij, int i, int j)
{
    int p(0), q(0);
    float a33[9]{0};

    p = 0;
    for(int k=0; k<4; k++)
    {
        if(k!=i)
        {
            q = 0;
            for(int l=0; l<4; l++)
            {
                if(l!=j)
                {
                    a33[p*3+q] = *(aij+4*k+l);
                    q++;
                }
            }
            p++;
        }
    }
    return det33(a33);
}


/** returns the cofactor of element i,j, in the 4x4 matrix aij */
double cofactor44(double *aij, int i, int j)
{
    int p(0), q(0);
    double a33[9]{0};

    p = 0;
    for(int k=0; k<4; k++)
    {
        if(k!=i)
        {
            q = 0;
            for(int l=0; l<4; l++)
            {
                if(l!=j)
                {
                    a33[p*3+q] = *(aij+4*k+l);
                    q++;
                }
            }
            p++;
        }
    }
    return det33(a33);
}

/**
*Returns the cofactor of an element in a 4x4 matrix of complex values.
*@param aij a pointer to a one-dimensional array holding the 16 complex values of the matrix.
*@param i the number of the element's line, starting at 0.
*@param j the number of the element's column, starting at 0.
*@return the cofactor of element (i,j).
*/
std::complex<double> cofactor44(std::complex<double> *aij, int i, int j)
{
    //returns the complex cofactor    of element i,j, in the 4x4 matrix aij
    int k(0),l(0),p(0),q(0);
    std::complex<double> a33[9];

    p = 0;
    for(k=0; k<4; k++)
    {
        if(k!=i)
        {
            q = 0;
            for(l=0; l<4; l++)
            {
                if(l!=j)
                {
                    a33[p*3+q] = *(aij+4*k+l);
                    q++;
                }
            }
            p++;
        }
    }
    return det33(a33);
}

/**
* Returns the determinant of a complex 4x4 matrix
* @param aij a pointer to a one-dimensional array holding the 16 complex double values of the matrix
* @return the matrix's determinant
*/
std::complex<double> det44(std::complex<double> *aij)
{
//    returns the determinant of a 4x4 matrix

    int i(0),p(0),q(0);
    double sign(0);
    std::complex<double> det, a33[16];
    det = 0.0;

    i=0;
    for(int j=0; j<4; j++)
    {
        p = 0;
        for(int k=0; k<4; k++)
        {
            if(k!=i)
            {
                q = 0;
                for(int l=0; l<4; l++)
                {
                    if(l!=j)
                    {
                        a33[p*3+q] = aij[4*k+l];
                        q++;
                    }
                }
                p++;
            }
        }
//        sign = pow(-1.0,i+j);
        sign = -(((i+j)%2)*2-1);
        det += sign * aij[4*i+j] * det33(a33);
    }

    return det;
}


/**________________________________________________________________________
* Finds the eigenvector associated to an eigenvalue.
* Solves the system A.V = lambda.V where A is a 4x4 complex matrix
* in input :
*    - matrix A
*    - the array of complex eigenvalues
* in output
*    - the array of complex eigenvectors
*
* The eigenvector is calculated by direct matrix inversion.
* One of the vector's component is set to 1, to avoid the trivial solution V=0;
*
* (c) Andre Deperrois October 2009
*@param a the complex two-dimensional 4x4 input matrix to diagonalize
*@param lambda the output array of four complex eigenvalues
*@param V the eigenvector as a one-dimensional array of complex values
*________________________________________________________________________ */
bool eigenVector(double a[][4], std::complex<double> lambda, std::complex<double> *V)
{
    std::complex<double> detm, detr;
    std::complex<double> r[9], m[9];
    int ii(0), jj(0), i(0), j(0), kp(0);

    // first find a pivot for which the  associated n-1 determinant is not zero
    bool bFound = false;
    kp=0;
    do
    {
        V[kp] = 1.0;
        ii= 0;
        for(i=0;i<4 ; i++)
        {
            if(i!=kp)
            {
                jj=0;
                for(j=0; j<4; j++)
                {
                    if(j!=kp)
                    {
                        m[ii*3+jj] = a[i][j];
                        jj++;
                    }
                }
                m[ii*3+ii] -= lambda;
                ii++;
            }
        }
        detm = det33(m);
        bFound = std::abs(detm)>0.0;
        if(bFound || kp>=3) break;
        kp++;
    }while(true);

    if(!bFound) return false;

    // at this point we have identified pivot kp
    // with a non-zero subdeterminant.
    // so solve for the other 3 eigenvector components.
    // using Cramer's rule

    //create rhs determinant
    jj=0;
    for(j=0; j<4; j++)
    {
        memcpy(r,m, 9*sizeof(std::complex<double>));
        if(j!=kp)
        {
            ii= 0;
            for(i=0; i<4; i++)
            {
                if(i!=kp)
                {
                    r[ii*3+jj] = - a[i][kp];
                    ii++;
                }
            }
            detr  = det33(r);
            V[j] = detr/detm;
            jj++;
        }
    }

    return true;
}


/** Simple routine for displaying a vector. */
void display_vec(double *vec, int n)
{
    for(int i=0; i<n; i++)
        qDebug("\t%17.9lg", vec[i]);
}


void  display_mat(QVector<double> const &mat, int size)
{
    display_mat(mat.constData(), size, size);
}


void display_mat(double const *mat, int rows, int cols)
{
    QString str, strong;
    if(cols<=0) cols = rows;
    for(int i=0; i<rows; i++)
    {
        strong=" ";
        for(int j=0; j<cols; j++)
        {
            str = QString::asprintf(" %17g ", mat[i*cols+j]);
            strong += str;
        }
//        std::string str = strong.toStdString();
//        const char* p = str.c_str();
        qDebug("%s", strong.toStdString().c_str());//avoid inverted commas
    }
}


void display_mat(float const*mat, int rows, int cols)
{
    QString str, strong;
    if(cols<=0) cols = rows;
    for(int i=0; i<rows; i++)
    {
        strong=" ";
        for(int j=0; j<cols; j++)
        {
            str = QString::asprintf(" %17g ", mat[i*cols+j]);
            strong += str;
        }
//        std::string str = strong.toStdString();
//        const char* p = str.c_str();
        qDebug("%s", strong.toStdString().c_str());//avoid inverted commas
    }
}


void display_mat(std::vector<std::vector<double>> const &mat)
{
    for(uint i=0; i<mat.size(); i++)
    {
        QString strong="    ";
        for(uint j=0; j<mat[i].size(); j++)
        {
            strong += QString("%1  ").arg(mat[i][j], 11, 'g', 5);
        }
//        std::string str = strong.toStdString();
//        const char* p = str.c_str();
        qDebug("%s", strong.toStdString().c_str());//avoid inverted commas
    }
}


void display_mat(QVector<QVector<double>> const &mat)
{
    for(int i=0; i<mat.size(); i++)
    {
        QString strong="    ";
        for(int j=0; j<mat.at(i).size(); j++)
        {
            strong += QString("%1  ").arg(mat[i][j], 11, 'g', 5);
        }
//        std::string str = strong.toStdString();
//        const char* p = str.c_str();
        qDebug("%s", strong.toStdString().c_str());//avoid inverted commas
    }
}



/** Simple routine for displaying a vector. */
void display_vec(float *vec, int n, bool bHorizontal)
{
    if(!bHorizontal)
    {
        for(int i=0; i<n; i++)
            qDebug("\t%17lg", double(vec[i]));
    }
    else
    {
        QString strange, strong;
        for(int i=0; i<n; i++)
        {
            strong = QString::asprintf("  %11g", double(vec[i]));
            strange += strong;
        }
        qDebug("%s", strange.toStdString().c_str());
    }
}


void saveMatrixtoFile(std::vector<double> const &mat, int N)
{
    QString filename = "/home/techwinder/tmp/matrix.csv";
    QFile XFile(filename);

    if (!XFile.open(QIODevice::WriteOnly))
    {
        return;
    }
    QTextStream out(&XFile);

    QString strange, strong;
    for(int row=0; row<N; row++)
    {
        strange.clear();
        for(int col=0; col<N; col++)
        {
            strong = QString::asprintf(" %11g;", mat.at(uint(row*N+col)));
            strange += strong;
        }
        strange += "\n";
        out << strange;
    }
    XFile.close();
}

/**
* L is lower triangular with unit values on the diagonal
* U is upper triangular
* LU is stored in A
*/
void CholevskiFactor(double *A, double *L, int n)
{
    for(int i=0; i<n*n; i++) L[i]=0.0;
    for(int i=0; i<n;   i++) L[i*n+i]=1.0;

    for (int i=0; i<n; i++)
    {
        for (int j=0; j<(i+1); j++)
        {
            double s = 0;
            for (int k=0; k<j; k++)
                s += L[i*n+k] * L[j*n+k];
            if(i==j) L[i*n+j] = sqrt(A[i*n+i] - s);
            else     L[i*n+j] = (1.0 / L[j*n+j] * (A[i*n+j] - s));
        }
    }
}


bool CholevskiSolve(double const*A, double* b, int n)
{
    double sum = 0.0;
    //forward substitute
    for (int i=0; i<n; i++)
    {
        if(fabs(A[i*n+i])<0.0000000001) return false;
        sum = b[i];
        for (int l=0; l<i; l++)
        {
            sum -= A[i*n+l] * b[l];
        }

        b[i] = sum/A[i*n+i];
    }

    //back-substitute. Now.
    for (int i=n-1; i>=0; i--)
    {
        sum = b[i];
        for (int j=i+1; j<n; j++)
            sum -= A[j*n+i] * b[j];
        b[i] = sum / A[i*n+i];
    }

    return true;
}


/**
* A is m rows x n columns
* B is n rows x q columns
* AB is m x q
*/
void matMult_SingleThread(double* const A, double* const B, double* AB, int m, int n, int q)
{
    if(m==2 && n==2 && q==2)
    {
        matMult22(A,B,AB);
        return;
    }
    else if(m==3 && n==3 && q==3)
    {
        matMult33(A,B,AB);
        return;
    }

    // access elements by address rather than by indices
    // and unroll two rows of A
    int half = m/2;
    double ytmp1=0, ytmp2=0;

    double *Apos1 = A;
    double *Apos2 = A+n;
    double *Bpos = B;
    double btmp;

    for(int j=0; j<q; j++)
    {
        for(int i=0; i<half; i++)
        {
            ytmp1 = ytmp2 = 0.0;

            for(int k=0; k<n; k++)
            {
                btmp = *(Bpos+k*q);
                ytmp1 += *(Apos1++) * btmp;
                ytmp2 += *(Apos2++) * btmp;
            }
            AB[ 2*i   *q +j] = ytmp1;
            AB[(2*i+1)*q +j] = ytmp2;

            Apos1 += n;
            Apos2 += n;
        }
        if(2*half!=m)
        {
            //odd number of rows, process last line
            ytmp1 = 0.0;

            for(int k=0; k<n; k++)
            {
                ytmp1 += *(Apos1++) * *(Bpos+k*q);
            }
            AB[(m-1)*q +j] = ytmp1;
        }

        Apos1 = A;
        Apos2 = A+n;
        Bpos++;
    }
}


void matMult(double* const A, double* const B, double* AB, int n, int nThreads)
{
    if(n==2)
    {
        matMult22(A,B,AB);
        return;
    }
    else if(n==3)
    {
        matMult33(A,B,AB);
        return;
    }
    else
    {
        if(nThreads<0) nThreads=QThread::idealThreadCount();
        matMult(A,B,AB,n,n,1,nThreads);
    }
}


void matVecMult2x2(double const *A, double const *X, double *Y)
{
    Y[0] = A[0]*X[0] + A[1]*X[1];
    Y[1] = A[2]*X[0] + A[3]*X[1];
}


void matVecMult3x3(double const *A, double const *X, double *Y)
{
    Y[0] = A[0]*X[0] + A[1]*X[1] + A[2]*X[2];
    Y[1] = A[3]*X[0] + A[4]*X[1] + A[5]*X[2];
    Y[2] = A[6]*X[0] + A[7]*X[1] + A[8]*X[2];
}


void matVecMult4x4(double const *A, double const *X, double *Y)
{
    Y[0] = A[0] *X[0] + A[1] *X[1] + A[2] *X[2] + A[3] *X[3];
    Y[1] = A[4] *X[0] + A[5] *X[1] + A[6] *X[2] + A[7] *X[3];
    Y[2] = A[8] *X[0] + A[9] *X[1] + A[10]*X[2] + A[11]*X[3];
    Y[3] = A[12]*X[0] + A[13]*X[1] + A[14]*X[2] + A[15]*X[3];
}


/**
* A is m rows x n columns
* B is n rows x q columns
* AB is m x q
*/
void matMult(double* const A, double* const B, double* AB, int nRows, int n, int nCols, int nThreads)
{
    if(nThreads<0) nThreads=QThread::idealThreadCount();
    int m_nBlocks = nThreads;
    if(m_nBlocks>nCols) m_nBlocks=nCols;
    MatMultData mmd(A, B, AB, nRows, n, nCols);
    if(nThreads>=1)
    {
        QFutureSynchronizer<void> futureSync;
        for(int iBlock=0; iBlock<m_nBlocks; iBlock++)
        {
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
            futureSync.addFuture(QtConcurrent::run(&matMultBlock, iBlock, mmd, nThreads));
#else
            futureSync.addFuture(QtConcurrent::run( [iBlock, mmd, nThreads]() mutable {matMultBlock(iBlock, mmd, nThreads);} ));
#endif
        }
        futureSync.waitForFinished();
    }
    else
    {
        m_nBlocks = 1;
        for(int iBlock=0; iBlock<m_nBlocks; iBlock++)
        {
//            matMultBlock(iBlock, A, B, AB, nRows, n, nCols);
            matMultBlock(iBlock, mmd, m_nBlocks);
        }
    }
}


/**
 * @brief matMultBlock multiplies a block of vector columns by a block of row vectors
 * @param iBlock
 * @param mmd
 */
void matMultBlock(int iBlock, MatMultData &mmd, int nThreads)
{
    int nBlocks = nThreads;
    if(nBlocks>mmd.nCols) nBlocks=mmd.nCols;
    int blocksize = mmd.nCols/nBlocks;

    int j0 = iBlock*blocksize;

    //adjust last block size to compensate for non exact column division

    int half = mmd.nRows/2;

    double ytmp1, ytmp2;

    double *Apos1 = mmd.A;
    double *Apos2 = mmd.A+mmd.n;
    double *Bpos = mmd.B+iBlock*blocksize;
    double btmp;
    if(iBlock==nBlocks-1)
        blocksize = mmd.nCols - iBlock*blocksize;

    // access elements by address rather than by indices and unroll two rows of A
    for(int j=0; j<blocksize; j++)
    {
        for(int i=0; i<half; i++)
        {
            ytmp1 = ytmp2 = 0.0;

            for(int k=0; k<mmd.n; k++)
            {
                btmp = *(Bpos+k*mmd.nCols);
                ytmp1 += *(Apos1++) * btmp;
                ytmp2 += *(Apos2++) * btmp;
            }
            mmd.AB[ 2*i   *mmd.nCols +j0+j] = ytmp1;
            mmd.AB[(2*i+1)*mmd.nCols +j0+j] = ytmp2;

            Apos1 += mmd.n;
            Apos2 += mmd.n;
        }
        if(2*half!=mmd.nRows)
        {
            //odd number of rows, process last line
            ytmp1 = 0.0;

            for(int k=0; k<mmd.n; k++)
            {
                ytmp1 += *(Apos1++) * *(Bpos+k*mmd.nCols);
            }
            mmd.AB[(mmd.nRows-1)*mmd.nCols +j0+j] = ytmp1;
        }

        Apos1 = mmd.A;
        Apos2 = mmd.A+mmd.n;
        Bpos++;
    }
}


/** orders the matrix array by column-major index
 * @param n    the size of the square matrix
 * @param aij  the input matrix ordered with row-major index
 * @param taij the output matrix ordered with column-major index
 */
void rowToColumnMajorIndex(int n, double *aij, double  *taij)
{
    for(int i=0; i<n; i++)
    {
        for(int j=0; j<n; j++)
        {
            taij[i + j*n] = aij[i*n + j];
        }
    }
}


/** orders the matrix array by column-major index
 * @param n    the size of the square matrix
 * @param aij  the input matrix ordered with row-major index
 * @param taij the output matrix ordered with column-major index
 */
void columnToRowMajorIndex(int n, double *aij, double  *taij)
{
    for(int i=0; i<n; i++)
    {
        for(int j=0; j<n; j++)
        {
            taij[i*n + j] = aij[i + j*n];
        }
    }
}


bool inverseMatrix_old(int n, double *aij, double *invAij)
{
    QElapsedTimer t;
    t.start();
    // make identity RHS
    // tempMat is ordered in column major index
    QVector<double>tempMat(n*n);
    memset(tempMat.data(), 0, ulong(n*n)*sizeof(double));
    for(int i=0; i<n; i++) tempMat[i+i*n] = 1.0;

    QVector<double> RHS(n);
    memset(RHS.data(), 0, ulong(n)*sizeof(double));
    RHS[0]=1;

    //solve each column vector by Gaussian elemination
    bool bCancel=false;

    if(!Gauss(aij, n, tempMat.data(), n, bCancel)) return false;

    // transpose back to row-major index
    columnToRowMajorIndex(n, tempMat.data(), invAij);

    return true;
}



double determinant(double *a,int n)
{
    double det = 0;
    double *m = nullptr;

    if (n<1)       return 0.0;
    else if (n==1) return a[0];
    else if (n==2) return a[0]*a[3] - a[1]*a[2];
    else if (n==3)
    {
        return    a[0]*a[4]*a[8]  - (a[0]*a[5]*a[7])
                -(a[1]*a[3]*a[8]) +  a[1]*a[5]*a[6]
                + a[2]*a[3]*a[7]  - (a[2]*a[4]*a[6]);
    }
    else
    {
        det = 0;
        for (int row=0; row<n; row++)
        {
            m = new double[ulong((n-1) * (n-1))*sizeof(double)];

            for(int i=0; i<n; i++)
            {
                if(i<row)
                    memcpy(m+i*(n-1), a + i*n+1, ulong(n-1)*sizeof(double));
                else if(i>row)
                    memcpy(m+(i-1)*(n-1), a + i*n+1, ulong(n-1)*sizeof(double));
            }
            det += pow(-1.0,row) * a[row*n+0] * determinant(m, n-1);
            delete [] m;
        }
    }
    return(det);
}


/** Simple routine for displaying a vector. */
void listArray(double *array, int size)
{
    for(int i=0; i<size; i++)
        qDebug("\t%17g", array[i]);
}


void listArray(const std::vector<double> &array)
{
    for(uint i=0; i<array.size(); i++)
        qDebug("\t%17g", array.at(i));
}

void listArrays(std::vector<double> const &array1, std::vector<double> const &array2)
{
    ulong maxsize = std::min(ulong(array1.size()), ulong(array2.size()));
    for(ulong i=0; i<maxsize; i++)
    {
        qDebug("\t%17g  \t%17g  ", array1.at(i), array2.at(i));
    }
}


void listArrays(std::vector<double> const &array1, std::vector<double> const &array2, std::vector<double> const &array3)
{
    ulong maxsize = std::min(ulong(array1.size()), ulong(array2.size()));
    maxsize = std::min(maxsize, ulong(array3.size()));
    for(ulong i=0; i<maxsize; i++)
    {
        qDebug("\t%17g  \t%17g  \t%17g", array1.at(i), array2.at(i), array3.at(i));
    }
}


void listArray(const QVector<double> &array)
{
    for(int i=0; i<array.size(); i++)
        qDebug("\t%17g", array.at(i));
}


void listArrays(const QVector<float> &array1, const QVector<float> &array2)
{
    for(int i=0; i<array1.size(); i++)
    {
        qDebug("\t%17g  \t%17g  ", double(array1.at(i)), double(array2.at(i)));
    }
}


void listArrays(const QVector<double> &array1, const QVector<double> &array2)
{
    for(int i=0; i<array1.size(); i++)
    {
        qDebug("\t%17g  \t%17g  ", array1.at(i), array2.at(i));
    }
}




/** solves a 2x2 linear system */
bool solve2x2(double *M, double *rhs, int nrhs)
{
    double det  = M[0]*M[3] - M[1]*M[2];
    if(fabs(det)<1.e-20) return false;

    for(int i=0; i<nrhs; i++)
    {
        double a = rhs[2*i];
        double b = rhs[2*i+1];
        rhs[2*i]   =  (a*M[3] - b*M[1])/det;
        rhs[2*i+1] = -(a*M[2] - b*M[0])/det;
    }
    return true;
}


/** solves a 3x3 linear system */
bool solve3x3(double *M, double *rhs, int nrhs)
{
    double x(0), y(0), z(0);
    double det = det33(M);
    if(fabs(det)<PRECISION) return false;
    for(int i=0; i<nrhs; i++)
    {
        x = rhs[3*i]*(M[4]*M[8]  -M[5]*M[7])       - rhs[3*i+1]*(M[1]*M[8]-M[2]*M[7])       + rhs[3*i+2]*(M[1]*M[5]-M[2]*M[4]);
        y = M[0]*(rhs[3*i+1]*M[8]-M[5]*rhs[3*i+2]) - M[3]*(rhs[3*i]*M[8]  -M[2]*rhs[3*i+2]) + M[6]*(rhs[3*i]*M[5]  -M[2]*rhs[3*i+1]);
        z = M[0]*(M[4]*rhs[3*i+2]-rhs[3*i+1]*M[7]) - M[3]*(M[1]*rhs[3*i+2]-rhs[3*i]*M[7])   + M[6]*(M[1]*rhs[3*i+1]-rhs[3*i]*M[4]);

        rhs[3*i]   = x /det;
        rhs[3*i+1] = y /det;
        rhs[3*i+2] = z /det;
    }

    return true;
}


double det33(double *M)
{
    return M[0]*(M[4]*M[8]-M[5]*M[7]) - M[3]*(M[1]*M[8]-M[2]*M[7]) + M[6]*(M[1]*M[5]-M[2]*M[4]);
}


float det33(float *M)
{
    double det(0);

    det  = M[0]*(M[4]*M[8]-M[5]*M[7]);
    det -= M[3]*(M[1]*M[8]-M[2]*M[7]);
    det += M[6]*(M[1]*M[5]-M[2]*M[4]);

    return det;
}


/** solves a 4x4 linear system */
bool solve4x4(double *M, double *rhs, int nrhs)
{
    if(!invert44(M)) return false;
    double x(0), y(0), z(0), t(0);
    for(int i=0; i<nrhs; i++)
    {
        x = M[0] *rhs[4*i] + M[1] *rhs[4*i+1] + M[2] *rhs[4*i+2] + M[3] *rhs[4*i+3];
        y = M[4] *rhs[4*i] + M[5] *rhs[4*i+1] + M[6] *rhs[4*i+2] + M[7] *rhs[4*i+3];
        z = M[8] *rhs[4*i] + M[9] *rhs[4*i+1] + M[10]*rhs[4*i+2] + M[11]*rhs[4*i+3];
        t = M[12]*rhs[4*i] + M[13]*rhs[4*i+1] + M[14]*rhs[4*i+2] + M[15]*rhs[4*i+3];
        rhs[4*i]   = x;
        rhs[4*i+1] = y;
        rhs[4*i+2] = z;
        rhs[4*i+3] = t;
    }

    return true;
}


void setIdentityMatrix(double *M, int n)
{
    memset(M, 0, n*n*sizeof(double));
    for(int i=0; i<n; i++) M[i*n+i]=1.0;
}


/**
 * @brief makeILU. Builds an incomplete LU factorization of matrix A.
 * As a dropping rule, all elements further away than p indices from the diagonal are ignored in input
 * and are set to zero in output.
 * Both matrices are assumed to have been pre-allocated.
 * @param A the input matrix
 * @param ILU the resulting ILU preconditioner
 * @param n the size of the matrix
 * @param p the dropping rule's parameter
 * @return true if the resulting ILU is invertible, false if it is singular. @todo not implemented.
 */
bool makeILUC(const double *A, double *ILU, int n, int p)
{
    memcpy(ILU, A, n*n*sizeof(double));
    ILUC(A, ILU,n, p);
    for(int i=0; i<n; i++) if(fabs(ILU[i*n+i])<PRECISION) return false;
    return true;
}


/**
 * @brief ILUC performs the incomplete LU factorization of a square matrix using Crout's algorithm.
 * We only consider the elements in a band of width pIn in the input matrix, i.e. abs(i-j)<pIn.
 * The rest of the coefficients is ignored, i.e. assumed to be zero.
 * We only output the elements inside a band of width pOut, i.e. abs(i-j)<pOut
 * @param ILU the output matrix
 * @param n the size of the matrices.
 * @param p the dropping rule's parameter
 */
void ILUC(double const*A, double *ILU, int n, int p)
{
    /**  Algorithm 10.8    */
    for (int k=0; k<n; k++)                     // 1. For k = 1 : n Do :
    {
        for(int i=0; i<k; i++)                  // 2. For i = 1 : k − 1 and
        {
            if(fabs(A[k*n+i])>0.0)            // 2. ....    if aki!=0 Do :
            {
                for(int l=k; l<n; l++)          // 3. ak,k:n = ak,k:n − aki * ai,k:n
                {
                    ILU[k*n+l] = A[k*n+l] - ILU[k*n+i] * ILU[i*n+l];
                }
            }
        }
        for(int i=0; i<k; i++)                  // 5. For i = 1 : k − 1 and if aik!=0 Do :
        {
            for(int l=k+1; l<n; l++)            // 6. ak+1:n.k = ak+1:n,k − aik * ak+1:n,i
            {
                ILU[l*n +k] -= ILU[i*n+k] * ILU[l*n+i];
            }
        }
        for(int i=k+1; i<n; i++)                // 8. aik = aik / akk for i = k + 1, ..., n
        {
            ILU[i*n+k] = ILU[i*n+k]/ILU[k*n+k];
        }
    }

    /** apply the output drop rule
     * @todo include this in the above calculations
     * @todo apply pIn filter */
    for(int i=0; i<n; i++)
    {
        for(int j=0; j<n; j++)
        {
            if (abs(i-j)>p) ILU[i*n+j] = 0.0;
        }
    }
}


/** Builds the symmetric Gauss-Seidel preconditioner in LU form
 * @brief makeSGS
 * @param A
 * @param SGSLU
 * @param n
 * @param p
 * @return true if the resulting LU factorization is invertible, false if it is singular. @todo not implemented.
 */
bool makeSGS(double const *A, double *SGSLU, int n)
{
    for(int i=0; i<n; i++)
    {
        if(fabs(A[i*n+i])<PRECISION) return false;
    }

    memcpy(SGSLU, A, n*n*sizeof(double));

    // D is the diagonal of A, −E its strict lower part, and −F its strict upper part
    // L = I-E.D-1
    // U = D-F   --> is A

    // make L - is unit lower triangular lij = eij/djj
    for(int i=0; i<n; i++)
    {
        for(int j=0; j<i; j++)
        {
            SGSLU[i*n+j] = A[i*n+j]/A[i*n+i];
        }
    }
    return true;
}



/**
 * @brief Performs the (fast) multiplication of a matrix by a vector.
 * @param A the nxn square matrix
 * @param V the column vector
 * @param n the size of the matrix and vecgor
 * @param res the resulting product
 */
void matVecMult(double const *A, double const *V, double *res, int nRows, int nCols)
{
    // Access matrix and vector elements by adresses rather than by indices
    // gives the compiler possibility for optimization

    // unroll two rows and four columns
    int half = nRows/2;
    double ytemp1(0);
    double ytemp2(0);
    for(int i=0; i+1<nRows; i+=2)
    {
        ytemp1=ytemp2=0;
        double const *xpos = V;

        double const *Apos1 = A +  i   *nCols;
        double const *Apos2 = A + (i+1)*nCols;

        int nBlocks = nCols / 8;
        double x0=0;
        double x1=0;

        for(int j=0; j<nBlocks; j++)
        {
            x0 = *(xpos+0);
            x1 = *(xpos+1);

            ytemp1 += x0 * *(Apos1+0);
            ytemp2 += x0 * *(Apos2+0);
            ytemp1 += x1 * *(Apos1+1);
            ytemp2 += x1 * *(Apos2+1);

            x0 = *(xpos+2);
            x1 = *(xpos+3);

            ytemp1 += x0 * *(Apos1+2);
            ytemp2 += x0 * *(Apos2+2);
            ytemp1 += x1 * *(Apos1+3);
            ytemp2 += x1 * *(Apos2+3);

            x0 = *(xpos+4);
            x1 = *(xpos+5);

            ytemp1 += x0 * *(Apos1+4);
            ytemp2 += x0 * *(Apos2+4);
            ytemp1 += x1 * *(Apos1+5);
            ytemp2 += x1 * *(Apos2+5);

            x0 = *(xpos+6);
            x1 = *(xpos+7);

            ytemp1 += x0 * *(Apos1+6);
            ytemp2 += x0 * *(Apos2+6);
            ytemp1 += x1 * *(Apos1+7);
            ytemp2 += x1 * *(Apos2+7);

            xpos  += 8;
            Apos1 += 8;
            Apos2 += 8;
        }

        Apos1 = A +  i   *nCols;
        Apos2 = A + (i+1)*nCols;

        for(int j=nBlocks*8; j<nCols; j++)
        {
            ytemp1 += (*(Apos1 +j)) * (*(V+j));
            ytemp2 += (*(Apos2 +j)) * (*(V+j));
        }

        res[i]   = ytemp1;
        res[i+1] = ytemp2;
    }

    if(2*half!=nRows)
    {
        // compute last row if odd number
        double const *Apos1 = A+2*half*nCols;
        double const *xpos = V;
        double ytemp=0;
        for(int j=0; j<nCols; j++)
        {
            ytemp += (*Apos1++) * (*xpos++);
        }
        res[2*half] = ytemp;
    }
}




