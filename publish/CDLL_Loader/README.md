DLL Loader / DLL Injections in C++ for Windows
==============

Intro
--------------
I wrote this project back in 2011 when I was playing a bit with DLL Injections.
The class is used to Inject DLL's into running process in Windows.

There are 4 files in this project
- CDLL_Loader.cpp
- CDLL_Loader.h
- String_Conversion.cpp
- String_Conversion.h

CDLL_Loader has methods to Load and Eject DLL from running process.
String Conversion is used to convert byte to string.


Usage Example
--------------
	CDLL_Loader loader;
	DWORD Pid = loader.GetProcessIdByName("cmd.exe");

	std::cout << "Cmd.exe PID:  " << Pid << std::endl;

	if (loader.InitDLL(Pid, "c://someDllFile.dll") == INIT_OK) {
		std::cout << "DLL Injected Succesfully!" << std::endl;
	}
