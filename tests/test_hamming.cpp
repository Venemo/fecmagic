
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
#include "../src/hamming.h"

using namespace std;
using namespace fecmagic;

int main() {
    HammingCode c;
    
    for (uint8_t i = 0; i <= 0b00001111; i++) {
        uint8_t coded = c.encode(i);
        
        // Test single-bit errors
        BitmaskCombination<uint8_t, 1, 7> bmc1;
        for (uint8_t mask = bmc1.next(); mask; mask = bmc1.next()) {
            bool decodeSuccess;
            uint8_t corrupted = coded ^ mask;
            uint8_t decoded = c.decode(corrupted, decodeSuccess);
            
            if (!decodeSuccess || decoded != i) {
                cout << "FAIL! Could not decode single-bit error." << endl
                    << " the input=\t" << BinaryPrint<uint8_t>(i) << endl
                    << " encoded=\t" << BinaryPrint<uint8_t>(coded) << endl
                    << " the mask=\t" << BinaryPrint<uint8_t>(mask) << endl
                    << " corrupted=\t" << BinaryPrint<uint8_t>(corrupted) << endl
                    << " output=\t" << BinaryPrint<uint8_t>(decoded) << endl;
            }
            else {
                //cout << "SUCCESS! Could decode single-bit error." << endl;
            }
            assert(decoded == i);
        }
        
    }

    return 0;
}
