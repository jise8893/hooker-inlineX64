


#include "Hooker.h"

int main(int argc, char* argv[])
{


    DWORD targetPid = 8732;
    Hooker* hooker = new Hooker(WDLL_NAME, targetPid);
    
    //hooker->Eject(); 
    hooker->Inject();  
    delete hooker; 

    return 0;
}

