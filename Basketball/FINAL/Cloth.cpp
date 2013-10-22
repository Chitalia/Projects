//
//  Cloth.cpp
//  ClothSimulation
//
//  Created by Franklin Fang on 11/14/11.
//  Copyright 2011 UCLA. All rights reserved.
//

#include "Cloth.h"
#include <cmath>
#include <queue>

typedef std::queue<Node*> NodeQueue;

GLint uProjection1;

bool check = true;

Cloth::Cloth(){
    WeaveCloth(11,20,0.035,0.075,12.0,0.5,0.1,0,5.1,-10.5, true);
    InitDraw();
}

Cloth::~Cloth(){

}

Node * Cloth::WeaveLine(float sx, float sy, float sz, int fixed, float radius){

    Node *Left;
    Left = new Node;
    Left->Mass = Mass;
    Left->Updated = -1;
    Left->Fixed = fixed;//head is fixed
    Left->Position.x = sx + radius;
    Left->Position.y = sy;
    Left->Position.z = sz;
    Left->pPosition = Left->Position;
    Left->kFriction = kFriction;
    
    Left->Left = 0;
    Left->Up = 0;
    Left->Down = 0;
    
    Node * line_head = Left;

    Node *Current;
    SpringDamper *Connection;
    Angel::vec3 Connectionvec;
    for (int i = 1; i < Num_c; i++) {
        Current = new Node;
        Current->Mass = Mass;
        Current->Fixed = fixed;
        Current->Updated = -1;
        Current->Force = Angel::vec3(0,-Mass*GRAVITY_CONSTANT,0);
        //Current->Position.x = sx +  i*Length;//interval of 0.1
        Current->Position.x = sx + radius * (cos((i*18) * DegreesToRadians));
        Current->Position.y = sy ;
        Current->Position.z = sz + radius * (sin((i*18) *DegreesToRadians));
        //Current->Position.z = sz;
        Current->pPosition = Current->Position;
        Current->oPosition = Current->Position;
        Current->kFriction = kFriction;
    
        
        Connectionvec = Current->Position - Left->Position;
        Connection = new SpringDamper;
        Connection->length = Length;
        Connection->Cdeform = Angel::length(Connectionvec) - Connection->length;
        Connection->Pdeform = Connection->Cdeform;
        Connection->kSpring = kSpring;
        Connection->kDamper = kDamper;
        Connection->Previous = Left;
        Connection->Next = Current;
        
        Left->Right = Connection;
        Current->Left = Connection;
        Current->Up = 0;
        Current->Down = 0;
        Current->Right = 0;
        Left = Current;
    }
    
//    Connectionvec = line_head->Position - Current->Position;
//    Connection = new SpringDamper;
//    Connection->length = Length;
//    Connection->Cdeform = Angel::length(Connectionvec) - Connection->length;
//    Connection->Pdeform = Connection->Cdeform;
//    Connection->kSpring = kSpring;
//    Connection->kDamper = kDamper;
//    Connection->Previous = Current;
//    Connection->Next = line_head;
//    
//    line_head->Left = Connection;
//    //Current->Right = Connection;
    
    
    
    Current->Fixed = fixed;
    
    return line_head;
}


void Cloth::WeaveConnect(Node* previous, Node* current){

    SpringDamper *Connection;
    Angel::vec3 Connectionvec;
    while (previous->Right) {
        Connectionvec = previous->Position - current->Position;
        Connection = new SpringDamper;
        Connection->length = Length;
        Connection->Cdeform = Angel::length(Connectionvec) - Connection->length;
        Connection->Pdeform = Connection->Cdeform;
        Connection->kSpring = kSpring;
        Connection->kDamper = kDamper;
        
        previous->Down = Connection;
        current->Up = Connection;
        
        Connection->Previous = previous;
        Connection->Next = current;
        
        previous = previous->Right->Next;
        current = current->Right->Next;
    }
    
    Connectionvec = previous->Position - current->Position;
    Connection = new SpringDamper;
    Connection->length = Length;
    Connection->Cdeform = Angel::length(Connectionvec) - Connection->length;
    Connection->Pdeform = Connection->Cdeform;
    Connection->kSpring = kSpring;
    Connection->kDamper = kDamper;
    
    previous->Down = Connection;
    current->Up = Connection;
    
    Connection->Previous = previous;
    Connection->Next = current;
    

}


void Cloth::WeaveCloth(int N_r, int N_c, float mass, float length, float ks, float kd, float kf, float sx, float sy, float sz, bool newLoop){

    Num_r = N_r;
    Num_c = N_c;
    Mass = mass;//default mass for nodes
    kSpring = ks;//default spring constant
    kDamper = kd;//default damper constant
    kFriction = kf;
    Length = length;//rest length of springdamper
    
    //weed the first line, then second line, hook up, then third line
    
    //weed the first line:
    
    Head = WeaveLine(sx, sy, sz,2, length*4);//first line is fixed
    
    
    Node* previous_line = Head;
    Node* current_line;
    current_line = WeaveLine(sx, sy-1*Length, sz,2, length*(3));
    WeaveConnect(previous_line, current_line);
    previous_line = current_line;
    
    int i;
    //weed the other lines, hook each line up with the previous line
    for (i = 3; i < Num_r - 5; i++){
        current_line = WeaveLine(sx, sy-i*Length, sz,0, length*(2.5));
        WeaveConnect(previous_line, current_line);
        previous_line = current_line;
    }
    current_line = WeaveLine(sx, sy-i*Length, sz,0, length*(2.5));
    WeaveConnect(previous_line, current_line);
    
    
    Manipulated = 0;
}

void Cloth::InitDraw(){
    
    // Initialize the data array on CPU
    NLinePoints = 2*(2*Num_c*Num_r- Num_c - Num_r);
    LinePoints = new Angel::vec4[NLinePoints];
    LineColors = new Angel::vec4[NLinePoints];
    LPointNormals = new Angel::vec3[NLinePoints];
       
    NTrianglePoints = 6*(Num_c-1)*(Num_r-1);
    TrianglePoints = new Angel::vec4[NTrianglePoints];
    TriangleColors = new Angel::vec4[NTrianglePoints];
    TPointNormals = new Angel::vec3[NTrianglePoints];
    
    //Create the Vertex Array and Buffers, bind them
    glGenVertexArraysAPPLE(1, &vertexArrayObject);
    glGenBuffers(1, &vertexBufferObject);//generate buffer for current vertex array
    
    //load and compile shaders on GPU, use current shader program
    shader = InitShader( "vCloth.glsl", "fCloth.glsl" );
    glUseProgram(shader);
}


