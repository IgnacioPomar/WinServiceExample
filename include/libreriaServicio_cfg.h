/*********************************************************************************************
*	Name		: libreriaServicio_cfg.h
*	Description	: Configuration of the project
********************************************************************************************/

#pragma once
#ifndef _LIBRERIA_SRVC_CFG_H_
#define _LIBRERIA_SRVC_CFG_H_

//If the solution is a dinamic library (dll), we need the next macro
#define LIBRERIASERVICIO_DLL

//IMPORTANT: the project who exports must have the preprocessor macro LIBRERIASERVICIO_EXPORTS

//see http://gcc.gnu.org/wiki/Visibility

// Generic helper definitions for shared library support
#if defined _WIN32 || defined __CYGWIN__
#define LIBRERIASERVICIO_HELPER_DLL_IMPORT __declspec(dllimport)
#define LIBRERIASERVICIO_HELPER_DLL_EXPORT __declspec(dllexport)
#define LIBRERIASERVICIO_HELPER_DLL_LOCAL
#else
#if __GNUC__ >= 4
#define LIBRERIASERVICIO_HELPER_DLL_IMPORT __attribute__ ((visibility ("default")))
#define LIBRERIASERVICIO_HELPER_DLL_EXPORT __attribute__ ((visibility ("default")))
#define LIBRERIASERVICIO_HELPER_DLL_LOCAL  __attribute__ ((visibility ("hidden")))
#else
#define LIBRERIASERVICIO_HELPER_DLL_IMPORT
#define LIBRERIASERVICIO_HELPER_DLL_EXPORT
#define LIBRERIASERVICIO_HELPER_DLL_LOCAL
#endif
#endif

// Now we use the generic helper definitions above to define LIBRERIASERVICIO_API and LIBRERIASERVICIO_LOCAL.
// LIBRERIASERVICIO_API is used for the public API symbols. It either DLL imports or DLL exports (or does nothing for static build)
// LIBRERIASERVICIO_LOCAL is used for non-api symbols.

#ifdef LIBRERIASERVICIO_DLL // defined if LIBRERIASERVICIO is compiled as a DLL
#ifdef LIBRERIASERVICIO_EXPORTS // defined if we are building the LIBRERIASERVICIO DLL (instead of using it)
#define LIBRERIASERVICIO_API LIBRERIASERVICIO_HELPER_DLL_EXPORT
#else
#define LIBRERIASERVICIO_API LIBRERIASERVICIO_HELPER_DLL_IMPORT
#endif // LIBRERIASERVICIO_EXPORTS
#define LIBRERIASERVICIO_LOCAL LIBRERIASERVICIO_HELPER_DLL_LOCAL
#else // LIBRERIASERVICIO_DLL is not defined: this means LIBRERIASERVICIO is a static lib.
#define LIBRERIASERVICIO_API
#define LIBRERIASERVICIO_LOCAL
#endif // LIBRERIASERVICIO_DLL




#ifdef __GNUC__
#define DEPRECATED __attribute__((deprecated))
#elif defined(_MSC_VER)
#define DEPRECATED __declspec(deprecated)
#else
#pragma message("WARNING: You need to implement DEPRECATED for this compiler")
#define DEPRECATED
#endif


#endif //_LIBRERIA_SRVC_CFG_H_
