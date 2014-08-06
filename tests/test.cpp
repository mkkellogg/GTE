/*
 
  Simple GLSL 1.5 Demo 
 
  www.lighthouse3d.com
 
*/
 
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <math.h>
#include <GL/glew.h>
#include <GL/glut.h>
 
#include "textfile.h"
 
#define M_PI       3.14159265358979323846
#define GL_GLEXT_PROTOTYPES

// Data for drawing Axis
float verticesAxis[] = {-20.0f, 0.0f, 0.0f, 1.0f,
            20.0f, 0.0f, 0.0f, 1.0f,
 
            0.0f, -20.0f, 0.0f, 1.0f,
            0.0f,  20.0f, 0.0f, 1.0f,
 
            0.0f, 0.0f, -20.0f, 1.0f,
            0.0f, 0.0f,  20.0f, 1.0f};
 
float colorAxis[] = {   0.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 0.0f};
 
// Data for triangle 1
float vertices1[] = {   -3.0f, 0.0f, -5.0f, 1.0f,
            -1.0f, 0.0f, -5.0f, 1.0f,
            -2.0f, 2.0f, -5.0f, 1.0f};
 
float colors1[] = { 0.0f, 0.0f, 1.0f, 1.0f,
            0.0f, 0.0f, 1.0f, 1.0f,
            0.0f,0.0f, 1.0f, 1.0f};
 
// Data for triangle 2
float vertices2[] = {   1.0f, 0.0f, -5.0f, 1.0f,
            3.0f, 0.0f, -5.0f, 1.0f,
            2.0f, 2.0f, -5.0f, 1.0f};
 
float colors2[] = { 1.0f, 0.0f, 0.0f, 1.0f,
            1.0f, 0.0f, 0.0f, 1.0f,
            1.0f,0.0f, 0.0f, 1.0f};
 
// Shader Names
const char *vertexFileName = "basic.vertex.shader";
const char *fragmentFileName = "basic.fragment.shader";
 
// Program and Shader Identifiers
GLuint p,v,f;
 
// Vertex Attribute Locations
GLuint vertexLoc, colorLoc;
 
// Uniform variable Locations
GLuint projMatrixLoc, viewMatrixLoc;
 
// VBO ids
GLuint vbos[6];
 
// storage for Matrices
float projMatrix[16];
float viewMatrix[16];
 
// ----------------------------------------------------
// VECTOR STUFF
//
 
// res = a cross b;
void crossProduct( float *a, float *b, float *res) {
 
    res[0] = a[1] * b[2]  -  b[1] * a[2];
    res[1] = a[2] * b[0]  -  b[2] * a[0];
    res[2] = a[0] * b[1]  -  b[0] * a[1];
}
 
// Normalize a vec3
void normalize(float *a) {
 
    float mag = sqrt(a[0] * a[0]  +  a[1] * a[1]  +  a[2] * a[2]);
 
    a[0] /= mag;
    a[1] /= mag;
    a[2] /= mag;
}
 
// ----------------------------------------------------
// MATRIX STUFF
//
 
// sets the square matrix mat to the identity matrix,
// size refers to the number of rows (or columns)
void setIdentityMatrix( float *mat, int size) {
 
    // fill matrix with 0s
    for (int i = 0; i < size * size; ++i)
            mat[i] = 0.0f;
 
    // fill diagonal with 1s
    for (int i = 0; i < size; ++i)
        mat[i + i * size] = 1.0f;
}
 
//
// a = a * b;
//
void multMatrix(float *a, float *b) {
 
    float res[16];
 
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            res[j*4 + i] = 0.0f;
            for (int k = 0; k < 4; ++k) {
                res[j*4 + i] += a[k*4 + i] * b[j*4 + k];
            }
        }
    }
    memcpy(a, res, 16 * sizeof(float));
 
}
 
// Defines a transformation matrix mat with a translation
void setTranslationMatrix(float *mat, float x, float y, float z) {
 
    setIdentityMatrix(mat,4);
    mat[12] = x;
    mat[13] = y;
    mat[14] = z;
}
 
// ----------------------------------------------------
// Projection Matrix
//
 
