#version 330 core

layout (location = 0) in vec3 position;
out vec3 TexCoords;
uniform mat4 Perspective;
uniform mat4 ModelView;

void main() {
    TexCoords = position;
    vec4 new_pos = Perspective * ModelView * vec4(position, 1.0);
    gl_Position = new_pos.xyww;
}
