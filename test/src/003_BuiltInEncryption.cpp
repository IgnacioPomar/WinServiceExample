/*********************************************************************************************
*	Name        : 002_Properties.cpp
*  Description :
********************************************************************************************/



#include <TinyCppUnit/TinyCppUnit.h>
#include <string>

#include "BuiltInEncryption.h"

struct TestConversionEqivalences
{
	std::string original;
	std::string b64Std;
	std::string b64Url;
};


TestConversionEqivalences equivalences[] = {
	//First, all posibilities of padding
	{"f",  "Zg==","Zg"},
	{"fo", "Zm8=","Zm8"},
	{"foo","Zm9v","Zm9v"},

	//Add another byte
	{"foob",  "Zm9vYg==","Zm9vYg"},
	{"fooba", "Zm9vYmE=","Zm9vYmE"},
	{"foobar","Zm9vYmFy","Zm9vYmFy"},


	// data using last two characters (difference between std and url)
	{"\x83" "\xef" "\xff" "\xfa", "g+//+g==", "g-__-g" }
};


UNIT_TEST_CASE (BuiltInEncryption_BAse64_Test)
{

	bool base64Endode = true;
	bool base64EndodeUrl = true;
	bool base64Decode = true;
	bool base64DecodeUrl = true;

	for (const TestConversionEqivalences& e : equivalences)
	{
		std::string toBase64;
		std::string toBase64Url;

		std::string fromBase64;
		std::string fromBase64Url;


		toBase64 = BuiltInEncryption::base64Encode (e.original, false);
		toBase64Url = BuiltInEncryption::base64Encode (e.original, true);

		fromBase64 = BuiltInEncryption::base64Decode (e.b64Std);
		fromBase64Url = BuiltInEncryption::base64Decode (e.b64Url);

		base64Endode &= 0 == toBase64.compare (e.b64Std);
		base64EndodeUrl &= 0 == toBase64Url.compare (e.b64Url);
		base64Decode &= 0 == fromBase64.compare (e.original);
		base64DecodeUrl &= 0 == fromBase64Url.compare (e.original);
	}

	//Prefer only one check for the whole set
	UNIT_CHECK (base64Endode);
	UNIT_CHECK (base64EndodeUrl);
	UNIT_CHECK (base64Decode);
	UNIT_CHECK (base64DecodeUrl);


}