void buildProjectionMatrix(float fov, float ratio, float nearP, float farP) {
 
    float f = 1.0f / tan (fov * (M_PI / 360.0));
 
    setIdentityMatrix(projMatrix,4);
 
    projMatrix[0] = f / ratio;
    projMatrix[1 * 4 + 1] = f;
    projMatrix[2 * 4 + 2] = (farP + nearP) / (nearP - farP);
    projMatrix[3 * 4 + 2] = (2.0f * farP * nearP) / (nearP - farP);
    projMatrix[2 * 4 + 3] = -1.0f;
    projMatrix[3 * 4 + 3] = 0.0f;
}
 
// ----------------------------------------------------
// View Matrix
//
// note: it assumes the camera is not tilted,
// i.e. a vertical up vector (remmeber gluLookAt?)
//
 
void setCamera(float posX, float posY, float posZ,
               float lookAtX, float lookAtY, float lookAtZ) {
 
    float dir[3], right[3], up[3];
 
    up[0] = 0.0f;   up[1] = 1.0f;   up[2] = 0.0f;
 
    dir[0] =  (lookAtX - posX);
    dir[1] =  (lookAtY - posY);
    dir[2] =  (lookAtZ - posZ);
    normalize(dir);
 
    crossProduct(dir,up,right);
    normalize(right);
 
    crossProduct(right,dir,up);
    normalize(up);
 
    float aux[16];
 
    viewMatrix[0]  = right[0];
    viewMatrix[4]  = right[1];
    viewMatrix[8]  = right[2];
    viewMatrix[12] = 0.0f;
 
    viewMatrix[1]  = up[0];
    viewMatrix[5]  = up[1];
    viewMatrix[9]  = up[2];
    viewMatrix[13] = 0.0f;
 
    viewMatrix[2]  = -dir[0];
    viewMatrix[6]  = -dir[1];
    viewMatrix[10] = -dir[2];
    viewMatrix[14] =  0.0f;
 
    viewMatrix[3]  = 0.0f;
    viewMatrix[7]  = 0.0f;
    viewMatrix[11] = 0.0f;
    viewMatrix[15] = 1.0f;
 
    setTranslationMatrix(aux, -posX, -posY, -posZ);
 
    multMatrix(viewMatrix, aux);
}
 
// ----------------------------------------------------

void changeSize(int w, int h) {
 
    float ratio;
    // Prevent a divide by zero, when window is too short
    // (you cant make a window of zero width).
    if(h == 0)
        h = 1;
 
    // Set the viewport to be the entire window
    glViewport(0, 0, w, h);
 
    ratio = (1.0f * w) / h;
    buildProjectionMatrix(53.13f, ratio, 1.0f, 30.0f);
}
 
void setupBuffers() 
{
    glEnableVertexAttribArray(colorLoc);
    glEnableVertexAttribArray(vertexLoc);
    

    // Generate vertex and color buffers for each triangle
    glGenBuffers(6, vbos);


    // First triangle
    // bind buffer for vertices and copy data into buffer
    glBindBuffer(GL_ARRAY_BUFFER, vbos[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices1), vertices1, GL_STATIC_DRAW);
    // bind buffer for colors and copy data into buffer
    glBindBuffer(GL_ARRAY_BUFFER, vbos[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(colors1), colors1, GL_STATIC_DRAW);
    

    // Second triangle 
    // bind buffer for vertices and copy data into buffer
    glBindBuffer(GL_ARRAY_BUFFER, vbos[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2), vertices2, GL_STATIC_DRAW);
    // bind buffer for colors and copy data into buffer
    glBindBuffer(GL_ARRAY_BUFFER, vbos[3]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(colors2), colors2, GL_STATIC_DRAW);
   

    // Third triangle
    // bind buffer for vertices and copy data into buffer
    glBindBuffer(GL_ARRAY_BUFFER, vbos[4]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesAxis), verticesAxis, GL_STATIC_DRAW);
    // bind buffer for colors and copy data into buffer
    glBindBuffer(GL_ARRAY_BUFFER, vbos[5]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(colorAxis), colorAxis, GL_STATIC_DRAW);
}
 
