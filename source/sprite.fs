#version 330 core
in vec2 TexCoords;
in vec2 VertexCoords;
out vec4 color;
uniform sampler2D image;
uniform vec3 spriteColor;
uniform vec2 PlayerPos;
uniform int LightOn;
void main() {
    vec2 lightPos = vec2(PlayerPos.x + 15.0f, PlayerPos.y + 15.0f);
    float Distance = distance(VertexCoords, lightPos);
    float attenuation = (1.0) / (1 + 0.02 * Distance);
    if(LightOn == 1) {
        attenuation = 1;
    }
    vec4 light = vec4(attenuation, attenuation, attenuation, attenuation);
    color = light * vec4(spriteColor, 1.0) * texture(image, TexCoords);

}