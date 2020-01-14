
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

#include "fecmagic-global.h"

//#define CRC_DEBUG
#ifdef CRC_DEBUG
#   include <iostream>
#   define DEBUG_PRINT(x) (::std::cout << "[CRC] " << x << ::std::endl << ::std::flush);
#else
#   define DEBUG_PRINT(x)
#endif

namespace fecmagic {

    template<typename T, T POLY, T INIT, T XOROUT, bool REF_IN, bool REF_OUT>
    T crc_calc(const uint8_t *input_bytes, size_t input_size)
    {
        if (!input_size) {
            return 0;
        }

        assert(input_bytes);
        T out = INIT;

        for (size_t byte_pos = 0; byte_pos < (input_size); ++byte_pos) {
            uint8_t in_byte = input_bytes[byte_pos];
            out ^= (REF_IN ? bitreverse_8(in_byte) : in_byte) << (sizeof(T) * 8 - 8);

            for (size_t bit_cnt = 0; bit_cnt < 8; ++bit_cnt) {
                uint32_t mask = -(out >> (sizeof(T) * 8 - 1));
                out <<= 1;
                out ^= (POLY & mask);
            }
        }

        if (REF_OUT) {
            static_assert(sizeof(T) <= sizeof(uint32_t), "Size of T must be less than or equal to 32-bit");
            out = bitreverse_32(out) >> (sizeof(uint32_t) * 8 - sizeof(T) * 8);
        }

        // Flip output bits
        out ^= XOROUT;

        return out;
    }
 
    uint16_t crc16_buypass(const uint8_t *input_bytes, size_t input_size)
    {
        return crc_calc<uint16_t, 0x8005, 0, 0, 0, 0>(input_bytes, input_size);
    }
 
    uint16_t crc16_arc(const uint8_t *input_bytes, size_t input_size)
    {
        return crc_calc<uint16_t, 0x8005, 0, 0, 1, 1>(input_bytes, input_size);
    }
 
    uint16_t crc16_usb(const uint8_t *input_bytes, size_t input_size)
    {
        return crc_calc<uint16_t, 0x8005, 0xFFFF, 0xFFFF, 1, 1>(input_bytes, input_size);
    }
 
    uint32_t crc32_iso(const uint8_t *input_bytes, size_t input_size)
    {
        return crc_calc<uint32_t, 0x04C11DB7, 0xFFFFFFFF, 0xFFFFFFFF, 1, 1>(input_bytes, input_size);
    }
 
    uint32_t crc32_posix(const uint8_t *input_bytes, size_t input_size)
    {
        return crc_calc<uint32_t, 0x04C11DB7, 0, 0xFFFFFFFF, 0, 0>(input_bytes, input_size);
    }
 
    uint32_t crc32_32c(const uint8_t *input_bytes, size_t input_size)
    {
        return crc_calc<uint32_t, 0x1EDC6F41, 0xFFFFFFFF, 0xFFFFFFFF, 1, 1>(input_bytes, input_size);
    }

}

#endif // FECMAGIC_CRC_H

