


#include "Hooker.h"

int main(int argc, char* argv[])
{
    BYTE shellCodeStart[] = { 0x48,0xB8 };// MOV RAX , XXX 
    BYTE shellCodeEnd[] = { 0xFF,0xE0 }; //jmp RAX 

    DWORD targetPid = 6196;
    Hooker* hooker = new Hooker(L"hookdll.dll", targetPid);
    
    hooker->Inject();
    delete hooker;

    return 0;
}

