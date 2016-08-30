
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
#include <cassert>
#include <cstddef>
#include <limits>
#include <utility>

#include "fecmagic-global.h"
#include "sequence.h"

//#define CONVOLUTIONAL_ENCODER_DEBUG
#ifdef CONVOLUTIONAL_ENCODER_DEBUG
#   include <iostream>
#   include "binaryprint.h"
#   define DEBUG_PRINT(x) (::std::cout << "[ConvolutionalEncoder] " << x << std::endl);
#else
#   define DEBUG_PRINT(x)
#endif

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
    template<typename TPuncturingMatrix, uint32_t ConstraintLength, typename TShiftReg, TShiftReg ...Polynomials>
    class PuncturedConvolutionalEncoder final {
        // Check template parameters using static asserts
        static_assert((sizeof(TShiftReg) * 8) >= ConstraintLength, "The shift register must be able to hold the constraint length of the code.");
        static_assert(ConstraintLength >= 2, "The ConstraintLength template parameter must be at least two.");
        static_assert(sizeof...(Polynomials) >= 2, "There must be at least two polynomials.");
        
    private:
        
        // Number of outputs of the convolutional code (each polynomial corresponds to an output)
        constexpr static uint32_t outputCount_ = sizeof...(Polynomials);
        
        // Unpack variadic template argument, to allow access to each polynomial
        constexpr static TShiftReg polynomials_[sizeof...(Polynomials)] = { Polynomials... };
        
        // Instantiate puncturing matrix
        TPuncturingMatrix puncturingMatrix;
        
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
                // Don't output when the next item in the puncturing matrix is zero
                if (0 == puncturingMatrix.next()) {
                    DEBUG_PRINT("punctured bit");
                    continue;
                }
                
                if (outBitPos == 7) {
                    DEBUG_PRINT("set " << outAddr << " byte to 0");
                    output[outAddr] = 0;
                }
                
                DEBUG_PRINT(outAddr << "/" << outBitPos << " shiftReg=" << BinaryPrint<TShiftReg>(shiftReg));
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
        explicit PuncturedConvolutionalEncoder(void *output = nullptr) {
            this->reset(output);
        }
        
        /**
         * @brief Copy constructor. Intentionally disabled for this class.
         */
        PuncturedConvolutionalEncoder(const PuncturedConvolutionalEncoder &other) = delete;
        
        /**
         * @brief Move constructor.
         */
        PuncturedConvolutionalEncoder(const PuncturedConvolutionalEncoder &&other) {
            this->operator=(other);
        }
        
        /**
         * @brief Copy assignment operator. Intentionally disabled for this class.
         */
        PuncturedConvolutionalEncoder &operator=(const PuncturedConvolutionalEncoder &other) = delete;
        
        /**
         * @brief Move assignment operator.
         */
        PuncturedConvolutionalEncoder &operator=(const PuncturedConvolutionalEncoder &&other) {
            this->output = std::move(other.output);
            this->shiftReg = std::move(other.shiftReg);
            this->outAddr = std::move(other.outAddr);
            this->outBitPos = std::move(other.outBitPos);
        };
        
        /**
         * @brief Resets the convolutional encoder and sets the given output.
         */
        void reset(void *output) {
            DEBUG_PRINT(::std::endl << "resetting encoder");
            this->puncturingMatrix.reset();
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
        static inline size_t calculateOutputSize(size_t inputSize) {
            // Calculate non-punctured output bits
            size_t outputBits = ((inputSize * 8) + ConstraintLength) * outputCount_;
            DEBUG_PRINT("inputsize=" << inputSize << ", constraintlength=" << ConstraintLength << ", non-punctured output bit count: " << outputBits);
            
            // Take puncturing into account
            size_t t = outputBits * TPuncturingMatrix::nonZeroes();
            size_t puncturedOutputBits = t / TPuncturingMatrix::count;
            if (0 != (t % TPuncturingMatrix::count)) {
                puncturedOutputBits += 1;
            }
            DEBUG_PRINT("punctured output bit count: " << puncturedOutputBits);
            
            // Calculate byte count
            size_t outputSize = puncturedOutputBits / 8;
            if (0 != puncturedOutputBits % 8) {
                outputSize += 1;
            }
            DEBUG_PRINT("output byte count: " << outputSize);
            
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
        void encode(const void *input, size_t inputSize) {
            DEBUG_PRINT("encode()ing");
            
            if (inputSize == 0) {
                return;
            }
            
            const uint8_t *inputBytes = reinterpret_cast<const uint8_t*>(input);
            assert(inputBytes != nullptr);
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
                    shiftReg |= (((inputBytes[inAddr] >> inBitPos) & 1) << (ConstraintLength - 1));
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
            DEBUG_PRINT("flush()ing");
            
            for (uint32_t i = 0; i < ConstraintLength; i++) {
                // Shift the register right
                shiftReg >>= 1;
                
                // Produce output
                produceOutput();
            }
        }
        
    };
    
    // Definition for the static member PuncturedConvolutionalEncoder::polynomials_
    template<typename TPuncturingMatrix, uint32_t ConstraintLength, typename TShiftReg, TShiftReg ...Polynomials>
    constexpr TShiftReg PuncturedConvolutionalEncoder<TPuncturingMatrix, ConstraintLength, TShiftReg, Polynomials...>::polynomials_[sizeof...(Polynomials)];
    
    template<uint32_t ConstraintLength, typename TShiftReg, TShiftReg ...Polynomials>
    using ConvolutionalEncoder = PuncturedConvolutionalEncoder<Sequence<uint8_t, 1>, ConstraintLength, TShiftReg, Polynomials...>;

}

#ifdef DEBUG_PRINT
#   undef DEBUG_PRINT
#endif

#endif // CONVOLUTIONAL_ENCODER
