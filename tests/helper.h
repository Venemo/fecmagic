
#ifndef CODEMAGIC_TESTS_HELPER_H
#define CODEMAGIC_TESTS_HELPER_H

#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <cassert>
#include <type_traits>

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

#endif // CODEMAGIC_TESTS_HELPER_H

