#version 440
layout(location = 0) in vec2 qt_TexCoord0;
layout(location = 0) out vec4 fragColor;
layout(binding = 1) uniform sampler2D source;
layout(std140, binding = 0) uniform buf {
	mat4 qt_Matrix;
	float qt_Opacity;
} ubuf;
void main()
{
	vec4 pixelColor = texture(source, qt_TexCoord0);
	float bw = dot(pixelColor.xyz, vec3(0.344, 0.5, 0.156));
	fragColor = vec4(bw, bw, bw, pixelColor.a) * ubuf.qt_Opacity;
}
