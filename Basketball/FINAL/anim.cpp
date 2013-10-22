////////////////////////////////////////////////////
// anim.cpp version 4.1
// Template code for drawing an articulated figure.
// CS 174A 
////////////////////////////////////////////////////

#ifdef WIN32
#include <windows.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>
#else
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

#ifdef WIN32
#include <GL/freeglut.h>
#include <GL/freeglut_ext.h>
#else
#include <GLUT/glut.h>
#endif



#include "Shapes.h"
#include "tga.h"

#include "Angel.h"

#include <iostream>
#include <math.h>


#include "Camera.h"
#include "Light.h"
#include "Cloth.h"
#include "Cube.h"

#ifdef __APPLE__
#define glutInitContextVersion(a,b)
#define glutInitContextProfile(a)
#define glewExperimental int glewExperimentalAPPLE
#define glewInit()
#endif

// Structs that hold the Vertex Array Object index and number of vertices of each shape.
ShapeData cubeData;
ShapeData sphereData;
ShapeData cylData;



mat4         model_view;
GLint        uModelView, uProjection, uView, uRotateBall;
GLint        uAmbient, uDiffuse, uSpecular, uLightPos, uShininess;
GLint        uTex, uEnableTex;

bool endPan = false;
float endTheta = -92;

bool baselineLeft = false;
bool isBaseline = false;
bool isAnimate = false;
bool isZero = false;
bool back = false;
bool pan = false;
bool startPan = false;
bool startPan1 = false;
int Width = 1000;
int Height = 800 ;
int Button = -1 ;
float Zoom = 1 ;
int PrevY = 0 ;

int Animate = 0 ;
int Recording = 0 ;

void save_image();
void set_colour(float r, float g, float b) ;

const int STRLEN = 100;
typedef char STR[STRLEN];

#define PI 3.1415926535897
#define X 0
#define Y 1
#define Z 2


// NEW

GLfloat theta = 0.0;
GLfloat theta2 = 0.0;
GLfloat theta3 = 0.0;
GLuint program;

bool rotateCubes = false;
bool scrollTexture = false;
GLfloat xCam = 0.0;
GLfloat yCam = -3.5;
GLfloat zCam = -0.5;
GLfloat xCamTheta = 0.0;
GLfloat yCamTheta = 0.0;
GLfloat ballX = -7.0;
GLfloat ballY = 3.0;
GLfloat ballZ = -10.54;
GLfloat ballXInit = 0.0;
GLfloat ballYInit = 3.0;
GLfloat ballZInit = -4.0;
GLfloat backBoardInit = -10.75;
GLfloat groundInit = 1.9;
GLfloat velYInit = 8.107;
GLfloat f_time = 0.0;
GLfloat rotballX = 180;
GLfloat rotballY = 180;
GLfloat rotballZ = 0;
GLfloat velY = velYInit;
GLfloat velZ;
GLfloat velX;
GLfloat bounces = 0;

GLfloat numPoints = 360;
GLfloat hitRimIndex = -1;
GLfloat preventHit = 0;
vec4 rimArr[360];
struct ballStart
{
	GLfloat initX;
	GLfloat initZ;
	GLfloat swishVelX;
	GLfloat swishVelZ;
};

ballStart shotPosArr[5];



GLfloat startTheta = 0;
GLfloat panTheta = 0;
GLfloat panArr[4] = {45, 0, -45, -92};
GLint shotCounter = 0;
GLint whichShot = 0;
GLint whichPan = 0;



GLfloat light_x = 15;
GLfloat light_y = 15;
GLfloat light_z = 30;


GLuint vao;
GLuint vbo;

//GLfloat yCamTheta = -90.0;
//GLfloat xCam = -12.4;
//GLfloat zCam = 10.50003;
//GLfloat ballX = 7.9;
//GLfloat ballY = 3.0;
//GLfloat ballZ = -10.54;

TgaImage courtImage;
TgaImage backWallImage;
TgaImage leftWallImage;
TgaImage backBoardImage;
TgaImage powerBarImage;
TgaImage scoreBoardImage;

//texture

GLuint texture_cube;
GLuint texture_bball;

GLuint court_texture;
GLuint backWall_texture;
GLuint leftWall_texture;
GLuint backBoard_texture;
GLuint powerBar_texture;
GLuint scoreBoard_texture;

bool net = false;

// The eye point and look-at point.
// Currently unused. Use to control a camera with LookAt().
/*Angel::vec4 eye{0, 0.0, 50.0,1.0};
Angel::vec4 ref{0.0, 0.0, 0.0,1.0};
Angel::vec4 up{0.0,1.0,0.0,0.0};*/

double TIME = 0.0 ;

void createPoints(vec4 a[]){
	for(int i = 0; i < numPoints; i++){
		a[i] = vec4(0.0f+(.3*cos((i*(360/numPoints) + 90)*DegreesToRadians)), 5.15f, -10.5f+(.3*sin((i*(360/numPoints)+90)* DegreesToRadians)), 1.0);
	}
}

//Global Variables
const int EscKey               = 27;
const int SpaceKey             = 32;

Camera  Pentax;
Light Lumina;
Cloth * myCloth;
Cube * myCube;

int STOP = -1;//control the animation
int DRAW_TYPE = DRAW_LINE;

GLfloat TIME1;
GLfloat TIME2;
GLfloat DTIME;//timers

GLfloat CursorX;
GLfloat CursorY;//Curosr Position


GLfloat  left = -1.0, right = 1.0;
GLfloat  bottom = -1.0, top = 1.0;
GLfloat  zNear = 0.3, zFar = 300.0;




float powerBarX = 0;
float accBarX = 0;
bool pbGoingUp = true;
bool accGoingUp = true;
int scoreCount = 0;
bool pbOn = true;
bool accOn = true;


bool powerGood = false;
GLfloat powerBad = 0;
bool accuracyGood = false;
GLfloat accuracyBad = 0;

bool scoreUpdated = false;

bool startGame = true;



/////////////////////////////////////////////////////
//    PROC: drawCylinder()
//    DOES: this function 
//          render a solid cylinder  oriented along the Z axis. Both bases are of radius 1. 
//          The bases of the cylinder are placed at Z = 0, and at Z = 1.
//
//          
// Don't change.
//////////////////////////////////////////////////////
void drawCylinder(void)
{
    glUniformMatrix4fv( uModelView, 1, GL_TRUE, model_view );
    glBindVertexArray( cylData.vao );
    glDrawArrays( GL_TRIANGLES, 0, cylData.numVertices );
}


//////////////////////////////////////////////////////
//    PROC: drawCube()
//    DOES: this function draws a cube with dimensions 1,1,1
//          centered around the origin.
// 
// Don't change.
//////////////////////////////////////////////////////

