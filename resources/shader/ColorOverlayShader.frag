varying highp vec2 qt_TexCoord0;
uniform lowp sampler2D source;
uniform highp float qt_Opacity;
uniform highp vec4 color;

void main() {
	highp vec4 pixelColor = texture2D(source, qt_TexCoord0);
	gl_FragColor = vec4(mix(pixelColor.rgb/max(pixelColor.a, 0.00390625), color.rgb/max(color.a, 0.00390625), color.a) * pixelColor.a, pixelColor.a) * qt_Opacity;
}
