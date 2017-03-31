<?xml version="1.0" encoding="UTF-8"?>
<BSL>
    <GL>
        <Vert>
            <![CDATA[
uniform vec2 ScreenDim;
layout (location = 0) in vec2 Position;
layout (location = 4) in vec4 Color0;
layout (location = 8) in vec2 TexCoord;
layout (location = 5) in vec4 InstColor;
layout (location = 14) in vec4 InstXYRS;
out vec2 TexCoordVary;
out vec4 Color0Vary;
void main()
{
    Color0Vary = Color0;
    TexCoordVary = TexCoord;
    vec2 TPos = vec2(Position.x * cos(InstXYRS.z) - Position.y * sin(InstXYRS.z), Position.x * sin(InstXYRS.z) + Position.y * cos(InstXYRS.z));
    TPos = vec2(TPos.x * InstXYRS.w + InstXYRS.x , TPos.y * InstXYRS.w + InstXYRS.y);
    gl_Position = vec4((TPos.x)*ScreenDim.x - 1.0, 1.0 - (TPos.y)*ScreenDim.y , 0.0 , 1.0);
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
    FragColor = Color0Vary * texture(Texture0 , TexCoordVary);
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
