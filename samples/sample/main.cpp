#include "../../code/headers/sprite.h"
#include "../../code/headers/system.h"


const void BeginSystem(void)
{
	float x[2] = { 2.0, 4.0 };
	float y[2] = { 2.0, 4.0 };
	
	BLGuid s1;// = blGenSprite("s1", "", "s1", 12, 12, 23, 1);
			  /*
			  blSpriteActionBegin(s1);
			  blSpriteParallelBegin(s1);
			  blSpriteActionMove(s1, x, y, 2, 0, 5, 0);
			  blSpriteActionMove(s1, x, y, 2, 0, 5, 0);
			  blSpriteActionMove(s1, x, y, 2, 0, 5, 0);
			  blSpriteParallelEnd(s1);
			  blSpriteActionMove(s1, x, y, 2, 0, 5, 0);
			  blSpriteParallelBegin(s1);
			  blSpriteActionRotate(s1, 360, 2, 0);
			  blSpriteActionRotate(s1, 360, 2, 0);
			  blSpriteParallelEnd(s1);
			  blSpriteActionEnd(s1, 1);
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


BL_MAIN(argc, argv)
{
	blSystemRun(
		"fdasdsfa23",
		1280,
		720,
		FALSE,
		FALSE,
		2,
		BeginSystem, StepSystem, EndSystem);
	return 0;
}

