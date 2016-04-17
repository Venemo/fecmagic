
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

#ifndef FECMAGIC_GLOBAL_H
#define FECMAGIC_GLOBAL_H

// Pre-defined macros for architectures:
// http://sourceforge.net/p/predef/wiki/Architectures/
#if !defined(FECMAGIC_NO_SSE2) && (defined(__i386__) || defined(_M_IX86) || defined(__x86_64__) || defined(_M_X64))
#    include <emmintrin.h>
#    define COMPILE_SSE2_CODE
#    if defined(__GNUC__)
#        define SSE2_SUPPORTED __builtin_cpu_supports("sse2")
#    elif defined(_MSC_VER)
#        define SSE2_SUPPORTED IsProcessorFeaturePresent(PF_XMMI64_INSTRUCTIONS_AVAILABLE)
#    else
#        define SSE2_SUPPORTED false
#    endif
#endif

#ifndef COMPILE_SSE2_CODE
#   define SSE2_SUPPORTED false
#endif

namespace fecmagic {

    /**
     * Computes the parity of the given number:
     * result is 1 when there are an odd number of ones,
     * and and 0 when there are an even number of ones.
     */
    inline uint8_t computeParity(unsigned x) {
#if defined(__GNUC__)
        return __builtin_parity(x);
#elif defined(_MSC_VER)
        return __popcnt(x) % 2;
#else
#   error "Unknown compiler, write parity check function"
#endif
    }
    
}

#endif // FECMAGIC_GLOBAL_H
