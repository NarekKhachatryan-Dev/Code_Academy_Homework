#ifndef MATRIX_H
#define MATRIX_H

#include <iostream>
#include <memory>

class Matrix {
private:
    std::unique_ptr<int[]> m_data;
    int m_size;

public:
    Matrix(int size);
    Matrix(const Matrix& other);
    ~Matrix();

    Matrix& operator= (const Matrix& other);

    Matrix operator* (const Matrix& other) const;
    Matrix operator* (int num) const;
    Matrix operator++();
    Matrix operator++(int);

    void init();
    void print() const;
    void initRandom();
    void pasteValue(int value, int row, int col);
    void transpose();
    void savetofile(const std::string& filename) const;
    void initfromfile(const std::string& filename);
    int& at(int row, int col);
    const int& at(int row, int col) const;

    friend std::ostream& operator<< (std::ostream& os, const Matrix& obj);
};

#endif