#include "Hooker.h"

Hooker::Hooker(const wstring& dllName, DWORD pid)
{
    mDllName = dllName;
    mTargetPid = pid;
    memset(szPath, 0, MAX_PATH);
    GetModuleFileName(NULL, szPath, MAX_PATH); 
    int iLen = wcslen(szPath);
    for (int i = iLen - 1; i >= 0; --i)
    {
        if ('\\' == szPath[i])
        {
            szPath[i] = '\0';
            break;
        }
    }
    wcscat_s(szPath, MAX_PATH, L"\\hookdll.dll"); 
}


Hooker::~Hooker()
{
    if (pHandle != NULL)
    {
        CloseHandle(pHandle);
    }

}
void Hooker::Eject()
{
    pHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, mTargetPid);
    if (pHandle == NULL)
    {
        int errCode = GetLastError();
        printf("OpenProcess Error Code: %d\n", errCode);
        return;
    }


    PVOID function = NULL; 
    LPVOID buffer = VirtualAllocEx(pHandle, NULL, MAX_PATH, MEM_COMMIT, PAGE_READWRITE); 

    if (buffer == NULL) 
    {
        int errCode = GetLastError(); 
        printf("VirtualAllocEx Error Code: %d\n", errCode); 
        return;
    } 
    WriteProcessMemory(pHandle, buffer, szPath, MAX_PATH, NULL); 


    function = GetProcAddress(GetModuleHandle(L"kernel32.dll"), "FreeLibrary"); 
    HANDLE pThread = NULL;
    if (CreateRemoteThread(pHandle, NULL, 0, (LPTHREAD_START_ROUTINE)function, buffer, 0, NULL) == NULL) 
    {
        int errCode = GetLastError(); 
        printf("CreateRemoteThread Error Code: %d\n", errCode);
        return;
    }
     
    WaitForSingleObject(pThread, INFINITE); 

}

void Hooker::Inject()
{
    pHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, mTargetPid);
    if (pHandle == NULL)
    {
        int errCode = GetLastError();
        printf("OpenProcess Error Code: %d\n", errCode);
        return;
    }
    
    
    PVOID function = NULL;
    LPVOID buffer = VirtualAllocEx(pHandle, NULL, MAX_PATH, MEM_COMMIT, PAGE_READWRITE);
    
    if (buffer == NULL)
    {
        int errCode = GetLastError();
        printf("VirtualAllocEx Error Code: %d\n", errCode);
        return;
    }
    
    WriteProcessMemory(pHandle, buffer, szPath, MAX_PATH, NULL);

    function=GetProcAddress(GetModuleHandle(L"kernel32.dll"), "LoadLibraryW");
    HANDLE pThread=NULL;
    if (  CreateRemoteThread(pHandle, NULL, 0, (LPTHREAD_START_ROUTINE)function, buffer, 0, NULL)==NULL )
    {
        int errCode = GetLastError();
        printf("CreateRemoteThread Error Code: %d\n", errCode);
        return;
    }
    
    WaitForSingleObject(pThread, INFINITE);
    
}



