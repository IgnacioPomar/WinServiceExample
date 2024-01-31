/*********************************************************************************************
*	Project		: https://github.com/IgnacioPomar/libOS
*	Name		: sysWinService.cpp
*	Description	: Funciones necesarias para interactuar con los servicios de Windows
********************************************************************************************/

#include <Windows.h>
#include <string>
#include "winSrvcUtls.h"

/*
#ifdef _WIN64
#pragma comment(lib, "Advapi64.lib")
#else
#pragma comment(lib, "Advapi32.lib")
#endif
*/

/**
 * Instala un servicio en el sistema. En caso de que el servicio ya existiera se considera la
 * operacion correcta
 *
 * \param [in] serviceName         Nombre del servicio a instalar
 * \param [in] displayName         Nombre a mostrar del servicio a instalar
 * \param [in] description	       Descripcion que se mostrara en el servicio
 * \param [in] path	               Path del ejecutable que se va a ejecutar
 * \param [in] isManualService     Indica si el servicio arrancara manualmente o de forma automátca
 * \param [in] vSrvcsDepends       Lista de servicios de los que dependemos
 * \param [in] numDepends          Numero de servicios de los que dependenmos
 * \return true: El servicio ya existía o se ha instalado correctamente
 *        false: Error al instalar el servicio
 */
bool WinSrvcUtls::install (const char* serviceName, const char* displayName, const char* description, const char* path, bool isManualService, const char* vSrvcsDepends [], int numDepends)
{
	SC_HANDLE handleSCManager;
	SC_HANDLE handleService;

	std::string dependencies;
	int startType;

	// Obtenemos referencia al Service Control Manager
	handleSCManager = OpenSCManager (NULL, NULL, SC_MANAGER_CREATE_SERVICE);
	if (handleSCManager == NULL)
	{
		return false;
	}


	// Creamos la lista de dependencias de otros servicios
	if (numDepends > 0)
	{
		dependencies = "";
		for (int i = 0; i < numDepends; i++)
		{
			// Debemos concatenar las cadenas separadas por 0.
			dependencies += vSrvcsDepends [i];
			dependencies += '\0';
		}
	}

	// Escogemos el tipo de arranque
	if (isManualService)
	{
		startType = SERVICE_DEMAND_START;
	}
	else
	{
		startType = SERVICE_AUTO_START;
	}


	// Registramos el servicio
	handleService = CreateService (
		handleSCManager, // handle del Service Control Manager
		serviceName, // nombre del servicio
		displayName, // nombre del servicio mostrado
		SERVICE_ALL_ACCESS, // permisos
		SERVICE_WIN32_OWN_PROCESS, // servicio corre en proceso propio
		startType, // arrancamos bajo demanda
		SERVICE_ERROR_NORMAL, // tratamiento de errores normal
		path, // path del ejecutable
		NULL, // no load ordering group
		NULL, // no tag identifier
		dependencies.c_str (), // sin dependencias
		NULL, // localSystem account
		NULL); // no password

	if (handleService == NULL)
	{
		int error = GetLastError ();
		CloseServiceHandle (handleSCManager);
		if (error != ERROR_SERVICE_EXISTS)
		{
			return false;
		}
		else //Si el servicio existe... devolvemos true
		{
			return true;
		}
	}
	else
	{
		SERVICE_DESCRIPTIONA sdBuf;
		sdBuf.lpDescription = (LPSTR) description;

		if (!ChangeServiceConfig2 (
			handleService,					// handle del service
			SERVICE_CONFIG_DESCRIPTION,		// cambiar: descripcion
			&sdBuf))						// nueva descripcion
		{
			return false;
		}

		//Si el arranque es automatico, arrancar al instalar
		if (!isManualService)
		{
			WinSrvcUtls::start (serviceName);
		}

		CloseServiceHandle (handleSCManager);
	}


	CloseServiceHandle (handleService);
	return true;
}



/**
 * Desinstala un servicio en el sistema
 *
 * \param [in] serviceName	 Nombre del sistema a desinstalar
 * \return  true: Se ha instalado correctamente (o si no existia previamente)
 *         false: No se ha logrado desinstalar
 */
bool WinSrvcUtls::uninstall (const char* serviceName)
{
	bool retVal = false;
	SC_HANDLE handleSCManager = NULL;
	SC_HANDLE handleService = NULL;

	// Obtenemos referencia al Service Control Manager
	handleSCManager = OpenSCManager (NULL, NULL, SC_MANAGER_CREATE_SERVICE);
	if (handleSCManager == NULL)
	{
		return false;
	}

	//Y ahora la del servicio en concreto
	handleService = OpenService (handleSCManager, serviceName, SERVICE_ALL_ACCESS | DELETE);
	if (handleService == NULL)
	{
		//No lo hemos localizado
		int error = GetLastError ();
		if (error == ERROR_SERVICE_DOES_NOT_EXIST)
		{
			//Pero el caso es que no existía de antemano
			retVal = true;
		}
		else
		{
			retVal = false;
		}
	}
	else
	{
		SERVICE_STATUS status;
		if (!QueryServiceStatus (handleService, &status))
		{
			//Si no podemos obtener información del servicio es que probablemente este bloqueado
			//Quizás haya que reinicia el sistema
			retVal = false;
		}
		else
		{
			bool isDeletable = true;
			if (status.dwCurrentState != SERVICE_STOPPED)
			{
				//Esta en ejecución: tratamos de detenerlo antes de eliminarlo
				if (!ControlService (handleService, SERVICE_CONTROL_STOP, &status))
				{
					//No hemnos podido pararlo
					isDeletable = false;
					retVal = false;
				}
			}

			//Tenemos instancia al servicio, y en principio esta cerrado
			if (isDeletable)
			{
				if (DeleteService (handleService))
				{
					retVal = true;
				}
				else
				{
					retVal = false;
				}
			}
		}
	}

	//Limpiamos y cerramos
	if (handleService != NULL)
	{
		CloseServiceHandle (handleService);
	}
	if (handleSCManager != NULL)
	{
		CloseServiceHandle (handleSCManager);
	}

	return retVal;
}


