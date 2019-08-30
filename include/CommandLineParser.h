/*********************************************************************************************
*	Name		: CommandLineParser.h
*	Description	: parse command line arguments inspired in apache commons cli
********************************************************************************************/

#pragma once
#ifndef _COMMAND_LINE_PARSER_H_
#define _COMMAND_LINE_PARSER_H_

#include "libOS_cfg.h"

/*
Currently suports the following formats
Windows like options (ie. dir /a c:\path)
GNU like long options (ie. du --human-readable --max-depth=1)
long options with single hyphen (ie. ant -projecthelp)


//YAGNI: may be in the future suport for this formats
POSIX like options (ie. tar -zxvf foo.tar.gz)
Short options with value attached (ie. gcc -O2 foo.c)
*/




class CommandLineParserPrivateData;
class Option;

class LIBOS_API CommandLineParser
{
public:
	CommandLineParser ();
	~CommandLineParser ();

	void addOption (const char * opt, const char * longOpt, const char * description, bool isRequired = false, int args = 0);

	void reset ();

	bool parse (int argc, const char * argv[]);
	void printHelp ();
	bool parseOrHelp (int argc, const char * argv[]);

	bool hasOption (const char * opt);
	int getNumOptionValues (const char * opt);
	const char * getOptionValue (const char * opt, int pos = 0);

	int getNumNameless ();
	const char * getNamelessValue (int pos = 0);

private:
	CommandLineParserPrivateData * pd;

	static constexpr const char VALUE_SEPARATOR = ',';

	bool checkOptions ();
	Option * parseOpt (const char * cleanopt);

};



#endif //_COMMAND_LINE_PARSER_H_