void Cloth::UpdateAll(double dt,double x, double y, double z, Light light){
    
    //Currently, Leave it static
    //TODO: Add Damper, Change to Implicit Method
    
    Node *CurrentNode;
    Node *LeftNode;
    Node *RightNode;
    Node *UpNode;
    Node *DownNode;
    SpringDamper *UpConnection;
    SpringDamper *DownConnection;
    SpringDamper *LeftConnection;
    SpringDamper *RightConnection;
    Angel::vec3 SpringForce;
    Angel::vec3 DamperForce;
    Angel::vec3 Gravity;
    Angel::vec3 Friction;//friction with air, proportional to velocity
    
    int updateFlag;
    
    NodeQueue node_queue;
    
    
    node_queue.push(Head);
    updateFlag = Head->Updated*(-1);
    Head->Updated = updateFlag;//mark as updated, means queued

    
    //calculate position based on memory position and velocity
    
    while (!node_queue.empty()) {
        CurrentNode = node_queue.front();
        node_queue.pop();
        
            SpringForce = 0;
            DamperForce = 0;
            //force on right
            RightConnection = CurrentNode->Right;
            if (RightConnection) {
                RightNode = RightConnection->Next;
                if(RightNode->Updated != updateFlag){
                    node_queue.push(RightNode);//push into the queue the node that has not been updated
                    RightNode->Updated = updateFlag;
                }
                
                SpringForce += RightConnection->kSpring*(RightConnection->Cdeform/RightConnection->length)*(RightNode->pPosition-CurrentNode->pPosition);
                DamperForce += RightConnection->kDamper*(RightConnection->Cdeform - RightConnection->Pdeform)/(RightConnection->length*dt)*(RightNode->pPosition-CurrentNode->pPosition);
            }
            //force on left
            LeftConnection = CurrentNode->Left;
            if (LeftConnection) {
                LeftNode = LeftConnection->Previous;
                if (LeftNode->Updated != updateFlag) {
                    node_queue.push(LeftNode);
                    LeftNode->Updated = updateFlag;
                }
                SpringForce += LeftConnection->kSpring*(LeftConnection->Cdeform/LeftConnection->length)*(LeftNode->pPosition-CurrentNode->pPosition);
                DamperForce += LeftConnection->kDamper*(LeftConnection->Cdeform - LeftConnection->Pdeform)/(LeftConnection->length*dt)*(LeftNode->pPosition-CurrentNode->pPosition);
            }
            //force on up
            UpConnection = CurrentNode->Up;
            if (UpConnection) {
                UpNode = UpConnection->Previous;
                if (UpNode->Updated != updateFlag) {
                    node_queue.push(UpNode);
                    UpNode->Updated = updateFlag;
                }
                SpringForce += UpConnection->kSpring*(UpConnection->Cdeform/UpConnection->length)*(UpNode->pPosition-CurrentNode->pPosition);
                DamperForce += UpConnection->kDamper*(UpConnection->Cdeform - UpConnection->Pdeform)/(UpConnection->length*dt)*(UpNode->pPosition-CurrentNode->pPosition);
            }
            //force on down
            DownConnection = CurrentNode->Down;
            if (DownConnection) {
                DownNode = DownConnection->Next;
                if(DownNode->Updated != updateFlag){
                    node_queue.push(DownNode);
                    DownNode->Updated = updateFlag;
                }
                SpringForce += DownConnection->kSpring*(DownConnection->Cdeform/DownConnection->length)*(DownNode->pPosition-CurrentNode->pPosition);
                DamperForce += DownConnection->kDamper*(DownConnection->Cdeform - DownConnection->Pdeform)/(DownConnection->length*dt)*(DownNode->pPosition-CurrentNode->pPosition);
            }
            
        if (!CurrentNode->Fixed) {//fixed node need no update of position here
            
            Gravity = Angel::vec3(0,-CurrentNode->Mass*GRAVITY_CONSTANT,0);
            Friction = -CurrentNode->kFriction*CurrentNode->Velocity;
            CurrentNode->Force = (SpringForce + Gravity + DamperForce + Friction);
//            if (CurrentNode->Force.x > 125)
//            {
//                printf("LOLOL");
//                CurrentNode->Force.x = 125;
//            }
//            if (CurrentNode->Force.y > 125)
//            {
//                printf("LOLOL");
//                CurrentNode->Force.y = 125;
//            }
//            if (CurrentNode->Force.z > 125)
//            {
//                printf("LOLOL");
//                CurrentNode->Force.z = 125;
//            }
//            if (CurrentNode->Force.x < -125)
//            {
//                printf("LOLOL");
//                CurrentNode->Force.x = -125;
//            }
//            if (CurrentNode->Force.y < -125)
//            {
//                printf("LOLOL");
//                CurrentNode->Force.y = -125;
//            }
//            if (CurrentNode->Force.z < -125)
//            {
//                printf("LOLOL");
//                CurrentNode->Force.z = -125;
//            }
            CurrentNode->Velocity += CurrentNode->Force*dt/CurrentNode->Mass;
            CurrentNode->Position += CurrentNode->Velocity*dt;

        }
        
    }

    
    //deal with manipulated node
    if(Manipulated){
        
        MadNode->Position = Angel::vec3(x,y,z);//x,y,z is the position of the manipulator
        MadNode->Velocity = Angel::vec3(0,0,0);
        MadNode->Force = Angel::vec3(0,0,0);
        
    }
    
    
    //update the old memory for position
    
    node_queue.push(Head);
    updateFlag = Head->Updated*(-1);
    Head->Updated = updateFlag;
    Angel::vec3 Connectionvec;

    while (!node_queue.empty()) {
        CurrentNode = node_queue.front();
        node_queue.pop();
        
        CurrentNode->pPosition = CurrentNode->Position;//update the previous position memory
        
        RightConnection = CurrentNode->Right;
        if (RightConnection) {
            RightNode = RightConnection->Next;
            if(RightNode->Updated != updateFlag){
               
                node_queue.push(RightNode);//push into the queue the node that has not been updated
                RightNode->Updated = updateFlag;//update flag
                
            }
        }
        
        LeftConnection = CurrentNode->Left;
        if (LeftConnection) {
            LeftNode = LeftConnection->Previous;
            if (LeftNode->Updated != updateFlag) {
                
                node_queue.push(LeftNode);
                LeftNode->Updated = updateFlag;
            }
        }
        
        UpConnection = CurrentNode->Up;
        if (UpConnection) {
            UpNode = UpConnection->Previous;
            if (UpNode->Updated != updateFlag) {
                node_queue.push(UpNode);
                UpNode->Updated = updateFlag;
            }
        }
        
        DownConnection = CurrentNode->Down;
        if (DownConnection) {
            DownNode = DownConnection->Next;
            if(DownNode->Updated != updateFlag){
                node_queue.push(DownNode);
                DownNode->Updated = updateFlag;
               
            }
        }

    }
    
    //update drawing buffer: Points and Colors, Normals, etc
    //update old memory for springdamper length
    
    int i = 0;
    int j = 0;
    double stretch;
    Node* line_head;
    line_head = Head;
    
    Angel::vec3 temp_normal_1;
    Angel::vec3 temp_normal_2;
    Angel::vec3 temp_normal_3;
    Angel::vec3 temp_normal_4;
 
    
    Angel::vec3 temp_vec_up;
    Angel::vec3 temp_vec_down;
    Angel::vec3 temp_vec_left;
    Angel::vec3 temp_vec_right;
    
    int temp_count;
    
    while (line_head) {
        CurrentNode = line_head;
        RightConnection = CurrentNode->Right;
        while (RightConnection) {
            
            RightNode = RightConnection->Next;
            
            LinePoints[2*i] = Angel::vec4(CurrentNode->Position.x,CurrentNode->Position.y,CurrentNode->Position.z,1); 
            LinePoints[2*i+1] = Angel::vec4(RightNode->Position.x,RightNode->Position.y,RightNode->Position.z,1); 
            
            
            if(CurrentNode->Up)
                temp_vec_up = CurrentNode->Up->Previous->Position - CurrentNode->Position;
            if(CurrentNode->Down)
                temp_vec_down = CurrentNode->Down->Next->Position - CurrentNode->Position;
            if(CurrentNode->Left)
                temp_vec_left = CurrentNode->Left->Previous->Position - CurrentNode->Position;
            if(CurrentNode->Right)
                temp_vec_right = CurrentNode->Right->Next->Position - CurrentNode->Position;
            
            LPointNormals[2*i]  = vec3(0,0,0);
            temp_count = 0;
            if(CurrentNode->Up&&CurrentNode->Left){
                temp_normal_1 = Angel::cross(temp_vec_up, temp_vec_left);
                LPointNormals[2*i] += temp_normal_1;
                temp_count ++;
            }
            if(CurrentNode->Up&&CurrentNode->Right){
                temp_normal_2 = Angel::cross(temp_vec_right, temp_vec_up);
                LPointNormals[2*i] += temp_normal_2;
                temp_count ++;
            }
            if(CurrentNode->Down&&CurrentNode->Right){
                temp_normal_3 = Angel::cross(temp_vec_down, temp_vec_right);
                LPointNormals[2*i] += temp_normal_3;
                temp_count ++;
            }
            if (CurrentNode->Left&&CurrentNode->Down){
                temp_normal_4 = Angel::cross(temp_vec_left, temp_vec_down);
                LPointNormals[2*i] += temp_normal_4;
                temp_count ++;
            }
                
            
            LPointNormals[2*i] = LPointNormals[2*i]/temp_count;//average over the neighbor faces' normals
            
            if(RightNode->Up)
                temp_vec_up = RightNode->Up->Previous->Position - RightNode->Position;
            if(RightNode->Down)
                temp_vec_down = RightNode->Down->Next->Position - RightNode->Position;
            if(RightNode->Left)
                temp_vec_left = RightNode->Left->Previous->Position - RightNode->Position;
            if(RightNode->Right)
                temp_vec_right = RightNode->Right->Next->Position - RightNode->Position;
            
            temp_count = 0;
            LPointNormals[2*i+1]  = vec3(0,0,0);
            if(RightNode->Up&&RightNode->Left){
                temp_normal_1 = Angel::cross(temp_vec_up, temp_vec_left);
                LPointNormals[2*i+1] += temp_normal_1;
                temp_count ++;
            }
            if(RightNode->Up&&RightNode->Right){
                temp_normal_2 = Angel::cross(temp_vec_right, temp_vec_up);
                LPointNormals[2*i+1] += temp_normal_2;
                temp_count ++;
            }
            if(RightNode->Down&&RightNode->Right){
                temp_normal_3 = Angel::cross(temp_vec_down, temp_vec_right);
                LPointNormals[2*i+1] += temp_normal_3;
                temp_count ++;
            }
            if (RightNode->Left&&RightNode->Down){
                temp_normal_4 = Angel::cross(temp_vec_left, temp_vec_down);
                LPointNormals[2*i+1] += temp_normal_4;
                temp_count ++;
            }
            
            LPointNormals[2*i+1] = LPointNormals[2*i+1]/temp_count;
                        
            stretch = fabs(RightConnection->Cdeform)/RightConnection->length;
            stretch = stretch/3.0;
            stretch =  sqrt(stretch);
            
            //stretch = Angel::length(CurrentNode->Force);
            
            LineColors[2*i] = Angel::vec4(stretch,1-stretch,0,1);//green when no stretch, red when very stressed
            LineColors[2*i+1] = Angel::vec4(stretch,1-stretch,0,1);//green when no stretch, red when very stressed
            
            i++;
            
            RightConnection->Pdeform = RightConnection->Cdeform;
            Connectionvec = CurrentNode->Position - RightNode->Position;
            RightConnection->Cdeform = Angel::length(Connectionvec) - RightConnection->length;

              
            DownConnection = CurrentNode->Down;
            
            if(DownConnection){//draw the down connection if exist
                DownNode = DownConnection->Next;
                
                LinePoints[2*i] = Angel::vec4(CurrentNode->Position.x,CurrentNode->Position.y,CurrentNode->Position.z,1); 
                LinePoints[2*i+1] = Angel::vec4(DownNode->Position.x,DownNode->Position.y,DownNode->Position.z,1); 
                
                if(CurrentNode->Up)
                    temp_vec_up = CurrentNode->Up->Previous->Position - CurrentNode->Position;
                if(CurrentNode->Down)
                    temp_vec_down = CurrentNode->Down->Next->Position - CurrentNode->Position;
                if(CurrentNode->Left)
                    temp_vec_left = CurrentNode->Left->Previous->Position - CurrentNode->Position;
                if(CurrentNode->Right)
                    temp_vec_right = CurrentNode->Right->Next->Position - CurrentNode->Position;
                
                temp_count = 0;
                LPointNormals[2*i]  = vec3(0,0,0);
                if(CurrentNode->Up&&CurrentNode->Left){
                    temp_normal_1 = Angel::cross(temp_vec_up, temp_vec_left);
                    LPointNormals[2*i] += temp_normal_1;
                    temp_count ++;
                }
                if(CurrentNode->Up&&CurrentNode->Right){
                    temp_normal_2 = Angel::cross(temp_vec_right, temp_vec_up);
                    LPointNormals[2*i] += temp_normal_2;
                    temp_count ++;
                }
                if(CurrentNode->Down&&CurrentNode->Right){
                    temp_normal_3 = Angel::cross(temp_vec_down, temp_vec_right);
                    LPointNormals[2*i] += temp_normal_3;
                    temp_count ++;
                }
                if (CurrentNode->Left&&CurrentNode->Down){
                    temp_normal_4 = Angel::cross(temp_vec_left, temp_vec_down);
                    LPointNormals[2*i] += temp_normal_4;
                    temp_count ++;
                }
                
                
                LPointNormals[2*i] = LPointNormals[2*i]/temp_count;//average over the neighbor faces' normals
                
                if(DownNode->Up)
                    temp_vec_up = DownNode->Up->Previous->Position - DownNode->Position;
                if(DownNode->Down)
                    temp_vec_down = DownNode->Down->Next->Position - DownNode->Position;
                if(DownNode->Left)
                    temp_vec_left = DownNode->Left->Previous->Position - DownNode->Position;
                if(DownNode->Right)
                    temp_vec_right = DownNode->Right->Next->Position - DownNode->Position;
                
                temp_count = 0;
                LPointNormals[2*i+1]  = vec3(0,0,0);
                if(DownNode->Up&&DownNode->Left){
                    temp_normal_1 = Angel::cross(temp_vec_up, temp_vec_left);
                    LPointNormals[2*i+1] += temp_normal_1;
                    temp_count ++;
                }
                if(DownNode->Up&&DownNode->Right){
                    temp_normal_2 = Angel::cross(temp_vec_right, temp_vec_up);
                    LPointNormals[2*i+1] += temp_normal_2;
                    temp_count ++;
                }
                if(DownNode->Down&&DownNode->Right){
                    temp_normal_3 = Angel::cross(temp_vec_down, temp_vec_right);
                    LPointNormals[2*i+1] += temp_normal_3;
                    temp_count ++;
                }
                if (DownNode->Left&&DownNode->Down){
                    temp_normal_4 = Angel::cross(temp_vec_left, temp_vec_down);
                    LPointNormals[2*i+1] += temp_normal_4;
                    temp_count ++;
                }
                
                LPointNormals[2*i+1] = LPointNormals[2*i+1]/temp_count;
            
                
                stretch = fabs(DownConnection->Cdeform)/DownConnection->length;
                stretch = stretch/3.0;
                stretch =  sqrt(stretch);
                
                LineColors[2*i] = Angel::vec4(stretch,1-stretch,0,1);//green when no stretch, red when very stressed
                LineColors[2*i+1] = Angel::vec4(stretch,1-stretch,0,1);//green when no stretch, red when very stressed
                
                i++;
                
                DownConnection->Pdeform = DownConnection->Cdeform;
                Connectionvec = CurrentNode->Position - DownNode->Position;
                DownConnection->Cdeform = Angel::length(Connectionvec) - DownConnection->length;
                
                
                
                //deal with triangle mesh
                

                //in this scope, means that current node has a valid right connection and down connection
                //so there exist two triangles
                
                TrianglePoints[6*j] = Angel::vec4(CurrentNode->Position.x,CurrentNode->Position.y,CurrentNode->Position.z,1);
                TrianglePoints[6*j+1] = Angel::vec4(RightNode->Position.x,RightNode->Position.y,RightNode->Position.z,1); 
                TrianglePoints[6*j+2] = Angel::vec4(DownNode->Position.x,DownNode->Position.y,DownNode->Position.z,1); 
                
                TrianglePoints[6*j+3] = Angel::vec4(RightNode->Down->Next->Position.x,RightNode->Down->Next->Position.y,RightNode->Down->Next->Position.z,1);
                TrianglePoints[6*j+4] = Angel::vec4(RightNode->Position.x,RightNode->Position.y,RightNode->Position.z,1);
                TrianglePoints[6*j+5] = Angel::vec4(DownNode->Position.x,DownNode->Position.y,DownNode->Position.z,1); 
                
                TriangleColors[6*j] = Angel::vec4(1,0,0,1);
                TriangleColors[6*j+1] = Angel::vec4(1,0,0,1);
                TriangleColors[6*j+2] = Angel::vec4(1,0,0,1);
                TriangleColors[6*j+3] = Angel::vec4(1,0,0,1);
                TriangleColors[6*j+4] = Angel::vec4(1,0,0,1);
                TriangleColors[6*j+5] = Angel::vec4(1,0,0,1);
            
                
                temp_count = 0;
                TPointNormals[6*j] = 0;
                if(CurrentNode->Up&&CurrentNode->Left){
                    temp_vec_up = CurrentNode->Up->Previous->Position - CurrentNode->Position;
                    temp_vec_left = CurrentNode->Left->Previous->Position - CurrentNode->Position;
                    temp_normal_1 = Angel::cross(temp_vec_up, temp_vec_left);
                    TPointNormals[6*j] += temp_normal_1;
                    temp_count ++;
                }
                if(CurrentNode->Up&&CurrentNode->Right){
                    temp_vec_up = CurrentNode->Up->Previous->Position - CurrentNode->Position;
                    temp_vec_right = CurrentNode->Right->Next->Position - CurrentNode->Position;
                    temp_normal_2 = Angel::cross(temp_vec_right, temp_vec_up);
                    TPointNormals[6*j] += temp_normal_2;
                    temp_count ++;
                }
                if(CurrentNode->Down&&CurrentNode->Right){
                    temp_vec_down = CurrentNode->Down->Next->Position - CurrentNode->Position;
                    temp_vec_right = CurrentNode->Right->Next->Position - CurrentNode->Position;
                    temp_normal_3 = Angel::cross(temp_vec_down, temp_vec_right);
                    TPointNormals[6*j] += temp_normal_3;
                    temp_count ++;
                }
                if (CurrentNode->Left&&CurrentNode->Down){
                    temp_vec_down = CurrentNode->Down->Next->Position - CurrentNode->Position;
                    temp_vec_left = CurrentNode->Left->Previous->Position - CurrentNode->Position;
                    temp_normal_4 = Angel::cross(temp_vec_left, temp_vec_down);
                    TPointNormals[6*j] += temp_normal_4;
                    temp_count ++;
                }
                
                TPointNormals[6*j] = TPointNormals[6*j]/temp_count;//average over the neighbor faces' normals
                
                temp_count = 0;
                TPointNormals[6*j+1] = 0;
                if(RightNode->Up&&RightNode->Left){
                    temp_vec_up = RightNode->Up->Previous->Position - RightNode->Position;
                    temp_vec_left = RightNode->Left->Previous->Position - RightNode->Position;
                    temp_normal_1 = Angel::cross(temp_vec_up, temp_vec_left);
                    TPointNormals[6*j+1] += temp_normal_1;
                    temp_count ++;
                }
                if(RightNode->Up&&RightNode->Right){
                    temp_vec_up = RightNode->Up->Previous->Position - RightNode->Position;
                    temp_vec_right = RightNode->Right->Next->Position - RightNode->Position;
                    temp_normal_2 = Angel::cross(temp_vec_right, temp_vec_up);
                    TPointNormals[6*j+1] += temp_normal_2;
                    temp_count ++;
                }
                if(RightNode->Down&&RightNode->Right){
                    temp_vec_down = RightNode->Down->Next->Position - RightNode->Position;
                    temp_vec_right = RightNode->Right->Next->Position - RightNode->Position;
                    temp_normal_3 = Angel::cross(temp_vec_down, temp_vec_right);
                    TPointNormals[6*j+1] += temp_normal_3;
                    temp_count ++;
                }
                if (RightNode->Left&&RightNode->Down){
                    temp_vec_down = RightNode->Down->Next->Position - RightNode->Position;
                    temp_vec_left = RightNode->Left->Previous->Position - RightNode->Position;
                    temp_normal_4 = Angel::cross(temp_vec_left, temp_vec_down);
                    TPointNormals[6*j+1] += temp_normal_4;
                    temp_count ++;
                }
                
                TPointNormals[6*j+1] = TPointNormals[6*j+1]/temp_count;//average over the neighbor faces' normals
                TPointNormals[6*j+4] = TPointNormals[6*j+1];
                
                temp_count = 0;
                TPointNormals[6*j+2] = 0;
                if(DownNode->Up&&DownNode->Left){
                    temp_vec_up = DownNode->Up->Previous->Position - DownNode->Position;
                    temp_vec_left = DownNode->Left->Previous->Position - DownNode->Position;
                    temp_normal_1 = Angel::cross(temp_vec_up, temp_vec_left);
                    TPointNormals[6*j+2] += temp_normal_1;
                    temp_count ++;
                }
                if(DownNode->Up&&DownNode->Right){
                    temp_vec_up = DownNode->Up->Previous->Position - DownNode->Position;
                    temp_vec_right = DownNode->Right->Next->Position - DownNode->Position;
                    temp_normal_2 = Angel::cross(temp_vec_right, temp_vec_up);
                    TPointNormals[6*j+2] += temp_normal_2;
                    temp_count ++;
                }
                if(DownNode->Down&&DownNode->Right){
                    temp_vec_down = DownNode->Down->Next->Position - DownNode->Position;
                    temp_vec_right = DownNode->Right->Next->Position - DownNode->Position;
                    temp_normal_3 = Angel::cross(temp_vec_down, temp_vec_right);
                    TPointNormals[6*j+2] += temp_normal_3;
                    temp_count ++;
                }
                if (DownNode->Left&&DownNode->Down){
                    temp_vec_down = DownNode->Down->Next->Position - DownNode->Position;
                    temp_vec_left = DownNode->Left->Previous->Position - DownNode->Position;
                    temp_normal_4 = Angel::cross(temp_vec_left, temp_vec_down);
                    TPointNormals[6*j+2] += temp_normal_4;
                    temp_count ++;
                }
                
                TPointNormals[6*j+2] = TPointNormals[6*j+2]/temp_count;//average over the neighbor faces' normals
                TPointNormals[6*j+5] = TPointNormals[6*j+2];
                
                temp_count = 0;
                TPointNormals[6*j+3] = 0;
                Node * RDNode = RightNode->Down->Next;
                if(RDNode->Up&&RDNode->Left){
                    temp_vec_up = RDNode->Up->Previous->Position - RDNode->Position;
                    temp_vec_left = RDNode->Left->Previous->Position - RDNode->Position;
                    temp_normal_1 = Angel::cross(temp_vec_up, temp_vec_left);
                    TPointNormals[6*j+3] += temp_normal_1;
                    temp_count ++;
                }
                if(RDNode->Up&&RDNode->Right){
                    temp_vec_up = RDNode->Up->Previous->Position - RDNode->Position;
                    temp_vec_right = RDNode->Right->Next->Position - RDNode->Position;
                    temp_normal_2 = Angel::cross(temp_vec_right, temp_vec_up);
                    TPointNormals[6*j+3] += temp_normal_2;
                    temp_count ++;
                }
                if(RDNode->Down&&RDNode->Right){
                    temp_vec_down = RDNode->Down->Next->Position - RDNode->Position;
                    temp_vec_right = RDNode->Right->Next->Position - RDNode->Position;
                    temp_normal_3 = Angel::cross(temp_vec_down, temp_vec_right);
                    TPointNormals[6*j+3] += temp_normal_3;
                    temp_count ++;
                }
                if (RDNode->Left&&RDNode->Down){
                    temp_vec_down = RDNode->Down->Next->Position - RDNode->Position;
                    temp_vec_left = RDNode->Left->Previous->Position - RDNode->Position;
                    temp_normal_4 = Angel::cross(temp_vec_left, temp_vec_down);
                    TPointNormals[6*j+3] += temp_normal_4;
                    temp_count ++;
                }
                
                TPointNormals[6*j+3] = TPointNormals[6*j+3]/temp_count;
                               
                j++;
                
                //For test purporse
                //std::cout<<j<<'\t'<<TPointNormals[6*j].x<<","<<TPointNormals[6*j].y<<","<<TPointNormals[6*j].z<<","<<std::endl;

            }
            
            CurrentNode = RightNode;
            RightConnection = CurrentNode->Right;
        }//draw a single line
        
        //deal the downconnection of the last node in current row
        
        DownConnection = CurrentNode->Down;
        
        if(DownConnection){//draw the down connection if exist
            DownNode = DownConnection->Next;
            
            LinePoints[2*i] = Angel::vec4(CurrentNode->Position.x,CurrentNode->Position.y,CurrentNode->Position.z,1); 
            LinePoints[2*i+1] = Angel::vec4(DownNode->Position.x,DownNode->Position.y,DownNode->Position.z,1); 
            
            if(CurrentNode->Up)
                temp_vec_up = CurrentNode->Up->Previous->Position - CurrentNode->Position;
            if(CurrentNode->Down)
                temp_vec_down = CurrentNode->Down->Next->Position - CurrentNode->Position;
            if(CurrentNode->Left)
                temp_vec_left = CurrentNode->Left->Previous->Position - CurrentNode->Position;
            if(CurrentNode->Right)
                temp_vec_right = CurrentNode->Right->Next->Position - CurrentNode->Position;
            
            temp_count = 0;
            LPointNormals[2*i]  = vec3(0,0,0);
            if(CurrentNode->Up&&CurrentNode->Left){
                temp_normal_1 = Angel::cross(temp_vec_up, temp_vec_left);
                LPointNormals[2*i] += temp_normal_1;
                temp_count ++;
            }
            if(CurrentNode->Up&&CurrentNode->Right){
                temp_normal_2 = Angel::cross(temp_vec_right, temp_vec_up);
                LPointNormals[2*i] += temp_normal_2;
                temp_count ++;
            }
            if(CurrentNode->Down&&CurrentNode->Right){
                temp_normal_3 = Angel::cross(temp_vec_down, temp_vec_right);
                LPointNormals[2*i] += temp_normal_3;
                temp_count ++;
            }
            if (CurrentNode->Left&&CurrentNode->Down){
                temp_normal_4 = Angel::cross(temp_vec_left, temp_vec_down);
                LPointNormals[2*i] += temp_normal_4;
                temp_count ++;
            }
            
            
            LPointNormals[2*i] = LPointNormals[2*i]/temp_count;//average over the neighbor faces' normals
            
            if(DownNode->Up)
                temp_vec_up = DownNode->Up->Previous->Position - DownNode->Position;
            if(DownNode->Down)
                temp_vec_down = DownNode->Down->Next->Position - DownNode->Position;
            if(DownNode->Left)
                temp_vec_left = DownNode->Left->Previous->Position - DownNode->Position;
            if(DownNode->Right)
                temp_vec_right = DownNode->Right->Next->Position - DownNode->Position;
            
            temp_count = 0;
            LPointNormals[2*i+1]  = vec3(0,0,0);
            if(DownNode->Up&&DownNode->Left){
                temp_normal_1 = Angel::cross(temp_vec_up, temp_vec_left);
                LPointNormals[2*i+1] += temp_normal_1;
                temp_count ++;
            }
            if(DownNode->Up&&DownNode->Right){
                temp_normal_2 = Angel::cross(temp_vec_right, temp_vec_up);
                LPointNormals[2*i+1] += temp_normal_2;
                temp_count ++;
            }
            if(DownNode->Down&&DownNode->Right){
                temp_normal_3 = Angel::cross(temp_vec_down, temp_vec_right);
                LPointNormals[2*i+1] += temp_normal_3;
                temp_count ++;
            }
            if (DownNode->Left&&DownNode->Down){
                temp_normal_4 = Angel::cross(temp_vec_left, temp_vec_down);
                LPointNormals[2*i+1] += temp_normal_4;
                temp_count ++;
            }
            
            LPointNormals[2*i+1] = LPointNormals[2*i+1]/temp_count;
                       
            stretch = fabs(DownConnection->Cdeform)/DownConnection->length;
            stretch = stretch/3.0;
            stretch =  sqrt(stretch);
            
            LineColors[2*i] = Angel::vec4(stretch,1-stretch,0,1);//green when no stretch, red when very stressed
            LineColors[2*i+1] = Angel::vec4(stretch,1-stretch,0,1);//green when no stretch, red when very stressed
            
            i++;
            
            DownConnection->Pdeform = DownConnection->Cdeform;
            Connectionvec = CurrentNode->Position - DownNode->Position;
            DownConnection->Cdeform = Angel::length(Connectionvec) - DownConnection->length;
            
        }
        
        if(line_head->Down){
            line_head = line_head->Down->Next;
        }
        else
            break;
    }
    
}

