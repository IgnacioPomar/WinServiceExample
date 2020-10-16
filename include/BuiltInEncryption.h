/*********************************************************************************************
*	Name		: BuiltInEncryption.h
*	Description	: Encryption algoritms in case crypto++  not found
********************************************************************************************/


#pragma once
#ifndef _BUILT_IN_ENCRYPTION_H_
#define _BUILT_IN_ENCRYPTION_H_

#include <string>
#include "libOS_cfg.h"


typedef unsigned long u32;
typedef unsigned char u8;


#define KEYLENGTH(keybits) ((keybits)/8)
#define RKLENGTH(keybits)  ((keybits)/8+28)
#define NROUNDS(keybits)   ((keybits)/32+6)


class LIBOS_API BuiltInEncryption
{
public:
	static constexpr int KEYBITS = 256;
	static constexpr int AES_BLOCK_SIZE = 16;

	static std::string base64Encode (const std::string &in, bool useBase64url = false);
	static std::string base64Decode (const std::string &in);

	static std::string aesEncrypt (const std::string &in, std::string &pass);
	static std::string aesDecrypt (const std::string &in, std::string &pass);
private:
	static void fillKeyFromPass (u8 * key, int keySize, std::string &pass);

	static int rijndaelSetupEncrypt (u32 *rk, const u8 *key, int keybits);
	static int rijndaelSetupDecrypt (u32 *rk, const u8 *key, int keybits);
	static void rijndaelEncrypt (const u32 *rk, int nrounds, const u8 plaintext[AES_BLOCK_SIZE], u8 ciphertext[AES_BLOCK_SIZE]);
	static void rijndaelDecrypt (const u32 *rk, int nrounds, const u8 ciphertext[AES_BLOCK_SIZE], u8 plaintext[AES_BLOCK_SIZE]);


};



#endif //_BUILT_IN_ENCRYPTION_H_