void drawCube(void)
{
    glUniform1f(uShininess, 100.0f);
   // glBindTexture( GL_TEXTURE_2D, texture_cube );
    //glUniform1i( uEnableTex, 1 );
    glUniformMatrix4fv( uModelView, 1, GL_TRUE, model_view );
    glBindVertexArray( cubeData.vao );
    glDrawArrays( GL_TRIANGLES, 0, cubeData.numVertices );
    glUniform1i( uEnableTex, 0 );
}


void drawBackBoard(void)
{
    glUniform1f(uShininess, 100.0f);
    glBindTexture( GL_TEXTURE_2D, backBoard_texture );
    glUniform1i( uEnableTex, 1 );
    glUniformMatrix4fv( uModelView, 1, GL_TRUE, model_view );
    glBindVertexArray( cubeData.vao );
    glDrawArrays( GL_TRIANGLES, 0, cubeData.numVertices );
    glUniform1i( uEnableTex, 0 );
}

void drawScoreBoard(void)
{
    glUniform1f(uShininess, 100.0f);
    glBindTexture( GL_TEXTURE_2D, scoreBoard_texture );
    glUniform1i( uEnableTex, 1 );
    glUniformMatrix4fv( uModelView, 1, GL_TRUE, model_view );
    glBindVertexArray( cubeData.vao );
    glDrawArrays( GL_TRIANGLES, 0, cubeData.numVertices );
    glUniform1i( uEnableTex, 0 );
}

void drawPowerBar(void)
{
    glUniform1f(uShininess, 100.0f);
    glBindTexture( GL_TEXTURE_2D, powerBar_texture );
    glUniform1i( uEnableTex, 1 );
    glUniformMatrix4fv( uModelView, 1, GL_TRUE, model_view );
    glBindVertexArray( cubeData.vao );
    glDrawArrays( GL_TRIANGLES, 0, cubeData.numVertices );
    glUniform1i( uEnableTex, 0 );
}

void drawCourt(void)
{
    glUniform1f(uShininess, 100.0f);
    glBindTexture( GL_TEXTURE_2D, court_texture );
    glUniform1i( uEnableTex, 1 );
    glUniformMatrix4fv( uModelView, 1, GL_TRUE, model_view );
    glBindVertexArray( cubeData.vao );
    glDrawArrays( GL_TRIANGLES, 0, cubeData.numVertices );
    glUniform1i( uEnableTex, 0 );
}

void drawBackWall(void)
{
    glUniform1f(uShininess, 5000000.0f);
    glBindTexture( GL_TEXTURE_2D, backWall_texture );
    glUniform1i( uEnableTex, 1 );
    glUniformMatrix4fv( uModelView, 1, GL_TRUE, model_view );
    glBindVertexArray( cubeData.vao );
    glDrawArrays( GL_TRIANGLES, cubeData.numVertices, cubeData.numVertices );
    glUniform1i( uEnableTex, 0 );
}

void drawLeftWall(void)
{
    glUniform1f(uShininess, 5000000.0f);
    glBindTexture( GL_TEXTURE_2D, leftWall_texture );
    glUniform1i( uEnableTex, 1 );
    glUniformMatrix4fv( uModelView, 1, GL_TRUE, model_view );
    glBindVertexArray( cubeData.vao );
    glDrawArrays( GL_TRIANGLES, cubeData.numVertices, cubeData.numVertices );
    glUniform1i( uEnableTex, 0 );
}

//void drawScene(){
//    
//    myCloth->Draw(DRAW_TYPE, DTIME/1000, Pentax, Lumina, CursorX, CursorY);
//    
//}


//////////////////////////////////////////////////////
//    PROC: drawSphere()
//    DOES: this function draws a sphere with radius 1
//          centered around the origin.
// 
// Don't change.
//////////////////////////////////////////////////////

void drawSphere(void)
{
    glBindTexture( GL_TEXTURE_2D, texture_bball);
    glUniform1i( uEnableTex, 1);
    glUniformMatrix4fv( uModelView, 1, GL_TRUE, model_view );
    glBindVertexArray( sphereData.vao );
    glDrawArrays( GL_TRIANGLES, 0, sphereData.numVertices );
    glUniform1i( uEnableTex, 0 );
}


//////////////////////////////////////////////////////
//    PROC: myKey()
//    DOES: this function gets caled for any keypresses
// 
//////////////////////////////////////////////////////

