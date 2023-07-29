#pragma once 
#include <Windows.h>
#include <iostream>
#include <string>
using namespace std;

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

