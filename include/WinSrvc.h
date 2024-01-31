/*********************************************************************************************
*	Project		: https://github.com/IgnacioPomar/libOS
*	Name		: WinSrvc.h
*	Description	: Template que debe usarse para cerar los servicios
********************************************************************************************/

//Esta clase no se exporta: es un template ya que su contenido debería ir en el ejecutable
//TODO: habría que incluir el equivalente que llame al demonizar para linux

#pragma once
#ifndef _WIN_SERVICE_TEMPLATE_H_
#define _WIN_SERVICE_TEMPLATE_H_

#include <Windows.h>
#include <type_traits> //Para std::is_base_of

#include "winSrvcBase.h"
#include "winSrvcUtls.h"


//Para usar esta template, debe tener implementada una función tal que:
//static Prueba& getStaticInstance ()

template <typename T>
class WinSrvc
{
private:
	static SERVICE_STATUS& getSrvcStatus ()
	{
		static SERVICE_STATUS srvcStatus;
		return srvcStatus;
	}
	static SERVICE_STATUS_HANDLE& getSrvcStatusHandle ()
	{
		static SERVICE_STATUS_HANDLE srvcStatusHandle;
		return srvcStatusHandle;
	}

public:

	/**
	* Obtenemos una instancia estática del objeto
	*/
	static T& getStaticInstance ()
	{
		static_assert(std::is_base_of<WinSrvcBase, T>::value, "El servicio debe heredar de la clase WinSrvcBase");
		static T staticInstance;
		return staticInstance;
	}

	/**
	* Convierte la ejecucion de la aplicacion en un servicio:
	* El hilo de ejecucion se mete en un bucle de eventos
	* \return	False si ha dado un error
	*/
	static bool runAsService ()
	{
		const char* serviceName = getStaticInstance ().getName ();

		SERVICE_TABLE_ENTRYA dispatchTable [] =
		{
			{ (LPSTR)serviceName , (LPSERVICE_MAIN_FUNCTION)srvcMain },
			{ NULL, NULL }
		};

		if (!StartServiceCtrlDispatcher (dispatchTable))
		{
			int error = GetLastError ();
			return false;
		}

		return true;
	}


	static void WINAPI srvcMain (DWORD argc, LPSTR* argv)
	{
		SERVICE_STATUS& srvcStatus = getSrvcStatus ();

		srvcStatus.dwServiceType = SERVICE_WIN32;
		srvcStatus.dwCurrentState = SERVICE_START_PENDING;
		srvcStatus.dwControlsAccepted = 0;
		srvcStatus.dwWin32ExitCode = 0;
		srvcStatus.dwServiceSpecificExitCode = 0;
		srvcStatus.dwCheckPoint = 0;
	#ifdef _DEBUG
		srvcStatus.dwWaitHint = 0xffffffff;
	#else
		srvcStatus.dwWaitHint = 10000;
	#endif

		WinSrvcBase& srvc = getStaticInstance ();
		SERVICE_STATUS_HANDLE& srvcStatusHandle = getSrvcStatusHandle ();

		srvcStatusHandle = RegisterServiceCtrlHandlerEx (srvc.getName (), ctrlHandlerEx, &srvc);

		if (srvcStatusHandle == (SERVICE_STATUS_HANDLE)0)
		{
			return;
		}

		SetServiceStatus (srvcStatusHandle, &srvcStatus);

		// Inicializamos lo que haya que inicializar del servicio
		if (!srvc.init (argc, argv))
		{
			srvcStatus.dwCurrentState = SERVICE_STOPPED;
			srvcStatus.dwCheckPoint = 0;
			srvcStatus.dwWaitHint = 0;
			srvcStatus.dwWin32ExitCode = -1; //Codigo de error "generico"

			SetServiceStatus (srvcStatusHandle, &srvcStatus);
		}
		else
		{
			// Inicialización completa: avisamos de que empieza la ejecución
			srvcStatus.dwCurrentState = SERVICE_RUNNING;
			srvcStatus.dwCheckPoint = 0;
			srvcStatus.dwWaitHint = 0;
			srvcStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;

			//En caso de que estemos en un servicio extendido, solicitamos los eventos extendidos
			if (std::is_base_of<WinSrvcBaseEx, T>::value)
			{
				srvcStatus.dwControlsAccepted |= SERVICE_ACCEPT_SESSIONCHANGE;
			}

			SetServiceStatus (srvcStatusHandle, &srvcStatus);

			//Y finalmente iniciamos el "proceso" del servicio
			srvc.run ();
		}
	}




