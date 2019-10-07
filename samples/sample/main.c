

//#define USE_SOKOL
#if defined USE_SOKOL
#include "sokol_app.h"
#include "sokol_audio.h"
#include "sokol_gfx.h"


#define BUF_SIZE (32)

sg_pass_action pass_action; 
float buf[BUF_SIZE];
int buf_pos = 0;
uint32_t count = 0;

void init(void) {
	saudio_setup(&(saudio_desc) { 0 });
	sg_setup(&(sg_desc) {
		.mtl_device = sapp_metal_get_device(),
			.mtl_renderpass_descriptor_cb = sapp_metal_get_renderpass_descriptor,
			.mtl_drawable_cb = sapp_metal_get_drawable,
			.d3d11_device = sapp_d3d11_get_device(),
			.d3d11_device_context = sapp_d3d11_get_device_context(),
			.d3d11_render_target_view_cb = sapp_d3d11_get_render_target_view,
			.d3d11_depth_stencil_view_cb = sapp_d3d11_get_depth_stencil_view
	});
	pass_action = (sg_pass_action) {
		.colors[0] = { .action = SG_ACTION_CLEAR,.val = {1.0f, 0.0f, 0.0f, 1.0f} }
	};
}

void frame(void) {
	float g = pass_action.colors[0].val[1] + 0.01f;
	pass_action.colors[0].val[1] = (g > 1.0f) ? 0.0f : g;
	sg_begin_default_pass(&pass_action, sapp_width(), sapp_height());
	sg_end_pass();
	sg_commit();

		// generate and push audio samples...
		int num_frames = saudio_expect();
		for (int i = 0; i < num_frames; i++) {
			// simple square wave generator
			buf[buf_pos++] = (count++ & (1 << 3)) ? 0.5f : -0.5f;
			if (buf_pos == BUF_SIZE) {
				buf_pos = 0;
				saudio_push(buf, BUF_SIZE);
			}
		}

}

void cleanup(void) {
	sg_shutdown();
}

sapp_desc sokol_main(int argc, char* argv[]) {
	return (sapp_desc) {
		.init_cb = init,
			.frame_cb = frame,
			.cleanup_cb = cleanup,
			.width = 400,
			.height = 600,
			.window_title = "Clear (sokol app)",
	};
}

#else
#include "../../code/headers/prerequisites.h"
#include "../../code/headers/sprite.h"
#include "../../code/headers/system.h"
//tmx
//ui

//framebuffer
//srgb
BLGuid s1;

const char* code = "SSize=110\n\
SSizeV=10\n\
ESize=40\n\
ESizeV=0\n\
SSpin=720\n\
SSpinV=0\n\
ESpin=0\n\
ESpinV=0\n\
SR=255\n\
SG=51\n\
SB=120\n\
SA=153\n\
SRV=25\n\
SGV=0\n\
SBV=0\n\
SAV=50\n\
ER=255\n\
EG=0\n\
EB=0\
EA=0\n\
ERV=0\n\
EGV=0\n\
EBV=0\n\
EAV=0\n\
Amount=289\n\
Duration=-1.0\n\
Life=1.1\n\
LifeV=0.4\n\
Velocity=262\n\
Angle=146\n\
AngleV=50\n\
Mode=Gravity\n\
GVelocity=244\n\
GVelocityV=5\n\
GEmitterW=15\n\
GEmitterH=-20\n\
GX=-1000\n\
GY=740\n\
GRad=0\n\
GRadV=0\n\
GTan=0\n\
GTanV=0\n\
RMaxRadius=0\n\
RMaxRadiusV=0\n\
RMinRadius=0\n\
RRot=0\n\
RRotTime=0";
const void BeginSystem(void)
{
	float x[2] = { 2.0, 4.0 };
	float y[2] = { 2.0, 4.0 };
	blOpenPlugin("TMX");
	BLBool (*_open)(IN BLAnsi*,IN BLAnsi*) = (BLBool(*)(IN BLAnsi*, IN BLAnsi*))blPluginProcAddress("TMX", "blTMXFileEXT");
	_open("content/isometric_grass_and_water.tmx",NULL);


	//s1 = blGenSprite("sss.bmg", NULL, tag, 640, 480, 12, 6, 0);
	//s1 = blGenSprite("cursor.bmg", NULL, "cursor", 60, 60, 12, 1, 0);
	//s1 = blGenSprite("uv.bmg", NULL, NULL, 200, 200, 12, 1, 0);

	//blSpriteAsCursor(s1);
	//blSpritePivot(s1, 0, 0, 1);
	//blSpriteMove(s1, 400, 400);
	//blSpriteGlow(s1, 0xFFdd8805, 3, 0);
	//blSpriteFlip(s1, 1, 1, 1);
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

const void StepSystem(BLU32 del)
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
		0,
		1,
		2,
		BeginSystem, StepSystem, EndSystem);
	return 0;
	BL_MAINEND
#endif

