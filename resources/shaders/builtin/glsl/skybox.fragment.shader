varying vec4 TexCoord0;
uniform samplerCube SKYBOX_TEXTURE;

void main()
{
    gl_FragColor = textureCube(SKYBOX_TEXTURE, TexCoord0);
}
