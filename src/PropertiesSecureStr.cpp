/*********************************************************************************************
*	Project		: https://github.com/IgnacioPomar/libOS
*	Name		: PropertiesSecureStr.cpp
*	Description	:  store the configurable application like in java .properties
********************************************************************************************/

#include "PropertiesSecureStr.h"

#ifdef HAS_CRYPTOCPP
#include <cryptopp/modes.h>
#include <cryptopp/aes.h>
#include <cryptopp/base64.h>
#include <cryptopp/filters.h>
#else
#include "BuiltInEncryption.h"
#pragma message (__FILE__"() : warning CRYPT: No crypto++ detected. Using built In encryption method")
#endif 

//-------------------------------------------------------------------------------
std::string PropertySecureStr::encrypt (std::string in, std::string pass)
{
#ifdef HAS_CRYPTOCPP
	return aesEncode (in, pass);
#else
	return BuiltInEncryption::base64Encode (BuiltInEncryption::aesEncrypt (in, pass));
#endif
}

std::string PropertySecureStr::decrypt (std::string in, std::string pass)
{

#ifdef HAS_CRYPTOCPP
	return aesDecode (in, pass);
#else
	return BuiltInEncryption::aesDecrypt (BuiltInEncryption::base64Decode (in), pass);
#endif
}





#ifdef HAS_CRYPTOCPP
//-------------------------------------------------------------------------------
//We are using  Crypto++ AES to store the text

void fillKeyAndIv (std::string& key, std::string& iv, const std::string pass)
{
	//TODO. extract a number from the machine and the pass to create the following values
	key = "qwertyuiopasdfghjklzxcvbnmqwerty";
	iv = "0123456789123456";

}



std::string PropertySecureStr::aesEncode (const std::string& in, std::string pass)
{
	std::string key;
	std::string iv;
	fillKeyAndIv (key, iv, pass);


	std::string str_out;
	CryptoPP::CFB_Mode<CryptoPP::AES>::Encryption encryption ((CryptoPP::byte*) key.c_str (), key.length (), (CryptoPP::byte*) iv.c_str ());
	CryptoPP::StringSource encryptor (in, true,
									  new CryptoPP::StreamTransformationFilter (encryption,
																				new CryptoPP::Base64Encoder (
																					new CryptoPP::StringSink (str_out),
																					false // do not append a newline
																				)
									  )
	);
	return str_out;

	return std::string ();
}

std::string PropertySecureStr::aesDecode (const std::string& in, std::string pass)
{
	std::string key;
	std::string iv;
	fillKeyAndIv (key, iv, pass);

	std::string str_out;
	CryptoPP::CFB_Mode<CryptoPP::AES>::Decryption decryption ((CryptoPP::byte*) key.c_str (), key.length (), (CryptoPP::byte*) iv.c_str ());

	CryptoPP::StringSource decryptor (in, true,
									  new CryptoPP::Base64Decoder (
										  new CryptoPP::StreamTransformationFilter (decryption,
																					new CryptoPP::StringSink (str_out)
										  )
									  )
	);
	return str_out;
}

#endif
