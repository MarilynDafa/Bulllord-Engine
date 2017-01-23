// test.c'syspp : ∂®“Âøÿ÷∆Ã®”¶”√≥Ã–Úµƒ»Îø⁄µ„°£
//
#include "../../../code/headers/system.h"
#include "../../../code/headers/audio.h"
#include "../../../code/headers/streamio.h"
#include "../../../code/headers/network.h"
#include <vector>
int main()
{
    bulllord_param_t pa;
    pa.appname = (const utf8*)"kkk";
    pa.fullscreen = 0;
    pa.screenheight = 768;
    pa.screenwidth = 1024;
    pa.vsync = 1;
    pa.userdata = -1;
    blSystemBegin(pa);
    //blRegisterArchive("de.bpk", "music");
    //blPatchArchive("patch100.pat", "music");
    //blWindowResize(1440, 900, FALSE);
    //event_t ev;
    //ev.type = ET_NET;
    //blInvokeEvent(&ev);
    //blAddDownloadList("182.92.212.113", "md84", "A00790084z", "dump.rdb", "ss");
    //blPlayVideo("rrr.wmv");
    do {
        
    } while (blSystemStep());
    
    blSystmEnd();
    return 0;
}

