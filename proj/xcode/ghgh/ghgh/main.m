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

void BeginSystem(void)
{
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
                800,
                480,
                FALSE,
                FALSE,
                2,
                BeginSystem,
                StepSystem,
                EndSystem);
    return 0;
}