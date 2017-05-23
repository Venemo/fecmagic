
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

#ifndef BITPACKER_H_
#define BITPACKER_H_

#include <cstdint>
#include <cstddef>
#include <cstring>
#include <limits>

//#define BITPACKER_DEBUG
#ifdef BITPACKER_DEBUG
#   include <iostream>
#   include "binaryprint.h"
#   define DEBUG_PRINT(x) (::std::cout << "[BitPacker] " << x << std::endl);
#else
#   define DEBUG_PRINT(x)
#endif

namespace fecmagic {
    
    template<typename TBlock, uint32_t blockSizeInBits, uint32_t affectedByteCount>
    class BitPacker final {
    
    private:
    
        uint_fast8_t shift_ = 0;
        size_t pos_ = 0;
        uint8_t *output_;
        
    public:
    
        BitPacker(uint8_t *output)
        : output_(output) { }
        
        void pack(TBlock block) {
            DEBUG_PRINT("pos=" << pos_ << ", shift=" << (uint32_t)shift_);
            uint8_t x[affectedByteCount];
            
            constexpr uint32_t l = affectedByteCount - 1;
            uint32_t sh;
            for (size_t i = 0; i < l; i++) {
                sh = blockSizeInBits - (i * 8) - shift_;
                x[i] = (block >> (sh & 0xff));
                DEBUG_PRINT("x[" << i << "] = " << BinaryPrint<uint8_t>(x[i]) << " sh=" << sh);
            }
            sh = (l * 8) - blockSizeInBits + shift_;
            x[l] = (block << (sh) & 0xff);
            DEBUG_PRINT("x[" << l << "] = " << BinaryPrint<uint8_t>(x[l]) << " sh=" << sh);
            
            if (shift_ != 0) {
                output_[pos_] |= x[0];
                DEBUG_PRINT("out[" << pos_ << "] = " << BinaryPrint<uint8_t>(output_[pos_]));
                pos_++;
            }
            
            shift_++;
            shift_ %= 8;
            
            for (size_t i = 1; i < affectedByteCount; i++) {
                output_[pos_] = x[i];
                DEBUG_PRINT("out[" << pos_ << "] = " << BinaryPrint<uint8_t>(output_[pos_]));
                if (i != (affectedByteCount - 1)) {
                    pos_++;
                }
            }
        }
        
    };
    
    template<typename TBlock, uint32_t blockSizeInBits, uint32_t affectedByteCount>
    class BitUnpacker final {
    
    private:
    
        uint_fast8_t shift_ = 0;
        size_t pos_ = 0;
        const uint8_t *input_;
        
    public:
    
        BitUnpacker(const uint8_t *input)
        : input_(input) { }
        
        TBlock unpack() {
            constexpr TBlock blockMask = ::std::numeric_limits<TBlock>::max() >> ((sizeof(TBlock) * 8) - blockSizeInBits);
            DEBUG_PRINT("pos = " << pos_);
        
            uint8_t x[affectedByteCount];
            if (shift_ == 0) {
                x[0] = 0;
                memcpy(x + 1, input_ + pos_, affectedByteCount - 1);
            }
            else {
                memcpy(x, input_ + pos_, affectedByteCount);
            }
            
            TBlock block = 0;
            
            constexpr uint32_t l = affectedByteCount - 1;
            uint32_t sh;
            for (size_t i = 0; i < l; i++) {
                sh = blockSizeInBits - shift_ - (i * 8);
                block |= (((TBlock)x[i]) << sh);
                DEBUG_PRINT("x[" << i << "] = " << BinaryPrint<uint8_t>(x[i]) << " sh=" << sh);
            }
            sh = (l * 8) - blockSizeInBits + shift_;
            block |= (((TBlock)x[l]) >> sh);
            DEBUG_PRINT("x[" << l << "] = " << BinaryPrint<uint8_t>(x[l]) << " sh=" << sh);
            
            if (shift_ == 0) {
                pos_ += (affectedByteCount - 2);
            }
            else {
                pos_ += (affectedByteCount - 1);
            }
            
            shift_++;
            shift_ %= 8;
            
            return block & blockMask;
        }
        
    };
    
}

#endif // BITPACKER_H_

