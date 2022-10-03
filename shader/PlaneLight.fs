#version 330 core

out vec4 FragColor;

uniform float LightIntensity;
uniform vec3 LightSourceColor;

void main()
{
	FragColor = vec4(LightIntensity * LightSourceColor, 1.0);
}