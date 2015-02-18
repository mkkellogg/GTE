varying vec4 TexCoord0;
uniform samplerCube SKYBOX_TEXTURE;

void main()
{
    gl_FragColor = texture(SKYBOX_TEXTURE, TexCoord0);
}
