/*********************************************************************************************
*	Name		: PropertyConfiguration.cpp
*	Description	:  store the configurable application like in java .properties
********************************************************************************************/

#define _CRT_SECURE_NO_WARNINGS

#include <list>
#include <unordered_map>
#include <string>
#include <iostream>

#include <fstream>

#include <algorithm> // May to min in getBool
#include <sstream> //parse string to bool in getBool

#include "Properties.h"


static constexpr const char * SPACE_CHARS = " \t";
static constexpr const char * COMMENT_CHARS = "#!";
static constexpr const char * EQUAL_CHARS = ":=";

class LIBOS_LOCAL PropLine
{
public:
	std::string key;
	std::string value;
	std::string comment;

	PropLine (std::string &key, std::string &value, std::string &comment);
	friend std::ostream & operator << (std::ostream &out, const PropLine &line);
};

typedef std::unordered_map <std::string, std::string * > Values;
typedef std::list <PropLine> Lines;


PropLine::PropLine (std::string & key, std::string & value, std::string & comment) :
	key (key), value (value), comment (comment)
{
}

std::ostream & operator<<(std::ostream & out, const PropLine & line)
{
	if (line.key.size () > 0)
	{
		out << line.key << " = " << line.value;
	}
	else if (line.comment.size () > 0)
	{

		out << line.comment;
	}

	out << std::endl;
	return out;
}


/**
* PIMPL class
*/
class LIBOS_LOCAL PropertyPrivateData
{
public:
	Values vals;
	Lines lines;
	bool isLoadedFromFile;

	void store (std::string key, std::string value, std::string comment);
	bool isCommentline (std::string & line);
};

void PropertyPrivateData::store (std::string key, std::string value, std::string comment)
{
	if (key.size () == 0)
	{
		//Its a comment line
		PropLine line (key, value, comment);
		lines.push_back (line);
	}
	else
	{
		Values::const_iterator got = vals.find (key);
		if (got == vals.end ())
		{
			PropLine line (key, value, comment);
			lines.push_back (line);
			PropLine &lineInVector = lines.back ();
			vals [key] = &lineInVector.value;
		}
		else
		{
			*got->second = value;
		}
	}
}

bool PropertyPrivateData::isCommentline (std::string & line)
{
	std::size_t comentPos = line.find_first_of (COMMENT_CHARS);
	std::size_t startPos = line.find_first_not_of (SPACE_CHARS);

	return  ((comentPos != std::string::npos) && (comentPos == startPos));
}

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>> Generic functions <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
/**
* Replace all ocurrences
* \param    [in/out]	str		Source String
* \param    [in]		from	replace part
* \param    [in]		to		replacement
* \see		https://stackoverflow.com/a/24315631
*/
void replaceAll (std::string & str, const std::string& from, const std::string& to)
{
	size_t start_pos = 0;
	while ((start_pos = str.find (from, start_pos)) != std::string::npos)
	{
		str.replace (start_pos, from.length (), to);
		start_pos += to.length ();
	}
}

/**
* Trim spaces
* \param    [in/out]   str
*/
void trim (std::string &str)
{
	std::size_t start = str.find_first_not_of (" \n\r\t");
	std::size_t end = str.find_last_not_of (" \n\r\t");

	if ((std::string::npos == start) || (std::string::npos == end))
		str.empty ();
	else
	{
		str.erase (++end);
		str.erase (0, start);
	}
}

/**
* Add the standar escape secuence in the  .properties standar
* \param    [in/out]   str
*/
void escape (std::string &str)
{
	//TODO: 

}

/**
* Erase the standar escape secuence in the  .properties standar
* \param    [in/out]   str
*/
void unescape (std::string &str)
{
	replaceAll (str, "\\\\", "\\");
	replaceAll (str, "\\ ", " ");
}



// >>>>>>>>>>>>>>>>>>>>>>>>>>>>> Class <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
/**
* Constructor: Create PIMPL class
*/
Properties::Properties ()
{
	this->pd = new PropertyPrivateData ();
}

/**
* Destructor: free PIMPL class
*/
Properties::~Properties ()
{
	delete this->pd;
}
/**
* Retrieve the value of a existing parameter (int type)
* \param    [in]   key
* \param    [in]   defaultValue
* \return
*/
int Properties::getInt (const char * key, int defaultValue)
{
	Values::const_iterator got = pd->vals.find (key);
	if (got == pd->vals.end ())
	{
		setInt (key, defaultValue);
		return defaultValue;
	}
	else
	{
		return std::stoi (*got->second);
	}
}

/**
* Retrieve the value of a existing parameter (double)
* \param    [in]   key
* \param    [in]   defaultValue
* \return
*/
double Properties::getDouble (const char * key, double defaultValue)
{
	Values::const_iterator got = pd->vals.find (key);
	if (got == pd->vals.end ())
	{
		setDouble (key, defaultValue);
		return defaultValue;
	}
	else
	{
		return std::stod (*got->second);
	}
}


