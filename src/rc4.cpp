#include "rc4.hpp"

namespace yama {

void RC4::prga(unsigned char* lpSrcData, unsigned char* cipher, int size) {
    for (int k = 0; k < size; k++) {
        prgaIndexA = (prgaIndexA + 1) % 256;
        prgaIndexB = (prgaIndexB + sbox[prgaIndexA]) % 256;
        swap(sbox, prgaIndexA, prgaIndexB);
        cipher[k] = sbox[(sbox[prgaIndexA] + sbox[prgaIndexB]) % 256] ^ lpSrcData[k];
    }
}

void RC4::prga(char* lpSrcData, char* cipher, int size) {
    prga((unsigned char*)lpSrcData, (unsigned char*)cipher, size);
}

void RC4::crypt(char* lpSrcData, char* lpDstData, int size) { prga(lpSrcData, lpDstData, size); }
void RC4::crypt(unsigned char* lpSrcData, unsigned char* lpDstData, int size) {
    prga(lpSrcData, lpDstData, size);
}
RC4::RC4() {}
void RC4::swap(unsigned char data[], int i, int j) {
    unsigned char temp = data[i];
    data[i] = data[j];
    data[j] = temp;
}
void RC4::ksa(unsigned char* key) {
    int j = 0;
    for (int i = 0; i < 256; i++) {
        j = (j + sbox[i] + key[i % sizeKey]) % 256;
        swap(sbox, i, j);
    }
}
void RC4::setKey(unsigned char k[], int size) {
    prgaIndexA = 0;
    prgaIndexB = 0;
    sizeKey = size;
    for (int i = 0; i < 256; i++) {
        sbox[i] = (unsigned char)i;
    }
    ksa(k);
}

}  // namespace yama