void myKey(unsigned char key, int x, int y)
{
    switch (key) {
        case 'q':
        case 27:
            exit(0);
            /*case 'r':
             rotateCubes = !rotateCubes;
             break ;
             case 's':
             scrollTexture = !scrollTexture;
             break;*/
        case 'i':
            zCam += 0.3;
            break;
        case 'm':
            zCam -= 0.3;
            break;
        case 'k':
            xCam -= 0.3;
            break;
        case 'j':
            xCam += 0.3;
            break;
        case 'o':
            yCam -= 0.3;
            break;
        case 'p':
            yCam += 0.3;
            break;
        case 't':
            xCamTheta += 1;
            break;
        case 'y':
            yCamTheta += 1;
            break;
        case 'g':
            xCamTheta -=1;
            break;
        case 'h':
            yCamTheta -=1;
            break;
        case 'r':
            ballX = 0;
            ballY = 3.0;
            ballZ = -4.0;
            f_time = 0;
            xCam = 0;
            yCam = -3.5;
            zCam = -0.5;
            xCamTheta = 0;
            yCamTheta = 0;
            back = false;
            isZero = true;
            isAnimate = false;
            rotballX = 160;
            rotballY = 90;
            isBaseline = false;
            break ;
        case ' ':
            if(!pan){
                if (pbOn)
                    pbOn = false;
                else
                {
                    printf("powerBarX: %f, accuracyBarX: %f\n", powerBarX, accBarX);
                    if (powerBarX > -50 && powerBarX < 50)
                        powerGood = true;
                    else
                    {
//                        if (powerBarX < 0)
//                            powerBarX += 30;
//                        else powerBarX -= 30;
                        powerBad = powerBarX/250;
                    }
                    if (accBarX > -50 && accBarX < 50)
                        accuracyGood = true;
                    else
                    {
//                        if (accBarX < 0)
//                            accBarX += 30;
//                        else accBarX -= 30;
                        accuracyBad = accBarX/250;
                    }
                    
                    if (powerGood)
                    {
                        if (whichShot == 0)
                            velX = shotPosArr[whichShot].swishVelX;
                        if (whichShot == 1)
                        {
                            velX = shotPosArr[whichShot].swishVelX;
                            velZ = shotPosArr[whichShot].swishVelZ;
                        }
                        if (whichShot == 2)
                            velZ = shotPosArr[whichShot].swishVelZ;
                        if (whichShot == 3)
                        {
                            velX = shotPosArr[whichShot].swishVelX;
                            velZ = shotPosArr[whichShot].swishVelZ;
                        }
                        if (whichShot == 4)
                            velX = shotPosArr[whichShot].swishVelX;
                    }
                    else
                    {
                        if (whichShot == 0)
                            velX = shotPosArr[whichShot].swishVelX + powerBad;
                        if (whichShot == 1)
                        {
                            velX = shotPosArr[whichShot].swishVelX + (1/sqrt(2))*powerBad;
                            velZ = shotPosArr[whichShot].swishVelZ + (1/sqrt(2))*powerBad;
                        }
                        if (whichShot == 2)
                            velZ = shotPosArr[whichShot].swishVelZ - powerBad;
                        if (whichShot == 3)
                        {
                            velX = shotPosArr[whichShot].swishVelX + (1/sqrt(2))*powerBad;
                            velZ = shotPosArr[whichShot].swishVelZ - (1/sqrt(2))*powerBad;
                        }
                        if (whichShot == 4)
                            velX = shotPosArr[whichShot].swishVelX - powerBad;

                    }
                    if (accuracyGood)
                    {
                        if (whichShot == 0)
                            velZ = shotPosArr[whichShot].swishVelZ;
                        if (whichShot == 1)
                        {
                            if (powerGood)
                            {
                                velX = shotPosArr[whichShot].swishVelX;
                                velZ = shotPosArr[whichShot].swishVelZ;
                            }
                        }
                        if (whichShot == 2)
                            velX = shotPosArr[whichShot].swishVelX;
                        if (whichShot == 3)
                        {
                            if (powerGood)
                            {
                                velX = shotPosArr[whichShot].swishVelX;
                                velZ = shotPosArr[whichShot].swishVelZ;
                            }
                        }
                        if (whichShot == 4)
                            velZ = shotPosArr[whichShot].swishVelZ;
                    }
                    else
                    {
                        if (whichShot == 0)
                            velZ = shotPosArr[whichShot].swishVelZ + accuracyBad;
                        if (whichShot == 1)
                        {
                                velX = shotPosArr[whichShot].swishVelX + (1/sqrt(2))*accuracyBad;
                                velZ = shotPosArr[whichShot].swishVelZ + (1/sqrt(2))*accuracyBad;
                        }
                        if (whichShot == 2)
                            velX = shotPosArr[whichShot].swishVelX + accuracyBad;
                        if (whichShot == 3)
                        {
                                velX = shotPosArr[whichShot].swishVelX + (1/sqrt(2))*accuracyBad;
                                velZ = shotPosArr[whichShot].swishVelZ - (1/sqrt(2))*accuracyBad;
                        }
                        if (whichShot == 4)
                            velZ = shotPosArr[whichShot].swishVelZ - accuracyBad;
                    }
                    
                    accuracyBad = 0;
                    powerBad = 0;
                    powerGood = false;
                    accuracyGood = false;
                    
                    scoreUpdated = false;
                    
                    accOn = false;
                    groundInit = 1.9;
                    backBoardInit = -10.75;
                    bounces = 0;
                    shotCounter++;
                    isAnimate = !isAnimate;
                    hitRimIndex = -1;
                }
            }
            break;
        case '1':
            baselineLeft = false;
            f_time = 0;
            back = false;
            isZero = true;
            isAnimate = false;
            isBaseline = true;
            yCamTheta = -90.0;
            xCam = -12.0;
            zCam = 10.50003;
            ballX = 7.9;
            ballY = 3.0;
            ballZ = -10.54;
            rotballX = 180;
            rotballY = 180;
            rotballZ = 180;
            break;
        case '2':
            baselineLeft = true;
            f_time = 0;
            back = false;
            isZero = true;
            isAnimate = false;
            isBaseline = true;
            yCamTheta = 90.0;
            xCam = 9.4;
            zCam = 10.50003;
            ballX = -7.0;
            ballY = 3.0;
            ballZ = -10.54;
            rotballX = 180;
            rotballY = 180;
            rotballZ = 180;
            break;
        case 's':
			xCam = 9.05;
			zCam = -8.03320;
			yCam = -10.0;
			startPan = !startPan;
			break;
        case 'z':
            yCamTheta = 90.0;
            xCam = 9.4;
            zCam = 10.50003;
            pan = !pan;
            break;
        case '9':
            net = !net;
            break;
            
        case 'v':
            ballX = 0;
            ballY = 3.0;
            ballZ = -4.0;
            f_time = 0;
            back = false;
            isZero = true;
            isAnimate = false;
            rotballX = 160;
            rotballY = 90;
            isBaseline = false;
            break ;
        case '0':
            scoreCount++;
            std::string file_no = "sb_" + (std::to_string(scoreCount%11)) + ".tga";
            const char * c = file_no.c_str();
            scoreBoardImage.loadTGA(c);
            glGenTextures( 1, &scoreBoard_texture );
            glBindTexture( GL_TEXTURE_2D, scoreBoard_texture );
            glTexImage2D(GL_TEXTURE_2D, 0, 4, scoreBoardImage.width, scoreBoardImage.height, 0,
                         (scoreBoardImage.byteCount == 3) ? GL_BGR : GL_BGRA,
                         GL_UNSIGNED_BYTE, scoreBoardImage.data );
            
            glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
            glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );
            glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            break;
            
            
    }
    glutPostRedisplay() ;

}

/*********************************************************
    PROC: myinit()
    DOES: performs most of the OpenGL intialization
     -- change these with care, if you must.

**********************************************************/

