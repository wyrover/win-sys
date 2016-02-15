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
#include "DLL_Loader.h"
#include <iostream>

/* ===========================================================
 *  CDLL_Loader::CDLL_Loader()
 *
 *	(*) Default constructor
 */
CDLL_Loader::CDLL_Loader(): 
	ProcessID(0), DllFilePath(0), init(false), injected(false) 
{
}

/* ===========================================================
 *  CDLL_Loader::CDLL_Loader(DWORD Pid,  char* DllPath)
 *
 *	(*) Initializing constructor
 *	Parameters: 
 *	Pid - Process ID to inject
 *	DllPath - The Dll path that you use to inject
 */
CDLL_Loader::CDLL_Loader(DWORD Pid,  char* DllPath): 
	ProcessID(Pid), DllFilePath(DllPath), init(true), injected(false) 
{
}

/* ====================================
 *  CDLL_Loader::~CDLL_Loader()
 *
 *	(*) Destructor
 */
CDLL_Loader::~CDLL_Loader()
{
	//EjectDll();

	ProcessID = 0;
	DllFilePath = 0;
	
	init = false;
	injected = false;
}

/* ===========================================================
 *  DLL_Results CDLL_Loader::InitDLL(DWORD Pid,  char* DllPath)
 *
 *	Parameters: 
 *	Pid - Process ID to inject
 *	DllPath - The Dll path that you use to inject
 */
DLL_Results CDLL_Loader::InitDLL(DWORD Pid,  char* DllPath)
{
	if (isInit())
		return INIT_OK;

	ProcessID = Pid;
	DllFilePath = DllPath;

	init = true;
	injected = false;

	return INIT_OK;
}

/* ===========================================================
 *  DWORD CDLL_Loader::GetProcessIdByName(char* process_name)
 *
 *  Return Value
 *	If the function succeeds, it returns the Process ID
 *	If the function fails, it will return NULL
 */
DWORD CDLL_Loader::GetProcessIdByName(char* process_name)
{
	HANDLE hProcess;
	PROCESSENTRY32 ProcessEntry = {0};
	ProcessEntry.dwSize = sizeof(PROCESSENTRY32);

	hProcess = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcess == INVALID_HANDLE_VALUE)
		return NULL;

	if (Process32First(hProcess, &ProcessEntry) != FALSE)
	{
		do {
			if (strcmp(process_name, String_Convertion::WStringToString(ProcessEntry.szExeFile).c_str() ) == 0)
			{
				return ProcessEntry.th32ProcessID;
			}
		} while (Process32Next(hProcess, &ProcessEntry) != FALSE);

	}

	CloseHandle(hProcess);
	return NULL;
}

/* ===========================================================
 *  DLL_Results CDLL_Loader::InjectDll()
 *
 *  Injecting the dll into the process
 *	Return Values:
 *	If the function succeeds, it returns the value DLLINJECT_OK
 *	If the function fails:
 *	NOT_INIT - Not initliazed
 *  DLL_ALREADY_INJECTED - The dll was already injected
 *  PROCESS_ERROR_OPEN - Unable to open process ID
 *  PROCESS_ERRORR_VALLOC - Unable to virtual alloc 
 *  PROCESS_ERROR_WRITE - Unable to write process memory
 *	PROCESS_ERROR_CREATE_RTHREAD - Unable to create remote thread
 */

DLL_Results CDLL_Loader::InjectDll()
{
	LPVOID VirtualMem;
	HANDLE hProcess, hRemoteThread;
	HMODULE hModule;

	if (!isInit())
		return NOT_INIT;

	if (isInjected())
		return DLL_ALREADY_INJECTED;

	hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, ProcessID);
	std::cout << "Last Error: " << GetLastError() << std::endl;
	if (hProcess == NULL)
		return PROCESS_ERROR_OPEN;

	VirtualMem = VirtualAllocEx (hProcess, NULL, strlen(DllFilePath), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	if (VirtualMem == NULL)
		return PROCESS_ERRORR_VALLOC;

	if (WriteProcessMemory(hProcess, (LPVOID)VirtualMem, DllFilePath, strlen(DllFilePath), NULL) == 0)
	{
		VirtualFreeEx(hProcess, NULL, (size_t)strlen(DllFilePath), MEM_RESERVE|MEM_COMMIT);
		CloseHandle(hProcess); 
		return PROCESS_ERROR_WRITE;
	}

	hModule = GetModuleHandle(L"kernel32.dll");
	hRemoteThread = CreateRemoteThread(hProcess, NULL, 0, 
						(LPTHREAD_START_ROUTINE)GetProcAddress(hModule, "LoadLibraryA"),
						  (LPVOID)VirtualMem, 0, NULL);

	if (hRemoteThread == NULL)
	{
		FreeLibrary(hModule);
		VirtualFreeEx(hProcess, NULL, (size_t)strlen(DllFilePath), MEM_RESERVE | MEM_COMMIT);
		CloseHandle(hProcess); 
		return PROCESS_ERROR_CREATE_RTHREAD;
	}
	
	WaitForSingleObject(hRemoteThread, INFINITE);
	GetExitCodeThread(hRemoteThread, &ThreadTeminationStatus);
	FreeLibrary(hModule);

	VirtualFreeEx(hProcess, NULL, (size_t)strlen(DllFilePath), MEM_RESERVE | MEM_COMMIT);
	CloseHandle(hRemoteThread);
    CloseHandle(hProcess); 
	injected = true;
	return DLLINJECT_OK;
}

/* ===========================================================
 *  DLL_Results CDLL_Loader::EjectDll()
 *
 *  Ejecting the dll from the process
 *	Return Values:
 *	If the function succeeds, it returns the value DLLEJECT_OK
 *	If the function fails:
 *  PROCESS_ERROR_OPEN - Unable to open process ID
 */
DLL_Results CDLL_Loader::EjectDll()
{
	HANDLE hProcess, hRemoteThread;
	HMODULE hModule;

	if (isInjected())
		return DLLEJECT_OK;

	hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, ProcessID);
	if (hProcess == NULL)
		return PROCESS_ERROR_OPEN;

	hModule = GetModuleHandle(L"kernel32.dll");
	hRemoteThread = CreateRemoteThread(hProcess, NULL, 0, 
						(LPTHREAD_START_ROUTINE)GetProcAddress(hModule, "FreeLibrary"),
						  (LPVOID)ThreadTeminationStatus, 0, NULL);

	if (hRemoteThread != NULL)
	{
		WaitForSingleObject(hRemoteThread, INFINITE);
		GetExitCodeThread(hRemoteThread, &ThreadTeminationStatus);
	}

	CloseHandle(hRemoteThread);
    CloseHandle(hProcess); 
	injected = false;
	return DLLEJECT_OK;
}
