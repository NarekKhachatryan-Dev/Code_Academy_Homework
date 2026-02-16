#ifndef MATRIX_H
#define MATRIX_H

#include <memory>
#include <vector>

template <typename T>
class Matrix {
public:
    Matrix(int size) : m_size(size) {
        m_data.resize(size);
        for (int i = 0; i < size; ++i) {
            m_data[i].resize(size);
        }
    }

    virtual ~Matrix() = default;

    const T& getElement(int row, int col) const {
        return m_data[row][col];
    }

    T& getElement(int row, int col) {
        return m_data[row][col];
    }

    void setElement(int row, int col, T value) {
        m_data[row][col] = std::move(value);
    }

    int getSize() const { return m_size; }

protected:
    int m_size;
    std::vector<std::vector<T>> m_data;
};

#endif