void myinit(void)
{
    //set up the camera
    Pentax.position = vec4(0,0,15.0,1.0);
    Pentax.lookat = vec4(0,0,-10000.0,1.0);
    Pentax.up = vec4(0,1.0,0,0);
    Pentax.fovy = 60;
    Pentax.aspect = 1.0;
    Pentax.znear = 5.0;
    Pentax.zfar = -100;
    
    //set up the light
    Lumina.position = vec4(0,15,0,1.0);
    Lumina.color = vec4(1.0,1.0,1.0,1.0);
    
    //set up the objects
    myCloth = new Cloth;
    //myCloth->WeaveCloth(8,20,0.05,0.9,50.0,0.5,0.1,-5.5,2.0,-5.0, true);//N, Mass, Length,ks,kd,kf,sx,sy,sz
    //myCloth->WeaveCloth(15,10,0.05,0.25,50.0,0.5,0.1,-1,6,-10, true);//N, Mass, Length,ks,kd,kf,sx,sy,sz
    
    myCloth->InitDraw();
    
    // Create a vertex array object
    //Create the Vertex Array and Buffers, bind them
    glGenVertexArraysAPPLE(1, &vao);
    glGenBuffers(1, &vbo);//generate buffer for current vertex array
    
    // Load shaders and use the resulting shader program
    program = InitShader( "vshader.glsl", "fshader.glsl" );
    glUseProgram(program);
    
    if (!backBoardImage.loadTGA("back5.tga"))
    {
        printf("Error loading image file\n");
        exit(1);
    }
    
    if (!courtImage.loadTGA("court.tga"))
    {
        printf("Error loading image file\n");
        exit(1);
    }
    
    if (!backWallImage.loadTGA("aud.tga"))
    {
        printf("Error loading image file\n");
        exit(1);
    }
    
    if (!leftWallImage.loadTGA("left_aud.tga"))
    {
        printf("Error loading image file\n");
        exit(1);
    }
    
    if (!powerBarImage.loadTGA("power_bar.tga"))
    {
        printf("Error loading image file\n");
        exit(1);
    }
    
    if (!scoreBoardImage.loadTGA("sb_0.tga"))
    {
        printf("Error loading image file\n");
        exit(1);
    }

    // Generate vertex arrays for geometric shapes
    generateSphere(program, &sphereData);
    generateCylinder(program, &cylData);
    
    generateCube(program, &cubeData, theta);

    uModelView  = glGetUniformLocation( program, "ModelView"  );
    uProjection = glGetUniformLocation( program, "Projection" );
    uView       = glGetUniformLocation( program, "View"       );

    glClearColor( 0.1, 0.1, 0.2, 1.0 ); // dark blue background

    uAmbient   = glGetUniformLocation( program, "AmbientProduct"  );
    uDiffuse   = glGetUniformLocation( program, "DiffuseProduct"  );
    uSpecular  = glGetUniformLocation( program, "SpecularProduct" );
    uLightPos  = glGetUniformLocation( program, "LightPosition"   );
    uShininess = glGetUniformLocation( program, "Shininess"       );
    uTex       = glGetUniformLocation( program, "Tex"             );
    uEnableTex = glGetUniformLocation( program, "EnableTex"       );
    
    uRotateBall  = glGetUniformLocation( program, "RotateBall"  );


    glEnable(GL_DEPTH_TEST);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    
	glGenTextures( 1, &backBoard_texture );
    glBindTexture( GL_TEXTURE_2D, backBoard_texture );
    glTexImage2D(GL_TEXTURE_2D, 0, 4, backBoardImage.width, backBoardImage.height, 0,
                 (backBoardImage.byteCount == 3) ? GL_BGR : GL_BGRA,
                 GL_UNSIGNED_BYTE, backBoardImage.data );
   // glGenerateMipmap(GL_TEXTURE_2D);
    
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    
    glGenTextures( 1, &powerBar_texture );
    glBindTexture( GL_TEXTURE_2D, powerBar_texture );
    glTexImage2D(GL_TEXTURE_2D, 0, 4, powerBarImage.width, powerBarImage.height, 0,
                 (powerBarImage.byteCount == 3) ? GL_BGR : GL_BGRA,
                 GL_UNSIGNED_BYTE, powerBarImage.data );
    // glGenerateMipmap(GL_TEXTURE_2D);
    
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);


    glGenTextures( 1, &scoreBoard_texture );
    glBindTexture( GL_TEXTURE_2D, scoreBoard_texture );
    glTexImage2D(GL_TEXTURE_2D, 0, 4, scoreBoardImage.width, scoreBoardImage.height, 0,
                 (scoreBoardImage.byteCount == 3) ? GL_BGR : GL_BGRA,
                 GL_UNSIGNED_BYTE, scoreBoardImage.data );
    
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glGenTextures( 1, &court_texture );
    glBindTexture( GL_TEXTURE_2D, court_texture );
    glTexImage2D(GL_TEXTURE_2D, 0, 4, courtImage.width, courtImage.height, 0,
                 (courtImage.byteCount == 3) ? GL_BGR : GL_BGRA,
                 GL_UNSIGNED_BYTE, courtImage.data );
    glGenerateMipmap(GL_TEXTURE_2D);
    
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    
    glGenTextures( 1, &backWall_texture );
    glBindTexture( GL_TEXTURE_2D, backWall_texture );
    glTexImage2D(GL_TEXTURE_2D, 0, 4, backWallImage.width, backWallImage.height, 0,
                 (backWallImage.byteCount == 3) ? GL_BGR : GL_BGRA,
                 GL_UNSIGNED_BYTE, backWallImage.data );
    glGenerateMipmap(GL_TEXTURE_2D);
    
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    
    
    glGenTextures( 1, &leftWall_texture );
    glBindTexture( GL_TEXTURE_2D, leftWall_texture );
    glTexImage2D(GL_TEXTURE_2D, 0, 4, leftWallImage.width, leftWallImage.height, 0,
                 (leftWallImage.byteCount == 3) ? GL_BGR : GL_BGRA,
                 GL_UNSIGNED_BYTE, leftWallImage.data );
    glGenerateMipmap(GL_TEXTURE_2D);
    
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    
    TgaImage bballImage;
    if (!bballImage.loadTGA("bball.tga"))
    {
        printf("Error loading image file\n");
        exit(1);
    }
    
    
    glGenTextures( 1, &texture_bball );
    glBindTexture( GL_TEXTURE_2D, texture_bball );
    glTexImage2D(GL_TEXTURE_2D, 0, 4, bballImage.width, bballImage.height, 0,
                 (bballImage.byteCount == 3) ? GL_BGR : GL_BGRA,
                 GL_UNSIGNED_BYTE, bballImage.data );
    
   
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    
    //glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    //glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    
    // Set texture sampler variable to texture unit 0
    // (set in glActiveTexture(GL_TEXTURE0))
    
    glUniform1i( uTex, 0);
    
    glUniform4f(uLightPos,  light_x, light_y, light_z, 0.0f);

    
}

/*********************************************************
    PROC: set_colour();
    DOES: sets all material properties to the given colour
    -- don't change
**********************************************************/

void set_colour(float r, float g, float b)
{
    float ambient  = 0.02f;
    float diffuse  = 3.20f;
    float specular = 0.2f;
    glUniform4f(uAmbient,  ambient*r,  ambient*g,  ambient*b,  1.0f);
    glUniform4f(uDiffuse,  diffuse*r,  diffuse*g,  diffuse*b,  1.0f);
    glUniform4f(uSpecular, specular*r, specular*g, specular*b, 1.0f);
}

