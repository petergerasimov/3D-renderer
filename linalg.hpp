#ifndef __LINALG_HPP__
#define __LINALG_HPP__

#include <vector>
#include <stdexcept>
#include <iostream>

template <class T>
class Matrix
{
    int rows;
    int cols;
    std::vector<std::vector<T>> data;
    public:
    Matrix() : rows(0), cols(0) {};
    Matrix(int rows, int cols) : rows(rows), cols(cols) {
        std::vector<T> row(cols, 0);
        for (int i = 0; i < rows; i++) {
            data.push_back(row);
        }
    };
    int getRows() const { return rows; }
    int getCols() const { return cols; }
    ///
    void print() {
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                std::cout << data[i][j] << " ";
            }
            std::cout << std::endl;
        }
    }
    std::vector<T>& operator[](int index) {
        return this->data[index];
    }
    std::vector<T> operator[](int index) const {
        return this->data[index];
    }
    Matrix& operator=(const std::vector<std::vector<T>>& other) {
        this->rows = other.size();
        this->cols = other[0].size();
        this->data = other;

        return *this;
    }
    Matrix& operator=(const std::vector<T>& other) {
        this->rows = 1;
        this->cols = other.size();
        this->data = {other};

        return *this;
    }
    Matrix operator+(Matrix& b) {
        if(this->rows != b.n || this->cols != b.m) {
            throw std::runtime_error("size missmatch");
        }
        Matrix toReturn(b.n, b.m);
        for(int i = 0; i < rows; i++) {
            for(int j = 0; j < cols; j++) {
                toReturn[i][j] = (*this)[i][j] + b[i][j];
            }
        }
        return toReturn;
    }
    Matrix operator*(Matrix const &b) const {
        Matrix<T> toReturn(this->getRows(), b.getCols());
        if (this->getCols() != b.getRows()) {
           throw std::runtime_error("Column of first matrix should be equal to row of second matrix");
        } else {
            for (int i = 0; i < this->getRows(); i++) {
                for (int j = 0; j < b.getCols(); j++) {
                    for (int k = 0; k < b.getRows(); k++) {
                        toReturn[i][j] = toReturn[i][j] + (*this)[i][k] * b[k][j];
                    }
                }
            }
        }
        return toReturn;
    }
    friend Matrix operator*(std::vector<T>& a, Matrix& b) {
        Matrix<T> amat;
        amat = a;
        return amat*b;
    }
    Matrix operator*(std::vector<T>& b) {
        Matrix<T> bmat(b.size(), 1);
        for (uint i = 0; i < b.size(); i++) {
            bmat[i][0] = b[i];
        }
        return (*this)*bmat;
    }
    

};


#endif //__LINALG_HPP__