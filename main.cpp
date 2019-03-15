#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>

#include <GL\glew.h>
#include <GLFW\glfw3.h>

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

#include "CommonValues.h"

#include "Window.h"
#include "Mesh.h"
#include "Shader.h"
#include "Camera.h"
#include "Texture.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Material.h"

#include "Model.h"

#include "Skybox.h"

const float toRadians = 3.14159265f / 180.0f;

GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
uniformSpecularIntensity = 0, uniformShininess = 0;

Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;
Shader directionalShadowShader;

Camera camera;

Texture brickTexture;
Texture dirtTexture;
Texture plainTexture;
Texture roadTexture;

Material shinyMaterial;
Material dullMaterial;

//Model xwing;
Model blackhawk;
Model car1;
Model cars;
Model house;
Model tree1;
Model tree2;
Model tree3;
Model tree4;
Model tree5;

Model rock1;
Model rock2;
Model Lamp1;
Model Lamp2;
Model Lamp3;
Model Lamp4;
Model Lamp5;
Model Lamp6;
Model Lamp7;
Model Lamp8;
Model Lamp9;
Model Lamp10;
Model Lamp11;
Model Lamp12;
Model Patch;
Model StopSign;
Model fence;
Model plants1;
Model plants2;
Model gate;
Model wood;
Model dog;
Model bird;
Model cat;
Model barrel;

DirectionalLight mainLight;
PointLight pointLights[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];


Skybox skybox;

unsigned int pointLightCount = 0;
unsigned int spotLightCount = 0;

GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
GLfloat blackhawkAngle = 0.0f;
GLfloat birdfly = 0.0f;


// Vertex Shader
static const char* vShader = "Shaders/shader.vert";

// Fragment Shader
static const char* fShader = "Shaders/shader.frag";

void calcAverageNormals(unsigned int * indices, unsigned int indiceCount, GLfloat * vertices, unsigned int verticeCount,
	unsigned int vLength, unsigned int normalOffset)
{
	for (size_t i = 0; i < indiceCount; i += 3)
	{
		unsigned int in0 = indices[i] * vLength;
		unsigned int in1 = indices[i + 1] * vLength;
		unsigned int in2 = indices[i + 2] * vLength;
		glm::vec3 v1(vertices[in1] - vertices[in0], vertices[in1 + 1] - vertices[in0 + 1], vertices[in1 + 2] - vertices[in0 + 2]);
		glm::vec3 v2(vertices[in2] - vertices[in0], vertices[in2 + 1] - vertices[in0 + 1], vertices[in2 + 2] - vertices[in0 + 2]);
		glm::vec3 normal = glm::cross(v1, v2);
		normal = glm::normalize(normal);

		in0 += normalOffset; in1 += normalOffset; in2 += normalOffset;
		vertices[in0] += normal.x; vertices[in0 + 1] += normal.y; vertices[in0 + 2] += normal.z;
		vertices[in1] += normal.x; vertices[in1 + 1] += normal.y; vertices[in1 + 2] += normal.z;
		vertices[in2] += normal.x; vertices[in2 + 1] += normal.y; vertices[in2 + 2] += normal.z;
	}

	for (size_t i = 0; i < verticeCount / vLength; i++)
	{
		unsigned int nOffset = i * vLength + normalOffset;
		glm::vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);
		vec = glm::normalize(vec);
		vertices[nOffset] = vec.x; vertices[nOffset + 1] = vec.y; vertices[nOffset + 2] = vec.z;
	}
}

