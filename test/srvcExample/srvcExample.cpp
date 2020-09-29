

#include "srvcExample.h"


//Funciones "Simples" que devuelven información constante del servicio
const char* SrvcEjemplo::getName ()
{
	return SrvcEjemplo::NAME;
}
const char* SrvcEjemplo::getDisplayName ()
{
	return SrvcEjemplo::DISPLAY_NAME;
}
const char* SrvcEjemplo::getDescription ()
{
	return SrvcEjemplo::DESCRIPTION;
}
bool SrvcEjemplo::isManual ()
{
	return SrvcEjemplo::IS_MANUAL_MODE;
}
const char** SrvcEjemplo::getDepends ()
{
	return (const char **)SrvcEjemplo::DEPENDENCIAS;
}
int SrvcEjemplo::getNumDepends ()
{
	return SrvcEjemplo::NUM_DEPENDENCIAS;
}

bool SrvcEjemplo::isOnline ()
{
	return varIsOnline;
}

//No adminitimos ni pausa ni reanudar... por lo que devuelven false
bool SrvcEjemplo::pause () { return false; }
bool SrvcEjemplo::resume () { return false; }



bool SrvcEjemplo::init (int argc, char * argv[])
{
	//INICIALIZA los datos del servicio: cargar ficheros cfg, etc...
	return true;
}


#include <thread>
#include <chrono>
class SrvcDta
{
public:
	bool isonline = true;

	void foo ()
	{
		while (isonline)
		{
			std::this_thread::sleep_for (std::chrono::seconds (1));
		}
	}


};



bool SrvcEjemplo::stop (bool isPaused)
{
	if (srvcDta != nullptr)
	{
		srvcDta->isonline = false;

		//Esperamnos a que cierre
		std::this_thread::sleep_for (std::chrono::seconds (1));

		//y eliminamos
		delete srvcDta;
		srvcDta = nullptr;

	}
	varIsOnline = false;
	return true;
}


bool SrvcEjemplo::run ()
{
	varIsOnline = true;
	srvcDta = new SrvcDta ();

	std::thread (&SrvcDta::foo, srvcDta);

	return true;
}
