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

#include "../../../../code/headers/system.h"
#include "../../../../code/headers/audio.h"
const void BeginSystem(void)
{
}

const void StepSystem(BLF32 _delta)
{
}

const void EndSystem(void)
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