/*********************************************************
**********************************************************
**********************************************************

    PROC: display()
    DOES: this gets called by the event handler to draw
          the scene, so this is where you need to build
          your ROBOT --  
      
        MAKE YOUR CHANGES AND ADDITIONS HERE

    Add other procedures if you like.

**********************************************************
**********************************************************
**********************************************************/
void display(void)
{
    glUniform4f(uLightPos, light_x, light_y, light_z, 0.0f);
    
    glUseProgram(program);
    
    glBindVertexArrayAPPLE(vao);//use as current vertex array
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    model_view = mat4(1.0f);
    
    glUniformMatrix4fv( uRotateBall, 1, GL_TRUE, mat4(1.0f) );
    
    
    //model_view *= Translate(0.0f + xCam, -3.50f + yCam, -0.50f + zCam);
    
    mat4 view = model_view;
    
    glUniformMatrix4fv( uView, 1, GL_TRUE, model_view );

    mat4 old_model_view = model_view;
    //model_view *= RotateX(xCamTheta);
	//model_view *= RotateY(yCamTheta);
    /*model_view *= Translate(2.435f + xCam, 0.0f + yCam, 0.0f + zCam);
    
    model_view *= Scale(Zoom);

    model_view *= Translate(-2.435f + xCam, 0.0f + yCam, 0.0f + zCam);*/

    set_colour(0.8f, 0.8f, 0.8f);
    
    old_model_view = model_view;
	model_view *= RotateX(xCamTheta);
	model_view *= RotateY(yCamTheta);
	model_view *= Translate(xCam, yCam, zCam);
    model_view *= Scale(20.0f, 3.0f, 30.0f);
    
    
    


    //model_view *= RotateX(theta2);


    drawCourt();
    
    set_colour(0.5f, 0.5f, 0.5f);
    
    model_view = old_model_view;
   	model_view *= RotateX(xCamTheta);
	model_view *= RotateY(yCamTheta);
    model_view *= Translate(0.0f + xCam, 5.750f + yCam, -20.0f + zCam);
    model_view *= Scale(20.0f, 8.50f, 10.0f);
    
    drawBackWall();
    
    model_view = old_model_view;
    set_colour(0.85f, 0.85f, 0.85f);
    model_view *= RotateX(xCamTheta);
	model_view *= RotateY(yCamTheta);
    model_view *= Translate(-9.0f + xCam, 6.0f + yCam, -11.0f + zCam);
    model_view *= Scale(1.0f, 8.50f, 23.0f);

    drawLeftWall();
    
    model_view = old_model_view;
    set_colour(0.85f, 0.85f, 0.85f);
    model_view *= RotateX(xCamTheta);
	model_view *= RotateY(yCamTheta);
    model_view *= Translate(9.0f + xCam, 6.0f + yCam, -11.0f + zCam);
    model_view *= Scale(1.0f, 8.50f, 23.0f);
    
    drawLeftWall();
    
    model_view = old_model_view;
    set_colour(0.85f, 0.85f, 0.85f);
    model_view *= RotateX(xCamTheta);
	model_view *= RotateY(yCamTheta);
    model_view *= Translate(-8.99f + xCam, 6.0f + yCam, 0.0f + zCam);
    model_view *= Scale(1.0f, 8.50f, 23.0f);
    
    drawLeftWall();
    
    model_view = old_model_view;
    set_colour(0.85f, 0.85f, 0.85f);
    model_view *= RotateX(xCamTheta);
	model_view *= RotateY(yCamTheta);
    model_view *= Translate(9.01f + xCam, 5.9f + yCam, 9.2f + zCam);
    model_view *= Scale(1.0f, 8.50f, 23.0f);
    
    drawLeftWall();
    
    set_colour(0.2f, 0.2f, 0.2f);
	model_view = old_model_view;
	model_view *= RotateX(xCamTheta);
	model_view *= RotateY(yCamTheta);
	model_view *= Translate(0.0f + xCam, 5.0f + yCam, -12.50f + zCam);
	model_view *= Scale(0.2f, 0.2f, 3.0f);
    
	drawCube();


	set_colour(0.20f, 0.20f, 0.20f);
	model_view = old_model_view;
	model_view *= RotateX(xCamTheta);
	model_view *= RotateY(yCamTheta);
	model_view *= Translate(0.0f + xCam, 3.0f + yCam, -14.0f + zCam);
	model_view *= Scale(0.4f, 4.50f, 0.4f);


	
	drawCube();
    
    //mat4 projection = Ortho( left, right, bottom, top, zNear, zFar );
    //glUniformMatrix4fv( uProjection, 1, GL_TRUE, projection );
    
    
    set_colour(0.40f, 0.40f, 0.40f);
	model_view = old_model_view;
	model_view *= RotateY(yCamTheta);
	model_view *= Translate((3.3541*sin((yCamTheta*DegreesToRadians)+0.463648)), -1.2, (3.3541*cos((yCamTheta*DegreesToRadians)+0.463648+PI)));
    model_view *= RotateX(xCamTheta);
    model_view *= RotateY(-yCamTheta);
	model_view *= Scale(0.75f, 0.10f, 0.01f);
	
	drawPowerBar();
    
    set_colour(.25f, 0.25f, 0.0f);
	model_view = old_model_view;
	model_view *= RotateY(yCamTheta);
	model_view *= Translate((3.354*sin((yCamTheta*DegreesToRadians)+0.463648))+(((accBarX/250)*0.35)*cos(yCamTheta*DegreesToRadians)), -1.2, (3.354*cos((yCamTheta*DegreesToRadians)+0.463648+PI))+(((accBarX/250)*0.35)*sin(yCamTheta*DegreesToRadians)));
    model_view *= RotateX(xCamTheta);
    model_view *= RotateY(-yCamTheta);
	model_view *= Scale(0.03f, 0.15f, 0.01f);
	
	drawCube();
    
    
    
    
    set_colour(0.40f, 0.40f, 0.40f);
	model_view = old_model_view;
    model_view *= Translate(0.75, -2.2, 0);
    model_view *= RotateZ(90);
	model_view *= RotateY(yCamTheta);
	model_view *= Translate((3.3541*sin((yCamTheta*DegreesToRadians)+0.463648)), -1, (3.3541*cos((yCamTheta*DegreesToRadians)+0.463648+PI)));
    model_view *= RotateX(xCamTheta);
    model_view *= RotateY(-yCamTheta);
	model_view *= Scale(0.75f, 0.10f, 0.01f);
	
	drawPowerBar();
    
    set_colour(.25f, 0.25f, 0.0f);
	model_view = old_model_view;
    model_view *= Translate(0.75, -2.2, 0);
    model_view *= RotateZ(90);
	model_view *= RotateY(yCamTheta);
	model_view *= Translate((3.354*sin((yCamTheta*DegreesToRadians)+0.463648))+(((powerBarX/250)*0.35)*cos(yCamTheta*DegreesToRadians)), -1, (3.354*cos((yCamTheta*DegreesToRadians)+0.463648+PI))+(((powerBarX/250)*0.35)*sin(yCamTheta*DegreesToRadians)));
    model_view *= RotateX(xCamTheta);
    model_view *= RotateY(-yCamTheta);
	model_view *= Scale(0.03f, 0.15f, 0.01f);
	
	drawCube();
    
    
    set_colour(0.40f, 0.40f, 0.40f);
	model_view = old_model_view;
	model_view *= RotateX(xCamTheta);
	model_view *= RotateY(yCamTheta);
	model_view *= Translate(0.0f + xCam, 7.30f + yCam, -11.0f + zCam);
	model_view *= Scale(0.70f, 0.60f, 0.1f);
    
    drawScoreBoard();

	set_colour(0.40f, 0.40f, 0.40f);
	model_view = old_model_view;
	model_view *= RotateX(xCamTheta);
	model_view *= RotateY(yCamTheta);
	model_view *= Translate(0.0f + xCam, 6.0f + yCam, -11.0f + zCam);
	model_view *= Scale(3.0f, 2.0f, 0.1f);

    glEnable(GL_BLEND);
	drawBackBoard();
    glDisable(GL_BLEND);

    
    set_colour(0.70f, 0.70f, 0.70f);
	model_view = old_model_view;
	model_view *= RotateX(xCamTheta);
	model_view *= RotateY(yCamTheta);
	model_view *= Translate(-1.51f + xCam, 6.00f + yCam, -11.0f + zCam);
	model_view *= Scale(0.01f, 2.0f, 0.1f);
    
    drawCube();
    
    set_colour(0.70f, 0.70f, 0.70f);
	model_view = old_model_view;
	model_view *= RotateX(xCamTheta);
	model_view *= RotateY(yCamTheta);
	model_view *= Translate(1.51f + xCam, 6.00f + yCam, -11.0f + zCam);
	model_view *= Scale(0.01f, 2.0f, 0.1f);
    
    drawCube();


	set_colour(1.0f, 0.0f, 0.0f);
	for(int i = 0; i < 20; i++){
		model_view = old_model_view;
	   	model_view *= RotateX(xCamTheta);
		model_view *= RotateY(yCamTheta);
		model_view *= Translate(0.0f+xCam, 5.15+yCam, -10.5+zCam);
		model_view *= RotateY((i*18));
		model_view *= Translate( 0.0f, 0.0f, 0.3f);
		//model_view *= RotateX(90);
		model_view *= Scale(.07,.013,.01);
		drawCylinder();
	}


	set_colour(.7f, .7f, .7f);
	model_view = old_model_view;
	model_view *= RotateX(xCamTheta);
	model_view *= RotateY(yCamTheta);
	model_view *= Translate(ballX + xCam, ballY + yCam, ballZ + zCam);
	model_view *= Scale(0.2f, 0.2f, 0.2f);
    
    mat4 rotation = RotateX(rotballX) * RotateY(rotballY) * RotateZ(rotballZ);
    
    glUniformMatrix4fv( uRotateBall, 1, GL_TRUE, rotation );
    glUniform4f(uLightPos,  7, 9, light_z, 0.0f);

	drawSphere();
    
    glUniformMatrix4fv( uRotateBall, 1, GL_TRUE, mat4(1.0f) );
    glUniform4f(uLightPos,  light_x, light_y, light_z, 0.0f);
    
    //printf("%f, %f, %f", ballX, ballY, ballZ);

    myCloth->NetHit(vec3(ballX, ballY, ballZ), 0.25);

    if (DTIME < 10) {
        model_view = old_model_view;
        model_view *= RotateX(xCamTheta);
        model_view *= RotateY(yCamTheta);
        model_view *= Translate(xCam, yCam, zCam);
        //model_view *= Translate(xCam*0.875, yCam, (zCam*0.875)+1.3375);
        myCloth->Draw(DRAW_TYPE, DTIME/500, model_view, Lumina, CursorX, CursorY, Width, Height);
        glutSwapBuffers();
    }
    
    
}

