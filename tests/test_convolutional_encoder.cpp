
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

//
// README
// ======
//
// This file is a simple program to test the convolutional encoder of fecmagic. The
// encoder is tested against a known-to-work old implementation that we used in an
// old project. The results produced by the new algorithm are simply compared to the
// results produced by the new one.

#include <iostream>
#include <cstdlib>
#include <cstring>
#include "../src/binaryprint.h"
#include "../src/convolutional-encoder.h"
#include "../src/convolutional-decoder.h"

using namespace std;
using namespace fecmagic;

constexpr uint8_t poly1 = 0x6d;
constexpr uint8_t poly2 = 0x4f;

// Creates an array that contains zeros and ones from a byte array
void bytearray2zeroone(unsigned bytelength, const unsigned char in[], unsigned char out[]) {
    unsigned i, j;
    unsigned char cmp;
    for (i = 0; i < bytelength; i++) {
        cmp = 128;
        for (j = 0; j < 8; j++) {
            out[i * 8 + j] = ((cmp & in[i]) != 0);
            cmp /= 2;
        }
    }
}

// Creates a byte array from an array that contains zeros and ones
void zeroone2bytearray(unsigned bytelength, const unsigned char in[], unsigned char out[]) {
    unsigned i, j;
    unsigned char cmp;
    for (i = 0; i < bytelength; i++) {
        out[i] = 0;
        cmp = 128;
        for (j = 0; j < 8; j++) {
            out[i] += in[i * 8 + j] * cmp;
            cmp /= 2;
        }
    }
}

void old_encode(unsigned char in[], unsigned char out[], size_t inputsize, size_t encodedlength) {
    size_t i;
    unsigned char shft = 0;
    
    memset(out, 0, encodedlength);

    for(i = 0; i < inputsize + 4; i++) {
        shft = (shft << 1) | ((i >= inputsize) ? 0 : in[i]);
        out[2 * i + 0] = computeParity(shft & poly1);
        out[2 * i + 1] = computeParity(shft & poly2);
    }
}

void encodeWithOldAlgorithm(uint8_t *input, size_t inputSize, uint8_t *output) {
    size_t inputbitsize = inputSize * 8;
    size_t encodedlength = inputbitsize * 2 + 8;

    unsigned char *input_bits = new unsigned char[inputbitsize];
    memset(input_bits, 0, inputbitsize);
    bytearray2zeroone(inputSize, input, input_bits);

    unsigned char *encoded_bits = new unsigned char[encodedlength];
    memset(encoded_bits, 0, encodedlength);
    
    old_encode(input_bits, encoded_bits, inputSize * 8, encodedlength);

    memset(output, 0, encodedlength / 8);
    zeroone2bytearray(encodedlength / 8, encoded_bits, output);
    
    delete [] input_bits;
    delete [] encoded_bits;
}

template<typename T>
constexpr T reverseBits(T v) {
    // Check template parameter
    static_assert(std::is_integral<T>::value == true, "You must use this function with integral types.");
    
    // Thanks to Sean Eron Anderson for making this algorithm,
    // see http://graphics.stanford.edu/~seander/bithacks.html#BitReverseObvious
    T r = v;
    uint32_t s = sizeof(v) * 8 - 1;
    
    for (v >>= 1; v; v >>= 1) {
        r <<= 1;
        r |= v & 1;
        s--;
    }
    r <<= s;
    
    return r;
}

bool testConvolutionalCode(size_t inputSize, const uint8_t *inputData) {
    // Create input
    uint8_t *input = new uint8_t[inputSize];
    memset(input, 0, inputSize);
    memcpy(input, inputData, inputSize);
    
    // Create output
    size_t outputSize = 2 * inputSize + 1;
    uint8_t *output1 = new uint8_t[outputSize];
    memset(output1, 0, outputSize);
    
    // Encode output using the new algorithm
    // NOTE: the ConvolutionalEncoder class shifts in bits from the other direction,
    //       so we need to reverse the polynomials as well, to get the same output.
    ConvolutionalEncoder<7, uint8_t, (reverseBits(poly1) >> 1), (reverseBits(poly2) >> 1)> encoder;
    encoder.encodeBlock(input, inputSize, output1);
    
    // Encode output using old (known to work) algorithm
    uint8_t *output2 = new uint8_t[outputSize];
    memset(output2, 0, outputSize);
    encodeWithOldAlgorithm(input, inputSize, output2);
    
    // Check if both algorithms create the same result
    bool success = (0 == memcmp(output1, output2, outputSize));
    
    // Free used memory
    delete [] input;
    delete [] output1;
    delete [] output2;
    
    return success;
}

int main() {
    cout << "poly1 = " << BinaryPrint<uint8_t>(poly1) << endl;
    cout << "poly2 = " << BinaryPrint<uint8_t>(poly2) << endl;
    
    // Test the encoder with the K=7, rate=1/2 (Voyager) code
    const char *testInput;
    testInput = "Hello!";
    cout << "test with '" << testInput << "': " << testConvolutionalCode(strlen(testInput) + 1, reinterpret_cast<const uint8_t*>(testInput)) << endl;
    testInput = "Hello world!";
    cout << "test with '" << testInput << "': " << testConvolutionalCode(strlen(testInput) + 1, reinterpret_cast<const uint8_t*>(testInput)) << endl;
    testInput = "Good morning, Captain! Are we awesome yet?";
    cout << "test with '" << testInput << "': " << testConvolutionalCode(strlen(testInput) + 1, reinterpret_cast<const uint8_t*>(testInput)) << endl;
    
    // Test the encoder for the code that is described here:
    // http://home.netcom.com/~chip.f/viterbi/algrthms.html
    ConvolutionalEncoder<3, uint8_t, 7, 5> enc3;
    uint8_t input[] = { 0b01011100, 0b10100010 };
    uint8_t output[] = { 0, 0, 0, 0, 0 };
    
    enc3.encodeBlock(input, 2, output);
    uint8_t expectedOutput[] = { 0b00111000, 0b01100111, 0b11100010, 0b11001110, 0b11000000 };
    
    if (0 == memcmp(output, expectedOutput, 5)) {
        cout << "yup, k=3, rate=1/2 works!" << endl;
    }
    else {
        cout << "nope, k=3, rate=1/2 doesn't work!" << endl;
    }
    
    ConvolutionalDecoder<15, 3, uint8_t, 7, 5> dec3;
    uint8_t decoded[] = { 0, 0, 0 };
    
    dec3.decodeBlock(expectedOutput, 5, decoded);
    
    if (0 == memcmp(decoded, input, 2)) {
        cout << "yup, k=3, rate=1/2 decode works!" << endl;
    }
    else {
        cout << "nope, k=3, rate=1/2 decode doesn't work!" << endl;
    }
    
    return 0;
}
