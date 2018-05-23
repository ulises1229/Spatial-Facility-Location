//
// Created by Ulises Olivares on 5/22/18.
//

#ifndef SRC_MATRIX_H
#define SRC_MATRIX_H

#include<vector>

class Matrix{
    std::vector<std::vector<bool> >m;
public:
    Matrix(unsigned int x, unsigned int y) {
        m.resize(x, std::vector<bool>(y,false));
    }
    class matrix_row {
        std::vector<bool>& row;
    public:
        matrix_row(std::vector<bool>& r) : row(r) {
        }
        _Bit_reference operator(unsigned int y) {
            return row.at(y);
        }
    };
    matrix_row& operator[](unsigned int x) {
        return matrix_row(m.at(x));
    }
};

#endif //SRC_MATRIX_H