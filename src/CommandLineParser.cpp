/*********************************************************************************************
*	Name		: CommandLineParser.cpp
*	Description	: parse command line arguments
********************************************************************************************/
#include <map>
#include <vector>
#include <list>
#include <string>

#include <iostream> //only for printHelp ()
#include <iomanip>  //for std::setw

#include "CommandLineParser.h"


/**
 * Local class: contains one option
 */
class LIBOS_LOCAL Option
{
public:

	//Constructor
	Option (const char * opt, const char * longOpt, const char * description, bool isRequired, int args);

	//only for the STL map interface
	Option () {};

	//definition
	std::string opt;
	std::string longOpt;
	std::string description;
	bool isRequired;
	int args; //0, non; -1 unlimited; else, fixed number

	void reset ();
	bool isFilled (bool retvalForUnlimited);

	//Internal use
	bool isSet;
	int numValues;
	std::vector<std::string> values;
};


/**
 * Constructor. Init the option
 */
Option::Option (const char * opt, const char * longOpt, const char * description, bool isRequired, int args) :
	opt (opt), longOpt (longOpt), description (description), isRequired (isRequired), args (args)
{
	reset ();
}

/**
 * Terter prupose: Clears the parse efect.
 */
void Option::reset ()
{
	isSet = false;
	numValues = 0;
	values.empty ();
}

/**
 * Check if the option is filled
 *
 * \param [in] retvalForUnlimited         if Option::args != -1 is ignored.
 *                                        Else, is the return value:
 *                                             false to allow more parameters
 *                                             true to show that is filled
 * \return true: the option  is filled
 *        false: not filled, there are values left
 */
bool Option::isFilled (bool retvalForUnlimited)
{
	if (args == 0)
	{
		return true;
	}
	else if (args > 0)
	{
		return args == values.size ();
	}
	else
	{
		//YAGNI: create a -2 to force at least one parameter?
		return retvalForUnlimited;
	}
}


typedef std::map<std::string, Option*> mapOpcs;


/**
 * Local class: black box of the private data for CommandLineParser
 */
class LIBOS_LOCAL CommandLineParserPrivateData
{
public:
	const char * argv_0 = "";
	bool hasNotExpectedOpts = false;
	std::list<Option> options;
	mapOpcs shortOptAccess;
	mapOpcs longOptAccess;
	std::vector<std::string> namelessValues;

	Option* findOpt (std::string opt);
};


/**
 * Search by long/short name the option
 *
 * \param [in] opt    option name. May be long or short
 * \return pointer to the parsed option
 */
Option * CommandLineParserPrivateData::findOpt (std::string opt)
{
	mapOpcs::iterator it = shortOptAccess.find (opt);
	if (it != shortOptAccess.end ())
	{
		return it->second;
	}

	it = longOptAccess.find (opt);
	if (it != longOptAccess.end ())
	{
		return it->second;
	}

	return nullptr;
}



/**
 * Constructor: create the private data object
 */
CommandLineParser::CommandLineParser ()
{
	this->pd = new CommandLineParserPrivateData ();
}


/**
 * Destructor: free the memory
 */
CommandLineParser::~CommandLineParser ()
{
	delete this->pd;
}



/**
 * Add a posible option to the parser
 *
 * \param [in] opt           Short name for the option
 * \param [in] longOpt       Long  name for the option
 * \param [in] description   Description, only to show usage
 * \param [in] isRequired    This option must be set or the parse will fail
 * \param [in] args          Number of value it accepts de option:
 *                              -1   Unllimited values
 *                               0   none aditional value
 *                               n   If a positive number, it will accep that as fixed number of parameters
 */
void CommandLineParser::addOption (const char * opt, const char * longOpt, const char * description, bool isRequired, int args)
{
	Option option (opt, longOpt, description, isRequired, args);

	pd->options.push_back (option);
	Option * optionStlPointer = &pd->options.back ();

	pd->shortOptAccess[option.opt] = optionStlPointer;
	pd->longOptAccess[option.longOpt] = optionStlPointer;
}


/**
 * Terter prupose: Clears the parse efect.
 */
void CommandLineParser::reset ()
{
	pd->namelessValues.empty ();
	pd->hasNotExpectedOpts = false;
	for (auto& opts : pd->options)
	{
		opts.reset ();
	}
}


/**
 * Parse the command line
 *
 * \param [in] argc    Number of arguments
 * \param [in] argv    Comand line arguments
 * \return true: the parse was succesful
 *        false: error parsing arguments (possible lack of parameters)
 */
bool CommandLineParser::parse (int argc, const char * argv[])
{
	if (argc < 1)
	{
		return checkOptions ();
	}
	pd->argv_0 = argv[0];

	for (int i = 1; i < argc; i++)
	{
		Option * currOpt = nullptr;
		const char * cmd = argv[i];
		bool isOpt = false;
		int offset = 0;

		//check if is new option
		if (cmd[0] == '/')
		{
			isOpt = true;
			offset = 1;
		}
		else if (cmd[0] == '-')
		{
			isOpt = true;
			offset = 1;
			if (cmd[offset] == '-') offset++;
		}

		if (!isOpt)
		{
			if (currOpt == nullptr)
			{
				pd->namelessValues.push_back (cmd);
			}
			else
			{
				currOpt->values.push_back (cmd);
			}
		}
		else
		{
			currOpt = parseOpt (&cmd[offset]);
		}

		if (currOpt != nullptr)
		{
			if (currOpt->isFilled (false))
			{
				currOpt = nullptr;
			}
		}
	}

	return checkOptions ();
}

