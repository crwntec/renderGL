#version 400
out vec4 frag_colour;
in vec3 oColor;
void main() {
	frag_colour = vec4(oColor, 1.0);
}