#version 330

uniform vec2  ViewTrans;
uniform float ViewScale;
uniform float ViewRatio;

in vec2 vertexXY;

out vec2 UV;

void main(void)
{
    UV.x = (vertexXY.x/ViewScale*ViewRatio + ViewTrans.x);
    UV.y = (vertexXY.y/ViewScale           + ViewTrans.y);
    gl_Position = vec4(vertexXY.x, vertexXY.y, 0.0f, 1.0f);
}
