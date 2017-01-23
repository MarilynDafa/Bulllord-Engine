//
//  main.m
//  sample-osx
//
//  Created by 周博 on 16-9-30.
//  Copyright (c) 2016年 trix. All rights reserved.
//

//#import <Cocoa/Cocoa.h>

//int main(int argc, const char * argv[]) {
//    return NSApplicationMain(argc, argv);
//}

#import <OpenGLES/ES3/gl.h>

#include "../../../../code/headers/system.h"
#include "../../../../code/headers/network.h"
#include "../../../../code/headers/audio.h"

void BeginSystem(void)
{
    //blGenAudio("ss.mp3", NULL, FALSE, 0, 0, 0);
   // blUserFolderDir();
    //blPlayVideo("eeee.mp4");
    
    
    
    BLU32 task;
    //blAddDownloadList("http://pic.mguwp.com/sdk/1.0.3.8/MiracleGamesSDKWP8.1.zip", "f:", &task);
    //blAddDownloadList("http://dlsw.baidu.com/sw-search-sp/soft/d6/14991/jsws_4.7.0.4169_setup.1459839716.exe", "f:", &task);
    
    //blBeginDownload();
    //BLU32 xx = blGenAudio("3851.wav", NULL, FALSE, 0, 0, 0);
    //xx = blPlayMusic("ss.mp3", NULL);
    //blDeleteAudio(xx);
    //blPlayVideo("7878.wmv");
    
    //BLAnsi sjj[1025];
    //blUrlBlockRequest("http://114.215.106.44/auth/CheckUserLogin.php?username=ahsdkfja&password=111111",0, sjj);
    
    blAddDownloadList("http://trimorphgroup.com/Boreas955.db", "ghgh", &task);
    //blAddDownloadList("http://dlsw.baidu.com/sw-search-sp/soft/d6/14991/jsws_4.7.0.4169_setup.1459839716.exe", "f:", &task);
    
    
    
    blBeginDownload();
}

void StepSystem(BLF32 _delta)
{
}

void EndSystem(void)
{
}

BL_MAIN(argc, argv)
{
    blSystemRun(
                "ahdjkhfakjdf",
                500,
                480,
                0,
                FALSE,
                2,
                BeginSystem,
                StepSystem,
                EndSystem);
    return 0;
}