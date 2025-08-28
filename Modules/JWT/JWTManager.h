//=====[#include guards - begin]===============================================

#ifndef _JWT_MANAGER_H_
#define _JWT_MANAGER_H_

#include "arm_book_lib.h"
#include "mbed.h"
#include "Non_Blocking_Delay.h"
#include "string.h"
#include <CustomJWT.h>


//=====[Declaration of public defines]=========================================
#define JWT_KEY "a-string-secret-at-least-256-bits-long"
#define JWT_KEY_LEN (sizeof(JWT_KEY) - 1)


//=====[Declaration of public data types]======================================

//=====[Declaration of public classes]=========================
/**
 * @class JWTManager
 * @brief Facade for encoding and decoding JSON Web Tokens (JWT).
 * @details Simplifies the interaction with the CustomJWT class by providing
 * a minimal interface to encode and decode tokens. It handles memory management,
 * signing, verification, and error reporting internally. This class implements
 * the Facade design pattern.
 */
class JWTManager {
public:
//=====[Declaration of public methods]=========================================
    /**
     * @brief Construct a new JWTManager object
     */
    JWTManager ();

    /**
     * @brief Destroy the JWTManager object
     */
    virtual ~JWTManager ();

    /**
     * @brief Encodes a JSON payload into a JWT string.
     * @param payloadToJWT Pointer to the input JSON payload string.
     * @param jwtEncoded Pointer to the output buffer to store the encoded JWT.
     */
    void encodeJWT(char * payloadToJWT, char * jwtEncoded);

    /**
     * @brief Decodes a JWT string and extracts the payload.
     * @param jwtToDecode Pointer to the JWT string to decode.
     * @param payloadRetrived Pointer to the buffer where the decoded payload will be stored.
     * @return true if decoding and verification were successful, false otherwise.
     */
    bool decodeJWT (char * jwtToDecode, char * payloadRetrived);

private:
//=====[Private Attributes]================================================
    CustomJWT * jwt;  ///< Pointer to the underlying JWT implementation
    char JWTKey [JWT_KEY_LEN + 1] = JWT_KEY; ///< Secret key for signing and verifying tokens
//=====[Declaration of private methods]=========================================
};


//=====[#include guards - end]=================================================

#endif // _JWT_MANAGER_H_
