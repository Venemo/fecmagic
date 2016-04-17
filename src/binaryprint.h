
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

#ifndef BINARYPRINT_H
#define BINARYPRINT_H

#include <iostream>
#include <type_traits>
#include <emmintrin.h>

namespace fecmagic {

    // Forward-declaration of the BinaryPrint class
    template <typename T>
    class BinaryPrint;
    
}

// Forward-declaration of the stream operators for BinaryPrint
template <typename T>
::std::ostream &operator<<(::std::ostream &os, const ::fecmagic::BinaryPrint<T> &bp);
#if defined(COMPILE_SSE2_CODE)
::std::ostream &operator<<(::std::ostream &os, const ::fecmagic::BinaryPrint<__m128i> &bp);
#endif

namespace fecmagic {

    /**
     * Convenience class for printing integers in binary form.
     */
    template<typename T = uint8_t>
    class BinaryPrint {
    private:
        // Friend: generic stream operator
        template<typename X>
        friend ::std::ostream &::operator<<(::std::ostream &os, const ::fecmagic::BinaryPrint<X> &bp);
        
        // Friend: specialized stream operator
        friend ::std::ostream &::operator<<(::std::ostream &os, const ::fecmagic::BinaryPrint<__m128i> &bp);
        
        // Wrapped value
        T val;
        
        // Separator
        ::std::string separator;
        
    public:
        /**
         * Constructs an instance of the BinaryPrint class.
         */
        inline explicit BinaryPrint(const T &v, std::string sep = std::string())
            : val(v), separator(sep) { }
        
    };

}

/**
 * Prints a BinaryPrint instance into a standard stream.
 */
template<typename T>
inline ::std::ostream &operator<<(::std::ostream &os, const ::fecmagic::BinaryPrint<T> &bp) {
    // Current bit index
    uint32_t x = 0;
    // Go through all bits with a mask
    for (T i = (1 << (sizeof(T) * 8 - 1)); i; i >>= 1) {
        // If appropriate, print a separator
        if ((x % 8 == 0) && (x != 0)) {
            os << bp.separator;
        }
        
        // Print current bit
        os << ((bp.val & i) ? "1" : "0");
        // Increase current bit index
        x++;
    }
    
    return os;
}

#if defined(COMPILE_SSE2_CODE)
/**
 * Prints a specialized SSE BinaryPrint instance into a standard stream.
 */
inline ::std::ostream &operator<<(::std::ostream &os, const ::fecmagic::BinaryPrint<__m128i> &bp) {
    // TODO: ifdef this function

    // Treat the 128-bit register as 16 pieces of 8-bit values
    union { __m128i x; uint8_t b[16]; } u;
    u.x = bp.val;
    
    // Print the 8-bit values individually
    for (uint8_t i = 0; i < 16; i++) {
        os << ::fecmagic::BinaryPrint<uint8_t>(u.b[i]) << bp.separator;
        // TODO: don't print last separator
    }
    
    return os;
}
#endif

#endif // BINARYPRINT_H

