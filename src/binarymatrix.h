
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

#ifndef BINARYMATRIX_H
#define BINARYMATRIX_H

#include <cstdint>
#include <cstring>
#include <cassert>
#include <cmath>
#include <cstdlib>
#include <type_traits>
#include <limits>
#include <exception>
#include <stdexcept>
#include <initializer_list>
#include <iostream>

#include "binaryprint.h"
#include "fecmagic-global.h"

namespace fecmagic {

    /**
     * @brief Represents a matrix that consists of zeroes and ones, with the given number of rows and columns.
     *
     * The BinaryMatrix class is an efficient implementation of matrices in GF(2)
     * that can only consist of zeroes and ones. It is intended for coding applications
     * such as block coders and interleavers.
     * It has the ability to multiply the matrix with a GF(2) vector and an efficient
     * transposition operation.
     *
     * Number of rows and columns are configurable but both must be multiples of 8 because
     * of internal implementation details.
     *
     * Template parameters:
     * - Rows: the number of rows, must be a multiple of 8
     * - Cols: the number of columns, must be a multiple of 8
     * 
     */
    template<unsigned Rows, unsigned Cols>
    class BinaryMatrix final {
    private:
        // Assert correct row and column numbers
        static_assert(Cols % 8 == 0, "BinaryMatrix: template parameter Cols must be a multiple of 8.");
        static_assert(Rows % 8 == 0, "BinaryMatrix: template parameter Rows must be a multiple of 8.");
        
        // Friend: stream operator for debug printing purposes
        template<unsigned R, unsigned C>
        friend std::ostream &::operator<<(std::ostream &os, const BinaryMatrix<R, C> &matrix);
        
        // Friend: transposed BinaryMatrix type
        friend class BinaryMatrix<Cols, Rows>;
        
        // Number of bytes required to hold the matrix
        static const constexpr std::size_t byteCount = (Cols / 8) * Rows;
        
        // Bytes that hold the matrix
        std::uint8_t bytes[byteCount];
        
        // Gets the upper 32 bits of the result of a 32-bit multiplication operation
        // NOTE: the result of 32-bit multiplication results in a 64-bit number, and this function returns the upper 32 bits of that
        inline static uint32_t getMultiplyUpperPart(const uint32_t &i1, const uint32_t &i2) {
            return static_cast<uint32_t>((static_cast<uint64_t>(i1) * static_cast<uint64_t>(i2)) >> 32);
        }

    public:
        /**
         * Creates an empty BinaryMatrix instance, filled with zeroes.
         */
        constexpr inline explicit BinaryMatrix() {
            std::memset(bytes, 0, byteCount);
        }
        
        /**
         * Creates a BinaryMatrix instance by copying a byte array.
         */
        constexpr inline explicit BinaryMatrix(std::uint8_t *b) {
            std::memcpy(bytes, b, byteCount);
        }
        
        /**
         * Creates a BinaryMatrix instance from an initializer list of bytes that are copied.
         */
        constexpr inline BinaryMatrix(const std::initializer_list<uint8_t> &init) {
            size_t i = 0;
            for (auto it = init.begin(); it != init.end(); it++) {
                bytes[i++] = *it;
            }
        }
        
        /**
         * Gets the internal buffer that stores the bytes that make up the matrix.
         */
        constexpr inline const uint8_t *getBytes() const {
            return bytes;
        }
        
        /**
         * Gets the number of bytes the matrix needs to store its data.
         */
        constexpr inline static std::size_t getByteCount() {
            return byteCount;
        }
        
        /**
         * Sets a specific bit in the matrix.
         */
        inline void setBit(uint32_t row, uint32_t col, uint8_t bit) {
            // A bit is either 0 or 1.
            assert(bit == 0 || bit == 1);

            // Position within the byte array
            uint32_t bytecol = col / 8;
            uint32_t bitcol = col  % 8;
            uint32_t targetAddr = row * (Cols / 8) + bytecol;
            
            // Mask
            uint32_t mask = ~(1 << (7 - bitcol));
            
            // Clear the bit
            this->bytes[targetAddr] &= mask;
            
            // Set the bit
            this->bytes[targetAddr] |= (bit << (7 - bitcol)); 
        }
        
