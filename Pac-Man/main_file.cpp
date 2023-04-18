/*
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#define GLM_FORCE_RADIANS
using namespace std;
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <stdlib.h>
#include <stdio.h>
#include "constants.h"
#include "lodepng.h"
#include "shaderprogram.h"
#include "myCube.h"
#include "myTeapot.h"
#include <fstream>
#include <vector>
#include <string>
#include<iostream>
#include "objloader.h"
#include "objloader.cpp"
#include <cmath>
#include <cstdlib>
using namespace glm;
float speed_x=0; //angular speed in radians
float speed_y=0; //angular speed in radians
float aspectRatio=1;
ShaderProgram *sp; //Pointer to the shader program
int points = 0; //points counter
int inner_walls = 0; //check if in inner walls
int ghost = 0; //check if contact with ghost


int licznik, licznik2, licznik3, licznik4, licznik5, licznik6, licznik7, licznik8, licznik9; //vertex counters
float k = 6.5, o = 7.5; //coordinates for points


GLuint tex0, tex1, tex2, tex4, tex6, tex8, tex10, tex12; //textures

//vectors used for loading data from obj files
std::vector< float > vertices;
std::vector< float > uvs;
std::vector< float > normals; 

std::vector< float > vertices2;
std::vector< float > uvs2;
std::vector< float > normals2; 

std::vector< float > vertices3;
std::vector< float > uvs3;
std::vector< float > normals3; 

std::vector< float > vertices4;
std::vector< float > uvs4;
std::vector< float > normals4; 

std::vector< float > vertices5;
std::vector< float > uvs5;
std::vector< float > normals5; 

std::vector< float > vertices6;
std::vector< float > uvs6;
std::vector< float > normals6; 

std::vector< float > vertices7;
std::vector< float > uvs7;
std::vector< float > normals7; 

std::vector< float > vertices8;
std::vector< float > uvs8;
std::vector< float > normals8; 

std::vector< float > vertices9;
std::vector< float > uvs9;
std::vector< float > normals9; 

//used for pcaman movement
quat O = quat(vec3(0, 0, 0));
vec3 pos = vec3(0, 0, 0);
//used for ghost movement
quat Og1 = quat(vec3(0, 0, 0));
vec3 posg1 = vec3(0, 0, 0);
quat Og2 = quat(vec3(0, 0, 0));
vec3 posg2 = vec3(0, 0, 0);
quat Og3 = quat(vec3(0, 0, 0));
vec3 posg3 = vec3(0, 0, 0);
quat Og4 = quat(vec3(0, 0, 0));
vec3 posg4 = vec3(0, 0, 0);
//used to store small points coordinates
std::vector<float> small_points_x1;
std::vector<float> small_points_z1;
std::vector<float> small_points_x2;
std::vector<float> small_points_z2;
std::vector<float> small_points_x3;
std::vector<float> small_points_z3;
std::vector<float> small_points_x4;
std::vector<float> small_points_z4;


int rotated;//tells if there was a turn/rotation
int start=0;//tells if the user decided to start the game
int collision;//tells if there was a collision
int straight;//tells if pacman was going straight
int dwie_sciany;//used to enable pacman movement when close to more than one wall

float pos_x, pos_z, temp_x, temp_z, pos_g1z, pos_g2z, pos_g3z, pos_g4z;//store rounded position data of ghosts and pacman 
float kamera, dystans = -20;//camera
//Error processing callback procedure
void error_callback(int error, const char* description) {
	fputs(description, stderr);
}
float roundoff(float value, unsigned char prec)
{
	float pow_10 = pow(10.0f, (float)prec);
	return round(value * pow_10) / pow_10;
}

void keyCallback(GLFWwindow* window,int key,int scancode,int action,int mods) {
	mat4 rotation;
    if (action==GLFW_PRESS) {
		if (key == GLFW_KEY_LEFT) {
			speed_y = 1;
			rotated++;
			collision++;
		}
		if (key == GLFW_KEY_RIGHT) {
			speed_y = 2;
			rotated++;
			collision++;
		}
		if (key == GLFW_KEY_DOWN) {
			speed_y = 3;
			rotated++;
			collision++;
		
		}
		if (key == GLFW_KEY_ENTER) {
			
			start++;
			collision++;
		}
		if (key == GLFW_KEY_W) {

			kamera=0.5;
		}
		if (key == GLFW_KEY_S) {

			kamera = -0.5;
		}
    }
    if (action==GLFW_RELEASE) {
		if (key == GLFW_KEY_LEFT) {
			speed_y = 0;
			rotated = 0;
			
		}
        if (key==GLFW_KEY_RIGHT) {
			speed_y = 0;
			rotated = 0;
			
		}
        if (key==GLFW_KEY_DOWN) {
			speed_y = 0;
			rotated = 0;
			
		}
		if (key == GLFW_KEY_W) {

			kamera = 0;
		}
		if (key == GLFW_KEY_S) {

			kamera = 0;
		}
    }
}
bool walls_inside(float x, float z) {//function used to recognise inner walls (labyrinth)
	float walls_inside[15][4] = {
	{-5.5258, -4.7305, -3.6621, -0.92747},
	{-5.5258, -4.7305, 0.30726, 6.5433},
	{-4.7305, -1.929, 2.9478, 3.7757},
	{-3.135, 3.1761, -3.6621, -2.8451},
	{-3.135, 3.1761, 5.6773, 6.4533},
	{-0.37826, 0.41932, -6.3507, -2.8451},
	{-0.37826, 0.41932, 2.9478, 6.4533},
	{1.9797, 4.7768, 2.9478, 3.7757},
	{4.7768, 5.572, -3.6621, -0.92747},
	{4.7768, 5.572, 0.30726, 6.5433},
	{-2.3916, -1.9672, -1.2564, 1.408},
	{-2.3916, -0.66482, 0.98609, 1.408},
	{-2.3916, 2.44, -1.2564, -0.83449},
	{0.71314, 2.44, 0.98609, 1.408},
	{2.0155, 2.44, -1.2564, 1.408} };

	for (int i = 0; i < 15; i++) {
		{
			if (x >= walls_inside[i][0] - 0.2 && x <= walls_inside[i][1] + 0.2 && z >= walls_inside[i][2] + 2 && z <= walls_inside[i][3] + 2) {
		
				return true;
			}
			else {

				continue;
			}
		}

	}
	return false;
}

void windowResizeCallback(GLFWwindow* window,int width,int height) {
    if (height==0) return;
    aspectRatio=(float)width/(float)height;
    glViewport(0,0,width,height);
}

GLuint readTexture(const char* filename) {
	GLuint tex;
	glActiveTexture(GL_TEXTURE0);

	//Load into computer's memory
	std::vector<unsigned char> image;   //Allocate a vector for image storage
	unsigned width, height;   //Variables for image size
	//Read image
	unsigned error = lodepng::decode(image, width, height, filename);

	//Import into graphics card's memory
	glGenTextures(1, &tex); //Initialize one handle
	glBindTexture(GL_TEXTURE_2D, tex); //Activate the handle
	//Import image into graphics card's memory associated with the handle
	glTexImage2D(GL_TEXTURE_2D, 0, 4, width, height, 0,
		GL_RGBA, GL_UNSIGNED_BYTE, (unsigned char*)image.data());

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	return tex;
}

//Initialization code procedure
void initOpenGLProgram(GLFWwindow* window) {
	//************Place any code here that needs to be executed once, at the program start************

	glClearColor(0,0,0,1);
	glEnable(GL_DEPTH_TEST);
	glfwSetWindowSizeCallback(window,windowResizeCallback);
	glfwSetKeyCallback(window,keyCallback);
	sp=new ShaderProgram("v_simplest.glsl",NULL,"f_simplest.glsl");
	tex0 = readTexture("bricks2_normal.png");

	tex1 = readTexture("metal_spec.png");

	tex2 = readTexture("yellow.png");
	

	tex4 = readTexture("blue.png");
	

	tex6 = readTexture("red.png");
	

	tex8 = readTexture("green.png");


	tex10 = readTexture("purple.png");


	tex12 = readTexture("bricks2_normal.png");

	//reading data from obj files
	bool res1 = loadOBJ("map4.obj", vertices, uvs, normals, licznik);
	bool res2 = loadOBJ("pacmodel3.obj", vertices2, uvs2, normals2, licznik2);
	bool res3 = loadOBJ("ghost1.obj", vertices3, uvs3, normals3, licznik3);
	bool res4 = loadOBJ("ghost2.obj", vertices4, uvs4, normals4, licznik4);
	bool res5 = loadOBJ("ghost3.obj", vertices5, uvs5, normals5, licznik5);
	bool res6 = loadOBJ("ghost4.obj", vertices6, uvs6, normals6, licznik6);
	bool res7 = loadOBJ("walls_grube1.obj", vertices7, uvs7, normals7, licznik7);
	bool res9 = loadOBJ("smallpoint.obj", vertices9, uvs9, normals9, licznik9);
}

//Release resources allocated by the program
void freeOpenGLProgram(GLFWwindow* window) {
	//************Place any code here that needs to be executed once, after the main loop ends************
	delete sp;
}
void model(std::vector< float >vertices, std::vector< float >uvs, std::vector< float >normals, int licznik, GLuint tex0, GLuint tex1, glm::mat4 M) {//function to draw a model
	
	glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(M));

	glEnableVertexAttribArray(sp->a("vertex")); 
	glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, vertices.data()); 

	glEnableVertexAttribArray(sp->a("texCoord0")); 
	glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0, uvs.data()); 

	glEnableVertexAttribArray(sp->a("normal")); 
	glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, normals.data()); 

	glUniform1i(sp->u("textureMap0"), 0); 
	glActiveTexture(GL_TEXTURE0); 
	glBindTexture(GL_TEXTURE_2D, tex0);

	glUniform1i(sp->u("textureMap1"), 1); 
	glActiveTexture(GL_TEXTURE1); 
	glBindTexture(GL_TEXTURE_2D, tex1);


	glDrawArrays(GL_TRIANGLES, 0, licznik);



	glDisableVertexAttribArray(sp->a("vertex")); 
	glDisableVertexAttribArray(sp->a("texCoord0")); 
	glDisableVertexAttribArray(sp->a("normal")); 
}
void draw_all(glm::mat4 M){//function to draw all models (clearer drawscene function)
	glm::mat4 M1 = M;
	M1 = glm::translate(M1, glm::vec3(0.0f, 0.0f, -2.0f));
	M1 = translate(M1, pos);
	M1 = M1 * toMat4(O);
	glm::mat4 M2 = M;
	M2 = glm::translate(M2, glm::vec3(k, 0.0f, o));
	M2 = glm::rotate(M2, -PI / 2, glm::vec3(0.0f, 1.0f, 0.0f));
	M2 = translate(M2, posg1);
	M2 = M2 * toMat4(Og1);
	glm::mat4 M3 = M;
	M3 = glm::translate(M3, glm::vec3(-k, 0.0f, o));
	M3 = glm::rotate(M3, PI, glm::vec3(0.0f, 1.0f, 0.0f));
	M3 = translate(M3, posg2);
	M3 = M3 * toMat4(Og2);
	glm::mat4 M4 = M;
	M4 = glm::translate(M4, glm::vec3(k, 0.0f, -o + 0.5));
	M4 = translate(M4, posg3);
	M4 = M4 * toMat4(Og3);
	glm::mat4 M5 = M;
	M5 = glm::translate(M5, glm::vec3(-k, 0.0f, -o + 0.5));
	M5 = glm::rotate(M5, PI / 2, glm::vec3(0.0f, 1.0f, 0.0f));
	M5 = translate(M5, posg4);
	M5 = M5 * toMat4(Og4);
	glm::mat4 M6 = M;
	M6 = glm::translate(M6, glm::vec3(0.0f, 0.0f, 0.0f));

	model(vertices, uvs, normals, licznik, tex0, tex1, M);
	model(vertices2, uvs2, normals2, licznik2, tex2, tex1, M1);
	model(vertices3, uvs3, normals3, licznik3, tex4, tex1, M2);
	model(vertices4, uvs4, normals4, licznik4, tex6, tex1, M3);
	model(vertices5, uvs5, normals5, licznik5, tex8, tex1, M4);
	model(vertices6, uvs6, normals6, licznik6, tex10, tex1, M5);
	model(vertices7, uvs7, normals7, licznik7, tex12, tex1, M);
	



	glm::mat4 M9 = M;
	M9 = glm::translate(M9, glm::vec3(k, 0.0f, 0.0f));
	model(vertices9, uvs9, normals9, licznik9, tex2, tex1, M9);
	small_points_x1.push_back(k);
	small_points_z1.push_back(0);
	glm::mat4 M10 = M;
	M10 = glm::translate(M10, glm::vec3(-k, 0.0f, 0.0f));
	model(vertices9, uvs9, normals9, licznik9, tex2, tex1, M10);
	small_points_x2.push_back(-k);
	small_points_z2.push_back(0);
	glm::mat4 M11 = M;
	M11 = glm::translate(M11, glm::vec3(0.0f, 0.0f, o));
	model(vertices9, uvs9, normals9, licznik9, tex2, tex1, M11);
	small_points_x3.push_back(0);
	small_points_z3.push_back(o);
	glm::mat4 M12 = M;
	M12 = glm::translate(M12, glm::vec3(0.0f, 0.0f, -o + 0.5));
	model(vertices9, uvs9, normals9, licznik9, tex2, tex1, M12);
	small_points_x4.push_back(0);
	small_points_z4.push_back(-o + 0.5);

	glm::mat4 temp_M9 = M9;
	glm::mat4 temp_M10 = M10;
	glm::mat4 temp_M11 = M11;
	glm::mat4 temp_M12 = M12;

	for (float i = 1; i < 7; i++) {
		M9 = temp_M9;
		M9 = glm::translate(M9, glm::vec3(0.0f, 0.0f, i));
		model(vertices9, uvs9, normals9, licznik9, tex2, tex1, M9);
		small_points_x1.push_back(k);
		small_points_z1.push_back(i);
		M10 = temp_M10;
		M10 = glm::translate(M10, glm::vec3(0.0f, 0.0f, -i));
		model(vertices9, uvs9, normals9, licznik9, tex2, tex1, M10);
		small_points_x2.push_back(-k);
		small_points_z2.push_back(-i);
		M11 = temp_M11;
		M11 = glm::translate(M11, glm::vec3(i, 0.0f, 0.0f));
		model(vertices9, uvs9, normals9, licznik9, tex2, tex1, M11);
		small_points_x3.push_back(i);
		small_points_z3.push_back(o);
		M12 = temp_M12;
		M12 = glm::translate(M12, glm::vec3(-i, 0.0f, 0.0f));
		model(vertices9, uvs9, normals9, licznik9, tex2, tex1, M12);
		small_points_x4.push_back(-i);
		small_points_z4.push_back(-o + 0.5);
		M9 = temp_M9;
		M9 = glm::translate(M9, glm::vec3(0.0f, 0.0f, -i));
		model(vertices9, uvs9, normals9, licznik9, tex2, tex1, M9);
		small_points_x1.push_back(k);
		small_points_z1.push_back(-i);
		M10 = temp_M10;
		M10 = glm::translate(M10, glm::vec3(0.0f, 0.0f, i));
		model(vertices9, uvs9, normals9, licznik9, tex2, tex1, M10);
		small_points_x2.push_back(-k);
		small_points_z2.push_back(i);
		M11 = temp_M11;
		M11 = glm::translate(M11, glm::vec3(-i, 0.0f, 0.0f));
		model(vertices9, uvs9, normals9, licznik9, tex2, tex1, M11);
		small_points_x3.push_back(-i);
		small_points_z3.push_back(o);
		M12 = temp_M12;
		M12 = glm::translate(M12, glm::vec3(i, 0.0f, 0.0f));
		model(vertices9, uvs9, normals9, licznik9, tex2, tex1, M12);
		small_points_x4.push_back(i);
		small_points_z4.push_back(-o + 0.5);
	}
}
//Drawing procedure
void drawScene(GLFWwindow* window,float angle_x,float angle_y) {
	//************Place any code here that draws something inside the window******************l

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 V=glm::lookAt(
        glm::vec3(0.0f,dystans+50,dystans),
        glm::vec3(0.0f,0.0f,0.0f),
        glm::vec3(0.0f,1.0f,0.0f)); 
    glm::mat4 P=glm::perspective(50.0f*PI/180.0f, 1.0f, 1.0f, 50.0f); 

    sp->use();
    
    glUniformMatrix4fv(sp->u("P"),1,false,glm::value_ptr(P));
    glUniformMatrix4fv(sp->u("V"),1,false,glm::value_ptr(V));

    glm::mat4 M=glm::mat4(1.0f);
	
	
	draw_all(M);

	

    glfwSwapBuffers(window); 
}



int main(void)
{
	
	GLFWwindow* window; 

	glfwSetErrorCallback(error_callback);

	if (!glfwInit()) { 
		fprintf(stderr, "Can't initialize GLFW.\n");
		exit(EXIT_FAILURE);
	}

	window = glfwCreateWindow(700, 700, "Pac-Man", NULL, NULL); 

	if (!window) 
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window); 
	glfwSwapInterval(1); 

	GLenum err;
	if ((err=glewInit()) != GLEW_OK) { 
		fprintf(stderr, "Can't initialize GLEW: %s\n", glewGetErrorString(err));
		exit(EXIT_FAILURE);
	}

	initOpenGLProgram(window); 
	//rotation angles for pacman and ghosts
	float angle_x=0; 
	float angle_y=0; 
	float angle_yg1 = PI; 
	float angle_yg2 = PI; 
	float angle_yg3 = PI; 
	float angle_yg4 = PI; 
	glfwSetTime(0); 
	//Main application loop
	while (!glfwWindowShouldClose(window)) //As long as the window shouldnt be closed yet...
	{
		dystans += kamera;
		//whole procedure of disallowing pacman to go through walls
		if ((-6.5 < pos.x) && (pos.x < 6.5) && (-5.0 < pos.z) && (pos.z < 9.5)) {
			
			angle_x = speed_x * glfwGetTime();
			if (walls_inside(pos.x, pos.z)) {
				inner_walls = 1;
				goto label1;
			}
			if ((speed_y == 1) && (rotated == 1)) {
				angle_y = PI / 2;
				rotated++;
				collision = 0;
				O = glm::rotate(O, angle_y, glm::vec3(0, 1, 0));
			}
			else if ((speed_y == 2) && (rotated == 1)) {
				angle_y = -PI / 2;
				rotated++;
				collision = 0;
				O = glm::rotate(O, angle_y, glm::vec3(0, 1, 0));
			}
			else if ((speed_y == 3) && (rotated == 1)) {
				label3:
				angle_y = PI;
				rotated++;
				collision = 0;
				O = glm::rotate(O, angle_y, glm::vec3(0, 1, 0));
			}
			else if (rotated == 0) {
				angle_y = 0.0f;
				collision = 0;
				straight = 1;
			}
			O = glm::rotate(O, angle_x, glm::vec3(1, 0, 0));

			O = normalize(O);

			vec3 dir = toMat3(O) * vec3(0, 0, 1);
			if (start == 1) {
				pos += 2 * (float)glfwGetTime() * dir;
			}
			
		}
		else if (((-6.5 >= pos.x) || (pos.x >= 6.5) || (-5.0 >= pos.z) || (pos.z >= 9.5))&&ghost==0) {
			if (((-6.5 >= pos.x) && (-5.0 >= pos.z)) || ((-6.5 >= pos.x) && (pos.z >= 9.5)) || ((pos.x >= 6.5) && (-5.0 >= pos.z)) || ((pos.x >= 6.5) && (pos.z >= 9.5))) {
				dwie_sciany = 1;
			}
			else {
				dwie_sciany = 0;
			}
			if ((rotated == 0)&&(collision==0)) {
				speed_x = 0;
				speed_y = 0;
				dwie_sciany = 0;
				
			}
			else if ((rotated == 0) && (dwie_sciany == 1)) {
			label1:
				speed_x = 0;
				speed_y = 0;
				dwie_sciany = 1;
				collision = 1;
				straight = 1;
				if (inner_walls == 1) {
					goto label3;
				}
				
			}
			else {
					angle_x = speed_x * glfwGetTime();
					if ((speed_y == 1) && (rotated == 1) && (straight == 2)) {
						angle_y = PI / 2;
						rotated++;
						collision = 0;
						straight = 1;
						O = glm::rotate(O, angle_y, glm::vec3(0, 1, 0));
					}
					else if ((speed_y == 2) && (rotated == 1) && (straight == 2)) {
						angle_y = -PI / 2;
						rotated++;
						collision = 0;
						straight = 1;
						O = glm::rotate(O, angle_y, glm::vec3(0, 1, 0));
					}
					else if ((speed_y == 3) && (rotated == 1)) {
						angle_y = PI;
						rotated++;
						collision = 1;
						O = glm::rotate(O, angle_y, glm::vec3(0, 1, 0));
					}
					else if (rotated == 0) {
						angle_y = 0.0f;
						collision = 1;
						straight = 2;
					}
					if ((speed_y == 1) && (rotated == 1) && (straight==1)) {
						angle_y = PI / 2;
						rotated++;
						collision = 1;
						O = glm::rotate(O, angle_y, glm::vec3(0, 1, 0));
					}
					else if ((speed_y == 2) && (rotated == 1) && (straight == 1)) {
						angle_y = -PI / 2;
						rotated++;
						collision = 1;
						O = glm::rotate(O, angle_y, glm::vec3(0, 1, 0));
					}
					O = glm::rotate(O, angle_x, glm::vec3(1, 0, 0));

					O = normalize(O);

					vec3 dir = toMat3(O) * vec3(0, 0, 1);
					pos += 2*(float)glfwGetTime() * dir;
					

			
			}
			
			
		}
		//ghost movement
		if ((posg1.z > 13) || (posg1.z < 0)) {

			angle_yg1 = PI;
			Og1 = glm::rotate(Og1, angle_yg1, glm::vec3(0, 1, 0));
		}
		vec3 dirg1 = toMat3(Og1) * vec3(0, 0, 1);
		posg1 += (float)glfwGetTime() * dirg1;
		if ((posg2.z > 14.5) || (posg2.z < 0)) {

			angle_yg2 = PI;
			Og2 = glm::rotate(Og2, angle_yg2, glm::vec3(0, 1, 0));
		}
		vec3 dirg2 = toMat3(Og2) * vec3(0, 0, 1);
		posg2 += (float)glfwGetTime() * dirg2;
		if ((posg3.z > 14.5) || (posg3.z < 0)) {

			angle_yg3 = PI;
			Og3 = glm::rotate(Og3, angle_yg3, glm::vec3(0, 1, 0));
		}
		vec3 dirg3 = toMat3(Og3) * vec3(0, 0, 1);
		posg3 += (float)glfwGetTime() * dirg3;
		if ((posg4.z > 13) || (posg4.z < 0)) {

			angle_yg4 = PI;
			Og4 = glm::rotate(Og4, angle_yg4, glm::vec3(0, 1, 0));
		}
		vec3 dirg4 = toMat3(Og4) * vec3(0, 0, 1);
		posg4 += (float)glfwGetTime() * dirg4;
		//rounded values of position coordinates
		pos_x=roundoff(pos.x, 2);
		pos_z=roundoff(pos.z, 1);
		pos_x = roundoff(pos.x, 1);
		pos_z = roundoff(pos.z, 1);
		pos_g1z = roundoff(posg1.z, 1);
		pos_g2z = roundoff(posg2.z, 1);
		pos_g3z = roundoff(posg3.z, 1);
		pos_g4z = roundoff(posg4.z, 1);
		//checking pacman-ghost collision
		if ((abs(abs(pos_x - 6.5) - pos_g1z) <= 0.1) && ((pos_z <= 9.6) && (pos_z >= 8))) {
			goto label2;
			speed_x = 0;
			speed_y = 0;
		}
		if ((abs((pos_x + 6.5) - pos_g4z) <= 0.1) && ((pos_z <= -3.5) && (pos_z >= -5.1))) {
			goto label2;
			speed_x = 0;
			speed_y = 0;
		}
		if ((abs(abs(pos_z - 9.5) - pos_g2z) <= 0.1) && ((pos_x <= -5.5) && (pos_x >= -6.6))) {
			goto label2;
			speed_x = 0;
			speed_y = 0;
		}
		if ((abs(pos_z + 5 - pos_g3z) <= 0.1) && ((pos_x <= 6.6) && (pos_x >= 5.5))) {
			goto label2;
			speed_x = 0;
			speed_y = 0;
		}
		//checking collision with points and adding to the final score
		if (((std::find(small_points_z1.begin(), small_points_z1.end(), pos_z)) != small_points_z1.end())&& ((pos_x <= 6.6) && (pos_x >= 5.5))) {
			if (temp_z != pos_z) {
				points += 10;
				cout << "points: ";
				cout << points << endl;
				
			}
		}
		if (((std::find(small_points_z2.begin(), small_points_z2.end(), pos_z)) != small_points_z2.end()) && ((pos_x <= -5.5) && (pos_x >= -6.6))) {
			if (temp_z != pos_z) {
				points += 10;
				cout << "points: ";
				cout << points << endl;

			}
		}
		if (((std::find(small_points_x3.begin(), small_points_x3.end(), pos_x)) != small_points_x3.end()) && ((pos_z <= 9.6) && (pos_z >= 9))) {
			if (temp_x != pos_x) {
				points += 10;
				cout << "points: ";
				cout << points << endl;

			}
		}
		if (((std::find(small_points_x4.begin(), small_points_x4.end(), pos_x)) != small_points_x4.end()) && ((pos_z <= -4.5) && (pos_z >= -5.1))) {
			if (temp_x != pos_x) {
				points += 10;
				cout << "points: ";
				cout << points << endl;

			}
		}

		temp_x = 0;
		temp_z = 0;
		if (temp_x != pos_x) {
			temp_x = pos_x;
		}
		if (temp_z != pos_z) {
			temp_z = pos_z;
		}
        glfwSetTime(0); //Zero the timer
		drawScene(window,angle_x,angle_y); //Execute drawing procedure
		glfwPollEvents(); //Process callback procedures corresponding to the events that took place up to now
	}
	label2:
	freeOpenGLProgram(window);

	glfwDestroyWindow(window); //Delete OpenGL context and the window.
	glfwTerminate(); //Free GLFW resources
	exit(EXIT_SUCCESS);
}

