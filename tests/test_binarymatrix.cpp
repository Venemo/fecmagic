
// This file is part of fecmagic, the forward error correction library.
// Copyright (c) 2016 Timur Kristóf
// Licensed to you under the terms of the MIT license.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#include <cassert>
#include <cstdlib>
#include <iostream>
#include "../src/binarymatrix.h"
#include "../src/binaryprint.h"
#include "../src/bitmaskcombination.h"

using namespace std;
using namespace fecmagic;

// Creates a sequence of random bytes with the given size
uint8_t *createRandomBytes(size_t size) {
    uint8_t *bytes = new uint8_t[size];
    for (size_t i = 0; i < size; i++) {
        bytes[i] = (uint8_t)(rand() % 255);
    }
    return bytes;
}

// Transposes a binary matrix in a simple and naive way.
// This method is used for verifying the more advanced implementations
// in the BinaryMatrix class.
template<unsigned R, unsigned C>
BinaryMatrix<C, R> naiveTranspose(const BinaryMatrix<R, C> &input) {
    BinaryMatrix<C, R> output;
    for (unsigned i = 0; i < input.rows(); i++) {
        for (unsigned j = 0; j < input.cols(); j++) {
            auto bit = input.getBit(i, j);
            output.setBit(j, i, bit);
        }
    }
    
    return output;
}

// Tests the transpose operation of a given matrix.
template<unsigned R, unsigned C>
void testTranspose(const BinaryMatrix<R, C> &m) {
    auto t1 = m.transpose();
    auto t2 = naiveTranspose(m);
    if (t1 != t2) {
        cout << "FAIL! " << R << "×" << C << endl;
        cout << "m:" << endl << m << endl;
        cout << "t1:" << endl << t1 << endl;
        cout << "t2:" << endl << t2 << endl;
    }
    assert(t1 == t2);
}

// Tests the transpose operation of a randomized matrix of the given size.
template<unsigned R, unsigned C>
void testTransposeRandom(unsigned n = 50) {
    size_t byteCount = BinaryMatrix<R, C>::getByteCount();
    uint8_t *bytes = new uint8_t[byteCount];
    
    for (unsigned nn = 0; nn < n; nn++) {
        //cout << R << "×" << C << " #" << nn << endl;
        
        for (size_t i = 0; i < byteCount; i++) {
            bytes[i] = (uint8_t)(rand() % 255);
        }
        
        BinaryMatrix<R, C> m(bytes);
        //cout << m;
        testTranspose(m);
    }
    
    delete bytes;
}

// Test transpose operation using pre-defined matrices
void transposeTests1() {
    testTranspose<8, 8>({
        0b10000000,
        0b00000001,
        0b00010000,
        0b00001000,
        0b01000000,
        0b00100000,
        0b00000100,
        0b00000010,
    });
    testTranspose<16, 8>({
        0b10000000,
        0b00000001,
        0b00010000,
        0b00001000,
        0b01000000,
        0b00100000,
        0b00000100,
        0b00000010,
        0b10000000,
        0b00000001,
        0b00010000,
        0b00001000,
        0b01000000,
        0b00100000,
        0b00000100,
        0b00000010,
    });
    testTranspose<16, 8>({
        0b00110000,
        0b00110001,
        0b11110111,
        0b10101000,
        0b01001110,
        0b01001110,
        0b01010001,
        0b01111011,
        0b11010000,
        0b00100110,
        0b01111011,
        0b10110010,
        0b11011111,
        0b00110101,
        0b11010010,
        0b01010100,
    });
    testTranspose<24, 8>({
        0b10000000,
        0b00000001,
        0b00010000,
        0b00001000,
        0b01000000,
        0b00100000,
        0b00000100,
        0b00000010,
        0b10000000,
        0b00000001,
        0b00010000,
        0b00001000,
        0b01000000,
        0b00100000,
        0b00000100,
        0b00000010,
        0b10000000,
        0b00000001,
        0b00010000,
        0b00001000,
        0b01000000,
        0b00100000,
        0b00000100,
        0b00000010,
    });
    testTranspose<24, 16>({
        0b10000000, 0b10000000,
        0b00000001, 0b00000001,
        0b00010000, 0b00010000,
        0b00001000, 0b00001000,
        0b01000000, 0b01000000,
        0b00100000, 0b00100000,
        0b00000100, 0b00000100,
        0b00000010, 0b00000010,
        0b10000000, 0b10000000,
        0b00000001, 0b00000001,
        0b00010000, 0b00010000,
        0b00001000, 0b00001000,
        0b01000000, 0b01000000,
        0b00100000, 0b00100000,
        0b00000100, 0b00000100,
        0b00000010, 0b00000010,
        0b10000000, 0b10000000,
        0b00000001, 0b00000001,
        0b00010000, 0b00010000,
        0b00001000, 0b00001000,
        0b01000000, 0b01000000,
        0b00100000, 0b00100000,
        0b00000100, 0b00000100,
        0b00000010, 0b00000010,
    });
}