        /**
         * Gets a specific bit from the matrix.
         */
        inline uint8_t getBit(uint32_t row, uint32_t col) const {
            // Position within the byte array
            uint32_t bytecol = col / 8;
            uint32_t bitcol = col  % 8;
            uint32_t targetAddr = row * (Cols / 8) + bytecol;
            
            // Get the bit
            uint8_t result = (this->bytes[targetAddr] >> (7 - bitcol)) & 1;
    //        std::cout << "getBit(" << row << "," << col << "): addr=" << targetAddr << " byte=" << BinaryPrint<uint8_t>(this->bytes[targetAddr]) << ", bit=" << (unsigned)result << std::endl;
            return result;
        }
        
        /**
         * Gets a pointer to the memory that stores a specified row.
         */
        inline const uint8_t *getRow(uint32_t rowNumber) const {
            return (this->bytes + (rowNumber * (Cols / 8)));
        }
        
        /**
         * @brief Calculates the product of the matrix and a GF(2) vector.
         *
         * This function multiplies the current GF(2) matrix with a GF(2) vector.
         *
         * Template parameters:
         * - Tin: the type that holds the GF(2) vector with which you wish to multiply
         * - Tout: the type that holds the GF(2) vector that is the result of the multiplication
         */
        template<typename Tin, typename Tout>
        inline Tout calculateProduct(const Tin &vec) const {
            // TODO: accelerate using SSE on Intel processors
            // TODO: accelerate using NEON on Cortex-A processors
            
            static_assert((sizeof(Tin) * 8) == Cols, "The input vector must have as many rows as the columns of the matrix.");
            static_assert((sizeof(Tout) * 8) >= Rows, "The output vector must have at least as many rows as the rows of the matrix.");
            
            Tout result = 0;
            const uint8_t *v = reinterpret_cast<const uint8_t*>(&vec);
            
            // Go through each row
            for (uint8_t i = 0; i < Rows; i++) {
                // Shift the result            
                result <<= 1;
            
                // Go through each column (in one-byte leaps) and compute the total parity of the row
                // TODO: speed this up by using more bits at the same time
                uint8_t parity = 0;
                for (uint8_t j = 0; j < sizeof(Tin); j++) {
                    // Get byte from matrix
                    uint8_t b1 = bytes[(i * Cols / 8) + j];
                    // Get byte from vector (compensate for little endianness)
                    uint8_t b2 = v[sizeof(Tin) - j - 1];
                    // Compute the parity
                    parity ^= computeParity(b1 & b2);
    //                std::cout << "bytes[...]=" << BinaryPrint<uint8_t>(bytes[(i * Cols / 8) + j]) << ", v[...]=" << BinaryPrint<uint8_t>(v[sizeof(Tin) - j - 1]) << std::endl;
                }
                // Set the LSB of the result
                result |= parity;
                
    //            std::cout << "(row " << (unsigned)i << ": " << (unsigned)parity << ", result=" << BinaryPrint<uint8_t>(result) << ")" << std::endl;
            }
            
            return result;
        }
        
        /**
         * Calculates the product of the current matrix and another.
         */
        template<unsigned X>
        BinaryMatrix<Rows, X> calculateProduct(const BinaryMatrix<Cols, X> &other) const {
            // Result matrix
            BinaryMatrix<Rows, X> result;
            
            // Columns become rows in the transposed matrix, so we can work with them easily
            auto otherTransposed = other.transpose();
            
            // Go through each row in the current matrix
            for (uint32_t i = 0; i < Rows; i++) {
                auto *r1 = this->getRow(i);
            
                // Go through each row in the transposed other matrix
                // (which correspond to columns in the original other matrix)
                for (uint32_t j = 0; j < X; j++) {
                    auto *r2 = otherTransposed.getRow(j);
                    
                    // TODO: speed this up by using more bits at the same time
                    uint8_t parity = 0;
                    for (uint8_t x = 0; x < (Cols / 8); x++) {
                        parity ^= computeParity(r1[x] & r2[x]);
                    }
                    
                    result.setBit(i, j, parity);
                }
            }
            
            return result;
        }
        
