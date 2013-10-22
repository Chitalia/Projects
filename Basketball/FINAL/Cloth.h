//
//  Cloth.h
//  ClothSimulation
//
//  Created by 静一 方 on 11/14/11.
//  Copyright 2011 UCLA. All rights reserved.
//

#ifndef ClothSimulation_Cloth_h
#define ClothSimulation_Cloth_h

#include <OpenGL/glu.h>
#include "Angel.h"
#include "Camera.h"
#include "Light.h"

#define GRAVITY_CONSTANT 9.8

#define DRAW_LINE 1
#define DRAW_TRIANGLE 2
#define DRAW_CLOTH 3

class SpringDamper;

class Node{
    //the Node class contains the mass, position, velocity and neighbor of the node
    //currently one Dimension
public:
    float Mass;
    int Fixed;
    int Updated; //-1 or 1; alternate
    Angel::vec3 Velocity;
    Angel::vec3 Position;
    Angel::vec3 pPosition;
    Angel::vec3 oPosition;
    Angel::vec3 Force;
    float kFriction;
    SpringDamper * Up;
    SpringDamper * Down;
    SpringDamper * Left;
    SpringDamper * Right;
    bool visited;
    Node()
    {Mass = 1; Fixed = 0; Updated  = -1; Velocity = Angel::vec3(0.0,0.0,0.0); kFriction = 1.0;Position = Angel::vec3(0.0,0.0,0.0); Force = Angel::vec3(0.0,-Mass*GRAVITY_CONSTANT,0.0);}//Default Constructor
    ~Node(){};
};

class SpringDamper{
public:
    float length;//default length
    float Cdeform;//Current deforAngel::mation
    float Pdeform;//Previous deforAngel::mation(last time step)
    float kSpring;//spring constant
    float kDamper;//damper constant
    Node * Previous;//previous node
    Node * Next;//next node
    SpringDamper(){kSpring = 1.0;  kDamper = 1.0;  length = 0.05; Cdeform = 0; Pdeform = 0;}
    ~SpringDamper(){}
};

class Cloth{
    
private:
    //real data
    int Num_r;
    int Num_c;
    Node *Head;//Currently, Head is fixed
    Node *End;
    float Mass;//Default Mass for each node
    float Length;
    float kSpring;//Default Spring constant for each spring-damper
    float kDamper;//Dafault Damper constant for each spring-damper
    float kFriction;//Default Friciton constant for each node
    
    //below are for drawing
    GLuint vertexArrayObject;                      
    GLuint vertexBufferObject;  
    GLuint shader;
    
    //data for drawing
    int NLinePoints;
    Angel::vec4 *LinePoints;//to be sent to GPU
    Angel::vec4 *LineColors;//to be sent to GPU
    Angel::vec3 *LPointNormals;
    
    int NTrianglePoints;
    Angel::vec4 *TrianglePoints;
    Angel::vec4 *TriangleColors;
    Angel::vec3 *TPointNormals;
    
    int Manipulated;
    vec3 Maniposition;//the position of the manipulator in 3D
    Node *MadNode;//the manipulated node
    Camera Eye;
    
public:
    
    Cloth();
    ~Cloth();
    Node * WeaveLine(float sx, float sy, float sz, int fixed, float radius);
    void WeaveConnect(Node* previous_line, Node* current_line);
    void WeaveCloth(int N_r, int N_c,float NodeMass,float SD_Length,float ks,float kd,float kf,float Start_x,float Start_y,float Start_z, bool newLoop);//Init the number of nodes, the mass of nodes and length of s-d, the Spring and Damper constants, the fixed point
    void UpdateAll(double deltat,double ManipX,double ManipY, double ManipZ, Light light);//Update the position, velocity, force, based on Newton Mechanics, Using Implicit Euler Method, Update the vertexs for drawing
    void InitDraw();//Init the vertexs and color data on GPU, Init the shader program, link the shader program with the buffer data
    void Draw(int type, double deltat , mat4 cMw,Light light, double cursor_x, double cursor_y, float w, float h);//Update data on GPU's buffer and draw the vertexs
    void Select(double cursorX,double cursorY,double range, Camera camera);//Give the mouse selection position (screen coordinate)
    
    void NetHit(vec3 ballPos, float ballRadius);//Give the mouse selection position (screen coordinate)
    
    void Deselect(){Manipulated = 0; std::cout<<"Node Dismanipulated!"<<std::endl;}
    void Anchor(){MadNode->Fixed = 1; std::cout<<"Node Fixed!"<<std::endl;};//Anchor the current Manipualated Node
    void Deanchor(){MadNode->Fixed = 0; std::cout<<"Node Released!"<<std::endl;}
    void MouseLeft(float cursorX, float cursorY, Camera camera);
    void MouseRight(float cursorX, float cursorY, Camera camera);
};

#endif
