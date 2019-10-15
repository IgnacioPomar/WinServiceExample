/*********************************************************************************************
*	Name        : 002_Properties.cpp
*  Description :
********************************************************************************************/



#include <TinyCppUnit/TinyCppUnit.h>
#include <string>
#include "Properties.h"



constexpr const char * VALOR_STR = "Valor en formato str";

UNIT_TEST_CASE (PropertyConfigurationTest)
{
	Properties props;

	props.load ("../test/data/test.properties");
	std::string path = props.getStr ("path", "");

	UNIT_CHECK (path.compare ("c:\\wiki\\templates") == 0);



	//----- Direct assign -----
	UNIT_CHECK (props.getInt ("key", 5) == 5);
	props.setInt ("key", 10);
	UNIT_CHECK (props.getInt ("key", 5) == 10);

	//----- strings -----
	props.setStr ("keyStr", VALOR_STR);
	props.setSecureStr ("Securekey", VALOR_STR, "1234567");

	std::string str = props.getStr ("keyStr", "XX");
	UNIT_CHECK (str.compare (VALOR_STR) == 0);


	str = props.getStr ("Securekey", "XX");
	UNIT_CHECK (str.compare (VALOR_STR) != 0); //It should be different

	props.getSecureStr ("Securekey", "XX", "1234567", str);
	UNIT_CHECK (str.compare (VALOR_STR) == 0);


	//Comment/descommet to check save
	props.save ("../test/dumps/tmp.properties");




	//----- Bool Type -----
	props.setBool ("bool", true);
	UNIT_CHECK (props.getBool ("bool", false));
	UNIT_CHECK (props.getBool ("nothing", true));

	props.setStr ("bool", "tRuE");
	UNIT_CHECK (props.getBool ("bool", false));

	props.setStr ("bool", "false");
	UNIT_CHECK (!props.getBool ("bool", true));
	props.setStr ("bool", "fAlsE");
	UNIT_CHECK (!props.getBool ("bool", true));
	props.setStr ("bool", "notBoolean");
	UNIT_CHECK (!props.getBool ("bool", true));

}