        /**
         * @brief Transposes the matrix.
         *
         * This method uses SSE2 instructions on processors where they are available.
         * Otherwise it falls back to a generic 32-bit implementation.
         */
        BinaryMatrix<Cols, Rows> transpose() const {
            // TODO: add support for NEON on Cortex-A processors
            
            BinaryMatrix<Cols, Rows> result;
            
            unsigned row, col;
            int i;
            
            if (SSE2_SUPPORTED) {
    #ifndef COMPILE_SSE2_CODE
    //#pragma message "Not compiling SSE2 code"
    #else
    //#pragma message "Compiling SSE2 code"

                // Variable that holds a 16x8 submatrix in a 128-bit register
                union {
                    __m128i x;
                    uint8_t b[16];
                } m16x8;
            
                // Process what we can in 16x8 blocks
                for (row = 0; row + 16 <= Rows; row += 16) {
                    for (col = 0; col < Cols; col += 8) {
                        // Fill the 16x8 submatrix, by putting all the bytes into a 128-bit register
                        // (Input is 16 pieces of uint8_t)
                        // NOTE: the resulting uint16_t must be put into its place taking into accunt endianness,
                        //       so here we play a small trick with the order of how things are handled. 
                        for (i = 0; i < 8; i++) {
                            // Figure out where we're taking the byte from
                            unsigned sourceAddr = ((row + i) * (Cols / 8)) + (col / 8);
                            // Put the byte into the 128-bit register (in reverse order)
                            m16x8.b[7 - i] = this->bytes[sourceAddr];
                        }
                        for (i = 8; i < 16; i++) {
                            // Figure out where we're taking the byte from
                            unsigned sourceAddr = ((row + i) * (Cols / 8)) + (col / 8);
                            // Put the byte into the 128-bit register (in reverse order)
                            m16x8.b[15 - i + 8] = this->bytes[sourceAddr];
                        }
    //                    std::cout << BinaryPrint<__m128i>(m16x8.x, "|") << std::endl;
                        
                        // Transpose the submatrix with clever SSE intrinsics
                        // (Output is 8 pieces of uint16_t)
                        for (i = 0; i < 8; i++) {
                            // Figure out where the current 16-bit row should go
                            unsigned targetAddr = ((col + i) * (Rows / 8) + (row) / 8);
    //                        std::cout << (unsigned)targetAddr << std::endl;
                            
                            // Create a transposed 16-bit row by taking the MSBs of the 8-bit bytes in the 128-bit register
                            uint16_t transposedRow = _mm_movemask_epi8(m16x8.x);
    //                        std::cout << BinaryPrint<uint16_t>(transposedRow, ":") << std::endl;
                            
                            // Put the 16-bit row into its destination
                            *reinterpret_cast<uint16_t*>(result.bytes + targetAddr) = transposedRow;
                            // Shift the 128-bit register left by 1 bit
                            m16x8.x = _mm_slli_epi64(m16x8.x, 1);
                        }
                    }
                }
                
                if (row != Rows) {
                    // There are 8 rows remaining.
                    
                    // Set the 128-bit register to zero
                    for (i = 0; i < 16; i++) {
                        m16x8.b[i] = 0;
                    }
                    
                    // Process the remaining rows in 8x8 units
                    for (col = 0; col < Cols; col += 8) {
                        // Fill half of the 128-bit register with the 8x8 submatrix
                        for (i = 0; i < 8; i++) {
                            unsigned sourceAddr = (row + i) * (Cols / 8) + col / 8;
                            m16x8.b[7 - i] = this->bytes[sourceAddr];
                        }
                        
                        // Transpose the 8x8 submatrix
                        for (i = 0; i < 8; i++) {
                            // Create a transposed 8-bit row by taking the MSBs of the 8-bit bytes in the 128-bit register
                            uint8_t transposedRow = _mm_movemask_epi8(m16x8.x);
                            // Figure out where the current 16-bit row should go
                            unsigned targetAddr = ((col + i) * (Rows / 8) + (row) / 8);
                            // Put the 8-bit row into its destination
                            *(result.bytes + targetAddr) = transposedRow;
                            // Shift the 128-bit register left by 1 bit
                            m16x8.x = _mm_slli_epi64(m16x8.x, 1);
                        }
                    }
                }
    #endif
            }
            else {
                // The following code works on any 32-bit architecture.
                // We do the same trick as above but using a 32-bit variable,
                // which means we can work with the matrix in dual 4×8 blocks
                
                // Variable that holds two 4×8 submatrices
                union {
                    uint32_t x[2];
                    uint8_t b[8];
                } m4x8d;
                
                for (row = 0; row < Rows; row += 8) {
                    for (col = 0; col < Cols; col += 8) {
                        // Fill the two 4×8 submatrices
                        // (Input is 8 pieces of uint8_t)
                        for (i = 0; i < 8; i++) {
                            uint32_t sourceAddr = (row + i) * (Cols / 8) + col / 8;
                            m4x8d.b[7 - i] = this->bytes[sourceAddr];
                            
                            
    //                        std::cout << "input" << i << ": " << BinaryPrint<uint8_t>(this->bytes[sourceAddr]) << std::endl;
                        }
                        
                        
    //                    std::cout << "packed input: " << BinaryPrint<uint32_t>(m4x8d.x[0]) << std::endl;
    //                    std::cout << "packed input: " << BinaryPrint<uint32_t>(m4x8d.x[1]) << std::endl;
                        
                        uint32_t targetAddr;
                        
                        for (i = 0; i < 7; i++) {
                            targetAddr = ((col + i) * (Rows / 8) + (row) / 8);
                            result.bytes[targetAddr]  = static_cast<uint8_t>(getMultiplyUpperPart(m4x8d.x[1] & (0x80808080 >> i), (0x02040810 << i)) & 0x0f) << 4;
                            result.bytes[targetAddr] |= static_cast<uint8_t>(getMultiplyUpperPart(m4x8d.x[0] & (0x80808080 >> i), (0x02040810 << i)) & 0x0f) << 0;
                            
    //                        std::cout << "output" << i << ": " << BinaryPrint<uint8_t>(result.bytes[targetAddr]) << std::endl;
                        }
                        
                        targetAddr = ((col + 7) * (Rows / 8) + (row) / 8);
                        result.bytes[targetAddr]  = static_cast<uint8_t>(getMultiplyUpperPart((m4x8d.x[1] << 7) & (0x80808080 >> 0), (0x02040810 << 0)) & 0x0f) << 4;
                        result.bytes[targetAddr] |= static_cast<uint8_t>(getMultiplyUpperPart((m4x8d.x[0] << 7) & (0x80808080 >> 0), (0x02040810 << 0)) & 0x0f) << 0;
                        
    //                    std::cout << "output7: " << BinaryPrint<uint8_t>(result.bytes[targetAddr]) << std::endl;
                    }
                }
                
            }
            
            return result;
        }
        