void CreateObjects()
{
	//unsigned int indices[] = {
	//	0, 3, 1,
	//	1, 3, 2,
	//	2, 3, 0,
	//	0, 1, 2
	//};

	unsigned int indices[] = {
			0, 4, 1,
			4,1,2,
			3, 4, 2,
			3, 4, 0,
			0,3,1,
			3,2,1
		};

	GLfloat vertices[] = {
		//	x      y      z			u	  v			nx	  ny    nz
			-1.0f, -1.0f, -1.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			//0.0f, -1.0f, 1.0f,		0.5f, 0.0f,		0.0f, 0.0f, 0.0f,

			-1.0f, -1.0f, 1.0f,		0.5f, 0.0f,		0.0f, 0.0f, 0.0f,
			1.0f, -1.0f, 1.0f,		0.5f, 0.0f,		0.0f, 0.0f, 0.0f,

			1.0f, -1.0f, -1.0f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, 1.5f, 0.0f,		0.5f, 1.0f,		0.0f, 0.0f, 0.0f
	};

	unsigned int floorIndices[] = {
		0, 2, 1,
		1, 2, 3
	};

	unsigned int roadIndices[] = {
		0, 2, 1,
		1, 2, 3
	};

	GLfloat floorVertices[] = {
		-25.0f, 0.0f, -30.0f,	0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		25.0f, 0.0f, -30.0f,	30.0f, 0.0f,	0.0f, -1.0f, 0.0f,
		-25.0f, 0.0f, 30.0f,	0.0f, 30.0f,	0.0f, -1.0f, 0.0f,
		25.0f, 0.0f, 30.0f,		30.0f, 30.0f,	0.0f, -1.0f, 0.0f
	};

	GLfloat road[] = {
		-20.0f, 0.0f, -30.0f,	0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		-10.0f, 0.0f, -30.0f,	30.0f, 0.0f,	0.0f, -1.0f, 0.0f,
		-20.0f, 0.0f, 30.0f,	0.0f, 30.0f,	0.0f, -1.0f, 0.0f,
		-10.0f, 0.0f, 30.0f,		30.0f, 30.0f,	0.0f, -1.0f, 0.0f
	};

	//calcAverageNormals(indices, 12, vertices, 32, 8, 5);
	calcAverageNormals(indices, 15, vertices, 40,8,5);

	//Mesh *obj1 = new Mesh();
	//obj1->CreateMesh(vertices, indices, 32, 12);
	//meshList.push_back(obj1);
	Mesh *obj1 = new Mesh();
	obj1->CreateMesh(vertices, indices, 40, 18);
	meshList.push_back(obj1);

	Mesh *obj2 = new Mesh();
	obj2->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj2);

	Mesh *obj3 = new Mesh();
	obj3->CreateMesh(floorVertices, floorIndices, 32, 6);
	meshList.push_back(obj3);

	Mesh *obj4 = new Mesh();
	obj4->CreateMesh(road, roadIndices, 32, 6);
	meshList.push_back(obj4);
}
 
void CreateShaders()
{
	Shader *shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);

	directionalShadowShader = Shader();
	directionalShadowShader.CreateFromFiles("Shaders/directional_shadow_map.vert", "Shaders/directional_shadow_map.frag");
}