/**********************************************
    PROC: myReshape()
    DOES: handles the window being resized 
    
      -- don't change
**********************************************************/

void myReshape(int w, int h)
{
    Width = w;
    Height = h;
    
    glViewport(0, 0, w, h);

    mat4 projection = Perspective(50.0f, (float)w/(float)h, 1.0f, 1000.0f);
    glUniformMatrix4fv( uProjection, 1, GL_TRUE, projection );
}




void idleCB(void)
{
//    if( Animate == 1 )
//    {
//        // TM.Reset() ; // commenting out this will make the time run from 0
//        // leaving 'Time' counts the time interval between successive calls to idleCB
//        if( Recording == 0 )
//            TIME = TM.GetElapsedTime() ;
//        else
//            TIME += 0.033 ; // save at 30 frames per second.
//        
//        eye.x = 20*sin(TIME);
//        eye.z = 20*cos(TIME);
//        
//        printf("TIME %f\n", TIME) ;
//        glutPostRedisplay() ; 
//    }

	
}


void timerFunc(int n)
//{
//    
//	if (pan && panTheta >= -90.0 && n%3 == 0){
//		/*xCam = -9.4 + sin(panTheta * DegreesToRadians);
//         zCam =  10.5 + cos(panTheta * DegreesToRadians);*/
//		//xCam -= .1678*cos(panTheta * DegreesToRadians);
//		/*zCam -= .1678*sin(panTheta * DegreesToRadians);*/
//		xCam -= .1638*cos(panTheta * DegreesToRadians);
//		zCam -= .1638*sin(panTheta * DegreesToRadians);
//		//printf ("xCam %f\n", xCam);
//		//printf ("zCam: %f\n", zCam);
//		panTheta -= 1;
//		yCamTheta -= 1;
//		
//	}
//    
//	if (isAnimate)
//	{
//		f_time += 0.001;
//		if(ballZ <= -10.85 && ballX <=1.5 && ballX >= -1.5 && ballY >= 5 && ballY <= 7)
//		{
//			back = true;
//			//printf("hit");
//		}
//		if(!back)
//		{
//			if (!isBaseline)
//			{
//				ballZ -= 0.015;
//			}
//			else
//			{
//				if (baselineLeft)
//				{
//					ballX += 0.014;
//				}
//				else
//					ballX -= 0.0162;
//			}
//			rotballZ += 10;
//			//ballY += (0.168* time) + ( (-0.98)*time*time ) / 2.0;
//			ballY += (0.177* f_time) + ( (-0.98)*f_time*f_time ) / 2.0;
//			velY += (-0.98)*f_time;
//			//printf("%f\n", velY);
//		}
//		else
//		{
//			ballZ += 0.015;
//			rotballX += 2;
//			//ballY += (0.168* time) + ( (-0.98)*time*time ) / 2.0;
//			ballY += (0.09* f_time) + ( (-0.98)*f_time*f_time ) / 2.0;
//			//printf("%f\n", ballZ);
//            
//            
//		}
//        
//        
//	}
//	if(isZero)
//		f_time = 0;
//	/*
//     time += 0.001;
//     ballZ -= 0.015;
//     ballY += (0.16* time) + ( (-0.98)*time*time ) / 2.0;
//     */
//    glutPostRedisplay();
//    glutTimerFunc(1, timerFunc, ++n);
//	
//}
{
    if(endPan && endTheta <= 0){
        xCam += .1638*cos(endTheta * DegreesToRadians);
		zCam += .1638*sin(endTheta * DegreesToRadians);
		endTheta += 1;
		yCamTheta += 1;
    }
	if (startPan && startTheta == -90.0){
		startPan  = false;
		startPan1 = true;
	}
    
	if (startPan1 && panTheta == 92)
		startPan1 = false;
    
	if(startPan && startTheta >= -90 && n%3 == 0){
		xCam -= .1570*cos(startTheta * DegreesToRadians);
		zCam -= .1570*sin(startTheta * DegreesToRadians);
		yCam += .072222;
		startTheta -= 1;
	}
    
	if(startPan1 && panTheta <= 92 && n%3 == 0){
		xCam += .1638*cos(panTheta * DegreesToRadians);
		zCam += .1638*sin(panTheta * DegreesToRadians);
		panTheta += 1;
		yCamTheta += 1;
		//printf ("Xcam: %f\n", xCam);
		//printf ("Zcam: %f\n", zCam);
	}
    
	if (panTheta == panArr[whichPan])
		pan = false;
	/*if (panTheta == panArr[1])
     pan = false;*/
    
	//if (pan && panTheta >= panArr[whichPan] && n%5 == 0){
	if (pan && panTheta >= -92.0 && n%3 == 0){
        xCam -= .1638*cos(panTheta * DegreesToRadians);
        zCam -= .1638*sin(panTheta * DegreesToRadians);
        ballX += .1213*cos(panTheta * DegreesToRadians);
        ballZ += .1213*sin(panTheta * DegreesToRadians);
        //printf ("Xcam: %f\n", xCam);
        //printf ("Zcam: %f\n", zCam);
        //printf ("yTheta: %f\n", yCamTheta);
        //printf ("ballX: %f\n", ballX);
        //printf ("ballZ: %f\n", ballZ);
        panTheta -= 1;
        yCamTheta -= 1;
        rotballY -= 1;
        //printf ("rotBallY: %f\n", rotballY);
        
	}
    
	if (isAnimate && n%1 == 0)
	{
		f_time += 0.01;
		/*printf("ballX: %f\n", ballX);
         printf("velX: %f\n", velX);
         printf("ballY: %f\n", ballY);
         printf("velY: %f\n", velY);
         printf("ballZ: %f\n", ballZ);
         printf("velZ: %f\n", velZ);*/
        
        if (whichShot == 0 || whichShot == 4){
            rotballZ += 10;
        }
        if (whichShot == 2)
            rotballX += 10;
        if(whichShot == 1 || whichShot == 3){
            //rotballX += (1/(sqrt(2))*10);
            rotballZ += (1/(sqrt(2))*10);
        }
        
        if ((5.10<ballY && ballY<5.15) && (-0.3 < ballX && ballX < 0.3) && (-10.8 < ballZ && ballZ < -10.2))
        {
            if (!scoreUpdated) {
                scoreCount++;
                scoreUpdated = true;
            }
            std::string file_no = "sb_" + (std::to_string(scoreCount%11)) + ".tga";
            const char * c = file_no.c_str();
            scoreBoardImage.loadTGA(c);
            glGenTextures( 1, &scoreBoard_texture );
            glBindTexture( GL_TEXTURE_2D, scoreBoard_texture );
            glTexImage2D(GL_TEXTURE_2D, 0, 4, scoreBoardImage.width, scoreBoardImage.height, 0,
                         (scoreBoardImage.byteCount == 3) ? GL_BGR : GL_BGRA,
                         GL_UNSIGNED_BYTE, scoreBoardImage.data );
            
            glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
            glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );
            glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        }
        
        
		GLfloat sqrtVal;
		for(int i = 0; i < numPoints; i++)
		{
			//printf("%f\n", rimArr[i].x);
			//continue;
			sqrtVal = sqrt(pow((rimArr[i].x - ballX), 2) + pow((rimArr[i].y - ballY), 2) + pow((rimArr[i].z - ballZ), 2));
			if( sqrtVal <= 0.2)
			{
                
				for(int i2 = 0; i2 < numPoints; i2++)
				{
					if(sqrt(pow((rimArr[i2].x - ballX), 2) + pow((rimArr[i2].y - ballY), 2) + pow((rimArr[i2].z - ballZ), 2)) < sqrtVal)
					{
						sqrtVal = sqrt(pow((rimArr[i2].x - ballX), 2) + pow((rimArr[i2].y - ballY), 2) + pow((rimArr[i2].z - ballZ), 2));
						i = i2;
					}
				}
                
				for(int temp = i-7; temp < i+8; temp++)
				{
                    
                    if(temp < 0)
                    {
						if((temp + numPoints) == hitRimIndex)
                            preventHit = 1;
                    }
					else if (temp >= numPoints)
					{
						if((temp - numPoints) == hitRimIndex)
                            preventHit = 1;
					}
                    else if(temp == hitRimIndex)
                        preventHit = 1;

				}
                if(preventHit != 1)
                {
                    printf(" %d\n", i);
                    //printf("%f\n", rimArr[i].x);
                    f_time = 0;
                    //timeZ = 0.0075;
                    ballZInit = ballZ;
                    ballYInit = ballY;
                    ballXInit = ballX;
                    vec3 ballPos = vec3(ballX, ballY, ballZ);
                    vec3 rimPoint = vec3(rimArr[i].x, rimArr[i].y, rimArr[i].z);
                    vec3 vel = vec3(velX, velY, velZ);
                    vec3 normal = rimPoint - ballPos;
                    GLfloat magnitudeN = sqrt((normal.x * normal.x) + (normal.y * normal.y) + (normal.z * normal.z));
                    //GLfloat magnitudeV = sqrt((velX * velX) + (velY * velY) + (velZ * velZ));
                    vec3 vProj;
                    
                    if(magnitudeN != 0)
                    {
                        vProj = (dot(vel, normal)/magnitudeN) * (normal/magnitudeN);
                    }
                    
                    
                    vec3 vT = vel - vProj;
                    vec3 vNew = ( (-0.8) * vProj) + vT;
                    velYInit = vNew.y;
                    velX = vNew.x;
                    velY = vNew.y;
                    velZ = vNew.z;
                    //isAnimate = false;
                    hitRimIndex = i;
                    //printf("normal: %f, %f, %f\n", normal.x, normal.y, normal.z);
                    //printf("vOld: %f, %f, %f\n", vel.x, vel.y, vel.z);
                    //printf("vNew: %f, %f, %f\n", vNew.x, vNew.y, vNew.z);
                    //return;
                    
                    
                    printf("HELLLLLLLO");
                    preventHit = 0;
                    //printf("%f\n", velY);
                }
                break;
            }
            
		}
        
        
        //	printf("%f\n", ballZ);
		//printf("%f\n", time);
		/*ballZ = ballZInit + velZ * timeZ;
         ballX = ballXInit + velX * timeZ;
         //velY = 8.907 + (-9.8 * time)/2.0;
         velY = velYInit + (-9.8*time)/2.0;
         ballY = ballYInit + velY * time - diff;*/
        
		ballZ +=  velZ * 0.01;
		ballX +=  velX * 0.01;
		//velY = 8.907 + (-9.8 * time)/2.0;
		velY = velYInit + (-9.8*f_time);
		ballY +=  velY * 0.01;
        
		if(ballY <= groundInit)
		{
			//velYInit = (-0.4) * velY;
			//time = 0;
			//printf("WOW %f\n", velY);
			groundInit = ballY;
			if(bounces < 2)
			{
				//printf("lol");
				velYInit = (-0.4) * velY;
				f_time = 0;
			}
			bounces++;
			if (shotCounter < 2 && bounces >= 2){
				ballX = shotPosArr[whichShot].initX;
				ballY = 3.0;
				ballZ= shotPosArr[whichShot].initZ;
                velYInit = 8.107;
				//printf("ballX: %f        ballZ: %f\n", ballX, ballZ);
				isAnimate = !isAnimate;
				//printf("%f\n", whichShot);
				bounces = 0;
                pbOn = true;
                accOn = true;
                pbGoingUp = true;
                accGoingUp = true;
                powerBarX = 0;
                accBarX = 0;
                rotballX = 180;
                rotballZ = 0;
			} else if (shotCounter >= 2 && bounces >= 2){
                if(whichShot == 4){
                    endPan = true;
                }
				ballX = shotPosArr[whichShot].initX;
				ballY = 3.0;
				ballZ= shotPosArr[whichShot].initZ;
                velYInit = 8.107;
				//printf("ballX: %f        ballZ: %f\n", ballX, ballZ);
				isAnimate = !isAnimate;
				//printf("%f\n", whichShot);
				shotCounter = 0;
				whichPan = whichShot%4;
				whichShot = whichPan+1;
				pan = true;
				bounces = 0;
                pbOn = true;
                accOn = true;
                pbGoingUp = true;
                accGoingUp = true;
                powerBarX = 0;
                accBarX = 0;
                rotballX = 180;
                rotballZ = 0;			}
		}
		if(ballZ <= backBoardInit && ballX >= -2.0 && ballX <= 2.0 && ballY <= 7.0 && ballY >= 3.0)
		{
			printf("HITTTTTTTTTTTTTT");
			//printf("%f\n", ballZ);
			//printf("%f\n", time);
			//ballZInit = ballZ;
			//printf("%f\n", ballZInit);
			backBoardInit = ballZ;
			velZ = (-0.25) * velZ;
			hitRimIndex = -1;
			//time = 0;
			//back = true;
			//printf("hit");
		}
        if(ballZ <= -13.0)
		{
			velZ = (-0.25) * velZ;
		}
        
        
	}
    glutPostRedisplay();
    glutTimerFunc(1, timerFunc, ++n);
	
}



