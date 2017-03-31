<?xml version="1.0" encoding="UTF-8"?>
<BSL>
	<GL>
		<Vert>
			<![CDATA[
uniform vec2 ScreenDim;
layout (location = 0) in vec2 Position;
layout (location = 4) in vec4 Color0;
layout (location = 8) in vec2 TexCoord;
out vec2 TexCoordVary;
out vec4 Color0Vary;
void main()
{
    Color0Vary = Color0;
    TexCoordVary = TexCoord;
    gl_Position = vec4(Position.x*ScreenDim.x - 1.0, 1.0 - Position.y*ScreenDim.y , 0.0 , 1.0);
}
            ]]>
		</Vert>
		<Frag>
			<![CDATA[
uniform sampler2D Texture0;
out vec4 FragColor;
in vec2 TexCoordVary;
in vec4 Color0Vary;
void main()
{
	if (Color0Vary.w < -0.5)
	{
		vec4 color = texture(Texture0 , TexCoordVary);	
		float gary = color.r*0.299 + color.g*0.587 + color.b*0.114;
		FragColor = vec4(gary, gary, gary, color.a);
	}
	else
	{
		vec4 color = (Color0Vary.z > -0.5) ? Color0Vary : vec4(1.0, 1.0, 1.0, texture(Texture0, Color0Vary.xy).a);
		FragColor = color * texture(Texture0, TexCoordVary) * vec4(1.0, 1.0, 1.0, Color0Vary.w);
	}
}
            ]]>
		</Frag>
	</GL>
    <DX>
    </DX>
    <VK>
    </VK>
    <MTL>
    </MTL>
</BSL>
