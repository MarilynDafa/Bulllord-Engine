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
#include "../../../../code/headers/gpu.h"
BLGuid tech;
BLGuid geo;
const void BeginSystem(void)
{
    tech = blGenTechnique("test.bsl", NULL, TRUE, FALSE);
    BLEnum _sem = BL_LL_POSITION;
    BLEnum decl = BL_VD_F32X2;

    BLF32 vbo2[] = {
        0 , 0,
        200 , 300,
        0 , 300,
    };
    BLU16 ib[] = {0 ,1, 2};
    //gl_Position = vec4(position.x*2.0*screensz.x - 1.0, 1.0 - position.y*2.0*screensz.y , 0.0 , 1.0);
    geo = blGenGeometryBuffer(0, BL_PT_TRIANGLES, FALSE, &_sem, &decl, 1, vbo2, sizeof(vbo2), ib, sizeof(ib), BL_IF_16);
    //blUpdateConstBuffer(BL_CB_SCREENSZ, sz);
}

const void StepSystem(BLF32 _delta)
{
    BLF32 sz[] = {2.0/500.0, 2.0/480};
    blTechUniform(tech, BL_UB_F32X2, "screensz", sz, sizeof(sz));
    
    
    blClearFrameBuffer(INVALID_GUID, 1,1,0);
    blDraw(tech, geo);
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
