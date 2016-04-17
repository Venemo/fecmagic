
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

#include <cassert>
#include <iostream>
#include "../src/bitmaskcombination.h"
#include "../src/binaryprint.h"

using namespace std;
using namespace fecmagic;

int main() {
    BitmaskCombination<uint8_t, 1> c1(1);
    for (uint8_t x = c1.next(), i = 1; x; x = c1.next(), i++) {
        cout << BinaryPrint<uint8_t>(x) << " <- " << (unsigned)i << endl;
    }
    cout << endl;
    
    BitmaskCombination<uint8_t, 7> c2(7);
    for (uint8_t x = c2.next(), i = 1; x; x = c2.next(), i++) {
        cout << BinaryPrint<uint8_t>(x) << " <- " << (unsigned)i << endl;
    }
    cout << endl;
    
    BitmaskCombination<uint8_t, 7> c3(2);
    for (uint8_t x = c3.next(), i = 1; x; x = c3.next(), i++) {
        cout << BinaryPrint<uint8_t>(x) << " <- " << (unsigned)i << endl;
    }
    cout << endl;
    
    BitmaskCombination<uint8_t, 7> c4(6);
    for (uint8_t x = c4.next(), i = 1; x; x = c4.next(), i++) {
        cout << BinaryPrint<uint8_t>(x) << " <- " << (unsigned)i << endl;
    }
    cout << endl;
    
    BitmaskCombination<uint8_t, 7> c5(3);
    for (uint8_t x = c5.next(), i = 1; x; x = c5.next(), i++) {
        cout << BinaryPrint<uint8_t>(x) << " <- " << (unsigned)i << endl;
    }
    cout << endl;
    
    BitmaskCombination<uint8_t, 7> c6(5);
    for (uint8_t x = c6.next(), i = 1; x; x = c6.next(), i++) {
        cout << BinaryPrint<uint8_t>(x) << " <- " << (unsigned)i << endl;
    }
    cout << endl;
    
    BitmaskCombination<uint8_t, 7> c7(4);
    for (uint8_t x = c7.next(), i = 1; x; x = c7.next(), i++) {
        cout << BinaryPrint<uint8_t>(x) << " <- " << (unsigned)i << endl;
    }
    cout << endl;
    
    BitmaskCombination<uint8_t, 7> c8(0);
    for (uint8_t x = c8.next(), i = 1; x; x = c8.next(), i++) {
        cout << BinaryPrint<uint8_t>(x) << " <- " << (unsigned)i << endl;
    }
    cout << endl;
    
    BitmaskCombination<uint8_t, 8> c9(8);
    for (uint8_t x = c9.next(), i = 1; x; x = c9.next(), i++) {
        cout << BinaryPrint<uint8_t>(x) << " <- " << (unsigned)i << endl;
    }
    cout << endl;
    
    // Test with less than the full width of the variable
    BitmaskCombination<uint8_t, 1, 6> aaa(1);
    for (uint8_t x = aaa.next(), i = 1; x; x = aaa.next(), i++) {
        cout << BinaryPrint<uint8_t>(x) << " <- " << (unsigned)i << endl;
    }
    cout << endl;
    
    return 0;
}

