/*********************************************************************************************
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
#include <vector>
#pragma message (__FILE__"() : warning CRYPT: No crypto++ detected: Properties::setSecureStr will use only base64")
#endif 

//-------------------------------------------------------------------------------
std::string PropertySecureStr::encrypt (std::string in, std::string pass)
{
#ifdef HAS_CRYPTOCPP
	return aesEncode (in, pass);
#else
	return base64Encode (in);
#endif
}

std::string PropertySecureStr::decrypt (std::string in, std::string pass)
{

#ifdef HAS_CRYPTOCPP
	return aesDecode (in, pass);
#else
	return base64Decode (in);
#endif
}





#ifdef HAS_CRYPTOCPP
//-------------------------------------------------------------------------------
//We are using  Crypto++ AES to store the text

void fillKeyAndIv (std::string &key, std::string &iv, const std::string pass)
{
	//TODO. extract a number from the machine and the pass to create the following values
	key = "qwertyuiopasdfghjklzxcvbnmqwerty";
	iv = "0123456789123456";

}



std::string PropertySecureStr::aesEncode (const std::string & in, std::string pass)
{
	std::string key;
	std::string iv;
	fillKeyAndIv (key, iv, pass);


	std::string str_out;
	CryptoPP::CFB_Mode<CryptoPP::AES>::Encryption encryption ((CryptoPP::byte*)key.c_str (), key.length (), (CryptoPP::byte*)iv.c_str ());
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

std::string PropertySecureStr::aesDecode (const std::string & in, std::string pass)
{
	std::string key;
	std::string iv;
	fillKeyAndIv (key, iv, pass);

	std::string str_out;
	CryptoPP::CFB_Mode<CryptoPP::AES>::Decryption decryption ((CryptoPP::byte*)key.c_str (), key.length (), (CryptoPP::byte*)iv.c_str ());

	CryptoPP::StringSource decryptor (in, true,
		new CryptoPP::Base64Decoder (
			new CryptoPP::StreamTransformationFilter (decryption,
				new CryptoPP::StringSink (str_out)
			)
		)
	);
	return str_out;
}

#else
//-------------------------------------------------------------------------------
//If we havent a proper cryptografy library, then... we use a basic base64 encode

static const std::string B64chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";//=
/**
* Encodes in base64 the passed string
* \param    [in]   in
* \return
*/
std::string PropertySecureStr::base64Encode (const std::string & in)
{
	std::string out;

	int val = 0, valb = -6;
	for (unsigned char c : in) {
		val = (val << 8) + c;
		valb += 8;
		while (valb >= 0) {
			out.push_back (B64chars[(val >> valb) & 0x3F]);
			valb -= 6;
		}
	}
	if (valb > -6) out.push_back (B64chars[((val << 8) >> (valb + 8)) & 0x3F]);
	while (out.size () % 4) out.push_back ('=');
	return out;
}


/**
* creates the index to read the base64chars
* \return
*/
std::vector<int> makeB64index ()
{
	std::vector<int> B64index (256, -1);
	for (int i = 0; i < 64; i++) B64index[B64chars[i]] = i;
	return B64index;
}


static const std::vector<int> B64index = makeB64index ();
/**
* Encodes in base64 the passed string
* \param    [in]   in
* \return
*/
std::string PropertySecureStr::base64Decode (const std::string & in)
{
	std::string out;
	int val = 0, valb = -8;
	for (unsigned char c : in) {
		if (B64index[c] == -1) break;
		val = (val << 6) + B64index[c];
		valb += 6;
		if (valb >= 0) {
			out.push_back (char ((val >> valb) & 0xFF));
			valb -= 8;
		}
	}
	return out;
}

#endif
