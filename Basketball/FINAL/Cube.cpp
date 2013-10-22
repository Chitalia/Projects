//
//  Cube.cpp
//  Cloth2D
//
//  Created by 静一 方 on 11/22/11.
//  Copyright 2011 UCLA. All rights reserved.
//

#include "Cube.h"

Cube::Cube(){
    InitData(Angel::vec3(0.0,0.0,0.0), Angel::vec3(0.0,0.0,0.0), 1.0);
}

Cube::Cube(Angel::vec3 center, Angel::vec3 rotate, float scale){
    InitData(center, rotate, scale);
}

void Cube::InitData(Angel::vec3 center, Angel::vec3 rotate, float scale){
    Center = center;
    Rotate = rotate;
    Size = scale;
}

void Cube::GenFace(int colorid, int a, int b, int c, int d )
{//generate two triangles for each face and assign colors to the vertices
    Colors[Index] = FaceColors[colorid]; Vertices[Index] = Points[a]; Index++;
    Colors[Index] = FaceColors[colorid]; Vertices[Index] = Points[b]; Index++;
    Colors[Index] = FaceColors[colorid]; Vertices[Index] = Points[c]; Index++;
    Colors[Index] = FaceColors[colorid]; Vertices[Index] = Points[a]; Index++;
    Colors[Index] = FaceColors[colorid]; Vertices[Index] = Points[c]; Index++;
    Colors[Index] = FaceColors[colorid]; Vertices[Index] = Points[d]; Index++;
}


void Cube::InitDraw(){
    // Initialize the data array on CPU

    Points[0] = Angel::vec4( -0.5, -0.5,  0.5, 1.0 );
    Points[1] = Angel::vec4( -0.5,  0.5,  0.5, 1.0 );
    Points[2] = Angel::vec4(  0.5,  0.5,  0.5, 1.0 );
    Points[3] = Angel::vec4(  0.5, -0.5,  0.5, 1.0 );
    Points[4] = Angel::vec4( -0.5, -0.5, -0.5, 1.0 );
    Points[5] = Angel::vec4( -0.5,  0.5, -0.5, 1.0 );
    Points[6] = Angel::vec4(  0.5,  0.5, -0.5, 1.0 );
    Points[7] = Angel::vec4(  0.5, -0.5, -0.5, 1.0 );
    
    FaceColors[0] = Angel::vec4( 0.0, 0.0, 0.0, 1.0 );
    FaceColors[1] = Angel::vec4( 1.0, 0.0, 0.0, 1.0 );
    FaceColors[2] = Angel::vec4( 1.0, 1.0, 0.0, 1.0 );
    FaceColors[3] = Angel::vec4( 0.0, 1.0, 0.0, 1.0 );
    FaceColors[4] = Angel::vec4( 0.0, 0.0, 1.0, 1.0 );
    FaceColors[5] = Angel::vec4( 1.0, 0.0, 1.0, 1.0 );
    
    Vertices = new Angel::vec4[36];
    Colors = new Angel::vec4[36]; 
    Index = 0;
    
    //generate the 6 faces with distinct colors
    GenFace( 5, 1, 0, 3, 2 );
    GenFace( 1, 2, 3, 7, 6 );
    GenFace( 2, 3, 0, 4, 7 );
    GenFace( 3, 6, 5, 1, 2 );
    GenFace( 4, 4, 5, 6, 7 );
    GenFace( 0, 5, 4, 0, 1 );
    
    OFFSET = sizeof(Vertices[0])*36;
    TOTAL = OFFSET + sizeof(Colors[0])*36;
    
    //Create the Vertex Array and Buffers, bind them
    glGenVertexArraysAPPLE(1, &vertexArrayObject);
    glBindVertexArrayAPPLE(vertexArrayObject);//use as current vertex array
    glGenBuffers(1, &vertexBufferObject);//generate buffer for current vertex array
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);//use as current buffer
    
    //Send data from CPU to GPU
    glBufferData(GL_ARRAY_BUFFER, TOTAL, NULL, GL_STATIC_DRAW);//send data to current buffer
    glBufferSubData(GL_ARRAY_BUFFER, 0, OFFSET, Vertices);
    glBufferSubData(GL_ARRAY_BUFFER, OFFSET, TOTAL - OFFSET, Colors);
    
    
    //load and compile shaders on GPU, use current shader program
    shader = InitShader( "vCube.glsl", "fCube.glsl" );
    glUseProgram(shader);
    
    
    // Link the Shader with the buffer data
    // initialize the vertex position attribute from the vertex shader
    GLuint position = glGetAttribLocation( shader, "vPosition" );
    glEnableVertexAttribArray( position );
    glVertexAttribPointer(position, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0)); 
    
    GLuint color = glGetAttribLocation(shader, "vColor");
    glEnableVertexAttribArray(color);
    glVertexAttribPointer(color, 4, GL_FLOAT, GL_FALSE,0, BUFFER_OFFSET(OFFSET));
    
}

void Cube::Draw(Camera camera){
    //Get new position of the cube and update the model view matrix
    Angel::mat4 wMo;//object to world matrix
    Angel::mat4 cMw;
    Angel::mat4 proj;

    glUseProgram(shader);
    glBindVertexArrayAPPLE(vertexArrayObject);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);//use as current buffer
    
    GLuint object2world = glGetUniformLocation(shader, "wMo");
    GLuint world2camera = glGetUniformLocation(shader, "cMw"); 
	GLuint projection = glGetUniformLocation(shader, "proj");
   
    //generate the Angel::matrixes
    wMo = Angel::Translate(Center);//can not understand why need Angel Scoop for this specific one
    proj = Angel::Perspective( camera.fovy, camera.aspect, camera.znear, camera.zfar );
    cMw = Angel::LookAt(camera.position,camera.lookat, camera.up );
    
    glUniformMatrix4fv( object2world , 1, GL_TRUE, wMo );
    glUniformMatrix4fv( world2camera, 1, GL_TRUE, cMw );
    glUniformMatrix4fv( projection, 1, GL_TRUE, proj );

    glDrawArrays( GL_TRIANGLES, 0, 36 );

}





