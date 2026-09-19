#ifdef GL_ES
precision highp float;
#endif

uniform sampler2D sf_Texture;
uniform bool sf_TextureEnabled;
uniform float pulse;
varying vec4 sf_FrontColor;
varying vec4 sf_TexCoord0;

void main()
{
    vec4 color = sf_FrontColor;
    if (sf_TextureEnabled)
        color *= texture2D(sf_Texture, sf_TexCoord0.xy);
    gl_FragColor = vec4(color.rgb * (0.75 + 0.25 * pulse), color.a);
}
