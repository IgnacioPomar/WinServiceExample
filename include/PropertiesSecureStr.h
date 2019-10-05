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
#endif
	static std::string encrypt (std::string in, std::string pass);
	static std::string decrypt (std::string in, std::string pass);
};



#endif //_PROPERTY_SECURE_STR_
