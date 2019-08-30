/*********************************************************************************************
*	Name		: tester_pragmalib.h
*	Description	: header with the libs used in this project
********************************************************************************************/

#pragma once
#ifndef _PRAGMA_LIB_TESTER_H_
#define _PRAGMA_LIB_TESTER_H_


#ifdef _DEBUG 
#define END_LIB_STD "d.lib"
#else
#define END_LIB_STD ".lib"
#endif

// Libraries the project needs
#pragma comment(lib, "TinyCppUnit" END_LIB_STD)
#pragma comment(lib, "libOS" END_LIB_STD)

#endif // _PRAGMA_LIB_TESTER_H_
