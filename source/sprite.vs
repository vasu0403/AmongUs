#version 330 core
layout (location = 0) in vec4 vertex; // <vec2 position, vec2 texCoords>
out vec2 TexCoords;
out vec2 VertexCoords;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
void main(){
    VertexCoords = (model * vec4(vertex.xy, 0.0, 1.0)).xy;
    TexCoords = vertex.zw;
    // gl_Position = projection * model * vec4(vertex.xy, 0.0, 1.0);
    gl_Position = projection * view * model * vec4(vertex.xy, 0.0, 1.0);
}