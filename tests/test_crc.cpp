
#include "../src/crc.h"
#include "../src/binaryprint.h"
#include "helper.h"
#include <iostream>

using namespace std;
using namespace fecmagic;

// Old crc generator algorithm:
// This is a know-to-work algorithm for the same CRC,
// used in an old project in some form at some point.
void old_crc_gen(unsigned char in[], uint32_t inSize, unsigned char crc_b[]) {
    constexpr uint32_t th = 17;
    uint32_t i, j;
    uint32_t k = inSize;
    unsigned char gen[th] = { 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1 };
    
    for (i = 0; i < k; i++) {
        if (in[i]) {
            for (j = 0; j < th; j++) {
                in[i + j] ^= gen[j];
            }
        }
    }
    
    for (i = 0; i < (th - 1); i++) {
        crc_b[i] = in[i + k];
    }
}

bool testCrc(uint8_t *input, uint32_t inputSize) {
    uint32_t testbitsize = inputSize * 8 + 2 * 8;
    uint8_t *testbits = new uint8_t[testbitsize];
    uint8_t *resultbits = new uint8_t[16];
    uint8_t *resultArr = new uint8_t[2];
    memset(testbits, 0, testbitsize);
    memset(resultbits, 0, 16);
    
    // Old algorithm
    bytearray2zeroone(inputSize, input, testbits);
    old_crc_gen(testbits, inputSize * 8, resultbits);
    zeroone2bytearray(2, resultbits, resultArr);
    uint16_t result1 = ((uint16_t)resultArr[0] << 8) | resultArr[1];
    
    // New algorithm
    uint16_t result2 = generateCrc16<0xC0028000>(input, inputSize);
    
//    cout << BinaryPrint<uint16_t>(result1) << endl;
//    cout << BinaryPrint<uint16_t>(result2) << endl;
    
    delete [] testbits;
    delete [] resultbits;
    delete [] resultArr;
    
    return result1 == result2;
}

int main() {
    uint8_t test1[] = { 3, 42, 16 };
    cout << (testCrc(test1, 3) ? "yes" : "no") << endl;
    
    for (uint32_t i = 0; i < 20; i++) {
        uint8_t test[3];
        for (uint32_t j = 0; j < 3; j++) {
            test[j] = rand() % 256;
        }
        //cout << "test: " << BinaryPrint<uint8_t>(test[0]) << " " << BinaryPrint<uint8_t>(test[1]) << " " << BinaryPrint<uint8_t>(test[2]) << endl;
        
        bool r = testCrc(test, 3);
        assert(r);
    }
    
    for (uint32_t i = 0; i < 20; i++) {
        uint8_t test[4];
        for (uint32_t j = 0; j < 4; j++) {
            test[j] = rand() % 256;
        }
        //cout << "test: " << BinaryPrint<uint8_t>(test[0]) << " " << BinaryPrint<uint8_t>(test[1]) << " " << BinaryPrint<uint8_t>(test[2]) << " " << BinaryPrint<uint8_t>(test[3]) << endl;
        
        bool r = testCrc(test, 4);
        assert(r);
    }
    
    for (uint32_t i = 0; i < 20; i++) {
        uint8_t test[10];
        for (uint32_t j = 0; j < 10; j++) {
            test[j] = rand() % 256;
        }
        //cout << "test: " << BinaryPrint<uint8_t>(test[0]) << " " << BinaryPrint<uint8_t>(test[1]) << " " << BinaryPrint<uint8_t>(test[2]) << endl;
        
        bool r = testCrc(test, 10);
        assert(r);
    }
    
    for (uint32_t i = 0; i < 20; i++) {
        uint8_t test[50];
        for (uint32_t j = 0; j < 50; j++) {
            test[j] = rand() % 256;
        }
        //cout << "test: " << BinaryPrint<uint8_t>(test[0]) << " " << BinaryPrint<uint8_t>(test[1]) << " " << BinaryPrint<uint8_t>(test[2]) << endl;
        
        bool r = testCrc(test, 50);
        assert(r);
    }
    
    uint8_t p[29] = { 0x02, 0x77, 0x37, 0x10, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00  };
    cout << testCrc(p, 29) << endl;
    
    cout << "all tests successful" << endl;

    return 0;
}