void Cloth::Draw(int type, double dt, mat4 cMw, Light light, double cursor_x, double cursor_y, float w, float h){
    //dt is the timestep, x and y are the cursor's input, camera defines the view atmrix and projection matrix
    //update the position and color of vertexesfor drawing
    
    glUseProgram(shader);
    glBindVertexArrayAPPLE(vertexArrayObject);//use as current vertex array
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);

    //Get new position of the cube and update the model view Angel::Angel::Angel::matrix
    //Angel::mat4 proj;
    
    GLint world2camera = glGetUniformLocation(shader, "cMw"); 
    uProjection1 = glGetUniformLocation(shader, "proj");
    
    
    GLint kAmbient = glGetUniformLocation(shader,"kAmbient");
    GLint kDiffuse = glGetUniformLocation(shader,"kDiffuse");
    GLint kSpecular = glGetUniformLocation(shader,"kSpecular");
    GLint shininess = glGetUniformLocation(shader,"shininess");
    GLint camera_position = glGetUniformLocation(shader, "cameraPosition");
    GLint light_position = glGetUniformLocation(shader, "lightPosition");
    
    //camera.position.z += 25;
    
    //generate the Angel::Angel::Angel::matrixes
    mat4 projection = Perspective(50.0f, (float)w/(float)h, 1.0f, 1000.0f);
    
    
    Angel::vec4 Ambient(0.2,0.0,0.0,1.0);
    Angel::vec4 Diffuse(0.5,0.0,0.0,1.0);
    Angel::vec4 Specular(0.3,0.3,0.3,1.0);
    glUniformMatrix4fv( world2camera, 1, GL_TRUE, cMw );
    glUniformMatrix4fv( uProjection1, 1, GL_TRUE, projection );
    
    glUniform4fv(kAmbient, 1, Ambient);
    glUniform4fv(kDiffuse, 1, Diffuse); 
    glUniform4fv(kSpecular, 1, Specular);
    //glUniform4fv(camera_position, 1, camera.position);
    glUniform4fv(light_position, 1, light.position);
    glUniform1f(shininess, 10);
    
    
    
    if(dt){//avoid division of zeros, no time no update
//        //based on the z, determine the zoom factor for x and y;
//        float zoom_factor = (camera.position.z - Maniposition.z)/(camera.position.z - camera.znear);
//        
//        Maniposition.x = zoom_factor*cursor_x*(camera.position.z - camera.znear)*tan(Angel::DegreesToRadians*camera.fovy/2);
//        Maniposition.y = zoom_factor*cursor_y*(camera.position.z - camera.znear)*tan(Angel::DegreesToRadians*camera.fovy*camera.aspect/2);
//        
        UpdateAll(dt, Maniposition.x, Maniposition.y, Maniposition.z, light);
    }
    
    GLuint position;
    GLuint color;
    GLuint normal;
    GLuint p2c;
    GLuint p2l;
    
    switch (type) {
        case DRAW_LINE:
            glPointSize(2.0f);
            glLineWidth(1.0f);
            
            //send the updated data to buffer
            glBufferData(GL_ARRAY_BUFFER, sizeof(LinePoints[0])*NLinePoints + sizeof(LineColors[0])*NLinePoints + sizeof(LPointNormals[0])*NLinePoints, NULL, GL_STATIC_DRAW);//send data to current buffer
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(LinePoints[0])*NLinePoints, LinePoints);
            glBufferSubData(GL_ARRAY_BUFFER, sizeof(LinePoints[0])*NLinePoints, sizeof(LineColors[0])*NLinePoints, LineColors);
            glBufferSubData(GL_ARRAY_BUFFER, sizeof(LineColors[0])*NLinePoints+sizeof(LinePoints[0])*NLinePoints,sizeof(LPointNormals[0])*NLinePoints, LPointNormals);

            position = glGetAttribLocation( shader, "vPosition" );
            glEnableVertexAttribArray( position );
            glVertexAttribPointer(position, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
            
            color = glGetAttribLocation(shader, "vColor");
            glEnableVertexAttribArray(color);
            glVertexAttribPointer(color, 4, GL_FLOAT, GL_FALSE,0, BUFFER_OFFSET(sizeof(LinePoints[0])*NLinePoints));
            
            normal = glGetAttribLocation(shader, "vNormal");
            glEnableVertexAttribArray(normal);
            glVertexAttribPointer(normal, 3, GL_FLOAT, GL_FALSE,0, BUFFER_OFFSET((sizeof(LineColors[0])+sizeof(LinePoints[0]))*NLinePoints));
            
            glUniform1i(glGetUniformLocation(shader, "renderType"), 1);
            
            //draw the vetexes
            glDrawArrays(GL_LINES, 0, NLinePoints);
            glDrawArrays(GL_POINTS, 0, NLinePoints);
            
            break;
            
        case DRAW_TRIANGLE:
            //send the updated data to buffer
            glBufferData(GL_ARRAY_BUFFER, (sizeof(TrianglePoints[0]) + sizeof(TriangleColors[0]) + sizeof(TPointNormals[0]))*NTrianglePoints, NULL, GL_STATIC_DRAW);//send data to current buffer
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(TrianglePoints[0])*NTrianglePoints, TrianglePoints);
            glBufferSubData(GL_ARRAY_BUFFER, sizeof(TrianglePoints[0])*NTrianglePoints, sizeof(TriangleColors[0])*NTrianglePoints, TriangleColors);
            glBufferSubData(GL_ARRAY_BUFFER, sizeof(TriangleColors[0])*NTrianglePoints+sizeof(TrianglePoints[0])*NTrianglePoints,sizeof(TPointNormals[0])*NTrianglePoints, TPointNormals);
            
            position = glGetAttribLocation( shader, "vPosition" );
            glEnableVertexAttribArray( position );
            glVertexAttribPointer(position, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0)); 
            
            color = glGetAttribLocation(shader, "vColor");
            glEnableVertexAttribArray(color);
            glVertexAttribPointer(color, 4, GL_FLOAT, GL_FALSE,0, BUFFER_OFFSET(sizeof(TrianglePoints[0])*NTrianglePoints));
            
            normal = glGetAttribLocation(shader, "vNormal");
            glEnableVertexAttribArray(normal);
            glVertexAttribPointer(normal, 3, GL_FLOAT, GL_FALSE,0, BUFFER_OFFSET((sizeof(TriangleColors[0])+sizeof(TrianglePoints[0]))*NTrianglePoints));
            
            glUniform1i(glGetUniformLocation(shader, "renderType"), 2);
            
            //draw the vetexes
            glDrawArrays(GL_TRIANGLES, 0, NTrianglePoints);
            
            break;
    }
    
        
    
    //remember the current camera for selction
    //Eye = camera;//shadow copy, but okay since Camera Class has no pointers
    
}

