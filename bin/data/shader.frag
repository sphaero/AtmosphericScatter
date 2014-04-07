//#version 120

uniform sampler2D earth;
uniform sampler2D earthnight;

uniform float fExposure;

void main (void)
{
    //gl_FragColor = gl_Color + 0.25 * gl_SecondaryColor;
    vec4 f4Color = gl_Color + texture2D(earth, gl_TexCoord[0].st) * gl_SecondaryColor + (texture2D(earthnight, gl_TexCoord[0].st)*(1.0- gl_SecondaryColor))*0.2;

    gl_FragColor = 1.0 - exp(f4Color * -fExposure);
}
/*
// this is how we receive the texture
uniform sampler2DRect tex0;
uniform sampler2DRect earth_night;

varying vec2 texCoordVarying;

void main()
{
    vec4 texel0 = texture2DRect(tex0, texCoordVarying);
    vec4 texel1 = texture2DRect(earth_night, texCoordVarying);
    if (texCoordVarying.x > 0.5) 
    {
        //gl_FragColor = vec4(1.0,0,0,1.0);
        gl_FragColor = texture2DRect(earth_night, texCoordVarying * vec2(4096,2048));
    } 
    else 
    {
        //gl_FragColor = vec4(texel0.rgb, texel0.a * texel1.a);
        gl_FragColor = texture2DRect(tex0, texCoordVarying * vec2(4096,2048));
    }
}
*/