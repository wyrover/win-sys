/* =========================================================
 * DLL_Loader.h												
 * *********************************************************
 *															
 * Class: CDLL_Loader										
 *															
 * Author:  Dan Revah										
 *															
 * Date:	25/11/2011 (DD/MM/YYYY)																		
 *															
 * (-!-) User-mode root (DLL Hooking)						
 * ======================================================= */
#ifndef _DLL_LOADER_H_
#define _DLL_LOADER_H_

#include <windows.h>
#include <TlHelp32.h>
#include "String_Convertion.h"

enum DLL_Results { NOT_INIT, INIT_OK, PROCESS_ERROR_OPEN, PROCESS_ERRORR_VALLOC, PROCESS_ERROR_WRITE, PROCESS_ERROR_CREATE_RTHREAD, DLLINJECT_OK, 
					PROCESS_SET_OK, PROCESS_ERROR_SNAPSHOT, PROCESS_ERROR_NOTFOUND, DLL_ALREADY_INJECTED, DLLEJECT_OK, DLLEJECT_FAIL };

/* ================================================
 * Begin CDLL_Loader Class Definition
* ================================================= */
class CDLL_Loader {
public:
	// Constructors
	CDLL_Loader(); // default consturctor 
	CDLL_Loader(DWORD Pid,  char* DllPath); // initializing constructor

	// Destructor
	~CDLL_Loader();

	// Status functions
	inline bool isInit() const { return init; }
	inline bool isInjected() const { return injected; }
	
	inline void SetProcessId(DWORD Pid) { ProcessID = Pid; }; // Set process id

	DWORD GetProcessIdByName(char* process_name); // Get process id by name

	DLL_Results InitDLL(DWORD Pid,  char* DllPath); // Initilazing the DLL
	DLL_Results InjectDll(); // Injecting the dll into the process
	DLL_Results EjectDll(); // Ejecting the dll from the process

private:
	DWORD ThreadTeminationStatus; // the handle to the of the newly loaded dll
	DWORD ProcessID; // the process id
	char* DllFilePath; // the dll path

	// Status variables
	bool injected;
	bool init;
};

#endif
