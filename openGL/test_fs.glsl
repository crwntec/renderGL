#version 400
out vec4 frag_colour;
uniform vec2 screenSize;
void main() {
	vec2 normCoords = vec2(gl_FragCoord.xy / screenSize);
	frag_colour = vec4(normCoords, 1, 1.0);
}