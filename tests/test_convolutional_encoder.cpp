
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

#include "helper.h"
#include "../src/binaryprint.h"
#include "../src/convolutional-encoder.h"
#include "../src/convolutional-decoder.h"

#include <iostream>

using namespace std;
using namespace fecmagic;

constexpr uint8_t poly1 = 0x6d;
constexpr uint8_t poly2 = 0x4f;

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



bool testConvolutionalCode(size_t inputSize, const uint8_t *inputData) {
    // NOTE: the ConvolutionalEncoder class shifts in bits from the other direction,
    //       so we need to reverse the polynomials as well, to get the same output.
    ConvolutionalEncoder<7, uint8_t, (reverseBits(poly1) >> 1), (reverseBits(poly2) >> 1)> encoder;
    
    // Create input
    uint8_t *input = new uint8_t[inputSize];
    memset(input, 0, inputSize);
    memcpy(input, inputData, inputSize);
    
    // Create output
    size_t outputSize = encoder.calculateOutputSize(inputSize);
    uint8_t *output1 = new uint8_t[outputSize];
    
    // Encode output using the new algorithm
    encoder.reset(output1);
    encoder.encode(input, inputSize);
    encoder.flush();
    
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

bool testStreamingSimple() {
    ConvolutionalEncoder<7, uint8_t, (reverseBits(poly1) >> 1), (reverseBits(poly2) >> 1)> enc1;
    
    const char *input1 = "Hello world, are we cool yet?";
    uint32_t inputSize = strlen(input1);
    uint32_t outputSize = enc1.calculateOutputSize(inputSize);
    
    uint8_t *output1 = new uint8_t[outputSize];
    
    enc1.reset(output1);
    enc1.encode(input1, inputSize);
    enc1.flush();
    
    const char *input2_1 = "Hello ";
    const char *input2_2 = "world, are";
    const char *input2_3 = " we cool yet?";
    uint8_t *output2 = new uint8_t[outputSize];
    
    ConvolutionalEncoder<7, uint8_t, (reverseBits(poly1) >> 1), (reverseBits(poly2) >> 1)> enc2;
    enc2.reset(output2);
    enc2.encode(input2_1, strlen(input2_1));
    enc2.encode(input2_2, strlen(input2_2));
    enc2.encode(input2_3, strlen(input2_3));
    enc2.flush();
    
    bool success = (0 == memcmp(output1, output2, outputSize));
    
    delete [] output1;
    delete [] output2;
    
    return success;
}

bool testStreaming() {
    ConvolutionalEncoder<7, uint8_t, (reverseBits(poly1) >> 1), (reverseBits(poly2) >> 1)> enc1;
    
    constexpr uint32_t inputSize = 40;
    uint32_t outputSize = enc1.calculateOutputSize(inputSize);
    
    // Create input1
    uint8_t *input = new uint8_t[inputSize];
    for (uint32_t i = 0; i < inputSize; i++) {
        input[i] = rand() % 256;
    }
    
//    cout << "input1:" << endl;
//    for (uint32_t i = 0; i < inputSize; i++) {
//        cout << BinaryPrint<uint8_t>(input[i]) << " ";
//    }
//    cout << endl;
    
    // Create input2
    constexpr uint32_t sd = 15;
    uint8_t *input2_1 = new uint8_t[inputSize - sd];
    uint8_t *input2_2 = new uint8_t[sd];
    memcpy(input2_1, input, inputSize - sd);
    memcpy(input2_2, input + inputSize - sd, sd);
    
//    cout << "input2:" << endl;
//    for (uint32_t i = 0; i < (inputSize - sd); i++) {
//        cout << BinaryPrint<uint8_t>(input2_1[i]) << " ";
//    }
//    for (uint32_t i = 0; i < (sd); i++) {
//        cout << BinaryPrint<uint8_t>(input2_2[i]) << " ";
//    }
//    cout << endl;
    
    // Create output1
    uint8_t *output = new uint8_t[outputSize];
    
    enc1.reset(output);
    enc1.encode(input, inputSize);
    enc1.flush();
    
//    cout << "output1:" << endl;
//    for (uint32_t i = 0; i < outputSize; i++) {
//        cout << BinaryPrint<uint8_t>(output[i]) << " ";
//    }
//    cout << endl;
    
    // Create output2
    uint8_t *output2 = new uint8_t[outputSize];
    
    ConvolutionalEncoder<7, uint8_t, (reverseBits(poly1) >> 1), (reverseBits(poly2) >> 1)> enc2;
    enc2.reset(output2);
    enc2.encode(input2_1, inputSize - sd);
    enc2.encode(input2_2, sd);
    enc2.flush();
    
    
//    cout << "output2:" << endl;
//    for (uint32_t i = 0; i < outputSize; i++) {
//        cout << BinaryPrint<uint8_t>(output2[i]) << " ";
//    }
//    cout << endl;

    uint8_t *output3 = new uint8_t[outputSize];
    enc1.reset(output3);
    enc1.encode(input2_1, inputSize - sd);
    enc1.encode(input2_2, sd);
    enc1.flush();
    
    bool success = (0 == memcmp(output, output2, outputSize));
    success == success && (0 == memcmp(output, output3, outputSize));
    
    delete [] input;
    delete [] input2_1;
    delete [] input2_2;
    delete [] output;
    delete [] output2;
    delete [] output3;
    
    return success;
}

bool testSequenceSimple() {
    Sequence<uint8_t, 1, 1, 0, 1> seq;
    
    assert(seq.next() == 1);
    assert(seq.next() == 1);
    assert(seq.next() == 0);
    assert(seq.next() == 1);
    
    assert(seq.next() == 1);
    assert(seq.next() == 1);
    assert(seq.next() == 0);
    assert(seq.next() == 1);
    
    assert(seq.next() == 1);
    assert(seq.next() == 1);
    assert(seq.next() == 0);
    assert(seq.next() == 1);
    
    assert(seq.next() == 1);
    assert(seq.next() == 1);
    seq.reset();
    
    assert(seq.next() == 1);
    assert(seq.next() == 1);
    assert(seq.next() == 0);
    assert(seq.next() == 1);
    
    return true;
}

bool testPuncturedSimple(const char *input) {
    // Test if the Sequence class works correctly
    testSequenceSimple();
    
    // Non-punctured
    ConvolutionalEncoder<3, uint8_t, 7, 5> nonPuncturedEncoder;
    PuncturedConvolutionalEncoder<Sequence<uint8_t, 1, 1, 0, 1>, 3, uint8_t, 7, 5> puncturedEncoder;
    
    // Input size
    size_t inputSize = strlen(input);
    
    // Output
    size_t outputSize = decltype(nonPuncturedEncoder)::calculateOutputSize(inputSize);
    size_t puncturedOutputSize = decltype(puncturedEncoder)::calculateOutputSize(inputSize);
    uint8_t *output = new uint8_t[outputSize];
    uint8_t *puncturedOutput = new uint8_t[puncturedOutputSize];
    
    // Encode input
    nonPuncturedEncoder.reset(output);
    nonPuncturedEncoder.encode(input, inputSize);
    nonPuncturedEncoder.flush();
    
    puncturedEncoder.reset(puncturedOutput);
    puncturedEncoder.encode(input, inputSize);
    puncturedEncoder.flush();
    
    // Examine each output bit
    uint8_t *outputBits = new uint8_t[outputSize * 8];
    uint8_t *puncturedOutputBits = new uint8_t[puncturedOutputSize * 8];
    
    bytearray2zeroone(outputSize, output, outputBits);
    bytearray2zeroone(puncturedOutputSize, puncturedOutput, puncturedOutputBits);
    
//    cout << "inputSize=" << inputSize << endl;
//    cout << "outputSize=" << outputSize << endl;
//    cout << "puncturedOutputSize=" << puncturedOutputSize << endl;
    
    Sequence<uint8_t, 1, 1, 0, 1> seq;
    for (size_t i = 0, j = 0; i < (outputSize * 8) && j < (puncturedOutputSize * 8); i++) {
        if (0 == seq.next()) {
            continue;
        }
        
        assert(outputBits[i] == puncturedOutputBits[j]);
        
        j++;
    }
    
//    // Print
//    cout << endl << "Non-punctured:" << endl;
//    for (size_t i = 0; i < outputSize; i++) {
//        cout << BinaryPrint<uint8_t>(output[i]) << " ";
//    }
//    cout << endl << "Punctured:" << endl;
//    for (size_t i = 0; i < puncturedOutputSize; i++) {
//        cout << BinaryPrint<uint8_t>(puncturedOutput[i]) << " ";
//    }
//    cout << endl;
    
    delete [] output;
    delete [] puncturedOutput;
    delete [] outputBits;
    delete [] puncturedOutputBits;
    
    return true;
}

int main() {
    srand(time(0));
    
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
    
    enc3.reset(output);
    enc3.encode(input, 2);
    enc3.flush();
    uint8_t expectedOutput[] = { 0b00111000, 0b01100111, 0b11100010, 0b11001110, 0b11000000 };
    
    if (0 == memcmp(output, expectedOutput, 5)) {
        cout << "yup, k=3, rate=1/2 works!" << endl;
    }
    else {
        cout << "nope, k=3, rate=1/2 doesn't work!" << endl;
    }
    
    uint8_t decoded[] = { 0, 0, 0 };
    ConvolutionalDecoder<15, 3, uint8_t, 7, 5> dec3(decoded);
    
    dec3.decode(expectedOutput, 5);
    dec3.flush();
    
    if (0 == memcmp(decoded, input, 2)) {
        cout << "yup, k=3, rate=1/2 decode works!" << endl;
    }
    else {
        cout << "nope, k=3, rate=1/2 decode doesn't work!" << endl;
    }
    
    cout << "Simple streaming: " << testStreamingSimple() << endl;
    
    for (uint32_t i = 0; i < 100; i++) {
        assert(testStreaming());
    }
    cout << "Random streaming: ok" << endl;
    
    cout << "Simple punctured encoding: " << testPuncturedSimple("Hello, world!") << endl;
    
    return 0;
}