void initScene(){
    //set up the camera
    Pentax.position = vec4(0,0,15.0,1.0);
    Pentax.lookat = vec4(0,0,-10000.0,1.0);
    Pentax.up = vec4(0,1.0,0,0);
    Pentax.fovy = 60;
    Pentax.aspect = 1.0;
    Pentax.znear = 5.0;
    Pentax.zfar = -100;

    //set up the light
    Lumina.position = vec4(0,15,0,1.0);
    Lumina.color = vec4(1.0,1.0,1.0,1.0);
    
    //set up the objects
    myCloth = new Cloth;
    myCloth->WeaveCloth(8,20,0.05,0.9,50.0,0.5,0.1,-5.5,2.0,-5.0, true);//N, Mass, Length,ks,kd,kf,sx,sy,sz
    myCloth->WeaveCloth(15,20,0.05,0.25,50.0,0.5,0.1,0,0,0, true);//N, Mass, Length,ks,kd,kf,sx,sy,sz
    
    myCloth->InitDraw();
    
    //    myCube =  new Cube;
    //    myCube->InitData(vec3(0.0,0.0,0.0), vec3(0,0,0), 1);
    //    myCube->InitDraw();
    
}

void idleCallback(){
    if (startGame)
    {
        xCam = 9.05;
        zCam = -8.03320;
        yCam = -10.0;
        startPan = !startPan;
        startGame = false;
    }
    
    if (powerBarX == 250 || powerBarX == -250)
        pbGoingUp = !pbGoingUp;
    
    if (pbGoingUp && pbOn)
        powerBarX+=10;
    else if (pbOn)
        powerBarX-=10;
    
    if (accBarX == 250 || accBarX == -250)
        accGoingUp = !accGoingUp;
    
    if (accGoingUp && accOn)
        accBarX+=10;
    else if (accOn)
        accBarX-=10;
    
    if(STOP == -1){
        TIME2 = glutGet( GLUT_ELAPSED_TIME );
        DTIME = TIME2 - TIME1;
        TIME1 = TIME2;
        glutPostRedisplay();
    }
    else{
        TIME2 = glutGet( GLUT_ELAPSED_TIME );
        DTIME = TIME2 - TIME1;
        TIME1 = TIME2;
    }
    
}

