/*********************************************************************************************
*	Name		: SrvcEjepmploServidorWeb.h
*	Description	: Implementación de un servicio que interactue
********************************************************************************************/



#pragma once
#ifndef _SERVICE_EJEMPLO_WEB_H_
#define _SERVICE_EJEMPLO_WEB_H_

#include "winSrvcBase.h"

//prototipos necesarios para evitar includes
struct MHD_Connection;
struct MHD_Daemon;


//Podría heredar de WinSrvcBaseEx si quisiesemos los extendidos
class SrvcEjepmploServidorWeb : public WinSrvcBase
{
private:
	//---- datos que describen el servicio ----
	static constexpr const char * NAME = "SrvcEjemplo";
	static constexpr const char * DISPLAY_NAME = "Servicio de ejemplo";
	static constexpr const char * DESCRIPTION = "Servicio para demostrar el uso de como generar un servicio, dejando un servicio web";
	static constexpr bool IS_MANUAL_MODE = true;
	static constexpr int NUM_DEPENDENCIAS = 2;
	static constexpr const char * DEPENDENCIAS [NUM_DEPENDENCIAS] = {"Netman", "nsi"}; //Necesitamos que haya arrancado la red correctamente


	//---- Constantes propias de este srevisio ----
	static constexpr int DEFAULT_PORT = 8898;

	//Datos necesarios para trabajar como servidor http
	unsigned int flags;
	unsigned int port;

	struct MHD_Daemon * httpDaemon;

	static int webServiceMain (void * srvcPresencia, MHD_Connection * connection, const char * url, const char * method, const char * version, const char * upload_data, size_t * upload_data_size, void ** ptr);





public:
	const char* getName ();
	const char* getDisplayName ();
	const char* getDescription ();
	bool isManual ();
	const char** getDepends ();
	int getNumDepends ();


	//Metodos de control del servicio
	bool init ();
	bool pause ();
	bool resume ();
	bool stop (bool isPaused);
	bool run ();

};


#endif //_SERVICE_EJEMPLO_WEB_H_
