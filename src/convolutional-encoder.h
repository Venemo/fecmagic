
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

#ifndef CONVOLUTIONAL_ENCODER
#define CONVOLUTIONAL_ENCODER

#include <cstdint>
#include <initializer_list>
#include <limits>
#include <tuple>

#include "fecmagic-global.h"

namespace fecmagic {

    /**
     * @brief Encoder that can produce convolutional code.
     * 
     * This is a customizable encoder that produces varying convolutional codes.
     *
     * Template parameters:
     * - ConstraintLength: the constraint length of the code
     * - TShiftReg: unsigned integral type that can hold the shift register
     * - Polynomials: the polynomials used for this convolutional code. Each polynomial corresponds to an output but, so in other words
         the code rate is the reciproc of the number of polynomials.
     */
    template<uint32_t ConstraintLength, typename TShiftReg = std::uint32_t, TShiftReg ...Polynomials>
    class ConvolutionalEncoder final {
        // Check template parameters using static asserts
        static_assert((sizeof(TShiftReg) * 8) >= ConstraintLength, "The shift register must be able to hold the constraint length of the code.");
    
    private:
        
        // Number of outputs of the convolutional code (each polynomial corresponds to an output)
        constexpr static uint32_t outputCount_ = sizeof...(Polynomials);
        
        // Unpack variadic template argument, to allow access to each polynomial
        constexpr static TShiftReg polynomials_[sizeof...(Polynomials)] = { Polynomials... };

    public:
    
        /**
         * @brief Creates a convolutional encoder
         */
        constexpr inline explicit ConvolutionalEncoder() { }
        
        /**
         * @brief Encodes the given block.
         *
         * Encodes the block of supplied input bytes using the current convolutional code.
         * The caller of this method is responsible for making sure that enough memory is
         * allocated to fit the output.
         *
         * This method is not suitable for streaming.
         */
        void encodeBlock(const uint8_t *input, size_t inputSize, uint8_t *output) {
            // The shift register
            TShiftReg shiftReg = 0;
            
            // Input position
            size_t inAddr = 0;
            uint32_t inBitPos = 7;
            
            // Output position
            size_t outAddr = 0;
            uint32_t outBitPos = 7;
            
            // Go through each input byte
            while (inAddr < inputSize || shiftReg != 0) {
                // Shift the register right
                shiftReg >>= 1;
                
                // If the input still has bytes, use them, otherwise just flush the register
                if (inAddr < inputSize) {
                    // Shift the next input bit into the register
                    shiftReg |= (((input[inAddr] >> inBitPos) & 1) << (ConstraintLength - 1));
                }
                
                // Compute outputs from the current state of the encoder
                for (uint32_t o = 0; o < outputCount_; o++) {
                    // Compute output and put it to its correct place
                    output[outAddr] |= (::fecmagic::computeParity(polynomials_[o] & shiftReg) << outBitPos);
                    
                    // Advance output bit position
                    if (outBitPos == 0) {
                        outAddr++;
                        outBitPos = 7;
                    }
                    else {
                        outBitPos--;
                    }
                }
                
                // Advance input bit position
                if (inBitPos == 0) {
                    inAddr++;
                    inBitPos = 7;
                }
                else {
                    inBitPos--;
                }
            }
        }
        
    };

}

#endif // CONVOLUTIONAL_ENCODER