void RenderScene()
{
	glm::mat4 model;
	
	//model = glm::translate(model, glm::vec3(1.0f, 5.0f, -6.0f));
	//model = glm::scale(model, glm::vec3(8.5f, 7.5f, 8.5f));
	//model = glm::rotate(model, -180.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	//brickTexture.UseTexture();
	//shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
	//meshList[0]->RenderMesh();

	//model = glm::mat4();
	//model = glm::translate(model, glm::vec3(7.0f, 0.0f, -5.0f));
	////model = glm::scale(model, glm::vec3(0.4f, 0.4f, 1.0f));
	//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	//dirtTexture.UseTexture();
	//dullMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
	//meshList[1]->RenderMesh();

	model = glm::mat4();
	model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
	//model = glm::scale(model, glm::vec3(0.4f, 0.4f, 1.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	plainTexture.UseTexture();
	shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
	meshList[2]->RenderMesh();

	//Draw the road
	model = glm::mat4();
	model = glm::translate(model, glm::vec3(0.0f, -1.9f, 0.0f));
	//model = glm::scale(model, glm::vec3(0.4f, 0.4f, 1.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	roadTexture.UseTexture();
	shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
	meshList[3]->RenderMesh();

	//model = glm::mat4();
	//model = glm::translate(model, glm::vec3(-9.0f, 2.0f, 16.0f));
	//model = glm::scale(model, glm::vec3(0.01f, 0.01f, 0.01f));
	//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	//shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
	//xwing.RenderModel();

	blackhawkAngle += 0.1f;
	if (blackhawkAngle > 360.0f)
	{
		blackhawkAngle = 0.1f;
	}

	birdfly -= 0.1f;
	if (birdfly < - 40.0f)
	{
		birdfly = 0.0f;
	}


	//model = glm::mat4();
	//model = glm::rotate(model, 4*blackhawkAngle * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	//model = glm::translate(model, glm::vec3(-10.0f, 3.0f, -1.0f));
	//model = glm::rotate(model, 180.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	//model = glm::rotate(model, -90.0f * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
	//model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
	//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	//shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
	//blackhawk.RenderModel();
	//
	//
	model = glm::mat4();
	model = glm::translate(model, glm::vec3(-14.0f, -2.0f, 25.0f));
	model = glm::rotate(model, -90.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.03f, 0.03f, 0.03f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
	car1.RenderModel();
	
	model = glm::mat4();
	model = glm::translate(model, glm::vec3(0.0f, -2.0f, 25.0f));
	model = glm::rotate(model, -90.0f * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.003f, 0.003f, 0.003f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
	cars.RenderModel();
	//
	model = glm::mat4();
	model = glm::translate(model, glm::vec3(5.0f, -2.0f, 0.0f));
	model = glm::rotate(model, 180.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
	house.RenderModel();
	
	//model = glm::mat4();
	//model = glm::translate(model, glm::vec3(19.0f, -2.0f, -6.0f));
	////model = glm::rotate(model, -90.0f * toRadians, glm::vec3(0.0f, 0.0f, 0.0f));
	//model = glm::scale(model, glm::vec3(0.7f, 0.7f, 0.7f));
	//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	//shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
	//tree1.RenderModel();
	//
	//model = glm::mat4();
	//model = glm::translate(model, glm::vec3(19.0f, -2.0f, -2.0f));
	////model = glm::rotate(model, -90.0f * toRadians, glm::vec3(0.0f, 0.0f, 0.0f));
	//model = glm::scale(model, glm::vec3(0.7f, 0.7f, 0.7f));
	//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	//shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
	//tree2.RenderModel();
	//
	//model = glm::mat4();
	//model = glm::translate(model, glm::vec3(19.0f, -2.0f, 2.0f));
	////model = glm::rotate(model, -90.0f * toRadians, glm::vec3(0.0f, 0.0f, 0.0f));
	//model = glm::scale(model, glm::vec3(0.7f, 0.7f, 0.7f));
	//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	//shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
	//tree3.RenderModel();
	//
	model = glm::mat4();
	model = glm::translate(model, glm::vec3(19.0f, -2.0f, 6.0f));
	//model = glm::rotate(model, -90.0f * toRadians, glm::vec3(0.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.7f, 0.7f, 0.7f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
	tree3.RenderModel();
	
	model = glm::mat4();
	model = glm::translate(model, glm::vec3(19.0f, -2.0f, 10.0f));
	//model = glm::rotate(model, -90.0f * toRadians, glm::vec3(0.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.7f, 0.7f, 0.7f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
	tree3.RenderModel();

	




	model = glm::mat4();
	model = glm::translate(model, glm::vec3(-20.0f, -2.1f, 5.0f));
	//model = glm::rotate(model, -90.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
	Lamp1.RenderModel();

	model = glm::mat4();
	model = glm::translate(model, glm::vec3(-11.0f, -2.1f, 5.0f));
	model = glm::rotate(model, -180.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
	Lamp1.RenderModel();


	model = glm::mat4();
	model = glm::translate(model, glm::vec3(-20.0f, -2.1f, 8.0f));
	//model = glm::rotate(model, -90.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
	Lamp1.RenderModel();

	model = glm::mat4();
	model = glm::translate(model, glm::vec3(-11.0f, -2.1f, 8.0f));
	model = glm::rotate(model, -180.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
	Lamp1.RenderModel();

	model = glm::mat4();
	model = glm::translate(model, glm::vec3(-20.0f, -2.1f, 11.0f));
	//model = glm::rotate(model, -90.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
	Lamp1.RenderModel();

	model = glm::mat4();
	model = glm::translate(model, glm::vec3(-11.0f, -2.1f, 11.0f));
	model = glm::rotate(model, -180.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
	Lamp1.RenderModel();

	model = glm::mat4();
	model = glm::translate(model, glm::vec3(-20.0f, -2.1f, 14.0f));
	//model = glm::rotate(model, -90.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
	Lamp1.RenderModel();

	model = glm::mat4();
	model = glm::translate(model, glm::vec3(-11.0f, -2.1f, 14.0f));
	model = glm::rotate(model, -180.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
	Lamp1.RenderModel();

	model = glm::mat4();
	model = glm::translate(model, glm::vec3(-20.0f, -2.1f, -5.0f));
	//model = glm::rotate(model, -90.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
	Lamp1.RenderModel();

	model = glm::mat4();
	model = glm::translate(model, glm::vec3(-11.0f, -2.1f, -5.0f));
	model = glm::rotate(model, -180.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
	Lamp1.RenderModel();


	model = glm::mat4();
	model = glm::translate(model, glm::vec3(-20.0f, -2.1f, -8.0f));
	//model = glm::rotate(model, -90.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
	Lamp1.RenderModel();

	model = glm::mat4();
	model = glm::translate(model, glm::vec3(-11.0f, -2.1f, -8.0f));
	model = glm::rotate(model, -180.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
	Lamp1.RenderModel();

	model = glm::mat4();
	model = glm::translate(model, glm::vec3(7.0f, -2.9f, -3.0f));
	model = glm::rotate(model, -90.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::rotate(model, -90.0f * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.15f, 0.11f, 0.15f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
	Patch.RenderModel();

	model = glm::mat4();
	model = glm::translate(model, glm::vec3(-20.0f, -2.1f, 19.0f));
	//model = glm::rotate(model, -90.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::rotate(model, -90.0f * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.02f, 0.015f, 0.015f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
	StopSign.RenderModel();

	model = glm::mat4();
	model = glm::translate(model, glm::vec3(6.0f, -2.1f, 14.0f));
	//model = glm::rotate(model, -90.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::rotate(model, -90.0f * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.02f, 0.02f, 0.02f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
	fence.RenderModel();
	
	model = glm::mat4();
	model = glm::translate(model, glm::vec3(20.0f, -1.0f, 16.0f));
	//model = glm::rotate(model, -90.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	//model = glm::rotate(model, -90.0f * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.004f, 0.004f, 0.004f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
	wood.RenderModel();
	
	model = glm::mat4();
	model = glm::translate(model, glm::vec3(18.5f, -1.0f, 16.0f));
	model = glm::rotate(model, -90.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	//model = glm::rotate(model, -90.0f * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.004f, 0.004f, 0.004f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
	wood.RenderModel();
	
	model = glm::mat4();
	model = glm::translate(model, glm::vec3(-4.0f, -2.1f, 14.0f));
	//model = glm::rotate(model, -90.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::rotate(model, -90.0f * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.03f, 0.05f, 0.05f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
	gate.RenderModel();

	model = glm::mat4();
	model = glm::translate(model, glm::vec3(10.5f, -2.1f, 14.0f));
	//model = glm::rotate(model, -90.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::rotate(model, -90.0f * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.03f, 0.05f, 0.05f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
	gate.RenderModel();

	model = glm::mat4();
	model = glm::translate(model, glm::vec3(0.0f, -1.8f, 17.0f));
	//model = glm::rotate(model, -90.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	//model = glm::rotate(model, -90.0f * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.005f, 0.005f, 0.005f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
	plants1.RenderModel();
	
	model = glm::mat4();
	model = glm::translate(model, glm::vec3(-3.0f, -1.8f, 17.0f));
	//model = glm::rotate(model, -90.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	//model = glm::rotate(model, -90.0f * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.005f, 0.005f, 0.005f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
	plants1.RenderModel();

	//model = glm::mat4();
	//model = glm::translate(model, glm::vec3(11.0f, -2.0f, 0.0f));
	//model = glm::rotate(model, -90.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	//model = glm::scale(model, glm::vec3(0.009f, 0.009f, 0.009f));
	//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	//shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
	//rock1.RenderModel();
	//
	//
	//model = glm::mat4();
	//model = glm::translate(model, glm::vec3(10.0f, -2.0f, 3.4f));
	//model = glm::rotate(model, -90.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	//model = glm::scale(model, glm::vec3(0.005f, 0.005f, 0.005f));
	//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	//shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
	//rock2.RenderModel();

	model = glm::mat4();
	model = glm::translate(model, glm::vec3(16.0, -1.5f, 0.4f));
	model = glm::rotate(model, -90.0f * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.07f, 0.07f, 0.07f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
	dog.RenderModel();

	model = glm::mat4();
	model = glm::translate(model, glm::vec3(17.0, -1.2f, -13.4f));
	//model = glm::rotate(model, -90.0f * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
	barrel.RenderModel();

	model = glm::mat4();
	model = glm::translate(model, glm::vec3(15.0, -1.5f, -5.4f));
	model = glm::rotate(model, -90.0f * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.12f, 0.12f, 0.12f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
	cat.RenderModel();

	model = glm::mat4();
	model = glm::translate(model, glm::vec3(25.0f+birdfly, 9.5f, 4.0f));
	model = glm::rotate(model, -90.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(25.0f, 25.0f, 25.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
	bird.RenderModel();

	model = glm::mat4();
	model = glm::translate(model, glm::vec3(25.0f+birdfly, 8.0f, 6.0f));
	model = glm::rotate(model, -90.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(25.0f, 25.0f, 25.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
	bird.RenderModel();

	model = glm::mat4();
	model = glm::translate(model, glm::vec3(25.0f+birdfly, 6.0f, 3.6f));
	model = glm::rotate(model, -90.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(25.0f, 25.0f, 25.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
	bird.RenderModel();
}

void DirectionalShadowMapPass(DirectionalLight* light)
{
	directionalShadowShader.UseShader();

	glViewport(0, 0, light->getShadowMap()->GetShadowWidth(), light->getShadowMap()->GetShadowHeight());

	light->getShadowMap()->Write();
	glClear(GL_DEPTH_BUFFER_BIT);

	uniformModel = directionalShadowShader.GetModelLocation();
	directionalShadowShader.SetDirectionalLightTransform(&light->CalculateLightTransform());

	RenderScene();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderPass(glm::mat4 viewMatrix, glm::mat4 projectionMatrix)
{
	glViewport(0, 0, 1280, 1024);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	skybox.DrawSkybox(viewMatrix, projectionMatrix);

	shaderList[0].UseShader();

	uniformModel = shaderList[0].GetModelLocation();
	uniformProjection = shaderList[0].GetProjectionLocation();
	uniformView = shaderList[0].GetViewLocation();
	uniformModel = shaderList[0].GetModelLocation();
	uniformEyePosition = shaderList[0].GetEyePositionLocation();
	uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
	uniformShininess = shaderList[0].GetShininessLocation();



	glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
	glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(viewMatrix));
	glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

	shaderList[0].SetDirectionalLight(&mainLight);
	shaderList[0].SetPointLights(pointLights, pointLightCount);
	shaderList[0].SetSpotLights(spotLights, spotLightCount);
	shaderList[0].SetDirectionalLightTransform(&mainLight.CalculateLightTransform());

	mainLight.getShadowMap()->Read(GL_TEXTURE1);
	shaderList[0].SetTexture(0);
	shaderList[0].SetDirectionalShadowMap(1);

	glm::vec3 lowerLight = camera.getCameraPosition();
	lowerLight.y -= 0.3f;
	//spotLights[0].SetFlash(lowerLight, camera.getCameraDirection());

	RenderScene();
}

int main()
{
	mainWindow = Window(1280, 1024); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();

	CreateObjects();
	CreateShaders();

	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 5.0f, 0.5f);

	brickTexture = Texture("Textures/pyramid.png");
	brickTexture.LoadTextureA();
	dirtTexture = Texture("Textures/dirt.png");
	dirtTexture.LoadTextureA();
	plainTexture = Texture("Textures/plain.png");
	plainTexture.LoadTextureA();
	roadTexture = Texture("Textures/road.png");
	roadTexture.LoadTextureA();

	shinyMaterial = Material(4.0f, 256);
	dullMaterial = Material(0.3f, 4);

	//xwing = Model();
	//xwing.LoadModel("Models/x-wing.obj");

	//blackhawk = Model();
	//blackhawk.LoadModel("Models/uh60.obj");
	//
	car1 = Model();
	car1.LoadModel("Models/BMW X5 4.obj");
	
	cars = Model();
	cars.LoadModel("Models/cars.obj");
	//
	house = Model();
	house.LoadModel("Models/farmhouse_obj.obj");
	
	//tree1 = Model();
	//tree1.LoadModel("Models/Tree.obj");
	//
	//tree2 = Model();
	//tree2.LoadModel("Models/Tree.obj");
	//
	tree3 = Model();
	tree3.LoadModel("Models/Tree.obj");
	
	tree4 = Model();
	tree4.LoadModel("Models/Tree.obj");
	
	tree5 = Model();
	tree5.LoadModel("Models/Tree.obj");



	//rock1 = Model();
	//rock1.LoadModel("Models/rocks_01_model.obj");
	//
	//rock2 = Model();
	//rock2.LoadModel("Models/rocks_01_model.obj");



	Lamp1 = Model();
	Lamp1.LoadModel("Models/StreetLamp.obj");
	//Lamp2 = Model();
	//Lamp2.LoadModel("Models/StreetLamp.obj");
	//Lamp3 = Model();
	//Lamp3.LoadModel("Models/StreetLamp.obj");
	//Lamp4 = Model();
	//Lamp4.LoadModel("Models/StreetLamp.obj");
	//Lamp5 = Model();
	//Lamp5.LoadModel("Models/StreetLamp.obj");
	//Lamp6 = Model();
	//Lamp6.LoadModel("Models/StreetLamp.obj");
	//Lamp7 = Model();
	//Lamp7.LoadModel("Models/StreetLamp.obj");
	//Lamp8 = Model();
	//Lamp8.LoadModel("Models/StreetLamp.obj");
	//Lamp9 = Model();
	//Lamp9.LoadModel("Models/StreetLamp.obj");
	//Lamp10 = Model();
	//Lamp10.LoadModel("Models/StreetLamp.obj");
	//Lamp11 = Model();
	//Lamp11.LoadModel("Models/StreetLamp.obj");
	//Lamp12 = Model();
	//Lamp12.LoadModel("Models/StreetLamp.obj");

	Patch = Model();
	Patch.LoadModel("Models/10450_Rectangular_Grass_Patch_v1_iterations-2.obj");

	StopSign = Model();
	StopSign.LoadModel("Models/13920_Wall_Street_Street_Sign_on_Post_v1_l3.obj");

	fence = Model();
	fence.LoadModel("Models/13080_Wrought_Iron_fence_with_brick_v1_L2.obj");
	
	gate = Model();
	gate.LoadModel("Models/13078_Wooden_Post_and_Rail_Fence_v1_l3.obj");
	
	wood = Model();
	wood.LoadModel("Models/fre005.obj");
	
	plants1 = Model();
	plants1.LoadModel("Models/plants1.obj");
	
	plants2 = Model();
	plants2.LoadModel("Models/plants1.obj");

	dog = Model();
	dog.LoadModel("Models/12228_Dog_v1_L2.obj");

	barrel = Model();
	barrel.LoadModel("Models/Barrel.obj");

	cat = Model();
	cat.LoadModel("Models/12224_Cat_v5_l3.obj");

	bird = Model();
	bird.LoadModel("Models/HUMBIRD.obj");

	mainLight = DirectionalLight(2048, 2048,
		1.0f, 1.0f, 1.0f,
		0.5f, 0.5f,
		0.0f, -10.0f, 5.0f);


	pointLights[0] = PointLight(0.0f, 0.0f, 1.0f,
		0.0f, 0.1f,
		0.0f, 2.0f, 0.0f,
		0.3f, 0.2f, 0.1f);
	pointLightCount++;
	pointLights[1] = PointLight(0.0f, 1.0f, 0.0f,
		0.0f, 0.1f,
		-4.0f, 2.0f, 0.0f,
		0.3f, 0.1f, 0.1f);
	pointLightCount++;
	pointLights[2] = PointLight(1.0f, 0.5f, 1.0f,
		0.0f, 0.1f,
		4.0f, 2.0f, 0.0f,
		0.3f, 0.1f, 0.1f);
	pointLightCount++;
	pointLights[3] = PointLight(1.0f, 0.0f, 0.0f,
		0.0f, 0.1f,
		6.0f, 2.0f, 0.0f,
		0.3f, 0.2f, 0.1f);
	pointLightCount++;

	
	spotLights[0] = SpotLight(1.0f, 1.0f, 1.0f,
		0.0f, 2.0f,
		-18.0f, 0.0f, 5.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		20.0f);
	spotLightCount++;
	spotLights[1] = SpotLight(1.0f, 1.0f, 1.0f,
		0.0f, 2.0f,
		-18.0f, 0.0f, 8.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		20.0f);
	spotLightCount++;
	spotLights[2] = SpotLight(1.0f, 1.0f, 1.0f,
		0.0f, 2.0f,
		-18.0f, 0.0f, 11.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		20.0f);
	spotLightCount++;
	spotLights[3] = SpotLight(1.0f, 1.0f, 1.0f,
		0.0f, 2.0f,
		-18.0f, 0.0f, 14.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		20.0f);
	spotLightCount++;
	std::cout << MAX_SPOT_LIGHTS;

	spotLights[4] = SpotLight(1.0f, 1.0f, 1.0f,
		0.0f, 2.0f,
		-13.0f, 0.0f, 5.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		20.0f);
	spotLightCount++;
	spotLights[5] = SpotLight(1.0f, 1.0f, 1.0f,
		0.0f, 2.0f,
		-13.0f, 0.0f, 8.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		20.0f);
	spotLightCount++;
	spotLights[6] = SpotLight(1.0f, 1.0f, 1.0f,
		0.0f, 2.0f,
		-13.0f, 0.0f, 11.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		20.0f);
	spotLightCount++;
	spotLights[7] = SpotLight(1.0f, 1.0f, 1.0f,
		0.0f, 2.0f,
		-13.0f, 0.0f, 14.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		20.0f);
	spotLightCount++;
	spotLights[8] = SpotLight(1.0f, 1.0f, 1.0f,
		0.0f, 2.0f,
		-13.0f, 0.0f, -8.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		20.0f);
	spotLightCount++;
	spotLights[9] = SpotLight(1.0f, 1.0f, 1.0f,
		0.0f, 2.0f,
		-18.0f, 0.0f, -8.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		20.0f);
	spotLightCount++;
	spotLights[10] = SpotLight(1.0f, 1.0f, 1.0f,
		0.0f, 2.0f,
		-13.0f, 0.0f, -5.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		20.0f);
	spotLightCount++;
	spotLights[11] = SpotLight(1.0f, 1.0f, 1.0f,
		0.0f, 2.0f,
		-18.0f, 0.0f, -5.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		20.0f);
	spotLightCount++;

	std::vector<std::string> skyboxFaces;
	skyboxFaces.push_back("Textures/Skybox/sorbin_rt.tga");
	skyboxFaces.push_back("Textures/Skybox/sorbin_lf.tga");
	skyboxFaces.push_back("Textures/Skybox/sorbin_up.tga");
	skyboxFaces.push_back("Textures/Skybox/sorbin_dn.tga");
	skyboxFaces.push_back("Textures/Skybox/sorbin_bk.tga");
	skyboxFaces.push_back("Textures/Skybox/sorbin_ft.tga");

	skybox = Skybox(skyboxFaces);

	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 100.0f);

	// Loop until window closed
	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime(); // SDL_GetPerformanceCounter();
		deltaTime = now - lastTime; // (now - lastTime)*1000/SDL_GetPerformanceFrequency();
		lastTime = now;

		// Get + Handle User Input
		glfwPollEvents();

		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		DirectionalShadowMapPass(&mainLight);
		RenderPass(camera.calculateViewMatrix(), projection);

		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}