	/**
	* Localiza el recurso solicitado y lo ejecuta
	* \see		https://docs.microsoft.com/es-es/windows/desktop/api/winsvc/nc-winsvc-lphandler_function_ex
	* \param    [in]		dwControl		Valor que  indiva el motivo por el que se invoca
	* \param    [in]		dwEventType		codigo extendido sobre el evento (de ser necesario)
	* \param    [in]		lpEventData		informacion extendida sobre el evento (de ser necesario)
	* \param    [in/out]	lpContext		(definido por el template) Puntero al servicio base
	* \return	False si ha dado un error
	*/
	static DWORD WINAPI ctrlHandlerEx (DWORD dwControl, DWORD dwEventType, LPVOID lpEventData, LPVOID lpContext)
	{
		WinSrvcBase& srvc = *(WinSrvcBase*)lpContext;
		SERVICE_STATUS& srvcStatus = getSrvcStatus ();
		SERVICE_STATUS_HANDLE& srvcStatusHandle = getSrvcStatusHandle ();

		switch (dwControl)
		{
		case SERVICE_CONTROL_PAUSE:
			if ((SERVICE_RUNNING == srvcStatus.dwCurrentState) && (srvc.pause ()))
			{
				srvcStatus.dwCurrentState = SERVICE_PAUSED;
				SetServiceStatus (srvcStatusHandle, &srvcStatus);
			}
			return  NO_ERROR;
			break;

		case SERVICE_CONTROL_CONTINUE:
			if ((SERVICE_PAUSED == srvcStatus.dwCurrentState) && (srvc.resume ()))
			{
				srvcStatus.dwCurrentState = SERVICE_RUNNING;
				SetServiceStatus (srvcStatusHandle, &srvcStatus);
			}
			return  NO_ERROR;
			break;

		case SERVICE_CONTROL_STOP:
			// Debemos detener el servicio
			if ((SERVICE_RUNNING == srvcStatus.dwCurrentState) || (SERVICE_PAUSED == srvcStatus.dwCurrentState))
			{
				bool isPaused = SERVICE_PAUSED == srvcStatus.dwCurrentState;

				srvcStatus.dwCurrentState = SERVICE_STOP_PENDING;
				SetServiceStatus (srvcStatusHandle, &srvcStatus);

				srvc.stop (isPaused);
				srvcStatus.dwWin32ExitCode = 0;
				srvcStatus.dwCurrentState = SERVICE_STOPPED;
				srvcStatus.dwCheckPoint = 0;
				srvcStatus.dwWaitHint = 0;
				SetServiceStatus (srvcStatusHandle, &srvcStatus);
			}
			return  NO_ERROR;
			break;

		case SERVICE_CONTROL_INTERROGATE:
			// Fall through to send current status.
			return  NO_ERROR;
			break;

			//---------------- Funciones extendidas ----------------
		case SERVICE_CONTROL_SESSIONCHANGE:
			srvc.sessionChange (dwEventType, ((WTSSESSION_NOTIFICATION*)lpEventData)->dwSessionId);
			return  NO_ERROR;
			//TODO: Modificar esto
			//return ERROR_CALL_NOT_IMPLEMENTED;
			break;

		default:
			return ERROR_CALL_NOT_IMPLEMENTED;
			break;
		}


	}


	//Metodos para administrar el servicio
	static bool install ()
	{
		WinSrvcBase& srvc = getStaticInstance ();

		HMODULE hModule = GetModuleHandle (NULL);
		CHAR path [MAX_PATH];
		GetModuleFileName (hModule, path, MAX_PATH);

		return WinSrvcUtls::install (srvc.getName (), srvc.getDisplayName (),
									 srvc.getDescription (), path, srvc.isManual (),
									 srvc.getDepends (), srvc.getNumDepends ());
	}

	static bool isInstalled ()
	{
		WinSrvcBase& srvc = getStaticInstance ();
		return WinSrvcUtls::isInstalled (srvc.getName ());
	}


	static bool uninstall ()
	{
		WinSrvcBase& srvc = getStaticInstance ();
		return WinSrvcUtls::uninstall (srvc.getName ());
	}


};


#include "CommandLineParser.h"

template <class T>
int srvcMain (int argc, char* argv [])
{
	if (argc > 1)
	{
		CommandLineParser cmdLine;
		cmdLine.addOption ("i", "install", "Install the service", true, 0);
		cmdLine.addOption ("u", "uninsall", "Unistall the service");
		cmdLine.addOption ("dbg", "DEBUG", "launch in debug mode (without service)");


		cmdLine.parse (argc, (const char**)argv);
		if (cmdLine.hasOption ("i"))
		{
			if (WinSrvc<T>::install ())
				printf ("Servicio correctamente instalado.\n");
			else
				printf ("ERROR: al instalar el servicio.\n");
		}
		else if (cmdLine.hasOption ("u"))
		{
			if (WinSrvc<T>::uninstall ())
				printf ("Servicio correctamente eliminado.\n");
			else
				printf ("ERROR: al eliminar el servicio\n");
		}
		else if (cmdLine.hasOption ("dbg"))
		{
			T srvc;
			srvc.init (argc, argv);
			srvc.standAloneRun ();
		}
		else
		{
			cmdLine.printHelp ();
		}
	}
	else
	{
		if (!WinSrvc<T>::runAsService ())
		{
			if (!WinSrvc<T>::isInstalled ())
			{
				T srvc;
				srvc.init (argc, argv);
				srvc.standAloneRun ();
			}
		}
	}

	return EXIT_SUCCESS;
}



#endif //_WIN_SERVICE_TEMPLATE_H_
