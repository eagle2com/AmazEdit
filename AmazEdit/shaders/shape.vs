#version 330 core
layout (location = 0) in vec2 position; // The position variable has attribute position 0
  
out vec4 vertexColor; // Specify a color output to the fragment shader

uniform vec4 fill_color;
uniform mat4 model;
uniform mat4 projection;

void main()
{
    gl_Position = projection * model * vec4(position, 0.0, 1.0);
    vertexColor = fill_color;
}