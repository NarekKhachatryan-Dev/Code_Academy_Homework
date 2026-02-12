#include <iostream>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <fstream>
#include <sstream>
#include "matrix.h"

Matrix::Matrix(int size) : m_size(size), m_data(new int[size * size]) {
    for (int i = 0; i < m_size * m_size; ++i) {
        m_data[i] = 0;
    }
}

Matrix::Matrix(const Matrix& other) : m_size(other.m_size), m_data(new int[other.m_size * other.m_size]) {
    for (int i = 0; i < m_size * m_size; ++i) {
        m_data[i] = other.m_data[i];
    }
}

Matrix& Matrix::operator=(const Matrix& other) {
    if (this != &other) {
        m_data.reset(new int[other.m_size * other.m_size]);
        m_size = other.m_size;
        for (int i = 0; i < m_size * m_size; ++i) {
            m_data[i] = other.m_data[i];
        }
    }
    return *this;
}

Matrix Matrix::operator*(const Matrix& other) const {
    Matrix result(m_size);
    if (m_size != other.m_size) {
        std::cout << "Matrix size doesn't fit!" << std::endl;
        return result;
    }
    for (int i = 0; i < m_size; ++i) {
        for (int j = 0; j < m_size; ++j) {
            for (int k = 0; k < m_size; ++k) {
                result.at(i, j) += at(i, k) * other.at(k, j);
            }
        }
    }
    return result;
}

Matrix Matrix::operator*(int num) const {
    Matrix result(*this);
    for (int i = 0; i < m_size * m_size; ++i) {
        result.m_data[i] = m_data[i] * num;
    }
    return result;
}

Matrix Matrix::operator++() {
    for (int i = 0; i < m_size * m_size; ++i) {
        ++m_data[i];
    }
    return *this;
}

Matrix Matrix::operator++(int) {
    Matrix temp(*this);
    for (int i = 0; i < m_size * m_size; ++i) {
        ++m_data[i];
    }
    return temp;
}

Matrix::~Matrix() = default;

int& Matrix::at(int row, int col) {
    return m_data[row * m_size + col];
}

const int& Matrix::at(int row, int col) const {
    return m_data[row * m_size + col];
}

void Matrix::init() {
    std::cout << "Enter elements for a " << m_size << "x" << m_size << " matrix:" << std::endl;
    for (int i = 0; i < m_size; ++i) {
        for (int j = 0; j < m_size; ++j) {
            std::cin >> at(i, j);
        }
    }
}

void Matrix::print() const {
    std::cout << "Matrix elements:" << std::endl;
    for (int i = 0; i < m_size; ++i) {
        for (int j = 0; j < m_size; ++j) {
            std::cout << at(i, j) << " ";
        }
        std::cout << std::endl;
    }
}

void Matrix::initRandom() {
    srand(time(0));
    for (int i = 0; i < m_size * m_size; ++i) {
        m_data[i] = rand() % 100;
    }
}

void Matrix::pasteValue(int value, int row, int col) {
    at(row, col) = value;
}

void Matrix::transpose() {
    for (int i = 0; i < m_size; ++i) {
        for (int j = i + 1; j < m_size; ++j) {
            std::swap(at(i, j), at(j, i));
        }
    }
    for (int i = 0; i < m_size; ++i) {
        for (int j = 0; j < m_size / 2; ++j) {
            std::swap(at(i, j), at(i, m_size - j - 1));
        }
    }
}

std::ostream& operator<<(std::ostream& os, const Matrix& obj) {
    for (int i = 0; i < obj.m_size; ++i) {
        for (int j = 0; j < obj.m_size; ++j) {
            os << obj.at(i, j);
            if (j < obj.m_size - 1) os << " ";
        }
        os << "\n";
    }
    return os;
}

void Matrix::savetofile(const std::string& filename) const {
    std::ofstream outfile(filename, std::ios::app);
    if(outfile.is_open()) {
        outfile << *this << std::endl;
        outfile.close();
    }
}

void Matrix::initfromfile(const std::string& filename) {
    std::ifstream infile(filename);
    if(infile.is_open()) {
        for(int i = 0; i < m_size; ++i) {
            for(int j = 0; j < m_size; ++j) {
                infile >> at(i, j);
            }
        }
    }
    infile.close();
}
