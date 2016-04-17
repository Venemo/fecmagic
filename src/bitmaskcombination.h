
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

#ifndef BITMASKCOMBINATION_H
#define BITMASKCOMBINATION_H

#include <cassert>
#include <type_traits>

namespace fecmagic {

    /**
     * @brief Produces all possible combinations of a bitmask with the given number of bits set.
     * 
     * This class can be used to produce bitmask combinations, which is useful for
     * testing forward error correction codes. It can be used, for example, to produce
     * all possible combinations of bit errors.
     *
     * Template parameters:
     * - T: the integral type in which to produce the combinations
     * - MaxN: the maximum number of allowed set bits (used for sizing an array)
     */
    template<typename T, size_t MaxN, size_t Length = (sizeof(T) * 8)>
    class BitmaskCombination final {
    private:
        // Assert T to be an integral type
        static_assert(std::is_integral<T>::value, "T must be an integral type.");
        
        // Number of set bits
        unsigned n_;
        
        // Position of each set bit
        unsigned x[MaxN];
        
        // Whether we are done iterating or not
        bool done;
        
        // Produces the current bit mask from the positions of each set bit
        inline T currentMask() const {
            // Initialize empty result
            T result = 0;
            
            // Go through each position and set the bits accordingly
            for (unsigned i = 0; i < n_; i++) {
                result |= (1 << (Length - x[i] - 1));
            }
            
            return result;
        }

    public:
        /**
         * Constructs an instance of the BitmaskCombination class.
         * Parameters:
         * - n: the number of set bits
         */
        inline explicit BitmaskCombination(unsigned n = MaxN)
            : n_(n), done(false) {
            
            // Number of combinations must not exceed maximum number of combinations
            assert(n_ <= MaxN);
            
            // Initialize the array in ascending order
            for (unsigned i = 0; i < n_; i++) {
                x[i] = i;
            }
            
            // If zero combinations are allowed, we are already done
            if (n_ == 0) {
                done = true;
            }
        }
        
        /**
         * Returns the next bit mask combination.
         */
        T next() {
            // Check if we are done with all the possible combinations
            if (done) {
                return 0;
            }
            
            // Get the current combination (this is the current result)
            T result = currentMask();
            
            // Advance the combination:
            // Go through the array backwards
            for (int i = n_ - 1; i >= 0; i--) {
                // See if the current element is at the max value
                if (x[i] == (Length - (n_ - i))) {
                    // Check if this is the last possible combination
                    if (i == 0) {
                        done = true;
                        break;
                    }
                    else {
                        // Set position of current element
                        x[i] = x[i - 1] + 2;
                        
                        // Fix positions of following elements
                        for (unsigned j = i + 1; j < n_; j++) {
                            x[j] = x[j - 1] + 1;
                        }
                    }
                }
                else {
                    // Increase current element
                    x[i] ++;
                    
                    // No need to loop further
                    break;
                }
            }
            
            return result;
        }

    };

}

#endif // BITMASKCOMBINATION_H

