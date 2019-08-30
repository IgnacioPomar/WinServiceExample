/*********************************************************************************************
*	Name        : 001_Historico.cpp
*  Description : Pruebas unitarias del funcionamiento del mantenimiento del historico
*	Copyright	(C) 2018  GRANTIA CAPITAL, SGIIC, S.A.
********************************************************************************************/

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


		const char * argvSuccess[] = { "exe", "-f", "--second=a,b", "/t", "something" , "--fi" };
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

		//

	}
	/*
	//Windows like options (ie.dir /a c:\path)
	{
		CommandLineParser cmdLine;
		cmdLine.addOption ("q", "owner", "show the file Owner");

		const char * argv[] = {"exe", "/q", "c:\\path"};
		cmdLine.parse (3, argv);


		UNIT_CHECK (cmdLine.hasOption("q"));

	}
	*/



	/*
Windows like options (ie. dir /a c:\path)
POSIX like options (ie. tar -zxvf foo.tar.gz)
GNU like long options (ie. du --human-readable --max-depth=1)
Short options with value attached (ie. gcc -O2 foo.c)
long options with single hyphen (ie. ant -projecthelp)
*/




}
