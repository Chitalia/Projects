//
//  Cube.h
//  Cloth2D
//
//  Created by 静一 方 on 11/22/11.
//  Copyright 2011 UCLA. All rights reserved.
//

#ifndef Cloth2D_Cube_h
#define Cloth2D_Cube_h

#include "Angel.h"
#include "Camera.h"

class Cube{
private:
    Angel::vec3 Center;//For generating translation Angel::matrix
    Angel::vec3 Rotate;//rotate in x y and z
    float Size;//For generating scaling Angel::matrix
    GLuint vertexArrayObject;                      
    GLuint vertexBufferObject;  
    GLuint shader;
    Angel::vec4 Points[8];//8 Vertices
    Angel::vec4 FaceColors[6];//6 Face Colors
    Angel::vec4* Vertices;
    Angel::vec4* Colors;
    int OFFSET;
    int TOTAL;
    int Index;
    
public:
    
    Cube();//Default constructor create a unit cube in center of screen
    Cube(Angel::vec3, Angel::vec3, float);//constructor creating a cube with size and center
    void GenFace(int colorid, int a, int b, int c, int d);
    void InitData(Angel::vec3,Angel::vec3,float);//Init the data
    void InitDraw();//Init the vertexs and color data on GPU, Init the shader program, link the shader program with the buffer data
    void Draw(Camera camera);//Update data on GPU's buffer and draw the vertexs
};



#endif