/**
* Retrieve the value of a existing parameter (string)
* \param    [in]   key
* \param    [in]   defaultValue
* \return
*/
const char * Properties::getStr (const char * key, const char * defaultValue)
{
	Values::const_iterator got = pd->vals.find (key);
	if (got == pd->vals.end ())
	{
		setStr (key, defaultValue);
		return defaultValue;
	}
	else
	{
		return (char *) (*got->second).c_str ();
	}
}




/**
* Retrieve the value of a existing parameter (bool)
* \param    [in]   key
* \param    [in]   defaultValue
* \return
*/
bool Properties::getBool (const char * key, bool defaultValue)
{
	Values::const_iterator got = pd->vals.find (key);
	if (got != pd->vals.end ())
	{
		std::string str = *got->second;
		std::transform (str.begin (), str.end (), str.begin (), ::tolower);
		std::istringstream is (str);
		bool b;
		is >> std::boolalpha >> b;
		return b;
	}
	else
	{
		setBool (key, defaultValue);
		return defaultValue;
	}
}


/**
* stores the value (bool) in the key param
* \param    [in]   key
* \param    [in]   value
*/
void Properties::setBool (const char * key, bool value)
{
	const char * v = (value) ? "true" : "false";
	pd->store (key, v, "");
}

/**
* stores the value (int) in the key param
* \param    [in]   key
* \param    [in]   value
*/
void Properties::setInt (const char * key, int value)
{
	pd->store (key, std::to_string (value), "");
}


/**
* stores the value (double) in the key param
* \param    [in]   key
* \param    [in]   value
*/
void Properties::setDouble (const char * key, double value)
{
	pd->store (key, std::to_string (value), "");
}


/**
* stores the value (string) in the key param
* \param    [in]   key
* \param    [in]   value
*/
void Properties::setStr (const char * key, const char * value)
{
	pd->store (key, value, "");
}


/**
* Stores a string in the file.
*
* IMPORTANT: this will use a "secret" and system information to generate a
* unique password. This is slightly more secure than plain text storage  (Since
* the "secret" will probably be embedded in the executable, and the system
* information is easy to extract).
* This should be only one extra measure.
*
*
* \param    [in]   key
* \param    [in]   value
*/
void Properties::setSecureStr (const char * key, const char * v, Secret secret)
{
	//TODO: make this function
	this->setStr (key, v);
}


/**
* Retrieve the value of a existing parameter (string)
* \see Properties::setSecureStr
* \param    [in]   key
* \param    [in]   defaultValue
* \return
*/
const char * Properties::getSecureStr (const char * key, const char * defaultValue, Secret secret)
{
	//TODO: make this function
	return this->getStr (key, defaultValue);
}



/**
* Check if the file has been loaded at least once from disk
* \return	true if loaded from disk
*/
bool Properties::isLoadedFromFile ()
{
	return pd->isLoadedFromFile;
}

/**
* Load file to memory. Can be called multiple times.
*
* \param    [in]   propfileName		file name and path to load
*/
void Properties::load (const char * propfileName)
{
	std::string linea;
	std::ifstream propFile (propfileName);

	bool isFirstLine = true;
	std::string key;
	std::string value;


	if (propFile.is_open ())
	{
		pd->isLoadedFromFile = true;
		while (std::getline (propFile, linea))
		{
			if (!isFirstLine)
			{
				trim (linea);
				value.append (linea);

				if ('\\' == linea.back ())
				{
					value.pop_back ();
				}
				else
				{
					pd->store (key, value, "");
					isFirstLine = true;
				}
			}
			else if (pd->isCommentline (linea))
			{
				pd->store ("", "", linea);
			}
			else
			{
				//Find equal separator
				std::size_t equalPos = linea.find_first_of (EQUAL_CHARS);
				if (equalPos != std::string::npos)
				{
					key = linea.substr (0, equalPos);
					value = linea.substr (++equalPos);

					trim (key);
					trim (value);

					unescape (key);
					unescape (value);

					if ('\\' == linea.back ())
					{
						value.pop_back ();
						isFirstLine = false;
					}
					else
					{
						pd->store (key, value, "");
					}
				}
				else //empty or wrong line
				{
					pd->store ("", "", linea);
				}
			}
		}
		propFile.close ();
	}
	else
	{
		pd->isLoadedFromFile = false;
	}
}



/**
* Save to disk the propfile
* \param    [in]   outFile		filename to save
*/
void Properties::save (const char * propertiesFile)
{
	std::ofstream propFile (propertiesFile);
	if (propFile.is_open ())
	{
		for (auto& line : pd->lines)
		{
			propFile << line;
		}
		propFile.close ();
	}
}
