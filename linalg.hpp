#ifndef __LINALG_HPP__
#define __LINALG_HPP__

#include <vector>
#include <stdexcept>
#include <iostream>

template <class T>
class Matrix
{
    int n;
    int m;
    std::vector<std::vector<T>> data;
    public:
    Matrix() : n(0), m(0) {};
    Matrix(int n, int m) : n(n), m(m) {
        std::vector<T> row(m, 0);
        for(int i = 0; i < n; i++)
        {
            data.push_back(row);
        }
    };
    // Matrix(const Matrix &rhs)
    // {
    //     n = rhs.n;
    //     m = rhs.m;
    //     data = rhs.data;
    // }
    ///
    int getRows() const
    {
        return n;
    }
    int getCols() const
    {
        return m;
    }
    ///
    void print()
    {
        for(int i = 0; i < n; i++)
        {
            for(int j = 0; j < m; j++)
            {
                std::cout << data[i][j] << " ";
            }
            std::cout << std::endl;
        }
    }
    std::vector<T>& operator[](int index)
    {
        return this->data[index];
    }
    std::vector<T> operator[](int index) const
    {
        return this->data[index];
    }
    Matrix& operator=(const std::vector<std::vector<T>>& other)
    {
        this->n = other.size();
        this->m = other[0].size();
        this->data = other;

        return *this;
    }
    Matrix& operator=(const std::vector<T>& other)
    {
        this->n = 1;
        this->m = other.size();
        this->data = {other};

        return *this;
    }
    Matrix operator+(Matrix& b) {
        if(this->n != b.n || this->m != b.m)
        {
            throw std::runtime_error("size missmatch");
        }
        Matrix toReturn(b.n,b.m);
        for(int i = 0; i < n; i++)
        {
            for(int j = 0; j < m; j++)
            {
                toReturn[i][j] = (*this)[i][j] + b[i][j];
            }
        }
        return toReturn;
    }
    Matrix operator*(Matrix const &b) const
    {
        Matrix<T> toReturn(this->getRows(), b.getCols());
        if (this->getCols() != b.getRows()) 
        {
           throw std::runtime_error("Column of first matrix should be equal to row of second matrix");
        } 
        else 
        {
            for(int i = 0; i < this->getRows(); i++)
            {
                for(int j = 0; j < b.getCols(); j++)
                {
                    for(int k = 0; k < b.getRows(); k++) {
                        toReturn[i][j] = toReturn[i][j] + (*this)[i][k] * b[k][j];
                    }
                }
            }
        }
        return toReturn;
    }
    friend Matrix operator*(std::vector<T>& a, Matrix& b)
    {
        Matrix<T> amat;
        amat = a;
        return amat*b;
    }
    Matrix operator*(std::vector<T>& b)
    {
        Matrix<T> bmat(b.size(),1);
        for(uint i = 0; i < b.size(); i++)
        {
            bmat[i][0] = b[i];
        }
        return (*this)*bmat;
    }
    

};


#endif //__LINALG_HPP__