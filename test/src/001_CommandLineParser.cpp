/*********************************************************************************************
*	Name        : 001_Historico.cpp
*  Description : Pruebas unitarias del funcionamiento del mantenimiento del historico
*	Copyright	(C) 2018  GRANTIA CAPITAL, SGIIC, S.A.
********************************************************************************************/

#include <string>
#include <TinyCppUnit/TinyCppUnit.h>

#include "CommandLineParser.h"

#define arrlen(arr)  sizeof (arr) / sizeof (void *)

//#define SHOW_HELP_FORMAT

UNIT_TEST_CASE (TestCommandLineParser)
{
#ifdef SHOW_HELP_FORMAT
	{
		CommandLineParser cmdLine;
		cmdLine.addOption ("long", "small", "Lorem ipsum dolor sit amet", true, 0);
		cmdLine.addOption ("s", "longlong", "consectetur adipiscing elit");
		cmdLine.addOption ("", "normal", "sed do eiusmod tempor incididunt ut labore et dolore magna aliqua");
		cmdLine.addOption ("a", "", "Ut enim ad minim veniam");

		const char * argv[] = { __FILE__ };
		cmdLine.parseOrHelp (arrlen (argv), argv);
	}
#endif // SHOW_HELP_FORMAT

	//Check for mandatory params
	{
		CommandLineParser cmdLine;
		cmdLine.addOption ("f", "first", "Forced parameter", true, 0);
		cmdLine.addOption ("s", "second", "Forced parameter two atributes", true, 2);
		cmdLine.addOption ("t", "third", "Forced parameter unlimited atributes", true, -1);
		cmdLine.addOption ("fo", "fourth", "optional parameter", false);
		cmdLine.addOption ("fi", "fifth", "optional parameter (fifth)", false);


		const char * argvSuccess[] = { "exe", "-f", "/t", "something" , "--fi", "--second=a,b" };
		UNIT_CHECK (cmdLine.parse (arrlen (argvSuccess), argvSuccess));

		cmdLine.reset ();
		const char * argvFail_1[] = { "exe" };
		UNIT_CHECK (!cmdLine.parse (arrlen (argvFail_1), argvFail_1));

		cmdLine.reset ();
		const char * argvFail_2[] = { "exe", "-t=a" };
		UNIT_CHECK (!cmdLine.parse (arrlen (argvFail_1), argvFail_1));

		cmdLine.reset ();
		const char * argvFail_3[] = { "exe", "-t=a", "-f", "-s" };
		UNIT_CHECK (!cmdLine.parse (arrlen (argvFail_1), argvFail_1));
	}



	//Retrieve options and values
	{
		CommandLineParser cmdLine;
		cmdLine.addOption ("a", "", "set");
		cmdLine.addOption ("b", "", "not set");
		cmdLine.addOption ("c", "", "", false, 2);
		cmdLine.addOption ("d", "", "", false, 1);
		cmdLine.addOption ("e", "", "", false, -1);

		const char * argv[] = { "execName", "-a", "-e", "val1", "val2", "val3", "-d", "val1", "extra1", "--c=val1,val2", "extra2" , "extra3" };
		UNIT_CHECK (cmdLine.parse (arrlen (argv), argv));

		UNIT_CHECK (cmdLine.hasOption ("a"));
		UNIT_CHECK (!cmdLine.hasOption ("b"));

		UNIT_CHECK (2 == cmdLine.getNumOptionValues ("c"));
		UNIT_CHECK (1 == cmdLine.getNumOptionValues ("d"));
		UNIT_CHECK (3 == cmdLine.getNumOptionValues ("e"));
		UNIT_CHECK (3 == cmdLine.getNumNameless ());

		std::string val1 ("val1");
		std::string val2 ("val2");
		std::string val3 ("val3");
		std::string extra1 ("extra1");
		std::string extra2 ("extra2");
		std::string extra3 ("extra3");


		UNIT_CHECK (0 == val1.compare (cmdLine.getOptionValue ("c")));
		UNIT_CHECK (0 == val1.compare (cmdLine.getOptionValue ("d")));
		UNIT_CHECK (0 == val1.compare (cmdLine.getOptionValue ("e")));

		UNIT_CHECK (0 == val2.compare (cmdLine.getOptionValue ("c", 1)));
		UNIT_CHECK (0 == val2.compare (cmdLine.getOptionValue ("e", 1)));

		UNIT_CHECK (0 == val3.compare (cmdLine.getOptionValue ("e", 2)));

		UNIT_CHECK (0 == extra1.compare (cmdLine.getNamelessValue ()));
		UNIT_CHECK (0 == extra2.compare (cmdLine.getNamelessValue (1)));
		UNIT_CHECK (0 == extra3.compare (cmdLine.getNamelessValue (2)));

	}



}
