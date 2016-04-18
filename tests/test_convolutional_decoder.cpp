
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
#include <random>
#include "../src/binaryprint.h"
#include "../src/convolutional-encoder.h"
#include "../src/convolutional-decoder.h"

//#define TEST_CONVOLUTIONAL_DECODER_DEBUG
#ifdef TEST_CONVOLUTIONAL_DECODER_DEBUG
#   define DEBUG_PRINT(x) (::std::cout << "[TestConvolutionalDecoder] " << x << std::endl);
#else
#   define DEBUG_PRINT(x)
#endif

using namespace std;
using namespace fecmagic;

constexpr uint8_t poly1 = 0x5b;
constexpr uint8_t poly2 = 0x79;

template<typename TEnc, typename TDec>
bool testEncodeAndDecode(TEnc &enc, TDec &dec, const char *data) {
    size_t dataSize = strlen(data) + 1;
    size_t encodedSize = TEnc::calculateOutputSize(dataSize);
    size_t decodedSize = TDec::calculateOutputSize(encodedSize);
    uint8_t *encInput = new uint8_t[dataSize];
    uint8_t *encOutput = new uint8_t[encodedSize];
    uint8_t *decOutput = new uint8_t[decodedSize];
    memcpy(encInput, data, dataSize);
    memset(encOutput, 0, encodedSize);
    memset(decOutput, 0, decodedSize);
    
#ifdef TEST_CONVOLUTIONAL_DECODER_DEBUG
    for (uint32_t i = 0; i < dataSize; i++) {
        cout << BinaryPrint<uint8_t>(encInput[i]) << " ";
    }
    cout << endl;
#endif
    
    enc.reset(encOutput);
    enc.encode(encInput, dataSize);
    enc.flush();
    
#ifdef TEST_CONVOLUTIONAL_DECODER_DEBUG
    for (uint32_t i = 0; i < encodedSize; i++) {
        cout << BinaryPrint<uint8_t>(encOutput[i]) << " ";
    }
    cout << endl;
#endif
    
    dec.reset(decOutput);
    dec.decode(encOutput, encodedSize);
    dec.flush();
    
#ifdef TEST_CONVOLUTIONAL_DECODER_DEBUG
    for (uint32_t i = 0; i < decodedSize; i++) {
        cout << BinaryPrint<uint8_t>(decOutput[i]) << " ";
    }
    cout << endl;
#endif
    
    bool success = (0 == memcmp(decOutput, encInput, dataSize));
    
    delete [] encInput;
    delete [] encOutput;
    delete [] decOutput;
    
    return success;
}

template<typename TEnc, typename TDec>
bool testEncodeAndDecodeWithBitErrors(TEnc &enc, TDec &dec, const char *data, uint32_t errorCount) {
    size_t dataSize = strlen(data) + 1;
    size_t encodedSize = TEnc::calculateOutputSize(dataSize);
    size_t decodedSize = TDec::calculateOutputSize(encodedSize);
    uint8_t *encInput = new uint8_t[dataSize];
    uint8_t *encOutput = new uint8_t[encodedSize];
    uint8_t *decOutput = new uint8_t[decodedSize];
    memcpy(encInput, data, dataSize);
    memset(encOutput, 0, encodedSize);
    memset(decOutput, 0, decodedSize);
    
#ifdef TEST_CONVOLUTIONAL_DECODER_DEBUG
    for (uint32_t i = 0; i < dataSize; i++) {
        cout << BinaryPrint<uint8_t>(encInput[i]) << " ";
    }
    cout << endl;
#endif
    
    enc.reset(encOutput);
    enc.encode(encInput, dataSize);
    enc.flush();
    
#ifdef TEST_CONVOLUTIONAL_DECODER_DEBUG
    for (uint32_t i = 0; i < encodedSize; i++) {
        cout << BinaryPrint<uint8_t>(encOutput[i]) << " ";
    }
    cout << endl;
#endif
    
    // Initialize random engine
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_real_distribution<double> dist(1.0, 10.0);
    
    // Add bit errors
    for (uint32_t i = 0; i < errorCount; i++) {
        uint32_t randomNumber = dist(mt);
        uint32_t byte = randomNumber / 8;
        uint32_t bit = randomNumber - (byte * 8);
        
        encOutput[byte] ^= (1 << bit);
    }
    
    // Decode
    dec.reset(decOutput);
    dec.decode(encOutput, encodedSize);
    dec.flush();
    
#ifdef TEST_CONVOLUTIONAL_DECODER_DEBUG
    for (uint32_t i = 0; i < decodedSize; i++) {
        cout << BinaryPrint<uint8_t>(decOutput[i]) << " ";
    }
    cout << endl;
#endif
    
    bool success = (0 == memcmp(decOutput, encInput, dataSize));
    
    delete [] encInput;
    delete [] encOutput;
    delete [] decOutput;
    
    return success;
}

int main() {
    cout << "Testing basic functionality (k=3, rate=1/3)" << endl;
    ConvolutionalEncoder<3, uint8_t, 7, 3, 5> enc2;
    ConvolutionalDecoder<15, 3, uint8_t, 7, 3, 5> dec2;
    ConvolutionalDecoder<2, 3, uint8_t, 7, 3, 5> dec2_2;
    assert(testEncodeAndDecode(enc2, dec2, "Hello!"));
    assert(testEncodeAndDecode(enc2, dec2_2, "Hello!"));
    cout << "OK" << endl;
    
    cout << "Testing basic functionality (k=3, rate=1/2)" << endl;
    ConvolutionalEncoder<3, uint8_t, 7, 5> enc1;
    ConvolutionalDecoder<10, 3, uint8_t, 7, 5> dec1;
    ConvolutionalDecoder<50, 3, uint8_t, 7, 5> dec1_2;
    ConvolutionalDecoder<5, 3, uint8_t, 7, 5> dec1_3;
    
    assert(testEncodeAndDecode(enc1, dec1, "Hello!"));
    assert(testEncodeAndDecode(enc1, dec1_2, "Hello!"));
    assert(testEncodeAndDecode(enc1, dec1_3, "Hello!"));
    cout << "OK" << endl;
    
    cout << "Testing basic functionality (k=7, rate=1/2)" << endl;
    ConvolutionalEncoder<7, uint8_t, poly1, poly2> encoder;
    ConvolutionalDecoder<100, 7, uint8_t, poly1, poly2> decoder;
    
    assert(testEncodeAndDecode(encoder, decoder, "Hello!"));
    assert(testEncodeAndDecode(encoder, decoder, "Good morning, Captain! Are we awesome yet?"));
    cout << "OK" << endl;
    
    cout << "Testing with 1-bit errors" << endl;
    for (uint32_t i = 0; i < 100; i++) {
        assert(testEncodeAndDecodeWithBitErrors(encoder, decoder, "Hello!", 1));
    }
    cout << "OK" << endl;
    
    cout << "Testing with 2-bit errors" << endl;
    for (uint32_t i = 0; i < 100; i++) {
        assert(testEncodeAndDecodeWithBitErrors(encoder, decoder, "Hello world! Are we awesome yet?", 2));
    }
    cout << "OK" << endl;
    
    cout << "Testing with 3-bit errors" << endl;
    for (uint32_t i = 0; i < 100; i++) {
        assert(testEncodeAndDecodeWithBitErrors(encoder, decoder, "Hello world! Are we awesome yet?", 3));
    }
    cout << "OK" << endl;
    
    return 0;
}

