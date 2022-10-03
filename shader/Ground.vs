#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;
out vec4 WorldPos;

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;


void main() {
	gl_Position = Projection * View * Model * vec4(aPos, 1.0);
	TexCoord = aTexCoord; 
	vec4 pos = Model * vec4(aPos, 1.0);
    WorldPos = pos.xyzw / pos.w;
}
