
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

#ifndef FECMAGIC_SEQUENCE_H
#define FECMAGIC_SEQUENCE_H

#include <cstdint>
#include <cstddef>

namespace fecmagic {

    template<typename T, T ... Numbers>
    class Sequence final {
    
    private:
        
        size_t index;
        
    public:
        
        constexpr static T count = sizeof...(Numbers);
        
        constexpr static T numbers[sizeof...(Numbers)] = { Numbers... };
        
        explicit inline Sequence() {
            reset();
        }
        
        // TODO: convert this into a C++11 constexpr function
        static inline size_t zeroes() {
            size_t n = 0;
            for (size_t i = 0; i < count; i++) {
                if (numbers[i] == 0) {
                    n++;
                }
            }
            return n;
        }
        
        static inline size_t nonZeroes() {
            return count - zeroes();
        }
        
        inline T current() const {
            return numbers[index];
        }
        
        inline T next() {
            index ++;
            
            if (index == count) {
                index = 0;
            }
            
            return current();
        }
        
        void reset() {
            index = count - 1;
        }
        
        
    };
    
    template<typename T, T ... Numbers>
    constexpr T Sequence<T, Numbers...>::numbers[sizeof...(Numbers)];

}

#endif // FECMAGIC_SEQUENCE_H

