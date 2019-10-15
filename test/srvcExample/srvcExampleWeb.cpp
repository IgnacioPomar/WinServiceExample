

#include "srvcExampleWeb.h"

#ifdef WEB_SERVICE

//Funciones "Simples" que devuelven información constante del servicio
const char* SrvcEjepmploServidorWeb::getName ()
{
	return SrvcEjepmploServidorWeb::NAME;
}
const char* SrvcEjepmploServidorWeb::getDisplayName ()
{
	return SrvcEjepmploServidorWeb::DISPLAY_NAME;
}
const char* SrvcEjepmploServidorWeb::getDescription ()
{
	return SrvcEjepmploServidorWeb::DESCRIPTION;
}
bool SrvcEjepmploServidorWeb::isManual ()
{
	return SrvcEjepmploServidorWeb::IS_MANUAL_MODE;
}
const char** SrvcEjepmploServidorWeb::getDepends ()
{
	return (const char **)SrvcEjepmploServidorWeb::DEPENDENCIAS;
}
int SrvcEjepmploServidorWeb::getNumDepends ()
{
	return SrvcEjepmploServidorWeb::NUM_DEPENDENCIAS;
}



#include <string>
#include <microhttpd.h>


//No adminitimos ni pausa ni reanudar... por lo que devuelven false
bool SrvcEjepmploServidorWeb::pause () { return false; }
bool SrvcEjepmploServidorWeb::resume () { return false; }


bool SrvcEjepmploServidorWeb::init (int argc, char * argv[])
{
	//Configuración harcodeada. Desde aqui se podría cargar un fichero .properties
	flags = MHD_USE_SELECT_INTERNALLY;
#if _DEBUG
	flags |= MHD_USE_DEBUG;
#endif

	//Otra opción  MHD_USE_THREAD_PER_CONNECTION;
	port = DEFAULT_PORT;
	httpDaemon = nullptr;

	//INICIALIZA los datos del servicio
	return true;
}


bool SrvcEjepmploServidorWeb::stop (bool isPaused)
{
	//Detenemos un servicio
	MHD_stop_daemon (httpDaemon);

	return true;
}


bool SrvcEjepmploServidorWeb::run ()
{
	//Lanzamos la ejecución del servicio
	void * contexto = this;


	httpDaemon = MHD_start_daemon (flags, port, nullptr, nullptr, &webServiceMain, contexto, MHD_OPTION_END);
	return (httpDaemon != nullptr);
}




//--------------------------------------------------------------


/**
* Cada llamada web que se reciba entra por aqui.
* Crea objetos con los parámetros GET de la llamada, identifica el servicio, y lo lanza
* \param    [in]   compendioSrvc	puntero al CompendioSrvc que ha recibido la llamada web
* \param    [in]   connection		- Uso interno - puntero a la conexión
* \param    [in]   url				Url de la llamada
* \param    [in]   method			El método HTTP usado por el cliente  (GET, PUT, DELETE, POST, etc.)
* \param    [in]   version			La versión HTTP usada por el cliente (por ejemplo HTTP/1.1)
* \param    [in]   upload_data		Los datos que se envían. si no caben su harán sucesivas
*									llamadas a este método para guardarlo en su totalidad
* \param    [in]   upload_data_size	Datos en el buffer upload_data (será el tamaño del buffer
*									salvo en la última llamada)
* \param    [in]   ptr				Puntero que se puede asociar para futuras llamadas a este método
*									(por ejemplo, si hay un POST que sube muchos datos)
* \return	Debe devolver MHD_YES si ha procesado la llamada correctamente, o  MHD_NO si se debe
*			cerrar el socket por un error al procesar la llamada
*/
int SrvcEjepmploServidorWeb::webServiceMain (void * srvcEjemplo, MHD_Connection * connection, const char * url, const char * method, const char * version, const char * upload_data, size_t * upload_data_size, void ** ptr)
{
	static int dummy;
	//WebServiceParameters wsParams;
	struct MHD_Response * response;
	int ret;

	/*
	wsParams.setMethod (method);
	if (!wsParams.isSuportedMethod ())
	{
		return MHD_NO; // unexpected method
	}
	*/

	// The first time only the headers are valid,   do not respond in the first round... 
	if (&dummy != *ptr)
	{
		*ptr = &dummy;
		return MHD_YES;
	}


	//YAGNI: Implementar la subida de ficheros meduiante POST
	//  https://www.gnu.org/software/libmicrohttpd/manual/html_node/microhttpd_002dpost.html
	if (0 != *upload_data_size)
		return MHD_NO; // De momento no soportamos ficheros
	*ptr = NULL; // clear context pointer 


	// Obtenemos los parametros query
	//MHD_get_connection_values (connection, MHD_GET_ARGUMENT_KIND, WebServiceParameters::parseQueryParameter, &wsParams);



	SrvcEjepmploServidorWeb * wc = (SrvcEjepmploServidorWeb *)srvcEjemplo;

	const char * page = wc->getDescription ();

	/*
	if (!wc->commands.executeResource (url, wsParams, ss))
	{
		printf ("[Descartado] %s - %s\n", method, url);
		return MHD_NO; // No es un servico esperado
	}
	*/




	// Preparamos la salida final
	//const char * page = ss.c_str ();

	//Posibles valores:
	// MHD_RESPMEM_PERSISTENT  - Buffer is a persistent (static/global) buffer that won't change for at least the lifetime of the response
	// MHD_RESPMEM_MUST_FREE   - Buffer is heap-allocated with 'malloc' (or equivalent) and should be freed by MHD
	// MHD_RESPMEM_MUST_COPY   - Buffer is in transient memory and only valid during the call to 'MHD_create_response_from_buffer'. MHD must make its own private copy of the data for processing.
	response = MHD_create_response_from_buffer (strlen (page), (void*)page, MHD_RESPMEM_MUST_COPY);
	ret = MHD_queue_response (connection, MHD_HTTP_OK, response);
	MHD_destroy_response (response);
	return ret;
}

#endif //WEB_SERVICE
