/*********************************************************************************************
*	Name		: PropertiesSecureStr.h
*	Description	:  store the configurable application like in java .properties
********************************************************************************************/


#pragma once
#ifndef _PROPERTY_SECURE_STR_
#define _PROPERTY_SECURE_STR_

//TODO: add support for https://botan.randombit.net/ 
#ifdef __has_include
#  if __has_include(<cryptopp/aes.h>) // crypto++ exists
#    define HAS_CRYPTOCPP
#  endif
#endif


#include <string>

#include "libOS_cfg.h"

class LIBOS_LOCAL PropertySecureStr
{
public:
#ifdef HAS_CRYPTOCPP
	static std::string aesEncode (const std::string &in, std::string pass);
	static std::string aesDecode (const std::string &in, std::string pass);
#else
	static std::string base64Encode (const std::string &in);
	static std::string base64Decode (const std::string &in);

	static std::string aesEncrypt (const std::string &in, std::string pass);
	static std::string aesDecrypt (const std::string &in, std::string pass);
#endif
	static std::string encrypt (std::string in, std::string pass);
	static std::string decrypt (std::string in, std::string pass);
};



#ifndef HAS_CRYPTOCPP

constexpr int KEYBITS = 256;
constexpr int AES_BLOCK_SIZE = 16;

int rijndaelSetupEncrypt (unsigned long *rk, const unsigned char *key, int keybits);
int rijndaelSetupDecrypt (unsigned long *rk, const unsigned char *key, int keybits);
void rijndaelEncrypt (const unsigned long *rk, int nrounds, const unsigned char plaintext[16], unsigned char ciphertext[16]);
void rijndaelDecrypt (const unsigned long *rk, int nrounds, const unsigned char ciphertext[16], unsigned char plaintext[16]);

#define KEYLENGTH(keybits) ((keybits)/8)
#define RKLENGTH(keybits)  ((keybits)/8+28)
#define NROUNDS(keybits)   ((keybits)/32+6)

#endif //HAS_CRYPTOCPP

#endif //_PROPERTY_SECURE_STR_