/**
 * Detiene la ejecución de un servcio si esta en marcha
 *
 * \param [in] serviceName	         Nombre del sistema a desinstalar
 * \param [in] maxMilisecondsWait	 Milisegundos a esperar antes de dar gfallo
 * \return true: si se logra detener el servicio
 *         false: si el servicio queda en ejecución
 */
bool WinSrvcUtls::stop (const char* serviceName, unsigned int maxMilisecondsWait)
{
	bool retVal = false;
	SC_HANDLE handleSCManager = NULL;
	SC_HANDLE handleService = NULL;
	SERVICE_STATUS serviceStatus;
	DWORD startTime;


	// Obtenemos referencia al Service Control Manager
	handleSCManager = OpenSCManager (NULL, NULL, GENERIC_ALL);
	if (handleSCManager == NULL)
	{
		return false;
	}

	handleService = OpenService (handleSCManager, serviceName, SERVICE_ALL_ACCESS);
	if (handleService != NULL)
	{
		// Si ya esta detenido no intentamos nada
		if (!QueryServiceStatus (handleService, &serviceStatus))
		{
			retVal = false;
		}
		else if (serviceStatus.dwCurrentState == SERVICE_STOPPED)
		{
			retVal = true;
		}
		else if (ControlService (handleService, SERVICE_CONTROL_STOP, &serviceStatus) == 0)
		{
			// Enviamos la señal de parada, y ns ha devuelto error

			// En caso de depuración queremos este log de error
			int error = GetLastError ();
			retVal = false;

		}
		else if (maxMilisecondsWait > 0)
		{
			// Esperamos a que se detenga el servicio
			startTime = GetTickCount ();
			while (true)
			{
				Sleep (serviceStatus.dwWaitHint);
				if (!QueryServiceStatus (handleService, &serviceStatus))
				{
					retVal = false;
					break;
				}

				if (serviceStatus.dwCurrentState == SERVICE_STOPPED)
				{
					retVal = true;
					break;
				}

				if (GetTickCount () - startTime > maxMilisecondsWait)
				{
					retVal = false;
					break;
				}
			}
		}
	}

	//Limpiamos y cerramos
	if (handleService != NULL)
	{
		CloseServiceHandle (handleService);
	}
	if (handleSCManager != NULL)
	{
		CloseServiceHandle (handleSCManager);
	}

	return retVal;
}


bool WinSrvcUtls::isInstalled (const char* serviceName)
{
	SC_HANDLE handleSCManager = NULL;
	SC_HANDLE handleService = NULL;

	// Obtenemos referencia al Service Control Manager
	handleSCManager = OpenSCManager (NULL, NULL, GENERIC_EXECUTE);
	if (handleSCManager == NULL)
	{
		return false;
	}

	handleService = OpenService (handleSCManager, serviceName, SERVICE_START);
	return handleService != NULL;

}


/**
 * Lanza la ejecucion de un servicio
 *
 * \param [in] serviceName	 Nombre del sistema a iniciar
 * \return true El servicio se ha ejecutado
 * false: el servicio no se esta ejecutando
 */
bool WinSrvcUtls::start (const char* serviceName)
{
	bool retVal = false;
	SC_HANDLE handleSCManager = NULL;
	SC_HANDLE handleService = NULL;

	// Obtenemos referencia al Service Control Manager
	handleSCManager = OpenSCManager (NULL, NULL, GENERIC_EXECUTE);
	if (handleSCManager == NULL)
	{
		return false;
	}

	handleService = OpenService (handleSCManager, serviceName, SERVICE_START);
	if (handleService == NULL)
	{
		retVal = false; //No esra instalado
	}
	else
	{
		const int res = StartService (handleService, 0, NULL);
		if (res != 0)
		{
			// If the function succeeds, the return value is nonzero.
			// \see http://msdn.microsoft.com/en-us/library/windows/desktop/ms686321(v=vs.85).aspx
			retVal = true;
		}
		else
		{
			const int error = GetLastError ();
			retVal = false;
		}
	}

	//Limpiamos y cerramos
	if (handleService != NULL)
	{
		CloseServiceHandle (handleService);
	}
	if (handleSCManager != NULL)
	{
		CloseServiceHandle (handleSCManager);
	}

	return retVal;
}