void setUniforms() {
 
    // must be called after glUseProgram
    glUniformMatrix4fv(projMatrixLoc,  1, 0, projMatrix);
    glUniformMatrix4fv(viewMatrixLoc,  1, 0, viewMatrix);
}
 
void renderScene(void) {
 
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
 
    setCamera(10,2,10,0,2,-5);
 
    glUseProgram(p);
    setUniforms();
 
    //glBindVertexArray(vao[0]);
    glBindBuffer(GL_ARRAY_BUFFER, vbos[0]);
    glVertexAttribPointer(vertexLoc, 4, GL_FLOAT, 0, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, vbos[1]);
    glVertexAttribPointer(colorLoc, 4, GL_FLOAT, 0, 0, 0);
    glDrawArrays(GL_TRIANGLES, 0, 3);
 
    //glBindVertexArray(vao[1]);
    glBindBuffer(GL_ARRAY_BUFFER, vbos[2]);
    glVertexAttribPointer(vertexLoc, 4, GL_FLOAT, 0, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, vbos[3]);
    glVertexAttribPointer(colorLoc, 4, GL_FLOAT, 0, 0, 0);
    glDrawArrays(GL_TRIANGLES, 0, 3);
 
    //glBindVertexArray(vao[2]);
    glBindBuffer(GL_ARRAY_BUFFER, vbos[4]);
    glVertexAttribPointer(vertexLoc, 4, GL_FLOAT, 0, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, vbos[5]);
    glVertexAttribPointer(colorLoc, 4, GL_FLOAT, 0, 0, 0);
    glDrawArrays(GL_LINES, 0, 6);
 
    glutSwapBuffers();
}
 
void processNormalKeys(unsigned char key, int x, int y) {
 
    if (key == 27) {
        glDeleteProgram(p);
        glDeleteShader(v);
        glDeleteShader(f);
        exit(0);
    }
}
 
#define printOpenGLError() printOglError(__FILE__, __LINE__)
 
int printOglError(char *file, int line)
{
    //
    // Returns 1 if an OpenGL error occurred, 0 otherwise.
    //
    GLenum glErr;
    int    retCode = 0;
 
    glErr = glGetError();
    while (glErr != GL_NO_ERROR)
    {
        printf("glError in file %s @ line %d: %s\n", file, line, gluErrorString(glErr));
        retCode = 1;
        glErr = glGetError();
    }
    return retCode;
}
 
void printShaderInfoLog(GLuint obj)
{
    int infologLength = 0;
    int charsWritten  = 0;
    char *infoLog;
 
    glGetShaderiv(obj, GL_INFO_LOG_LENGTH,&infologLength);
 
    if (infologLength > 0)
    {
        infoLog = (char *)malloc(infologLength);
        glGetShaderInfoLog(obj, infologLength, &charsWritten, infoLog);
        printf("%s\n",infoLog);
        free(infoLog);
    }
}
 
void printProgramInfoLog(GLuint obj)
{
    int infologLength = 0;
    int charsWritten  = 0;
    char *infoLog;
 
    glGetProgramiv(obj, GL_INFO_LOG_LENGTH,&infologLength);
 
    if (infologLength > 0)
    {
        infoLog = (char *)malloc(infologLength);
        glGetProgramInfoLog(obj, infologLength, &charsWritten, infoLog);
        printf("%s\n",infoLog);
        free(infoLog);
    }
}

char *getShaderLog(GLuint obj)
{
    int infologLength = 0;

    int charsWritten  = 0;
    char *infoLog;

    glGetShaderiv(obj, GL_INFO_LOG_LENGTH,&infologLength);

    if (infologLength > 0)
    {
        infoLog = (char *)malloc(infologLength);
        glGetShaderInfoLog(obj, infologLength, &charsWritten, infoLog);

        return infoLog;
    }

    return NULL;
}
 
