/*********************************************************************************************
*	Name		: SourceBuiltInEncryption_Base64.cpp
*	Description	: Base64 algoritm in case crypto++  not found
*	Based On	: https://stackoverflow.com/a/34571089/74785
*	License		: CC BY-SA 3.0 (https://creativecommons.org/licenses/by-sa/3.0/)
*	Author		: Original - Manuel Martinez (The original code has been modified)
********************************************************************************************/

//Added support for the useBase64url as specified in RFC 4648

#include <vector>
#include "BuiltInEncryption.h"
//-------------------------------------------------------------------------------
// Builtin implementation of a base64 encode/decode algorithm

constexpr const char * base64Alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";// (pad) =
constexpr const char * base64urlAlphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";// (pad) =

/*
 Lookup table compatible for both alphabets.

 Generated with:
		signed char B64index[256];
		for (signed char & x : B64index) x = -1;
		for (int i = 0; i < 64; i++) B64index[base64Alphabet[i]] = i;
		for (int i = 62; i < 64; i++) B64index[base64urlAlphabet[i]] = i;

		int i = 0;const char * sep = "";
		for (int x : B64index)	{printf ("%s%d", sep, x); if (++i == 32) {i = 0;sep = ", \n";} else sep = ", ";}
*/

constexpr signed char B64index[256] = {
-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 62, -1, 62, -1, 63, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -1, -1, -1, -1, -1, -1,
-1,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1, 63,
-1, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, -1, -1, -1, -1, -1,
-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
};


/**
* Encodes in base64 the passed string
* \param    [in]   in
* \return
*/
std::string BuiltInEncryption::base64Encode (const std::string & in, bool useBase64url)
{
	std::string out;

	const char * alphabet = (useBase64url) ? base64urlAlphabet : base64Alphabet;

	int val = 0, valb = -6;
	for (unsigned char c : in)
	{
		val = (val << 8) + c;
		valb += 8;
		while (valb >= 0)
		{
			out.push_back (alphabet[(val >> valb) & 0x3F]);
			valb -= 6;
		}
	}

	if (valb > -6)
	{
		out.push_back (alphabet[((val << 8) >> (valb + 8)) & 0x3F]);
	}

	// We can be skipp the padding as stated in the section 3.2.
	if (!useBase64url)
	{
		while (out.size () % 4) out.push_back ('=');
	}

	return out;
}

/**
* Encodes in base64 the passed string
* \param    [in]   in
* \return
*/
std::string BuiltInEncryption::base64Decode (const std::string & in)
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
