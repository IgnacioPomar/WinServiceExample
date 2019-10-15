/*********************************************************************************************
*	Name		: SrvcEjemplo.h
*	Description	: Implementación de un servicio que interactue
********************************************************************************************/



#pragma once
#ifndef _SERVICE_EJEMPLO_H_
#define _SERVICE_EJEMPLO_H_

#include "winSrvcBase.h"

class SrvcDta;

//Podría heredar de WinSrvcBaseEx si quisiesemos los extendidos
class SrvcEjemplo : public WinSrvcBase
{
private:
	//---- datos que describen el servicio ----
	static constexpr const char * NAME = "SrvcEjemplo";
	static constexpr const char * DISPLAY_NAME = "Servicio de ejemplo";
	static constexpr const char * DESCRIPTION = "Servicio para demostrar el uso de como generar un servicio con un simple hilo";
	static constexpr bool IS_MANUAL_MODE = true;
	static constexpr int NUM_DEPENDENCIAS = 2;
	static constexpr const char * DEPENDENCIAS[NUM_DEPENDENCIAS] = { "Netman", "nsi" }; //Necesitamos que haya arrancado la red correctamente


	//Para ejecutar el servicio
	SrvcDta * srvcDta;


public:
	const char* getName ();
	const char* getDisplayName ();
	const char* getDescription ();
	bool isManual ();
	const char** getDepends ();
	int getNumDepends ();


	//Metodos de control del servicio
	bool init (int argc, char * argv[]);
	bool pause ();
	bool resume ();
	bool stop (bool isPaused);
	bool run ();

};


#endif //_SERVICE_EJEMPLO_H_
