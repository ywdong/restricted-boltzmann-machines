/*
 * =====================================================================================
 *
 *       Filename:  matrix.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2012年12月20日 13时24分35秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Weidong Yang (cn), ywdong@mail2.sysu.edu.cn
 *        Company:  
 *
 * =====================================================================================
 */

#include <iostream>
#include <cassert>
#include <cmath>
#include <cstdlib>
#include <iomanip>
#include <cstdio>

using namespace std;

class Matrix {
    public:
        int m_row;
        int m_col;
        double** m_values;
        Matrix();
        Matrix(int row, int col);
        Matrix(int row, int col, double** values);
        ~Matrix();
        // copy construtor
        Matrix(const Matrix& source);
        // overload assignment operator
        Matrix& operator= (const Matrix& source);
        // 必须返回对象时别返回引用
        friend Matrix operator* (const Matrix& left, const Matrix& right);
        friend Matrix operator+ (const Matrix& left, const Matrix& right);
        friend Matrix operator- (const Matrix& left, const Matrix& right);
        Matrix Transpose() const;
        Matrix Logistic() const;
        Matrix Random() const;
        void Print(int width=10) const;
};

Matrix::Matrix()
{
    m_row = 0;
    m_col = 0;
    m_values = NULL;
}

Matrix::Matrix(int row, int col)
{
    m_row = row;
    m_col = col;
    m_values = new double*[row];
    for (int i=0; i<row; i++)
    {
        m_values[i] = new double[col];
    }
    // init
    for (int i=0; i<row; i++)
        for (int j=0; j<col; j++)
            m_values[i][j] = 0.0;
}

Matrix::Matrix(int row, int col, double** values)
{
    m_row = row;
    m_col = col;
    // allocate 2D array
    m_values = new double*[row];
    for (int i=0; i<row; i++)
        m_values[i] = new double[col];
    // assignment
    for (int i=0; i<row; i++)
        for (int j=0; j<col; j++)
            m_values[i][j] = *((double*)values + col*i + j);

}

Matrix::~Matrix()
{
    //printf("%s:%d:%s:delete\n", __FILE__, __LINE__, __func__);
    if (m_values != NULL)
    {
        for (int i=0; i<m_row; i++)
        {
            delete[] m_values[i];
        }
        delete[] m_values;
        m_values = NULL;
    }
}

Matrix::Matrix(const Matrix& source)
{
    m_row = source.m_row;
    m_col = source.m_col;

    m_values = new double*[m_row];
    for (int i=0; i<m_row; i++)
        m_values[i] = new double[m_col];

    for (int i=0; i<m_row; i++)
        for (int j=0; j<m_col; j++)
            m_values[i][j] = source.m_values[i][j];
}

Matrix& Matrix::operator=(const Matrix& source)
{

    // check for self-assignment
    if (this == &source)
        return *this;

    // first we need to deallocate any value that this matrix is holding!
    if (m_values != NULL)
    {
        for (int i=0; i<m_row; i++)
            delete[] m_values[i];
        delete[] m_values;
    }
    // shallow copy
    m_row = source.m_row;
    m_col = source.m_col;

    // deep copy m_values
    m_values = new double*[m_row];
    for (int i=0; i<m_row; i++)
        m_values[i] = new double[m_col];
    
    // copy the m_values from source matrix
    for (int i=0; i<m_row; i++)
        for (int j=0; j<m_col; j++)
            m_values[i][j] = source.m_values[i][j];

    // for chaining
    return *this;
}

Matrix operator*(const Matrix& left, const Matrix& right)
{
    assert(left.m_col == right.m_row);

    Matrix dest(left.m_row, right.m_col); //当函数返回时会调用dest对象的析构函数

    for (int i=0; i<left.m_row; i++)
        for (int j=0; j<right.m_col; j++)
            for (int k=0; k<left.m_col; k++)
                dest.m_values[i][j] += left.m_values[i][k] * right.m_values[k][j];

    return dest; // 会使用拷贝构造函数生成一个临时的副本，所以即使dest析构了也没事
}

Matrix operator+(const Matrix& left, const Matrix& right)
{
    assert(left.m_col == right.m_col && left.m_row == right.m_row);

    Matrix dest(left.m_row, left.m_col);
    for (int i=0; i<left.m_row; i++)
        for (int j=0; j<left.m_col; j++)
            dest.m_values[i][j] = left.m_values[i][j] + right.m_values[i][j];
    
    return dest;
}

Matrix operator-(const Matrix& left, const Matrix& right)
{
    assert(left.m_col == right.m_col && left.m_row == right.m_row);

    Matrix dest(left.m_row, left.m_col);
    for (int i=0; i<left.m_row; i++)
        for (int j=0; j<left.m_col; j++)
            dest.m_values[i][j] = left.m_values[i][j] - right.m_values[i][j];
    
    return dest;
}

Matrix Matrix::Transpose() const
{
    Matrix dest(m_col, m_row);

    for (int i=0; i<m_row; i++)
        for (int j=0; j<m_col; j++)
            dest.m_values[j][i] = m_values[i][j];

    return dest;
}


Matrix Matrix::Logistic() const
{
    Matrix dest(m_row, m_col);

    for (int i=0; i<m_row; i++)
        for (int j=0; j<m_col; j++)
            dest.m_values[i][j] = 1.0 / (1 + exp(0-m_values[i][j]));

    return dest;
}

void Matrix::Print(int width) const
{
    for (int i=0; i<m_row; i++)
    {
        for (int j=0; j<m_col; j++)
        {
            cout << left << setw(width) << m_values[i][j];
        }
        cout << endl;
    }
}

Matrix Matrix::Random() const
{
    Matrix dest(m_row, m_col);
    for (int i=0; i<m_row; i++)
    {
        for (int j=0; j<m_col; j++)
        {
            double tmp = rand()/(RAND_MAX+1.0);
            if (m_values[i][j] > tmp)
                dest.m_values[i][j] = 1;
            else
                dest.m_values[i][j] = 0;
            }
    }
    return dest;
}
