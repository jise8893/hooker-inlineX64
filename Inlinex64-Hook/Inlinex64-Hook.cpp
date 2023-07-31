


#include "Hooker.h"

int main(int argc, char* argv[])
{


    DWORD targetPid = 22368;
    Hooker* hooker = new Hooker(WDLL_NAME, targetPid);
    
    //hooker->Eject(); 
    hooker->Inject();  
    delete hooker; 

    return 0;
}

