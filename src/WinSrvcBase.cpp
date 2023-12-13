/*********************************************************************************************
*	Project		: https://github.com/IgnacioPomar/libOS
*	Name		: winSrvcBase.cpp
*	Description	: Clase base para que hereden los srevicios
********************************************************************************************/



#include <string>
#include <chrono>	//Para sleep en standAloneRun
#include <thread>	//Para sleep en standAloneRun

#include <Windows.h>  //Para HANDLE (necesario para WTSQuerySessionInformation)
#include <wtsapi32.h> //Para la función WTSQuerySessionInformation

#include "WinSrvcBase.h"

void WinSrvcBase::standAloneRun ()
{
	if (this->run ())
	{
		while (isOnline ())
		{
			std::this_thread::sleep_for (std::chrono::seconds (5));
		}
		this->stop (false);
	}
}

void WinSrvcBase::sessionChange (unsigned long changeType, unsigned long sessionId)
{
	// ---- NO DEBE TENER IMPLEMENTACION ----
	//Sólo existre para dar la posibilidad (O no), de ser implementada por WinSrvcBaseEx
}


/**
* Transforma los valores recibidos por la sesión windows, y lo transforma a algo entendible por la intranet
* \see https://docs.microsoft.com/en-us/windows/desktop/api/wtsapi32/nf-wtsapi32-wtsquerysessioninformationa
* \see https://docs.microsoft.com/es-es/windows/desktop/TermServ/wm-wtssession-change
* \param    [in]	changeType		Codigo de Microsoft indicando el motivo de la llamada
* \param    [in]	sessionId		Identificador de la sesión que se esta ejecutando
*/
void WinSrvcBaseEx::sessionChange (unsigned long changeType, unsigned long sessionId)
{
	//Convertimos el changeType a un enum propio
	SESS_CHANGE_TYPE change;
	switch (changeType)
	{
	case WTS_SESSION_LOGON:
		change = SESS_CHANGE_TYPE::SESSION_LOGON;
		break;
	case WTS_SESSION_LOGOFF:
		change = SESS_CHANGE_TYPE::SESSION_LOGOFF;
		break;
	case WTS_SESSION_LOCK:
		change = SESS_CHANGE_TYPE::SESSION_LOCK;
		break;
	case WTS_SESSION_UNLOCK:
		change = SESS_CHANGE_TYPE::SESSION_UNLOCK;
		break;

	default:
		change = SESS_CHANGE_TYPE::NONE;
	}

	//Sólo nos molestamos en obtener el usuario si vamos a transmitirlo
	if (SESS_CHANGE_TYPE::NONE != change)
	{
		//Obtenemos el nombre del dominio y el usuario.
		std::string domainUser = "";
		DWORD bytesReturned = 0;
		LPTSTR pData = NULL;

		WTSQuerySessionInformation (WTS_CURRENT_SERVER_HANDLE, sessionId, WTS_INFO_CLASS::WTSUserName, &pData, &bytesReturned);
		domainUser += pData;
		WTSFreeMemory (pData);

		domainUser += "@";

		WTSQuerySessionInformation (WTS_CURRENT_SERVER_HANDLE, sessionId, WTS_INFO_CLASS::WTSDomainName, &pData, &bytesReturned);
		domainUser += pData;
		WTSFreeMemory (pData);

		//Y mandamos el mensaje a la clase hija
		this->sessionChange (change, domainUser.c_str ());
	}
}