// Tests transpose operation using random generated matrices.
void transposeTests2() {
    testTransposeRandom<8, 8>();
    testTransposeRandom<16, 8>();
    testTransposeRandom<8, 16>();
    testTransposeRandom<24, 8>();
    testTransposeRandom<8, 24>();
    testTransposeRandom<24, 24>();
    testTransposeRandom<32, 24>();
    testTransposeRandom<80, 80>();
    testTransposeRandom<80, 8>();
    testTransposeRandom<8, 80>();
    testTransposeRandom<800, 800>();
}

template<size_t R, size_t C, size_t X>
BinaryMatrix<R, X> naiveCalculateProduct(const BinaryMatrix<R, C> &m1, const BinaryMatrix<C, X> &m2) {
    BinaryMatrix<R, X> result;
    
    for (size_t i = 0; i < result.rows(); i++) {
        for (size_t j = 0; j < result.cols(); j++) {
            uint8_t bit = 0;
            for (size_t x = 0; x < C; x++) {
                bit ^= (m1.getBit(i, x) & m2.getBit(x, j));
            }
            result.setBit(i, j, bit);
        }
    }
    
    return result;
}

template<size_t R, size_t C, size_t X>
void matrixProductTest(const BinaryMatrix<R, C> &m1, const BinaryMatrix<C, X> &m2) {
    auto product1 = m1.calculateProduct(m2);
    auto product2 = naiveCalculateProduct<R, C, X>(m1, m2);
    assert(product1 == product2);
}

template<size_t R, size_t C, size_t X>
void randomMatrixProductTest(unsigned n = 50) {
    for (unsigned i = 0; i < n; i++) {
        uint8_t *randomBytes1 = createRandomBytes(R * C / 8);
        uint8_t *randomBytes2 = createRandomBytes(C * X / 8);
        
        BinaryMatrix<R, C> m1(randomBytes1);
        BinaryMatrix<C, X> m2(randomBytes2);
        matrixProductTest<R, C, X>(m1, m2);
        
        delete [] randomBytes1;
        delete [] randomBytes2;
    }
}

void matrixProductTests1() {
    matrixProductTest<16, 16, 16>({
        0b10000000, 0b10000000,
        0b00000001, 0b00000001,
        0b00010000, 0b00010000,
        0b00001000, 0b00001000,
        0b01000000, 0b01000000,
        0b00100000, 0b00100000,
        0b00000100, 0b00000100,
        0b00000010, 0b00000010,
        0b10000000, 0b10000000,
        0b00000001, 0b00000001,
        0b00010000, 0b00010000,
        0b00001000, 0b00001000,
        0b01000000, 0b01000000,
        0b00100000, 0b00100000,
        0b00000100, 0b00000100,
        0b00000010, 0b00000010,
    }, {
        0b10000000, 0b10000000,
        0b00000001, 0b00000001,
        0b00010000, 0b00010000,
        0b00001000, 0b00001000,
        0b01000000, 0b01000000,
        0b00100000, 0b00100000,
        0b00000100, 0b00000100,
        0b00000010, 0b00000010,
        0b10000000, 0b10000000,
        0b00000001, 0b00000001,
        0b00010000, 0b00010000,
        0b00001000, 0b00001000,
        0b01000000, 0b01000000,
        0b00100000, 0b00100000,
        0b00000100, 0b00000100,
        0b00000010, 0b00000010,
    });
}

void matrixProductTests2() {
    randomMatrixProductTest<16, 8, 24>();
    randomMatrixProductTest<24, 32, 16>();
    randomMatrixProductTest<8, 8, 8>();
}

int main() {
    transposeTests1();
    transposeTests2();
    matrixProductTests1();
    matrixProductTests2();
    // TODO: test vector multiplication too
    
    return 0;
}

