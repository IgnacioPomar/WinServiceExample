/*********************************************************************************************
*	Name		: winSrvcUtls.h
*	Description	: Funciones necesarias para interactuar con los servicios de Windows
********************************************************************************************/



#pragma once
#ifndef _WIN_SERVICES_UTILS_H_
#define _WIN_SERVICES_UTILS_H_

//#include <Windows.h>
#include "libOS_cfg.h"

class LIBOS_API WinSrvcUtls
{
public:
	static bool install (const char* serviceName, const char* displayName,
						 const char* description, const char* path,
						 bool isManualService, const char* vServicesDependency [],
						 int numServicesDependency);
	static bool uninstall (const char* serviceName);

	static bool start (const char* serviceName);
	static bool stop (const char* serviceName, unsigned int maxMilisecondsWait);

};







#endif //_WIN_SERVICES_UTILS_H_
