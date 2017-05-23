
#include "../src/bitpacker.h"
#include "../src/binaryprint.h"
#include "helper.h"
#include <cstdlib>
#include <ctime>
#include <iostream>

using namespace fecmagic;
using namespace std;

int main() {
    srand(time(nullptr));
    
    // Create input =========================================================
    
    constexpr size_t inputBlocks = 22;
    uint32_t input[inputBlocks] = { 0 };
    uint8_t input_test[inputBlocks * 23] = { 0 };
    
    for (size_t i = 0; i < inputBlocks; i++) {
        uint32_t block = rand();
        block >>= (32 - 23);
        
        input[i] = block;
        uint8_t block_test[4] = {
            static_cast<uint8_t>((block >> 24) & 0xff),
            static_cast<uint8_t>((block >> 16) & 0xff),
            static_cast<uint8_t>((block >> 8) & 0xff),
            static_cast<uint8_t>((block & 0xff))
        };
        uint8_t block_test_zeroone[32];
        bytearray2zeroone(4, block_test, block_test_zeroone);
        memcpy(input_test + (i * 23), block_test_zeroone + 32 - 23, 23);
        
        cout << "block #" << i << ":\t";
        for (size_t j = 0; j < 23; j++) {
            cout << (uint32_t) *(block_test_zeroone + 32 - 23 + j);
        }
        cout << endl;
    }
    
    for (size_t i = 0; i < inputBlocks * 23; i++) {
        if (i % 23 == 0)
            cout << endl << "block #?:\t";
        
        cout << (uint32_t) input_test[i];
    }
    
    cout << endl;
    
    // Pack =================================================================

    cout << endl << "pack ======================" << endl;
        
    uint8_t packed[64] = { 0 };
    uint8_t packed_test[64 * 8] = { 0 };
    
    BitPacker<uint32_t, 23, 4> packer(packed);
    
    for (size_t i = 0; i < inputBlocks; i++) {        
        packer.pack(input[i]);
    }
    

//    for (size_t i = 0; i < 64; i++) {
//        if (i % 4 == 0) {
//            cout << endl;
//        }
//        else {
//            cout << " ";
//        }
//        cout << BinaryPrint<uint8_t>(packed[i]);
//    }
//    
//    cout << endl;
    
    bytearray2zeroone(64, packed, packed_test);
    
    cout << endl << "comparison ====================" << endl;
    
    if (0 == memcmp(packed_test, input_test, inputBlocks * 23)) {
        cout << "Packed bits match the input." << endl;
    }
    else {
        cout << "Packed bits DON'T match the input." << endl;
        return 1;
    }
    
    cout << endl << "unpack =====================" << endl;
    
    // Unpack ==================================================================
    
    
    uint32_t unpacked[inputBlocks] = { 0 };
    uint8_t unpacked_test[inputBlocks * 23] = { 0 };
    BitUnpacker<uint32_t, 23, 4> unpacker(packed);
    
    for (size_t i = 0; i < inputBlocks; i++) {
        uint32_t block = unpacker.unpack();
        unpacked[i] = block;
        
        uint8_t block_test[4] = {
            static_cast<uint8_t>((block >> 24) & 0xff),
            static_cast<uint8_t>((block >> 16) & 0xff),
            static_cast<uint8_t>((block >> 8) & 0xff),
            static_cast<uint8_t>((block & 0xff))
        };
        uint8_t block_test_zeroone[32];
        bytearray2zeroone(4, block_test, block_test_zeroone);
        memcpy(unpacked_test + (i * 23), block_test_zeroone + 32 - 23, 23);
    }
    
    for (size_t i = 0; i < inputBlocks * 23; i++) {
        if (i % 23 == 0)
            cout << endl << "block #?:\t";
        
        cout << (uint32_t) unpacked_test[i];
        if (unpacked_test[i] != input_test[i]) {
            cout << " <- mismatch at " << i << endl;
            return 1;
        }
    }
    
    cout << endl;
    
    // Check match
    if (0 == memcmp(input, unpacked, inputBlocks)) {
        cout << "unpacked blocks match the input" << endl;
    }
    else {
        cout << "unpacked blocks DON'T match the input :-(" << endl;
    }
    
    
    return 0;
}

