#include "vld.h"
#include "../../code/headers/sprite.h"
#include "../../code/headers/system.h"

BLGuid s1;
const void BeginSystem(void)
{
	float x[2] = { 2.0, 4.0 };
	float y[2] = { 2.0, 4.0 };
	const char* tag = "a1,a2,a3,a4,a5,a6,a7,a8,a9";
	s1 = blGenSprite("sss.bmg", NULL, tag, 640, 480, 12, 6, 0);
	blSpritePivot(s1, 0, 0, 1);
	//blSpriteMove(s1, 400, 400);
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
	blDeleteSprite(s1);
}


BL_MAIN(argc, argv)
{
	blSystemRun(
		"fdasdsfa23",
		1280,
		720,
		FALSE,
		1,
		2,
		BeginSystem, StepSystem, EndSystem);
	return 0;
}

