/*********************************************************************************************
*	Project		: https://github.com/IgnacioPomar/libOS
*	Name		: libOS_cfg.h
*	Description	: Configuration of the project
********************************************************************************************/

#pragma once
#ifndef _LIBOS_CFG_H_
#define _LIBOS_CFG_H_

//If the solution is a dinamic library (dll), we need the next macro
#define LIBOS_DLL

//IMPORTANT: the project who exports must have the preprocessor macro LIBOS_EXPORTS

//see http://gcc.gnu.org/wiki/Visibility

// Generic helper definitions for shared library support
#if defined _WIN32 || defined __CYGWIN__
#define LIBOS_HELPER_DLL_IMPORT __declspec(dllimport)
#define LIBOS_HELPER_DLL_EXPORT __declspec(dllexport)
#define LIBOS_HELPER_DLL_LOCAL
#else
#if __GNUC__ >= 4
#define LIBOS_HELPER_DLL_IMPORT __attribute__ ((visibility ("default")))
#define LIBOS_HELPER_DLL_EXPORT __attribute__ ((visibility ("default")))
#define LIBOS_HELPER_DLL_LOCAL  __attribute__ ((visibility ("hidden")))
#else
#define LIBOS_HELPER_DLL_IMPORT
#define LIBOS_HELPER_DLL_EXPORT
#define LIBOS_HELPER_DLL_LOCAL
#endif
#endif

// Now we use the generic helper definitions above to define LIBOS_API and LIBOS_LOCAL.
// LIBOS_API is used for the public API symbols. It either DLL imports or DLL exports (or does nothing for static build)
// LIBOS_LOCAL is used for non-api symbols.

#ifdef LIBOS_DLL // defined if LIBOS is compiled as a DLL
#ifdef LIBOS_EXPORTS // defined if we are building the LIBOS DLL (instead of using it)
#define LIBOS_API LIBOS_HELPER_DLL_EXPORT
#else
#define LIBOS_API LIBOS_HELPER_DLL_IMPORT
#endif // LIBOS_EXPORTS
#define LIBOS_LOCAL LIBOS_HELPER_DLL_LOCAL
#else // LIBOS_DLL is not defined: this means LIBOS is a static lib.
#define LIBOS_API
#define LIBOS_LOCAL
#endif // LIBOS_DLL




#ifdef __GNUC__
#define DEPRECATED __attribute__((deprecated))
#elif defined(_MSC_VER)
#define DEPRECATED __declspec(deprecated)
#else
#pragma message("WARNING: You need to implement DEPRECATED for this compiler")
#define DEPRECATED
#endif


#endif //_LIBOS_CFG_H_
