
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

#include <limits>
#include <cstdint>
#include <cassert>

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
     * - Polynomials: the polynomials used for this convolutional code. Each polynomial
     *   corresponds to an output but, so in other words the code rate is the reciproc
     *   of the number of polynomials.
     *
     */
    template<uint32_t ConstraintLength, typename TShiftReg = std::uint32_t, TShiftReg ...Polynomials>
    class ConvolutionalEncoder final {
        // Check template parameters using static asserts
        static_assert((sizeof(TShiftReg) * 8) >= ConstraintLength, "The shift register must be able to hold the constraint length of the code.");
        static_assert(ConstraintLength >= 2, "The ConstraintLength template parameter must be at least two.");
        static_assert(sizeof...(Polynomials) >= 2, "There must be at least two polynomials.");
    
    private:
        
        // Number of outputs of the convolutional code (each polynomial corresponds to an output)
        constexpr static uint32_t outputCount_ = sizeof...(Polynomials);
        
        // Unpack variadic template argument, to allow access to each polynomial
        constexpr static TShiftReg polynomials_[sizeof...(Polynomials)] = { Polynomials... };
        
        // Output
        uint8_t *output;
        
        // Output position
        size_t outAddr;
        uint32_t outBitPos;
        
        // The shift register
        TShiftReg shiftReg = 0;
        
        inline void produceOutput() {
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
        }
        
    public:
    
        /**
         * @brief Creates a convolutional encoder
         */
        constexpr inline explicit ConvolutionalEncoder(void *output = nullptr) {
            this->reset(output);
        }
        
        /**
         * @brief Copy constructor. Intentionally disabled for this class.
         */
        ConvolutionalEncoder(const ConvolutionalEncoder &other) = delete;
        
        /**
         * @brief Move constructor.
         */
        ConvolutionalEncoder(const ConvolutionalEncoder &&other) {
            this->operator=(other);
        }
        
        /**
         * @brief Copy assignment operator. Intentionally disabled for this class.
         */
        ConvolutionalEncoder &operator=(const ConvolutionalEncoder &other) = delete;
        
        /**
         * @brief Move assignment operator.
         */
        ConvolutionalEncoder &operator=(const ConvolutionalEncoder &&other) {
            this->output = std::move(other.output);
            this->shiftReg = std::move(other.shiftReg);
            this->outAddr = std::move(other.outAddr);
            this->outBitPos = std::move(other.outBitPos);
        };
        
        /**
         * @brief Resets the convolutional encoder and sets the given output.
         */
        void reset(void *output) {
            this->output = reinterpret_cast<uint8_t*>(output);
            this->shiftReg = 0;
            this->outAddr = 0;
            this->outBitPos = 7;
        }
        
        /**
         * @brief Returns the size of the output you need to allocate for a given input.
         *
         * Output size: give space to encoded bits and
         * after that, allow the encoder to be flushed.
         */
        static inline uint32_t calculateOutputSize(uint32_t inputSize) {
            // Number of bytes occupied by the constraint length
            constexpr uint32_t constraintLengthBytes = (ConstraintLength / 8) + (((ConstraintLength % 8) == 0) ? 0 : 1);
            // Total output size
            uint32_t outputSize = (inputSize * outputCount_) + constraintLengthBytes;
            
            return outputSize;
        }
        
        /**
         * @brief Encodes the given block.
         *
         * Encodes the block of supplied input bytes using the current convolutional code.
         * The caller of this method is responsible for making sure that enough memory is
         * allocated to fit the output.
         *
         * This method is suitable for streaming.
         */
        void encode(const uint8_t *input, size_t inputSize) {
            if (inputSize == 0) {
                return;
            }
            
            assert(input != nullptr);
            assert(output != nullptr);
            
            // Input position
            size_t inAddr = 0;
            uint32_t inBitPos = 7;
            
            // Go through each input byte
            while (inAddr < inputSize) {
                // Shift the register right
                shiftReg >>= 1;
                
                // If the input still has bytes, use them, otherwise just flush the register
                if (inAddr < inputSize) {
                    // Shift the next input bit into the register
                    shiftReg |= (((input[inAddr] >> inBitPos) & 1) << (ConstraintLength - 1));
                }
                
                // Produce output
                produceOutput();
                
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
        
        /**
         * @brief Flushes the encoder, outputting the remaining bits until the shift register is empty.
         */
        void flush() {
            while (shiftReg != 0) {
                // Shift the register right
                shiftReg >>= 1;
                
                // Produce output
                produceOutput();
            }
        }
        
    };

}

#endif // CONVOLUTIONAL_ENCODER
