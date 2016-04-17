
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
#include <string>
#include <iostream>
#include <sstream>
#include "../src/binaryprint.h"

using namespace std;
using namespace fecmagic;

int main() {
    ostringstream ss;
    
    ss << BinaryPrint<uint8_t>(0b10101010);
    assert(ss.str() == "10101010");
    ss.str(string());
    
    ss << BinaryPrint<uint8_t>(0b00000000);
    assert(ss.str() == "00000000");
    ss.str(string());
    
    ss << BinaryPrint<uint16_t>(0b1111111111111111);
    assert(ss.str() == "1111111111111111");
    ss.str(string());
    
    ss << BinaryPrint<uint16_t>(0b1111111111111111, "|");
    assert(ss.str() == "11111111|11111111");
    ss.str(string());
    
    ss << BinaryPrint<uint32_t>(0b01010101010101010101010101010101);
    assert(ss.str() == "01010101010101010101010101010101");
    ss.str(string());
    
    ss << BinaryPrint<uint32_t>(0b01010101010101010101010101010101, " ");
    assert(ss.str() == "01010101 01010101 01010101 01010101");
    ss.str(string());
    
    cout << "BinaryPrint test successful" << endl;
    
    return 0;
}

