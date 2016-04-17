
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

#include <iostream>
#include "../src/golay.h"

using namespace std;
using namespace fecmagic;

int main() {
    GolayCode c;
    
    // One-bit errors
    cout << "Testing one-bit errors" << endl;
    for (uint16_t i = 0; i <= 0xfff; i++) {
        uint32_t coded = c.encode(i);
        
        // Test single-bit errors
        BitmaskCombination<uint32_t, 1, 24> bmc1;
        for (uint32_t mask = bmc1.next(); mask; mask = bmc1.next()) {
            bool decodeSuccess;
            uint32_t corrupted = coded ^ mask;
            uint32_t decoded = c.decode(corrupted, decodeSuccess);
            
            if (!decodeSuccess || decoded != i) {
                cout << "FAIL! Could not decode one-bit error." << endl
                    << " the input=\t" << BinaryPrint<uint32_t>(i) << endl
                    << " encoded=\t" << BinaryPrint<uint32_t>(coded) << endl
                    << " the mask=\t" << BinaryPrint<uint32_t>(mask) << endl
                    << " corrupted=\t" << BinaryPrint<uint32_t>(corrupted) << endl
                    << " output=\t" << BinaryPrint<uint32_t>(decoded) << endl;
            }
            else {
                cout << "\rSo far so good: " << i;
            }
            assert(decoded == i);
        }
    }
    cout << endl;
    
    // Two-bit errors
    cout << "Testing two-bit errors" << endl;
    for (uint16_t i = 0; i <= 0xfff; i++) {
        uint32_t coded = c.encode(i);
        
        // Test single-bit errors
        BitmaskCombination<uint32_t, 2, 24> bmc1;
        for (uint32_t mask = bmc1.next(); mask; mask = bmc1.next()) {
            bool decodeSuccess;
            uint32_t corrupted = coded ^ mask;
            uint32_t decoded = c.decode(corrupted, decodeSuccess);
            
            if (!decodeSuccess || decoded != i) {
                cout << "FAIL! Could not decode two-bit error." << endl
                    << " the input=\t" << BinaryPrint<uint32_t>(i) << endl
                    << " encoded=\t" << BinaryPrint<uint32_t>(coded) << endl
                    << " the mask=\t" << BinaryPrint<uint32_t>(mask) << endl
                    << " corrupted=\t" << BinaryPrint<uint32_t>(corrupted) << endl
                    << " output=\t" << BinaryPrint<uint32_t>(decoded) << endl;
            }
            else {
                cout << "\rSo far so good: " << i;
            }
            assert(decoded == i);
        }
    }
    cout << endl;
    
    // Three-bit errors
    cout << "Testing three-bit errors" << endl;
    for (uint16_t i = 0; i <= 0xfff; i++) {
        uint32_t coded = c.encode(i);
        
        // Test single-bit errors
        BitmaskCombination<uint32_t, 3, 24> bmc1;
        for (uint32_t mask = bmc1.next(); mask; mask = bmc1.next()) {
            bool decodeSuccess;
            uint32_t corrupted = coded ^ mask;
            uint32_t decoded = c.decode(corrupted, decodeSuccess);
            
            if (!decodeSuccess || decoded != i) {
                cout << "FAIL! Could not decode three-bit error." << endl
                    << " the input=\t" << BinaryPrint<uint32_t>(i) << endl
                    << " encoded=\t" << BinaryPrint<uint32_t>(coded) << endl
                    << " the mask=\t" << BinaryPrint<uint32_t>(mask) << endl
                    << " corrupted=\t" << BinaryPrint<uint32_t>(corrupted) << endl
                    << " output=\t" << BinaryPrint<uint32_t>(decoded) << endl;
            }
            else {
                cout << "\rSo far so good: " << i;
            }
            assert(decoded == i);
        }
    }
    cout << endl;
    
    return 0;
}
