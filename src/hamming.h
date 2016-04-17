
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

#ifndef HAMMING_H
#define HAMMING_H

#include "blockcode.h"

namespace fecmagic {

    class HammingCode final : public BlockCode<1, uint8_t, uint8_t, uint8_t> {
    public:
        // TODO: add the possibility of choosing different Hamming codes
        inline explicit HammingCode()
            : BlockCode({
                0,
                0b00001101,
                0b00001011,
                0b00001000,
                0b00000111,
                0b00000100,
                0b00000010,
                0b00000001,
            }, {
                0,
                0,
                0,
                0,
                0,
                0b01010101,
                0b00110011,
                0b00001111,
            }, {
                0,
                0,
                0,
                0,
                0b00010000,
                0b00000100,
                0b00000010,
                0b00000001,
            }) { }
    };

}

#endif // HAMMING_H

