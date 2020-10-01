/*********************************************************************************************
*	Name		: SourceBuiltInEncryption_Base64.cpp
*	Description	: Base64 algoritm in case crypto++  not found
*	Based On	: https://stackoverflow.com/a/34571089/74785
*	License		: CC BY-SA 3.0 (https://creativecommons.org/licenses/by-sa/3.0/)
*	Author		: Original - Manuel Martinez (The original code has been modified)
********************************************************************************************/

#include <vector>
#include "BuiltInEncryption.h"
//-------------------------------------------------------------------------------
// Builtin implementation of a base64 encode/decode algorithm

static const std::string B64chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";//=
/**
* Encodes in base64 the passed string
* \param    [in]   in
* \return
*/
std::string BuiltInEncryption::base64Encode (const std::string & in)
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