//void displayCallback()
//{
//    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//    
//    if (DTIME < 10) {
//        drawScene();
//        glutSwapBuffers();
//    }
//}


/*********************************************************
     PROC: main()
     DOES: calls initialization, then hands over control
           to the event handler, which calls 
           display() whenever the screen needs to be redrawn
**********************************************************/

int main(int argc, char** argv) 
{
    createPoints(rimArr);
	shotPosArr[0].initX = -7.0;
	shotPosArr[0].initZ = -10.54;
	shotPosArr[0].swishVelX = 5.35;
	shotPosArr[0].swishVelZ = 0;
    
	shotPosArr[1].initX = -5.013687;
	shotPosArr[1].initZ = -5.365479;
	shotPosArr[1].swishVelX = 3.9;
	shotPosArr[1].swishVelZ = -4.9;
    
	
	//shotPosArr[2].initX = 0.004101;
	shotPosArr[2].initX = -0.117199;
	shotPosArr[2].initZ = -3.287045;
	shotPosArr[2].swishVelX = 0.05;
	shotPosArr[2].swishVelZ = -5.55;
    
	shotPosArr[3].initX = 4.814817;
	shotPosArr[3].initZ = -5.279707;
	shotPosArr[3].swishVelX = -3.7;
	shotPosArr[3].swishVelZ = -4.05;
    
	shotPosArr[4].initX = 6.891135;
	shotPosArr[4].initZ = -10.418774;
	shotPosArr[4].swishVelX = -5.28;
	shotPosArr[4].swishVelZ = 0;
    
	velZ = shotPosArr[0].swishVelZ;
	velX = shotPosArr[0].swishVelX;
    
    
    glutInit(&argc, argv);
    // If your code fails to run, uncommenting these lines may help.
    //glutInitContextVersion(3, 2);
    //glutInitContextProfile(GLUT_CORE_PROFILE);
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowPosition (0, 0);
    glutInitWindowSize(Width,Height);
    glutCreateWindow(argv[0]);
    glewExperimental = GL_TRUE;
    glewInit();

    //initScene();
    
    myinit();

    glutIdleFunc(idleCB) ;
    glutReshapeFunc (myReshape);
    glutKeyboardFunc( myKey );
    glutTimerFunc(10, timerFunc, 0);
    glutIdleFunc(idleCallback);
    glutDisplayFunc(display);

    
    
    TIME1 = glutGet( GLUT_ELAPSED_TIME );//Start Timer
    
    glutMainLoop();

    return 0;         // never reached

}




