//
//  Camera.h
//  Cloth2D
//
//  Created by 静一 方 on 11/22/11.
//  Copyright 2011 UCLA. All rights reserved.
//

#ifndef Cloth2D_Camera_h
#define Cloth2D_Camera_h

#include "Angel.h"
//this class defines the camera's position, lookat point, rotation, etc
//it also defines the kind of projection the camera is using: orthographics or perspective
//currently, only perspective 
class Camera{
public:
    //define camera's aligment
    Angel::vec4 position;
    Angel::vec4 lookat;
    Angel::vec4 up;
    //define the perspective devision
    float fovy;
    float aspect;
    float znear;
    float zfar;
    
    Camera(){
        position = Angel::vec4(0,0,1,1);
        lookat = Angel::vec4(0,0,0,1);
        up = Angel::vec4(0,1,0,0);
        fovy = 90;
        aspect = 1;
        znear = 1;
        zfar = -1;
    };//default constructor
};


#endif
