
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
#include <cstdlib>
#include <cstring>
#include "../src/binaryprint.h"
#include "../src/convolutional-encoder.h"
#include "../src/convolutional-decoder.h"

using namespace std;
using namespace fecmagic;

constexpr uint8_t poly1 = 0x5b;
constexpr uint8_t poly2 = 0x79;

template<typename TEnc, typename TDec>
void testEncodeAndDecode(TEnc &enc, TDec &dec, const char *data) {
    size_t dataSize = strlen(data) + 1;
    size_t encodedSize = dataSize * 2 + 1;
    uint8_t *encInput = new uint8_t[dataSize];
    uint8_t *encOutput = new uint8_t[encodedSize];
    uint8_t *decOutput = new uint8_t[dataSize + 1];
    memcpy(encInput, data, dataSize);
    memset(encOutput, 0, encodedSize);
    memset(decOutput, 0, dataSize + 1);
    
    for (uint32_t i = 0; i < dataSize; i++) {
        cout << BinaryPrint<uint8_t>(encInput[i]) << " ";
    }
    cout << endl;
    
    enc.encodeBlock(encInput, dataSize, encOutput);
    
    for (uint32_t i = 0; i < encodedSize; i++) {
        cout << BinaryPrint<uint8_t>(encOutput[i]) << " ";
    }
    cout << endl;
    
    dec.decodeBlock(encOutput, encodedSize, decOutput);
    
    for (uint32_t i = 0; i < dataSize + 1; i++) {
        cout << BinaryPrint<uint8_t>(decOutput[i]) << " ";
    }
    cout << endl;
    
    bool success = (0 == memcmp(decOutput, encInput, dataSize));
    cout << success << endl;
    
    delete [] encInput;
    delete [] encOutput;
    delete [] decOutput;
}

int main() {
    ConvolutionalEncoder<7, uint8_t, poly1, poly2> encoder;
    ConvolutionalDecoder<35, 7, uint8_t, poly1, poly2> decoder;
    
    testEncodeAndDecode(encoder, decoder, "Hello!");
    testEncodeAndDecode(encoder, decoder, "Good morning, Captain! Are we awesome yet?");
    
    return 0;
}

