
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

#ifndef GOLAY_H
#define GOLAY_H

#include "blockcode.h"

namespace fecmagic {

    class GolayCode final : public BlockCode<3, uint32_t, uint16_t, uint16_t> {
    public:
        inline explicit GolayCode()
            : BlockCode({
                // 24x12 generator matrix (represented in 32x16)
                0, 0,
                0, 0,
                0, 0,
                0, 0,
                0, 0,
                0, 0,
                0, 0,
                0, 0,
                0b00001000, 0b00000000,
                0b00000100, 0b00000000,
                0b00000010, 0b00000000,
                0b00000001, 0b00000000,
                0b00000000, 0b10000000,
                0b00000000, 0b01000000,
                0b00000000, 0b00100000,
                0b00000000, 0b00010000,
                0b00000000, 0b00001000,
                0b00000000, 0b00000100,
                0b00000000, 0b00000010,
                0b00000000, 0b00000001,
                0b00001001, 0b11110001,
                0b00000100, 0b11111010,
                0b00000010, 0b01111101,
                0b00001001, 0b00111110,
                0b00001100, 0b10011101,
                0b00001110, 0b01001110,
                0b00001111, 0b00100101,
                0b00001111, 0b10010010,
                0b00000111, 0b11001001,
                0b00000011, 0b11100110,
                0b00000101, 0b01010111,
                0b00001010, 0b10101011,
            }, {
                // 12x24 parity check matrix (represented as 16x32)
                0, 0, 0,
                0, 0, 0,
                0, 0, 0,
                0, 0, 0,
                0, 0b10011111, 0b00011000, 0b00000000,
                0, 0b01001111, 0b10100100, 0b00000000,
                0, 0b00100111, 0b11010010, 0b00000000,
                0, 0b10010011, 0b11100001, 0b00000000,
                0, 0b11001001, 0b11010000, 0b10000000,
                0, 0b11100100, 0b11100000, 0b01000000,
                0, 0b11110010, 0b01010000, 0b00100000,
                0, 0b11111001, 0b00100000, 0b00010000,
                0, 0b01111100, 0b10010000, 0b00001000,
                0, 0b00111110, 0b01100000, 0b00000100,
                0, 0b01010101, 0b01110000, 0b00000010,
                0, 0b10101010, 0b10110000, 0b00000001,
            }, {
                // 12x24 decode matrix (represented as 16x32)
                0, 0, 0, 0,
                0, 0, 0, 0,
                0, 0, 0, 0,
                0, 0, 0, 0,
                0, 0b10000000, 0b00000000, 0,
                0, 0b01000000, 0b00000000, 0,
                0, 0b00100000, 0b00000000, 0,
                0, 0b00010000, 0b00000000, 0,
                0, 0b00001000, 0b00000000, 0,
                0, 0b00000100, 0b00000000, 0,
                0, 0b00000010, 0b00000000, 0,
                0, 0b00000001, 0b00000000, 0,
                0, 0b00000000, 0b10000000, 0,
                0, 0b00000000, 0b01000000, 0,
                0, 0b00000000, 0b00100000, 0,
                0, 0b00000000, 0b00010000, 0,
            }) { }
    };

}

#endif // GOLAY_H

