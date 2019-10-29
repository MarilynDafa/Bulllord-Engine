const char* SHADER_2D_GLOW = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\
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
uniform vec4 Glow;\
uniform vec2 TexSize;\
uniform vec4 Border;\
out vec4 FragColor;\
in vec2 TexCoordVary;\
in vec4 Color0Vary;\
vec4 Texture2D(sampler2D Tex, vec2 Coord)\
{\
	if ((Coord.x * TexSize.x > Border.x) && (Coord.y * TexSize.y > Border.y) && (Coord.x * TexSize.x < Border.z) && (Coord.y * TexSize.y < Border.w))\
		return texture(Tex, Coord);\
	else\
		return vec4(0.0);\
}\
void main()\
{\
	vec4 preClr = Texture2D(Texture0, TexCoordVary);\
	if (preClr.a >= 0.5)\
	{\
		FragColor = Color0Vary * preClr*2;\
		return;\
	}\
	FragColor =  Texture2D(Texture0, vec2(TexCoordVary.x - 3.0 / TexSize.x, TexCoordVary.y - 3.0 / TexSize.y)) * 0.09;\
	FragColor += Texture2D(Texture0, vec2(TexCoordVary.x - 3.0 / TexSize.x, TexCoordVary.y - 2.0 / TexSize.y)) * 0.09;\
	FragColor += Texture2D(Texture0, vec2(TexCoordVary.x - 3.0 / TexSize.x, TexCoordVary.y - 1.0 / TexSize.y)) * 0.09;\
	FragColor += Texture2D(Texture0, vec2(TexCoordVary.x - 3.0 / TexSize.x, TexCoordVary.y - 0.0 / TexSize.y)) * 0.09;\
	FragColor += Texture2D(Texture0, vec2(TexCoordVary.x - 3.0 / TexSize.x, TexCoordVary.y + 1.0 / TexSize.y)) * 0.09;\
	FragColor += Texture2D(Texture0, vec2(TexCoordVary.x - 3.0 / TexSize.x, TexCoordVary.y + 2.0 / TexSize.y)) * 0.09;\
	FragColor += Texture2D(Texture0, vec2(TexCoordVary.x - 3.0 / TexSize.x, TexCoordVary.y + 3.0 / TexSize.y)) * 0.09;	\
	FragColor += Texture2D(Texture0, vec2(TexCoordVary.x - 2.0 / TexSize.x, TexCoordVary.y - 3.0 / TexSize.y)) * 0.12;\
	FragColor += Texture2D(Texture0, vec2(TexCoordVary.x - 2.0 / TexSize.x, TexCoordVary.y - 2.0 / TexSize.y)) * 0.12;\
	FragColor += Texture2D(Texture0, vec2(TexCoordVary.x - 2.0 / TexSize.x, TexCoordVary.y - 1.0 / TexSize.y)) * 0.12;\
	FragColor += Texture2D(Texture0, vec2(TexCoordVary.x - 2.0 / TexSize.x, TexCoordVary.y - 0.0 / TexSize.y)) * 0.12;\
	FragColor += Texture2D(Texture0, vec2(TexCoordVary.x - 2.0 / TexSize.x, TexCoordVary.y + 1.0 / TexSize.y)) * 0.12;\
	FragColor += Texture2D(Texture0, vec2(TexCoordVary.x - 2.0 / TexSize.x, TexCoordVary.y + 2.0 / TexSize.y)) * 0.12;\
	FragColor += Texture2D(Texture0, vec2(TexCoordVary.x - 2.0 / TexSize.x, TexCoordVary.y + 3.0 / TexSize.y)) * 0.12;	\
	FragColor += Texture2D(Texture0, vec2(TexCoordVary.x - 1.0 / TexSize.x, TexCoordVary.y - 3.0 / TexSize.y)) * 0.15;\
	FragColor += Texture2D(Texture0, vec2(TexCoordVary.x - 1.0 / TexSize.x, TexCoordVary.y - 2.0 / TexSize.y)) * 0.15;\
	FragColor += Texture2D(Texture0, vec2(TexCoordVary.x - 1.0 / TexSize.x, TexCoordVary.y - 1.0 / TexSize.y)) * 0.15;\
	FragColor += Texture2D(Texture0, vec2(TexCoordVary.x - 1.0 / TexSize.x, TexCoordVary.y - 0.0 / TexSize.y)) * 0.15;\
	FragColor += Texture2D(Texture0, vec2(TexCoordVary.x - 1.0 / TexSize.x, TexCoordVary.y + 1.0 / TexSize.y)) * 0.15;\
	FragColor += Texture2D(Texture0, vec2(TexCoordVary.x - 1.0 / TexSize.x, TexCoordVary.y + 2.0 / TexSize.y)) * 0.15;\
	FragColor += Texture2D(Texture0, vec2(TexCoordVary.x - 1.0 / TexSize.x, TexCoordVary.y + 3.0 / TexSize.y)) * 0.15;	\
	FragColor += Texture2D(Texture0, vec2(TexCoordVary.x + 1.0 / TexSize.x, TexCoordVary.y - 3.0 / TexSize.y)) * 0.15;\
	FragColor += Texture2D(Texture0, vec2(TexCoordVary.x + 1.0 / TexSize.x, TexCoordVary.y - 2.0 / TexSize.y)) * 0.15;\
	FragColor += Texture2D(Texture0, vec2(TexCoordVary.x + 1.0 / TexSize.x, TexCoordVary.y - 1.0 / TexSize.y)) * 0.15;\
	FragColor += Texture2D(Texture0, vec2(TexCoordVary.x + 1.0 / TexSize.x, TexCoordVary.y - 0.0 / TexSize.y)) * 0.15;\
	FragColor += Texture2D(Texture0, vec2(TexCoordVary.x + 1.0 / TexSize.x, TexCoordVary.y + 1.0 / TexSize.y)) * 0.15;\
	FragColor += Texture2D(Texture0, vec2(TexCoordVary.x + 1.0 / TexSize.x, TexCoordVary.y + 2.0 / TexSize.y)) * 0.15;\
	FragColor += Texture2D(Texture0, vec2(TexCoordVary.x + 1.0 / TexSize.x, TexCoordVary.y + 3.0 / TexSize.y)) * 0.15;	\
	FragColor += Texture2D(Texture0, vec2(TexCoordVary.x + 2.0 / TexSize.x, TexCoordVary.y - 3.0 / TexSize.y)) * 0.12;\
	FragColor += Texture2D(Texture0, vec2(TexCoordVary.x + 2.0 / TexSize.x, TexCoordVary.y - 2.0 / TexSize.y)) * 0.12;\
	FragColor += Texture2D(Texture0, vec2(TexCoordVary.x + 2.0 / TexSize.x, TexCoordVary.y - 1.0 / TexSize.y)) * 0.12;\
	FragColor += Texture2D(Texture0, vec2(TexCoordVary.x + 2.0 / TexSize.x, TexCoordVary.y - 0.0 / TexSize.y)) * 0.12;\
	FragColor += Texture2D(Texture0, vec2(TexCoordVary.x + 2.0 / TexSize.x, TexCoordVary.y + 1.0 / TexSize.y)) * 0.12;\
	FragColor += Texture2D(Texture0, vec2(TexCoordVary.x + 2.0 / TexSize.x, TexCoordVary.y + 2.0 / TexSize.y)) * 0.12;\
	FragColor += Texture2D(Texture0, vec2(TexCoordVary.x + 2.0 / TexSize.x, TexCoordVary.y + 3.0 / TexSize.y)) * 0.12;	\
	FragColor += Texture2D(Texture0, vec2(TexCoordVary.x + 3.0 / TexSize.x, TexCoordVary.y - 3.0 / TexSize.y)) * 0.09;\
	FragColor += Texture2D(Texture0, vec2(TexCoordVary.x + 3.0 / TexSize.x, TexCoordVary.y - 2.0 / TexSize.y)) * 0.09;\
	FragColor += Texture2D(Texture0, vec2(TexCoordVary.x + 3.0 / TexSize.x, TexCoordVary.y - 1.0 / TexSize.y)) * 0.09;\
	FragColor += Texture2D(Texture0, vec2(TexCoordVary.x + 3.0 / TexSize.x, TexCoordVary.y - 0.0 / TexSize.y)) * 0.09;\
	FragColor += Texture2D(Texture0, vec2(TexCoordVary.x + 3.0 / TexSize.x, TexCoordVary.y + 1.0 / TexSize.y)) * 0.09;\
	FragColor += Texture2D(Texture0, vec2(TexCoordVary.x + 3.0 / TexSize.x, TexCoordVary.y + 2.0 / TexSize.y)) * 0.09;\
	FragColor += Texture2D(Texture0, vec2(TexCoordVary.x + 3.0 / TexSize.x, TexCoordVary.y + 3.0 / TexSize.y)) * 0.09;	\
	FragColor += Texture2D(Texture0, vec2(TexCoordVary.x + 0.0 / TexSize.x, TexCoordVary.y - 3.0 / TexSize.y)) * 0.16;\
	FragColor += Texture2D(Texture0, vec2(TexCoordVary.x + 0.0 / TexSize.x, TexCoordVary.y - 2.0 / TexSize.y)) * 0.16;\
	FragColor += Texture2D(Texture0, vec2(TexCoordVary.x + 0.0 / TexSize.x, TexCoordVary.y - 1.0 / TexSize.y)) * 0.16;\
	FragColor += Texture2D(Texture0, vec2(TexCoordVary.x + 0.0 / TexSize.x, TexCoordVary.y + 1.0 / TexSize.y)) * 0.16;\
	FragColor += Texture2D(Texture0, vec2(TexCoordVary.x + 0.0 / TexSize.x, TexCoordVary.y + 2.0 / TexSize.y)) * 0.16;\
	FragColor += Texture2D(Texture0, vec2(TexCoordVary.x + 0.0 / TexSize.x, TexCoordVary.y + 3.0 / TexSize.y)) * 0.16;\
	FragColor += preClr * 0.16;	\
	FragColor.rgb = Glow.rgb * FragColor.a * Glow.a / 9.0;\
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