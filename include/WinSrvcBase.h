/*********************************************************************************************
*	Name		: winSrvcBase.h
*	Description	: Clase base para que hereden los srevicios
********************************************************************************************/



#pragma once
#ifndef _WIN_SERVICES_BASE_H_
#define _WIN_SERVICES_BASE_H_

#include "libOS_cfg.h"


class LIBOS_API WinSrvcBase
{
public:
	//Metodos que dan información sobre este servicio concreto
	virtual const char* getName () = 0;
	virtual const char* getDisplayName () = 0;
	virtual const char* getDescription () = 0;
	virtual bool isManual () = 0;
	virtual const char** getDepends () = 0;
	virtual int getNumDepends () = 0;


	// Variables exclusivas para una ejecución en modo stand alone
	bool isOnline;

	//Metodos de control del servicio

	/**
	* Se ejecutará antes de lanzar el servicio por primera vez: el objetivo es inicializar datos
	*  \return	si devuelve false, el servicio no se iniciará, por lo que debe devolver true
	*/
	virtual bool init (int argc, char * argv[]) = 0;

	/**
	* Cuando desde el SCM se reciba una orden de pausado, se llamará a esta función
	* \return	Si devuelve false, se considerará que no se ha logrado pausar
	*			Si devuelve true, se notificará al SCM que el poceso esta pausado
	*/
	virtual bool pause () = 0;

	/**
	* Cuando desde el SCM se reciba una orden de reanudar, se llamará a esta función
	* Sólo se invocará con aquellos servicios que se hayan marcado como pausados.
	* \return	Si devuelve false, el proceso se mantendrá en pausa
	*			Si devuelve true, se notificará al SCM que el poceso esta en ejecución
	*/
	virtual bool resume () = 0;

	/**
	* Cuando desde el SCM se reciba una orden de detener, se invocará a esta función
	* \param    [in]   isPaused		Indica si el anterior estado era en pausa o en ejecución
	* \return	Actualmente se ignora el resultado
	*/
	virtual bool stop (bool isPaused) = 0;

	/**
	* Cuando desde el SCM se reciba una orden de iniciar, se llamará a esta función
	* (a través de la ejecución dede el main del programa).
	* Debe devolver el control inmediatamenet: necesita un hilo de ejecución paralela
	* \return	Actualmente se ignora el resultado
	*/
	virtual bool run () = 0;


	//-------------- funciones que se ejecutan en el base  --------------
	/**
	* Esta funcion implementa una ejecución que NO usa el SCM.
	* Su cometido es facilitar la depurac ión al dar un sistema de ejecución
	* identico al servicio, pero en el espaciod e trabajo del usuario
	*/
	void standAloneRun ();


	/**
	* En exclusiva para ser sobreescrita por WinSrvcBaseEx en caso de querer ser usada
	*/
	virtual void sessionChange (unsigned long changeType, unsigned long sessionId);


};

enum SESS_CHANGE_TYPE
{
	NONE, //reservado para conexiones o desconexiones por escritorio remoto
	SESSION_LOGON,
	SESSION_LOGOFF,
	SESSION_LOCK,
	SESSION_UNLOCK
};

/**
 * En caso de necesitar alguna de las funciones extendidas, se deberá heredar de esta clase
 */
class  WinSrvcBaseEx : public WinSrvcBase
{
public:
	/**
	* Obtiene el nombre del usuario e invoca a su homonima
	*/
	void sessionChange (unsigned long changeType, unsigned long sessionId);

	virtual void sessionChange (SESS_CHANGE_TYPE change, const char * user) = 0;
};



#endif //_WIN_SERVICES_BASE_H_
