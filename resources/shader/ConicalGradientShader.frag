#version 440
layout(location = 0) in vec2 qt_TexCoord0;
layout(location = 0) out vec4 fragColor;
layout(binding = 1) uniform sampler2D source;
layout(binding = 2) uniform sampler2D gradientSource;
layout(std140, binding = 0) uniform buf {
	mat4 qt_Matrix;
	float qt_Opacity;
	float startAngle;
	vec2 center;
} ubuf;
void main()
{
	float maskAlpha = texture(source, qt_TexCoord0).a;
	const float PI = 3.14159265;
	const float PIx2inv = 0.1591549;
	float a = (atan((ubuf.center.y - qt_TexCoord0.t), (ubuf.center.x - qt_TexCoord0.s)) + PI - ubuf.startAngle) * PIx2inv;
	fragColor = texture(gradientSource, vec2(0.0, fract(a))) * maskAlpha * ubuf.qt_Opacity;
}
