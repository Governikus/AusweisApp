#version 440
layout(location = 0) in vec2 qt_TexCoord0;
layout(location = 0) out vec4 fragColor;
layout(binding = 1) uniform sampler2D source;
layout(binding = 2) uniform sampler2D maskSource;
layout(std140, binding = 0) uniform buf {
	mat4 qt_Matrix;
	float qt_Opacity;
} ubuf;
void main()
{
	fragColor = texture(source, qt_TexCoord0.st) * texture(maskSource, qt_TexCoord0.st).a * ubuf.qt_Opacity;
}
