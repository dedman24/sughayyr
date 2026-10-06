#version 460 core

in vec2 textCoord;
out vec4 fragColour;

uniform sampler2D ourTexture;

void main(){
    fragColour = texture(ourTexture, textCoord);
}