void Cloth::Select(double x, double y, double range, Camera camera){
    //Based on the mouse clicked position, shot ray into the 3D scene,
    //for each node, search for the nearest node within range to the shoted ray
    
    //equation of the ray: (0,0,Camera.Position) + t*(tan(Camera.fovy/2*x),tan(Camera.fovy*Camera.aspect/2*y),1);
    Angel::vec4 CameraPosition(camera.position);
    Angel::vec4 Ray(tan(Angel::DegreesToRadians*x*camera.fovy/2),tan(Angel::DegreesToRadians*y*camera.fovy*camera.aspect/2),-1.0,0.0);//vector
    Ray = Angel::normalize(Ray);
    Angel::vec4 Camera2Node;
    double temp;
    //Search for the node closest to the ray within the range
    Node* CurrentNode;
    Node* NextNode;
    SpringDamper* Connection;
    Angel::vec2 Connectionvec;
    double mindist = 1000;
    double tempdist = 1000;
    
    
    NodeQueue node_queue;
    
    node_queue.push(Head);
    int updateFlag = Head->Updated*(-1);
    Head->Updated = updateFlag;
    
    //calculate position based on memory position and velocity
    while (!node_queue.empty()) {
        CurrentNode = node_queue.front();
        node_queue.pop();
        
        Camera2Node = Angel::vec4(CurrentNode->Position) - CameraPosition;
        temp = dot(vec3(Camera2Node.x, Camera2Node.y, Camera2Node.z), vec3(Ray.x, Ray.y, Ray.z));
        tempdist = sqrt(dot(vec3(Camera2Node.x, Camera2Node.y, Camera2Node.z), vec3(Camera2Node.x, Camera2Node.y, Camera2Node.z)) - temp*temp);
        
        if(tempdist < mindist){
            MadNode = CurrentNode;
            mindist = tempdist;
        }
        
        Connection = CurrentNode->Left;
        if (Connection) {
            NextNode = Connection->Previous;
            if(NextNode->Updated != updateFlag){
                node_queue.push(NextNode);
                NextNode->Updated = updateFlag;
            }
        }
        
        Connection = CurrentNode->Right;
        if (Connection) {
            NextNode = Connection->Next;
            if(NextNode->Updated != updateFlag){
                node_queue.push(NextNode);
                NextNode->Updated = updateFlag;
            }
        }
        
        Connection = CurrentNode->Up;
        if (Connection) {
            NextNode = Connection->Previous;
            if(NextNode->Updated != updateFlag){
                node_queue.push(NextNode);
                NextNode->Updated = updateFlag;
            }
        }
        
        Connection = CurrentNode->Down;
        if (Connection) {
            NextNode = Connection->Next;
            if(NextNode->Updated != updateFlag){
                node_queue.push(NextNode);
                NextNode->Updated = updateFlag;
            }
        }
        
        CurrentNode->Updated = updateFlag;
        
    }//end while
    
    if(mindist < range){
        Manipulated = 1;
        std::cout<<"Node Manipulated!"<<std::endl;
        //determine the position of the manipulator here
        Maniposition = vec3(MadNode->Position);
    }
    else{
        Deselect();
    } 
    
    //printf("Left Button Clicked: %f, %f\n",x,y);
}