        inline bool isZero() const {
            // Check all bytes for zero
            for (size_t i = 0; i < BinaryMatrix<Rows, Cols>::byteCount; i++) {
                // If any byte is non-zero, return false
                if (this->bytes[i] != 0) {
                    return false;
                }
            }
            
            // If all bytes are zero, return true
            return true;
        }
        
        inline bool operator==(const BinaryMatrix<Rows, Cols> &other) const {
            return memcmp(other.bytes, this->bytes, BinaryMatrix<Rows, Cols>::byteCount) == 0;
        }
        
        inline bool operator!=(const BinaryMatrix<Rows, Cols> &other) const {
            return memcmp(other.bytes, this->bytes, BinaryMatrix<Rows, Cols>::byteCount) != 0;
        }
        
        /** Returns the number of rows in the matrix. */
        inline std::uint32_t rows() const {
            return Rows;
        }
        
        /** Returns the number of columns in the matrix. */
        inline std::uint32_t cols() const {
            return Cols;
        }

    };

}

/** Prints a binary matrix, for debugging purposes */
template<unsigned Rows, unsigned Cols>
::std::ostream &operator<<(::std::ostream &os, const ::fecmagic::BinaryMatrix<Rows, Cols> &matrix) {
    for (uint8_t i = 0; i < Rows; i++) {
        for (uint8_t j = 0; j < Cols / 8; j++) {
            os << ::fecmagic::BinaryPrint<uint8_t>(matrix.bytes[i * Cols / 8 + j]);
        }
        os << std::endl;
    }
    
    return os;
}

#endif // BINARYMATRIX_H

