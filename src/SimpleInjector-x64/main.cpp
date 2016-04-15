#include "stdafx.h"
#include <Windows.h>
#include <TlHelp32.h>

HMODULE GetRemoteModuleHandleA(DWORD dwProcessId, const char* szModule)
{
    HANDLE tlh = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwProcessId);
    MODULEENTRY32 modEntry;
    modEntry.dwSize = sizeof(MODULEENTRY32);
    Module32First(tlh, &modEntry);

    do {
        if (_stricmp(szModule, modEntry.szModule) == 0) {
            CloseHandle(tlh);
            return modEntry.hModule;
        }
    } while (Module32Next(tlh, &modEntry));

    CloseHandle(tlh);
    return NULL;
}

DWORD GetProcessIdFromProcessName(const char* szProcessName)
{
    HANDLE tlh = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
    PROCESSENTRY32 procEntry;
    procEntry.dwSize = sizeof(PROCESSENTRY32);
    Process32First(tlh, &procEntry);

    do {
        printf("proc: %s\n", procEntry.szExeFile);

        if (_stricmp(szProcessName, procEntry.szExeFile) == 0) {
            CloseHandle(tlh);
            return procEntry.th32ProcessID;
        }
    } while (Process32Next(tlh, &procEntry));

    CloseHandle(tlh);
    return GetCurrentProcessId();
}

int main(int argc, char* argv[])
{
    DWORD dwProcessId = GetCurrentProcessId();
    char szProcessName[256] = {0};
    char szModuleName[MAX_PATH] = {0};

    for (int i = 1; i < argc; i++) {
        if (_stricmp(argv[i], "-pid") == 0 && i < (argc - 1)) {
            dwProcessId = atoi(argv[i + 1]);
        }

        if (_stricmp(argv[i], "-name") == 0 && i < (argc - 1)) {
            strcpy_s(szProcessName, argv[i + 1]);
        }

        if (_stricmp(argv[i], "-dll") == 0 && i < (argc - 1)) {
            strcpy_s(szModuleName, argv[i + 1]);
        }
    }

    if (strlen(szModuleName) == 0) {
        printf("Module name is required...\n");
        return 0;
    }

    if (strlen(szProcessName) == 0 && dwProcessId == GetCurrentProcessId()) {
        printf("Invalid parameters!\n");
        return 0;
    }

    if (strlen(szProcessName) > 0) {
        if (dwProcessId == GetCurrentProcessId()) { // Only change the processid if it's not already set
            dwProcessId = GetProcessIdFromProcessName(szProcessName);

            if (dwProcessId == GetCurrentProcessId()) {
                printf("Failed to obtain process \"%s\"...\n", szProcessName);
                return 0;
            }
        }
    }

    HMODULE hKernel = LoadLibraryA("kernel32.dll");
    DWORD64 dwLoadLibraryA = (DWORD64) GetProcAddress(hKernel, "LoadLibraryA") - (DWORD64) hKernel;
    printf("kernel32.dll: %016llX\n", hKernel);
    printf("LoadLibraryA: %016llX\n", dwLoadLibraryA);
    printf("Module Name: %s\n", szModuleName);
    char szCurrentModulePath[MAX_PATH] = {0};
    GetModuleFileNameA(GetModuleHandle(NULL), szCurrentModulePath, MAX_PATH);

    for (size_t i = strlen(szCurrentModulePath); i > 0; i--) {
        if (szCurrentModulePath[ i ] == '\\') {
            szCurrentModulePath[ i + 1 ] = 0;
            break;
        }
    }

    strcat_s(szCurrentModulePath, szModuleName);
    printf("Full Path: %s\n", szCurrentModulePath);
    DWORD dwFileAttributes = GetFileAttributesA(szCurrentModulePath);

    if (dwFileAttributes == INVALID_FILE_ATTRIBUTES && GetLastError() == ERROR_FILE_NOT_FOUND) {
        printf("File not found...\n");
        return 0;
    }

    printf("Injecting: %s\n", szCurrentModulePath);
    HMODULE hRemoteKernel = GetRemoteModuleHandleA(dwProcessId, "kernel32.dll");

    if (hRemoteKernel == NULL) {
        printf("Failed to locate kernel32 in remote process...\n");
        return 0;
    }

    printf("kernel32 (remote): 0x%016llX\n", hRemoteKernel);
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcessId);

    if (hProcess == INVALID_HANDLE_VALUE) {
        printf("Failed to locate remote process...\n");
        return 0;
    }

    LPVOID lpModuleName = VirtualAllocEx(hProcess, NULL, strlen(szCurrentModulePath) + 1, MEM_COMMIT, PAGE_EXECUTE_READWRITE);

    if (lpModuleName == NULL) {
        printf("Failed to allocate module name in remote process...\n");
        return 0;
    }

    if (WriteProcessMemory(hProcess, lpModuleName, szCurrentModulePath, strlen(szCurrentModulePath), NULL) == FALSE) {
        printf("Failed to write module name in remote process...\n");
        return 0;
    }

    DWORD64 dwRemoteLoadLibraryAddress = ((DWORD64)hRemoteKernel + dwLoadLibraryA);
    printf("LoadLibraryA (remote): %016llX\n", dwRemoteLoadLibraryAddress);
    HANDLE hThread = CreateRemoteThread(hProcess, 0, 0, (LPTHREAD_START_ROUTINE) dwRemoteLoadLibraryAddress, lpModuleName, 0, 0);
    printf("Injecting... ");
    WaitForSingleObject(hThread, INFINITE);
    printf("Injected!\n");
    return 0;
}