GLuint setupShaders() {
 
    char *vs = NULL,*fs = NULL,*fs2 = NULL;
 
    GLuint p,v,f;
 
    v = glCreateShader(GL_VERTEX_SHADER);
    f = glCreateShader(GL_FRAGMENT_SHADER);
 
    vs = textFileRead((char *)vertexFileName);
    fs = textFileRead((char *)fragmentFileName);
 
    const char * vv = vs;
    const char * ff = fs;
 
    glShaderSource(v, 1, &vv,NULL);
    glShaderSource(f, 1, &ff,NULL);
 
    free(vs);free(fs);
 
    GLint shaderCompiled;

    printf("compiling vertex shader...\n");
    glCompileShader(v);
    glGetShaderiv(v, GL_COMPILE_STATUS, &shaderCompiled);
    if(shaderCompiled == GL_FALSE)
    {
    	char * info = getShaderLog(shaderCompiled);
	if(info != NULL)printf("%s\n", info);
	else printf("NULL error info.\n");
    }
    else
    {
	printf("Vertex shader compiled!\n");

    }

    printf("compiling fragment shader...\n");
    glCompileShader(f);
    glGetShaderiv(f, GL_COMPILE_STATUS, &shaderCompiled);
    if(shaderCompiled == GL_FALSE)
    {
    	char * info = getShaderLog(shaderCompiled);
	if(info != NULL)printf("%s\n", info);
	else printf("NULL error info.\n");
    }
    else
    {
	printf("Fragment shader compiled!\n");

    }
 
    printShaderInfoLog(v);
    printShaderInfoLog(f);
 
    p = glCreateProgram();
    glAttachShader(p,v);
    glAttachShader(p,f);
 
   // glBindFragDataLocation(p, 0, "outputF");
    glLinkProgram(p);
    printProgramInfoLog(p);
 
    vertexLoc = glGetAttribLocation(p,"position");
    colorLoc = glGetAttribLocation(p, "color"); 

    printf("vertexLoc: %d\n", vertexLoc);
    printf("colorLoc: %d\n", colorLoc);
 
    projMatrixLoc = glGetUniformLocation(p, "projMatrix");
    viewMatrixLoc = glGetUniformLocation(p, "viewMatrix");
 
    return(p);
}
 

void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // your drawing code goes here

    glutSwapBuffers();
}

void reshape(int width, int height)
{
    glViewport(0, 0, width, height);
}

void idle(void)
{
    glutPostRedisplay();
}

int main(int argc, char **argv) {
    
   /* glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(100,100);
    glutInitWindowSize(320,320);

  
    if(!glutGet(GLUT_DISPLAY_MODE_POSSIBLE))
    {
       exit(1);
    }

    glutCreateWindow("Lighthouse3D");
    glutDisplayFunc(renderScene);
    glutIdleFunc(renderScene);
    glutReshapeFunc(changeSize);
    glutKeyboardFunc(processNormalKeys);
 
    glewInit();
    if (glewIsSupported("GL_VERSION_2_0"))
        printf("Ready for OpenGL 2.0\n");
    else {
        printf("OpenGL 2.0 not supported\n");
        exit(1);
    }
 
    glEnable(GL_DEPTH_TEST);
    glClearColor(1.0,1.0,1.0,1.0);
 
    p = setupShaders(); 
    setupBuffers(); 
 
    glutMainLoop(); 
 
    return(0); */

    glutInit(&argc, argv);

    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(640, 480);

    if(!glutGet(GLUT_DISPLAY_MODE_POSSIBLE))
    {
       exit(1);
    }

    (void)glutCreateWindow("GLUT Program");

    glewExperimental = GL_TRUE; 
    glewInit();
    if (glewIsSupported("GL_VERSION_2_0"))
        printf("Ready for OpenGL 2.0\n");
    else {	
        printf("OpenGL 2.0 not supported\n");
        exit(1);
    }

    p = setupShaders(); 
    setupBuffers(); 

    glutDisplayFunc(renderScene);
    glutIdleFunc(renderScene);
    glutReshapeFunc(changeSize);
    glutKeyboardFunc(processNormalKeys);

    glEnable(GL_DEPTH_TEST);
    glClearColor(1.0,1.0,1.0,1.0);

    glutMainLoop(); 

    return EXIT_SUCCESS;
}