void Cloth::NetHit(vec3 ballPos, float ballRadius) {
    
    Node* CurrentNode;
    NodeQueue node_queue;
    float sqr;
    float sqr2;
    
    Node* NextNode;
    SpringDamper* Connection;
    Angel::vec2 Connectionvec;
    
    int updateFlag = Head->Updated*(-1);
    Head->Updated = updateFlag;
    
    node_queue.push(Head);
    while (!node_queue.empty()) {
        CurrentNode = node_queue.front();
        node_queue.pop();
        

        if (CurrentNode->Fixed != 2) {
            CurrentNode->Fixed = 0;
        }
        
        sqr = sqrt(pow(CurrentNode->Position.x-ballPos.x, 2)+pow(CurrentNode->Position.y-ballPos.y, 2)+
                       pow(CurrentNode->Position.z-ballPos.z, 2));
        
        //sqr2 = sqrt(pow(CurrentNode->oPosition.x-ballPos.x, 2)+pow(CurrentNode->oPosition.y-ballPos.y, 2)+
        //            pow(CurrentNode->oPosition.z-ballPos.z, 2));//sqrt(pow(CurrentNode->oPosition.x-ballPos.x, 2) + pow(CurrentNode->oPosition.z-ballPos.z, 2));
        
       // printf("%f, %f, %f", CurrentNode->Position.x, CurrentNode->Position.y, CurrentNode->Position.z);
        
        if((sqr < ballRadius) && CurrentNode->Fixed != 2)
        {
            Manipulated = 0;
            vec3 normalized = normalize(CurrentNode->Position - ballPos);
//            if ((normalized.y < -0.3)   && (sqr2 <0.1)) {
//                            printf("%f, %f  %f\n", CurrentNode->Position.x, CurrentNode->Position.y, CurrentNode->Position.z);
//                CurrentNode->Position = vec3(((ballRadius/sqr2)*((ballRadius+(CurrentNode->oPosition - ballPos).y)/ballRadius)* (CurrentNode->oPosition - ballPos).x) + ballPos.x, CurrentNode->oPosition.y, ((ballRadius/sqr2) *((ballRadius+(CurrentNode->oPosition - ballPos).y)/ballRadius) * (CurrentNode->oPosition - ballPos).z) +ballPos.z);
//                printf("CurrXZ: %f, %f  %f\n", CurrentNode->Position.x, CurrentNode->Position.y, CurrentNode->Position.z);
//            }
//            else {
                CurrentNode->Position = (ballRadius/sqr * (CurrentNode->Position - ballPos))+ballPos;
                CurrentNode->Fixed = 1;
//            }
        }
        
        Connection = CurrentNode->Left;
        if (Connection) {
            NextNode = Connection->Previous;
            if(NextNode->Updated != updateFlag){
                node_queue.push(NextNode);
                NextNode->Updated = updateFlag;
            }
        }
        
        Connection = CurrentNode->Right;
        if (Connection) {
            NextNode = Connection->Next;
            if(NextNode->Updated != updateFlag){
                node_queue.push(NextNode);
                NextNode->Updated = updateFlag;
            }
        }
        
        Connection = CurrentNode->Up;
        if (Connection) {
            NextNode = Connection->Previous;
            if(NextNode->Updated != updateFlag){
                node_queue.push(NextNode);
                NextNode->Updated = updateFlag;
            }
        }
        
        Connection = CurrentNode->Down;
        if (Connection) {
            NextNode = Connection->Next;
            if(NextNode->Updated != updateFlag){
                node_queue.push(NextNode);
                NextNode->Updated = updateFlag;
            }
        }
        
        CurrentNode->Updated = updateFlag;
    }
    
}



void Cloth::MouseLeft(float x, float y, Camera camera){

    //for testing the click return value:
    
    printf("Cursor Clicked: x: %f, y: %f\n",x,y);
    
    //Search for selected point
    if(this->Manipulated){//if already manipulated, anchor when left click again
        this->Anchor();
        this->Deselect();
    }
    else{
        this->Select(x,y, 0.5, camera);
    }

}

void Cloth::MouseRight(float x, float y, Camera camera){
    if(this->Manipulated){
        this->Deanchor();
        this->Deselect();
    }
}

