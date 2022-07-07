#ifndef SRC_RC4_HPP
#define SRC_RC4_HPP

#include "common.h"

namespace yama {
class RC4 {
   public:
    /**
     * Set/Reset the key use this method if you want to reuse the same ARC4 structure again
     * @param k the key
     * @param size the size of the key
     */
    void setKey(unsigned char* k, int size);
    /**
     * Encrypts a string
     * @param in String to encrypt
     * @param out String to decrypt
     * @param size size of the key to encrypt
     */
    void crypt(unsigned char* in, unsigned char* out, int size);
    /**
     * Encrypts a string
     * @param in String to encrypt
     * @param out String to decrypt
     * @param size size of the key to encrypt
     */
    void crypt(char* in, char* out, int size);
    RC4();

   protected:
    void ksa(unsigned char* key);
    void swap(unsigned char data[], int i, int j);
    void prga(unsigned char* plaintext, unsigned char* cipher, int size);
    void prga(char* plaintext, char* cipher, int size);
    unsigned char sbox[256];
    int sizeKey, prgaIndexA, prgaIndexB;
};

}  // namespace yama
#endif  // SRC_RC4_HPP
