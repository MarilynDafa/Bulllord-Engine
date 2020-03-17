const char* SHADER_LINE = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\
<BSL>\
	<GL>\
		<Vert>\
			<![CDATA[\
layout (location = 0) in vec4 PosDist;\
layout (location = 4) in vec4 Color0;\
flat out vec3 StartPosVary;\
out vec3 VertPosVary;\
out vec4 Color0Vary;\
uniform mat4 MVP;\
void main()\
{\
    Color0Vary = Color0;\
    gl_Position = MVP * vec4(PosDist.xyz, 1.0);\
    VertPosVary = gl_Position.xyz / gl_Position.w;\
    StartPosVary = VertPosVary;\
}\
            ]]>\
		</Vert>\
		<Frag>\
			<![CDATA[\
flat in vec3 StartPosVary;\
in vec3 VertPosVary;\
in vec4 Color0Vary;\
uniform vec4 Param;\
out vec4 FragColor;\
void main()\
{\
    vec2 dir  = (VertPosVary.xy-StartPosVary.xy) * Param.zw / 2.0;\
    float dist = length(dir);\
    if (fract(dist / (Param.x + Param.y)) > Param.x/(Param.x + Param.y))\
        discard; \
    FragColor = Color0Vary;\
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
