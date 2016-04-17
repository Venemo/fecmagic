
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

#ifndef BLOCKCODE_H
#define BLOCKCODE_H

#include "binaryprint.h"
#include "binarymatrix.h"
#include "bitmaskcombination.h"
#define ROUND_8(x) ((x) + (((x) % 8) > 0 ? (8 - ((x) % 8)) : 0))

namespace fecmagic {

    /**
     * @brief Base class for block codes.
     * 
     * The BlockCode class implements basic functionality that is useful
     * for all block codes. It only needs the necessary matrices to work.
     * However, this class only contains a very wasteful algorithm for
     * decoding, so it is suggested to create derived classes for specific
     * block codes that override the decode method.
     *
     * Template parameters:
     * - Maximum number errors that can be corrected by the code
     * - Type used for storing codewords
     * - Type used for storing a source block
     * - Type used for storing syndromes
     */
    template<unsigned MaxCorrectedErrors, typename TCodeword, typename TSourceBlock, typename TSyndrome = uint32_t>
    class BlockCode {
    private:
        // Static assertions for some general things
        static_assert(sizeof(TCodeword) >= sizeof(TSourceBlock), "Codeword size must be greater than or equal to the source block size.");
        static_assert((sizeof(TCodeword) - sizeof(TSourceBlock)) <= sizeof(TSyndrome), "Syndrome must fit into the syndrome type.");

        // Useful constants
        static constexpr unsigned CodewordEffectiveLength = sizeof(TCodeword) * 8;
        static constexpr unsigned SourceBlockEffectiveLength = sizeof(TSourceBlock) * 8;
        static constexpr unsigned SyndromeEffectiveLength = sizeof(TSyndrome) * 8;

        // Matrices
        BinaryMatrix<CodewordEffectiveLength, SourceBlockEffectiveLength> generator_;
        BinaryMatrix<SyndromeEffectiveLength, CodewordEffectiveLength> parityCheck_;
        BinaryMatrix<SourceBlockEffectiveLength, CodewordEffectiveLength> decoder_;
        
    protected:
        /**
         * Calculates a syndrome of a codeword.
         */
        inline TSyndrome calculateSyndrome(const TCodeword &codeword) const {
            return parityCheck_.template calculateProduct<TCodeword, TSyndrome>(codeword);
        }
        
        /**
         * Tries every possible combination of corrections
         * until either the possibilities run out or the codeword
         * is corrected.
         *
         * This algorithm should work with any block code, but it is
         * suggested to use more sophisticated ways for specific
         * codes for which better algorithms exist.
         */
        inline TCodeword fixCodeword(const TCodeword &codeword, TSyndrome syndrome, bool &success) const {
    //        std::cout << "Syndrome is: " << BinaryPrint<TSyndrome>(syndrome) << std::endl;
            
            // Try every possible way to flip bits, to a maximum of MaxCorrectedErrors
            for (unsigned attempts = 1; attempts <= MaxCorrectedErrors; attempts++) {
                // Create helper for constructing bitmask combinations
                BitmaskCombination<TCodeword, MaxCorrectedErrors> b(attempts);
                
                // Go through all possible combinations
                for (TCodeword mask = b.next(); mask != 0; mask = b.next()) {
                    TSyndrome s = calculateSyndrome(mask);
                    
                    if (s == syndrome) {
                        TCodeword result = codeword ^ mask;
                        syndrome = calculateSyndrome(result);
                        
                        if (syndrome == 0) {
    //                        std::cout << "correct mask is: " << BinaryPrint<TCodeword>(mask) << std::endl;
    //                        std::cout << "codeword fixed to: " << BinaryPrint<TCodeword>(result) << std::endl;
                            success = true;
                            return result;
                        }
                        else {
                            std::cout << "unfixable error detected." << std::endl;
                            success = false;
                            return 0;
                        }
                        
                    }
                }
            }
            
            // If we didn't manage to fix it, we assume we detected an unfixable error
            success = false;
            return 0;
        }

    public:
        /**
         * @brief Initializes the BlockCode class.
         *
         * Constructor parameters:
         * - Generator matrix
         * - Parity check matrix
         * - Decoder matrix
         */
        inline explicit BlockCode(const decltype(generator_) &generator, const decltype(parityCheck_) &parityCheck, const decltype(decoder_) &decoder)
            : generator_(generator), parityCheck_(parityCheck), decoder_(decoder) { }
        
        /**
         * Encodes a block into a codeword.
         */
        inline TCodeword encode(const TSourceBlock &input) const {
            return generator_.template calculateProduct<TSourceBlock, TCodeword>(input);
        }
        
        /**
         * Decodes a codeword into a block, or tells if an unfixable error is detected.
         */
        inline virtual TSourceBlock decode(const TCodeword &input, bool &success) const {
            // Initial value of success is true
            success = true;
            // Check if codeword has any issues
            TCodeword codeword = input;
            TSyndrome syndrome = calculateSyndrome(input);
            if (syndrome != 0) {
                // See if the codeword has issues and if so, try to fix it
                codeword = fixCodeword(input, syndrome, success);
            }
            
            // If couldn't fix it, return 0
            if (!success) {
                return 0;
            }
            
            // If the codeword is okay, decode it
            return decoder_.template calculateProduct<TCodeword, TSourceBlock>(codeword);
        }
    };

}

#endif // BLOCKCODE_H

