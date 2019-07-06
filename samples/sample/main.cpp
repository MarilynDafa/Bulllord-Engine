#include "../../code/headers/prerequisites.h"
#include "../../code/headers/sprite.h"
#include "../../code/headers/system.h"
//tmx
//ui

//framebuffer
//srgb
BLGuid s1;
const void BeginSystem(void)
{
	float x[2] = { 2.0, 4.0 };
	float y[2] = { 2.0, 4.0 };
	const char* tag = "a1,a2,a3,a4,a5,a6,a7,a8,a9";
	//s1 = blGenSprite("sss.bmg", NULL, tag, 640, 480, 12, 6, 0);
	//s1 = blGenSprite("cursor.bmg", NULL, "cursor", 60, 60, 12, 1, 0);
	//s1 = blGenSprite("uv.bmg", NULL, NULL, 200, 200, 12, 1, 0);
	
	//blSpriteAsCursor(s1);
	//blSpritePivot(s1, 0, 0, 1);
	//blSpriteMove(s1, 400, 400);
	//blSpriteGlow(s1, 0xFFdd8805, 3, 0);
	//blSpriteFlip(s1, 1, 1, 1);
	//blOpenPlugin("blTMX");

	//BLBool (*_open)(IN BLAnsi*,IN BLAnsi*) = (BLBool(*)(IN BLAnsi*, IN BLAnsi*))blGetPluginProcAddress("blTMX", "blTMXFileEXT");
	//blWorkingDir(FALSE);
	//_open("m2.tmx",NULL);
	//blSpriteScissor(s1, 0, 0, 390, 390);
	//blSpriteAlpha(s1, 0.1, 1);
	// = blGenSprite("s1", "", "s1", 12, 12, 23, 1);
			  /*
			  blSpriteParallelBegin(s1);
			  blSpriteActionMove(s1, x, y, 2, 0, 5, 0);
			  blSpriteActionMove(s1, x, y, 2, 0, 5, 0);
			  blSpriteActionMove(s1, x, y, 2, 0, 5, 0);
			  blSpriteParallelEnd(s1);
			  blSpriteActionMove(s1, x, y, 2, 0, 5, 0);
			  blSpriteParallelBegin(s1);
			  blSpriteActionRotate(s1, 360, 2, 0);
			  blSpriteParallelEnd(s1);
			  dump();

			  blDeleteSprite(s1);

			  */


}

const void StepSystem(BLU32)
{
}

const void EndSystem(void)
{
}


BL_MAINBEGIN(argc, argv)
	blSystemRun(
		"fdasdsfa23",
		480,
		800,
		480,
		800,
		1,
		FALSE,
		1,
		2,
		BeginSystem, StepSystem, EndSystem);
	return 0;
	BL_MAINEND

