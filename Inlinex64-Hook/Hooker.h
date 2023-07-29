#pragma once 
#include <Windows.h>
#include <iostream>
#include <string>
#include <TlHelp32.h>
using namespace std;
#define WDLL_NAME L"hookdll.dll"
#define DLL_NAME hookdll.dll
#define PDLL_NAME L"\\hookdll.dll"
class Hooker
{ 
private:
    wstring mDllName;
    DWORD mTargetPid;
    HANDLE pHandle;
    TCHAR szPath[MAX_PATH];
public:
    Hooker(const wstring& dllName,DWORD pid);
    Hooker() = delete;
    ~Hooker();
public:
    void Inject();
    void Eject();
};

