/* AES Cipher Library
 * Adapted Mbed version of Neil Thiessen code:
 * Copyright (c) 2016 Neil Thiessen
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

//=====[#include guards - begin]===============================================
#ifndef AES_H
#define AES_H

//==================[Libraries]===============================================
#include "mbed.h"

//=====[Declaration of public data types]======================================

//=====[Declaration of public classes]=========================================

/**
 * @class AES
 * @brief AES block cipher implementation supporting ECB and CBC modes.
 * This class provides AES encryption and decryption functionality with support
 * for 128, 192, and 256-bit keys. It supports ECB and CBC cipher modes.
 */
class AES {
public:
//=====[Declaration of public methods]=========================================
    /**
     * @enum KeySize
     * @brief Enum for supported AES key lengths.
     */
    enum KeySize {
        KEY_128 = 4,    /**< 128-bit AES key */
        KEY_192 = 6,    /**< 192-bit AES key */
        KEY_256 = 8     /**< 256-bit AES key */
    };

    /**
     * @enum CipherMode
     * @brief Enum for supported cipher modes.
     */
    enum CipherMode {
        MODE_ECB,   /**< Electronic codebook */
        MODE_CBC    /**< Cipher block chaining */
    };

    /** 
     * @brief Default constructor. Initializes internal state to zero.
     */
    AES();

    /** Create an AES object with the specified parameters
     *
     * @param key Pointer to the AES key.
     * @param keySize The AES key size as a KeySize enum.
     * @param mode The cipher mode as a CipherMode enum (MODE_ECB by default).
     * @param iv Pointer to the 16B initialization vector (NULL by default).
     */
    AES(const char* key, KeySize keySize, CipherMode mode = MODE_ECB, const char* iv = nullptr);

    /**
     * @brief Destructor. Clears sensitive data.
     */
    ~AES();

    /** Set up this AES object for encryption/decryption with the specified parameters
     *
     * @param key Pointer to the AES key.
     * @param keySize The AES key size as a KeySize enum.
     * @param mode The cipher mode as a CipherMode enum (MODE_ECB by default).
     * @param iv Pointer to the 16B initialization vector (NULL by default).
     */
    void setup(const char* key, KeySize keySize, CipherMode mode = MODE_ECB, const char* iv = nullptr);

    /** Encrypt the specified data in-place, using CTS or zero-padding if necessary
     *
     * @param data Pointer to the data to encrypt (minimum 16B for output).
     * @param length The length of the data to encrypt in bytes.
     */
    void encrypt(void* data, size_t length);

    /** Encrypt the specified data, using CTS or zero-padding if necessary
     *
     * @param src Pointer to the data to encrypt.
     * @param dest Pointer to an array in which to store the encrypted data (minimum 16B).
     * @param length The length of the data to encrypt in bytes.
     */
    void encrypt(const void* src, char* dest, size_t length);

    /** Decrypt the specified data in-place, and remove the padding if necessary
     *
     * @param data Pointer to the data to decrypt (minimum 16B).
     * @param length The length of the decrypted data without padding in bytes.
     */
    void decrypt(void* data, size_t length);

    /** Decrypt the specified data, and remove the padding if necessary
     *
     * @param src Pointer to the data to decrypt (minimum 16B).
     * @param dest Pointer to an array in which to store the decrypted data.
     * @param length The length of the decrypted data without padding in bytes.
     */
    void decrypt(const char* src, void* dest, size_t length);

    /**
     * @brief Clears all sensitive internal data (key, state, carry vector).
     */
    void clear();

private:
//=====[Private constants]=================================================
    static const char m_Sbox[256];
    static const char m_InvSbox[256];
    static const unsigned int m_Rcon[10];

//=====[Declaration of privates atributes]=========================================
    CipherMode m_CipherMode;        /**< Current cipher mode (ECB or CBC) */
    int m_Rounds;                   /**< Number of AES rounds based on key size */
    unsigned int m_Key[60];         /**< Expanded key schedule */
    char m_State[16];               /**< 128-bit internal state buffer */
    char m_CarryVector[16];         /**< CBC mode carry vector (IV) */

//=====[Declaration of privates methods]=========================================
    /**
     * @brief Internal AES encryption routine on current state.
     */
    void aesEncrypt();

    /**
     * @brief Internal AES decryption routine on current state.
     */
    void aesDecrypt();

    /**
     * @brief Expands the input key into a key schedule.
     * @param key Pointer to the input key.
     * @param nk Number of 32-bit words in the key (4, 6, or 8).
     */
    void expandKey(const char* key, int nk);

    /**
     * @brief Rotates a 32-bit word left by 1 byte. 
     * @param w Word to rotate.
     * @return Rotated word.
     */
    unsigned int rotWord(unsigned int w);

  /**
     * @brief Rotates a 32-bit word right by 1 byte.
     * 
     * @param w Word to rotate.
     * @return Rotated word.
     */
    unsigned int invRotWord(unsigned int w);

    /**
     * @brief Applies S-box substitution to each byte in a word.
     * 
     * @param w Input word.
     * @return Substituted word.
     */
    unsigned int subWord(unsigned int w);

    /**
     * @brief Applies S-box substitution to entire state.
     */
    void subBytes();

    /**
     * @brief Applies inverse S-box substitution to entire state.
     */
    void invSubBytes();

    /**
     * @brief Performs row-wise shifting on the state.
     */
    void shiftRows();

    /**
     * @brief Performs inverse row-wise shifting on the state.
     */
    void invShiftRows();

    /**
     * @brief Galois field multiplication.
     * 
     * @param a First operand.
     * @param b Second operand.
     * @return Result of multiplication.
     */
    char gmul(char a, char b);

    /**
     * @brief Performs MixColumns transformation on a column.
     * 
     * @param r Pointer to the 4-byte column.
     */
    void mul(char* r);

    /**
     * @brief Performs inverse MixColumns transformation on a column.
     * 
     * @param r Pointer to the 4-byte column.
     */
    void invMul(char* r);

    /**
     * @brief Applies MixColumns to the entire state.
     */
    void mixColumns();

    /**
     * @brief Applies inverse MixColumns to the entire state.
     */
    void invMixColumns();

    /**
     * @brief Applies the round key to the state.
     * 
     * @param round Round number (0 to m_Rounds).
     */
    void addRoundKey(int round);
};

#endif //  _AES_H_
