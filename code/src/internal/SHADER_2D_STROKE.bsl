const char* SHADER_2D_STROKE = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\
<BSL>\
	<GL>\
		<Vert>\
			<![CDATA[\
layout (std140) uniform UBO\
{\
	vec2 ScreenDim; \
}; \
layout (location = 0) in vec2 Position;\
layout (location = 4) in vec4 Color0;\
layout (location = 8) in vec2 TexCoord;\
out vec2 TexCoordVary;\
out vec4 Color0Vary;\
void main()\
{\
    Color0Vary = Color0;\
    TexCoordVary = TexCoord;\
    gl_Position = vec4(Position.x*ScreenDim.x - 1.0, 1.0 - Position.y*ScreenDim.y , 0.0 , 1.0);\
}\
            ]]>\
		</Vert>\
		<Frag>\
			<![CDATA[\
uniform sampler2D Texture0;\
uniform vec4 Stroke;\
uniform vec2 TexSize;\
uniform vec4 Border;\
out vec4 FragColor;\
in vec2 TexCoordVary;\
in vec4 Color0Vary;\
const float[] CosArray = float[](1.0, 0.866, 0.5, 0.0, -0.5, -0.866, -0.1, -0.866, -0.5, 0.0, 0.5, 0.866);\
const float[] SinArray = float[](0.0, 0.5, 0.866, 1.0, 0.866, 0.5, 0.0, -0.5, -0.866, -1.0, -0.866, -0.5);\
vec4 Texture2D(sampler2D Tex, vec2 Coord)\
{\
	if ((Coord.x * TexSize.x > Border.x + Stroke.a) && (Coord.y * TexSize.y > Border.y + Stroke.a) && (Coord.x * TexSize.x < Border.z - Stroke.a) && (Coord.y * TexSize.y < Border.w - Stroke.a))\
		return texture(Tex, Coord);\
	else\
		return vec4(0.0);\
}\
int IsStroke(int index)\
{\
	int stroke = 0;\
	float a = Texture2D(Texture0, vec2(TexCoordVary.x + Stroke.a * CosArray[index] / TexSize.x, TexCoordVary.y + Stroke.a * SinArray[index] / TexSize.y)).a;\
	if (a >= 0.5)\
	{\
		stroke = 1;\
	}\
	return stroke;\
}\
void main()\
{\
	vec4 preClr = Texture2D(Texture0, TexCoordVary);\
	if (preClr.a >= 0.5)\
	{\
		FragColor = Color0Vary * preClr;\
		return;\
	}\
	int strokeCount = 0;\
	strokeCount += IsStroke(0);\
	strokeCount += IsStroke(1);\
	strokeCount += IsStroke(2);\
	strokeCount += IsStroke(3);\
	strokeCount += IsStroke(4);\
	strokeCount += IsStroke(5);\
	strokeCount += IsStroke(6);\
	strokeCount += IsStroke(7);\
	strokeCount += IsStroke(8);\
	strokeCount += IsStroke(9);\
	strokeCount += IsStroke(10);\
	strokeCount += IsStroke(11);\
	bool stroke = false;\
	if (strokeCount > 0)\
	{\
		stroke = true;\
	}\
	if (stroke)\
	{\
		preClr.rgb = Stroke.rgb;\
		preClr.a = 1.0;\
	}\
	FragColor = Color0Vary * preClr;\
}\
            ]]>\
		</Frag>\
	</GL>\
    <DX>\
    </DX>\
    <VK>\
    </VK>\
    <MTL>\
    </MTL>\
</BSL>";