/**
 * A single option. Also Check if has values inside
 *
 * \param [in] cleanopt    option name without special chars
 * \return pointer to the parsed option
 */
Option * CommandLineParser::parseOpt (const char * cleanopt)
{
	std::string cleanCmd = cleanopt;
	std::string vals;
	bool hasVals = false;
	Option * retVal = nullptr;

	std::string::size_type pos = cleanCmd.find ('=');

	if (pos != std::string::npos)
	{
		hasVals = true;
		vals = cleanCmd.substr (pos + 1);
		cleanCmd = cleanCmd.substr (0, pos);
	}


	retVal = pd->findOpt (cleanCmd);
	if (retVal != nullptr)
	{
		retVal->isSet = true;

		if (hasVals)
		{
			size_t pos = 0;
			std::string token;
			while ((pos = vals.find (VALUE_SEPARATOR)) != std::string::npos)
			{
				token = vals.substr (0, pos);
				vals.erase (0, pos + 1); //token size plus Value separator size

				retVal->values.push_back (token);
			}
			retVal->values.push_back (token);
			retVal = nullptr;
		}
	}
	else
	{
		pd->hasNotExpectedOpts = true;
	}

	return retVal;
}



/**
 * Check if all the mandatory options are filled
 *
 * \return true: all ok
 *        false: possible lack of options
 */
bool CommandLineParser::checkOptions ()
{
	for (auto& opts : pd->options)
	{
		Option &opt = opts;

		if (opt.isRequired && !opt.isSet)
		{
			return false;
		}

		if (opt.isSet && !opt.isFilled (true))
		{
			return false;
		}
	}
	return true;
}


/**
 * Print how to use the command
 */
void CommandLineParser::printHelp ()
{
	std::cout << "usage: " << pd->argv_0 << std::endl;

	//To prety format, we search the max length
	size_t nameLen = 0;
	size_t longNameLen = 0;
	for (auto& opts : pd->options)
	{
		Option &opt = opts;
		if (opt.opt.size () > nameLen)
		{
			nameLen = opt.opt.size ();
		}
		if (opt.longOpt.size () > longNameLen)
		{
			longNameLen = opt.longOpt.size ();
		}
	}

	//Now, we print
	for (auto& opts : pd->options)
	{
		Option &opt = opts;

		//Opt name
		size_t optLen = opt.opt.size ();
		if (optLen == 0)
		{
			std::setw (nameLen + 3); //one for space and one for ,
		}
		else
		{
			std::cout << std::setw (nameLen - optLen) << " -" << opt.opt << ",";
		}

		//Opt long name
		optLen = opt.longOpt.size ();
		if (optLen == 0)
		{
			std::setw (longNameLen + 5); //for tab plus --
		}
		else
		{
			std::cout << "--" << opt.opt << "   " << std::setw (longNameLen - optLen);
		}

		//description
		std::cout << opt.description << std::endl;
	}
}


/**
 * Parse the command line. if it fails, show the help
 *
 * \see CommandLineParser::parse
 *
 * \param [in] argc    Number of arguments
 * \param [in] argv    Comand line arguments
 * \return true: the parse was succesful
 *        false: error parsing arguments (possible lack of parameters)
 */
bool CommandLineParser::parseOrHelp (int argc, const char * argv[])
{
	if (parse (argc, argv))
	{
		return true;
	}
	else
	{
		printHelp ();
		return false;
	}
}



/**
 * Check if the specified option has been set
 *
  * \param [in] opt    Short or long name of the argument
 * \return true or false
 */
bool CommandLineParser::hasOption (const char * opt)
{
	Option * pOpc = pd->findOpt (opt);
	if (pOpc != nullptr)
	{
		return pOpc->isSet;
	}
	else return false;
}


/**
 * Get the number of values it contains the specified option
 *
 * \param [in] opt    Short or long name of the argument
 * \return number of values
 */
int CommandLineParser::getNumOptionValues (const char * opt)
{
	Option * pOpc = pd->findOpt (opt);
	if (pOpc != nullptr)
	{
		return (int)pOpc->values.size ();
	}
	else return 0;
}


/**
 * Get the specified value of the option
 *
 * \param [in] opt    Short or long name of the argument
 * \param [in] pos    position of the value
 * \return nullptr in error case, the value otherwise
 */
const char * CommandLineParser::getOptionValue (const char * opt, int pos)
{
	Option * pOpc = pd->findOpt (opt);
	if (pOpc != nullptr)
	{
		if ((size_t)pos < pOpc->values.size ())
		{
			return pOpc->values.at (pos).c_str ();
		}
		else return nullptr;
	}
	else return nullptr;
}

/**
 * Get the number of nameless values (not inside option)
 *
 * \return number of values
 */

int CommandLineParser::getNumNameless ()
{
	return (int)pd->namelessValues.size ();
}

/**
 * Get the specified nameless value (not inside option)
 *
 * \param [in] pos    position of the value
 * \return nullptr in error case, the value otherwise
 */
const char * CommandLineParser::getNamelessValue (int pos)
{
	if ((size_t)pos < pd->namelessValues.size ())
	{
		return pd->namelessValues.at (pos).c_str ();
	}
	else return nullptr;
}
