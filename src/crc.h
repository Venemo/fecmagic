
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

#ifndef FECMAGIC_CRC_H
#define FECMAGIC_CRC_H

#include <cassert>
#include <cstdint>
#include <cstring>

//#define CRC_DEBUG
#ifdef CRC_DEBUG
#   include <iostream>
#   include "binaryprint.h"
#   define DEBUG_PRINT(x) (::std::cout << "[CRC] " << x << ::std::endl << ::std::flush);
#else
#   define DEBUG_PRINT(x)
#endif

namespace fecmagic {

    template<uint32_t poly>
    uint16_t generateCrc16(const uint8_t *input, uint32_t inputSize) {
        constexpr uint32_t affectedByteCount = sizeof(decltype(poly));
        
        if (inputSize == 0) {
            return 0;
        }
        
        uint32_t inAddr = 0;
        uint32_t inBitPos = 7;
        
        uint8_t affectedBytes[affectedByteCount];
        memset(affectedBytes, 0, affectedByteCount);
        for (inAddr = 0; inAddr < affectedByteCount && inAddr < inputSize; inAddr++) {
            affectedBytes[inAddr] = input[inAddr];
        }
        inAddr = 0;
        
        while (inAddr < inputSize) {
            uint8_t inBit = (affectedBytes[0] >> inBitPos) & 1;
            if (inBit) {
                DEBUG_PRINT("one: ");
                for (uint32_t i = 0; i < affectedByteCount; i++) {
                    uint8_t mask = (uint8_t)(((poly >> (7 - inBitPos)) >> ((affectedByteCount - i - 1) * 8)) & 0xff);
                    DEBUG_PRINT(BinaryPrint<uint8_t>(affectedBytes[i]) << " ^ " << BinaryPrint<uint8_t>(mask) << " ");
                    affectedBytes[i] ^= mask;
                }
            }
            else {
                DEBUG_PRINT("zero: ");
                for (uint32_t i = 0; i < affectedByteCount; i++) {
                    DEBUG_PRINT(BinaryPrint<uint8_t>(affectedBytes[i]) << " ^ 0");
                }
            }
            
            // Advance input bit position
            if (inBitPos == 0) {
                inAddr++;
                inBitPos = 7;
                
                DEBUG_PRINT("-----");
                for (uint32_t i = 1; i < affectedByteCount; i++) {
                    affectedBytes[i - 1] = affectedBytes[i];
                }
                if ((inAddr + affectedByteCount - 1) < inputSize) {
                    affectedBytes[affectedByteCount - 1] = input[inAddr + affectedByteCount - 1];
                }
                else {
                    affectedBytes[affectedByteCount - 1] = 0;
                }
            }
            else {
                inBitPos--;
            }
        }
        
        uint16_t output = ((uint16_t)affectedBytes[0] << 8) | (uint16_t)affectedBytes[1];
        return output;
    }

}

#endif // FECMAGIC_CRC_H

