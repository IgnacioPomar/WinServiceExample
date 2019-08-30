/*********************************************************************************************
*	Name		: Ejemplo_pragmalib.h
*	Description	: header with the libs used in this project
********************************************************************************************/

#pragma once
#ifndef _PRAGMA_LIB_CTRL_PRESENCIA_H_
#define _PRAGMA_LIB_CTRL_PRESENCIA_H_


#include "srvcExampleCfg.h"

#ifdef _DEBUG 
#define END_LIB_STD "d.lib"
#else
#define END_LIB_STD ".lib"
#endif


// Libraries the project needs
#pragma comment(lib, "libOS" END_LIB_STD)

#ifdef WEB_SERVICE

#ifdef _DEBUG 
#pragma comment(lib, "libmicrohttpd-dll_d.lib")
#else
#pragma comment(lib, "libmicrohttpd-dll.lib")
#endif


#endif //WEB_SERVICE



#endif // _PRAGMA_LIB_VSMAKE_H_
