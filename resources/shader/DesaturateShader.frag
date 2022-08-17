varying highp vec2 qt_TexCoord0;
uniform lowp sampler2D source;
uniform highp float qt_Opacity;

void main() {
	highp vec4 pixelColor = texture2D(source, qt_TexCoord0);
	highp float bw = dot(pixelColor.xyz, vec3(0.344, 0.5, 0.156));
	gl_FragColor = vec4(bw, bw, bw, pixelColor.a) * qt_Opacity;
}
