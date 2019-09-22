/*********************************************************************************************
*	Name		: PropertyConfiguration.h
*	Description	:  store the configurable application like in java .properties
********************************************************************************************/


#pragma once
#ifndef _PROPERTY_CONFIGURATION_
#define _PROPERTY_CONFIGURATION_


#include "libOS_cfg.h"


typedef struct Secret { const char s [8]; } Secret;


class PropertyPrivateData;

class LIBOS_API Properties
{
public:
	Properties ();
	~Properties ();
	Properties (const Properties&) = delete;
	Properties& operator=(Properties const&) = delete;

	void save (const char * outFile);
	void load (const char * propfileName);

	int    getInt (const char* key, int defaultValue);
	const char * getStr (const char* key, const char * defaultValue);
	bool   getBool (const char* key, bool defaultValue);
	double getDouble (const char* key, double defaultValue);

	void   setBool (const char* key, bool value);
	void   setInt (const char* key, int value);
	void   setStr (const char* key, const char * value);
	void   setDouble (const char* key, double value);

	//
	const char * getSecureStr (const char* key, const char * defaultValue, Secret secret);
	void setSecureStr (const char* key, const char * value, Secret secret);
	bool isLoadedFromFile ();

private:
	PropertyPrivateData * pd;

};

#endif  //_PROPERTY_CONFIGURATION_
