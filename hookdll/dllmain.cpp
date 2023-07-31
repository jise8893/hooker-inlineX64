// dllmain.cpp : DLL 애플리케이션의 진입점을 정의합니다.
#include "pch.h"
#include <cstdio>
#include <cstdint>



void pack_little_endian(uint64_t& value, uint8_t* buffer) {
    for (int i = 0; i < 8; i++) {
        buffer[i] = (uint8_t)(value & 0xFF);
        value >>= 8;
    }
}
void pack32_little_endian(uint32_t& value, uint8_t* buffer) {
    for (int i = 0; i < 4; i++) {
        buffer[i] = (uint8_t)(value & 0xFF);
        value >>= 8;
    }
}
// 리틀엔디언 형식으로 패킹된 8바이트 바이트를 64비트 정수로 언패킹하는 함수
uint64_t unpack_little_endian(const uint8_t* buffer) {
    uint64_t value = 0;
    for (int i = 7; i >= 0; i--) {
        value <<= 8;
        value |= buffer[i];
    }
    return value;
}
// 원본 WriteFile 함수의 포인터
static BOOL(WINAPI* OriginalWriteFile)(
    HANDLE hFile,
    LPCVOID lpBuffer,
    DWORD nNumberOfBytesToWrite,
    LPDWORD lpNumberOfBytesWritten,
    LPOVERLAPPED lpOverlapped
    )=nullptr;

// 후킹할 WriteFile 함수
extern "C" __declspec(dllexport) BOOL WINAPI HookedWriteFile(
    HANDLE hFile,
    LPCVOID lpBuffer,
    DWORD nNumberOfBytesToWrite,
    LPDWORD lpNumberOfBytesWritten,
    LPOVERLAPPED lpOverlapped
);




BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    {
       HMODULE hMod = GetModuleHandle(L"kernel32.dll");

       PVOID functionAddress = GetProcAddress(hMod, "WriteFile");
       if (functionAddress == NULL)
       {
           
           TCHAR TEXT[100]; 
           wsprintf(TEXT, L"GETprocaddress Error %d", GetLastError()); 
           MessageBox(NULL, TEXT, NULL, NULL); 
           
       }
      

       BYTE shellCodeStart[] = { 0x48,0xB8 };// MOV RAX , XXX (주소 8바이트)
       BYTE shellCodeEnd[] = { 0xFF,0xE0 }; //jmp RAX  명령어 총 4바이트 합 12바이트
       BYTE originByte[12] = { 0, };
       memcpy(originByte, shellCodeStart, sizeof(shellCodeStart));
       
       

       //HookedWriteFile
       uint64_t value = (uint64_t) & HookedWriteFile;
       uint8_t buffer[8];
       pack_little_endian(value, buffer);

       memcpy(originByte + sizeof(shellCodeStart), buffer, sizeof(void*));
       memcpy(originByte + sizeof(shellCodeStart) + sizeof(void*), shellCodeEnd,sizeof(shellCodeEnd));

       DWORD oldProtect;


       // 함수 후킹 건곳에서 12바이트 떨어진 곳에 기존 점프 write [해당위치가 execute가 가능한  메모리로 확인]
       
       uint8_t copyBuffer[6];
       memcpy(copyBuffer, functionAddress, sizeof(copyBuffer));

       uint32_t rvaValue = 0;
       uint8_t rvaBuffer[4] = { 0, };
       uint32_t rvaAddress = 0;

       memcpy(rvaBuffer, (char*)functionAddress + sizeof(BYTE) * 2, sizeof(rvaBuffer));
       rvaValue = unpack_little_endian(rvaBuffer);
       rvaValue -=12; 
       pack32_little_endian(rvaValue, rvaBuffer);

       memcpy(copyBuffer + 2, rvaBuffer, sizeof(rvaBuffer));

      

   
       //Original Function 주소 입력
       OriginalWriteFile = reinterpret_cast<BOOL(WINAPI*)( 
           HANDLE hFile, 
           LPCVOID lpBuffer,  
           DWORD nNumberOfBytesToWrite, 
           LPDWORD lpNumberOfBytesWritten, 
           LPOVERLAPPED lpOverlapped 
           )>((char*)functionAddress+12); 
       
       
       

       if (VirtualProtect(functionAddress, (SIZE_T)12, PAGE_EXECUTE_READWRITE, &oldProtect) == FALSE)
       {
           TCHAR TEXT[100];
           wsprintf(TEXT, L"VirtualProtect Error %d", GetLastError()); 
           MessageBox(NULL, TEXT, NULL, NULL);
       }
       //후킹함수 작성
       if (WriteProcessMemory(GetCurrentProcess(), functionAddress, originByte, sizeof(originByte), NULL))
       {
           MessageBox(NULL,L"WriteProcessMemory Successed", NULL, NULL);
       }

       //Original function 작성
       if (WriteProcessMemory(GetCurrentProcess(), (char*)functionAddress+12, copyBuffer, sizeof(copyBuffer), NULL))
       {
           MessageBox(NULL, L"WriteProcessMemory2 Successed", NULL, NULL);
       }

    }
        break;
    case DLL_THREAD_ATTACH:
    {

    }
        break;
    case DLL_THREAD_DETACH:
    {

    }
        break;
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

extern "C" __declspec(dllexport) BOOL WINAPI HookedWriteFile(
    HANDLE hFile,
    LPCVOID lpBuffer,
    DWORD nNumberOfBytesToWrite,
    LPDWORD lpNumberOfBytesWritten,
    LPOVERLAPPED lpOverlapped
)
{
    // 원하는 동작 수행
    MessageBox(NULL, L"Hooked WriteFile!", L"Hook Message", MB_OK);

    // 원본 WriteFile 함수 호출
    return OriginalWriteFile(hFile, lpBuffer, nNumberOfBytesToWrite, lpNumberOfBytesWritten, lpOverlapped);  
}

