#include "Laborator8.h"

#include <vector>
#include <string>
#include <iostream>

#include <Core/Engine.h>

using namespace std;

/*
	Tema 3 EGC

	Gogea Mihail 334CC 

*/

// Propietatile unei bile
class Bila {

public:
	float pozX;
	float pozY;
	float pozZ;
	float vitezaBila;
	int on;
	int cazut;
	int numar;
	bool up;
};

Bila sfera;
Bila biluta;
Bila biluta1;
Bila biluta2;
int dimensiuneApa = 200; // Dimensiunea plasei
int soare = 0; // pentru culoarea soarelui

float G = 9.81f; // Acc. gravitationala

float L = 12.23f;
float S = 0.22f;
float Pi2 = 6.28f;

float corX, corZ; // Cordonatele unde sa produs unda

float A, Timp, Phi, W; // Datele pentru formula

float animatie = 0; // Pentru animatia apei

float lung; // distanta undei

// Aceste variabile imi trebuiesc pentru efectul slow-motion
bool efect;
int timpIncetinire;
float deltaTimeSeconds1;

Laborator8::Laborator8()
{
}

Laborator8::~Laborator8()
{
}

void Laborator8::Init()
{

	renderCameraTarget = false;

	camera = new Laborator::Camera();
	camera->Set(glm::vec3(0, 2, 3.5f), glm::vec3(0, 1, 0), glm::vec3(0, 1, 0));

	efect = false;
	timpIncetinire = 16;

	polygonMode = GL_FILL;
	sfera.pozX = 0;
	sfera.pozY = 1.5f;
	sfera.pozZ = 0;

	lung = 0.0f;

	biluta.on = 0;
	biluta.cazut = 0;

	Timp = 0;

	W = (float)(Pi2 / L);

	Phi = (float)(S * Pi2 / L);
	A = 0.0f;

	projectionMatrix = glm::perspective(RADIANS(60), window->props.aspectRatio, 0.01f, 200.0f);
	// **************************************************************************************************

	// Creez apa
	{
		vector<VertexFormat> vertices;
		
		for (int i = 0; i < dimensiuneApa; i++) {
			for (int j = 0; j < dimensiuneApa; j++) {
				vertices.push_back(VertexFormat(glm::vec3(i, 0, j), glm::vec3(0, 1, 1)));
			}
		}

		vector<unsigned short> indices;

		for (int i = 0; i < dimensiuneApa - 1; i++) {
			for (int j = 0; j < dimensiuneApa - 1; j++) {
				indices.push_back(j + i * dimensiuneApa);
				indices.push_back(j + (i+1) * dimensiuneApa);
				indices.push_back(1 + j + (i+1) * dimensiuneApa);
				indices.push_back(j + i * dimensiuneApa);
				indices.push_back(1 + j + (i + 1) * dimensiuneApa);
				indices.push_back(1 + j + i*dimensiuneApa);
				
			}
		}
		

		

		meshes["plan"] = new Mesh("Apa");
		meshes["plan"]->InitFromData(vertices, indices);
	}

	// **************************************************************************************************


	{
		Mesh* mesh = new Mesh("sphere");
		mesh->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "sphere.obj");
		meshes[mesh->GetMeshID()] = mesh;
	}


	// Create a shader program for drawing face polygon with the color of the normal
	{
		Shader *shader = new Shader("ShaderLab8");
		shader->AddShader("Source/Laboratoare/Laborator8/Shaders/VertexShader.glsl", GL_VERTEX_SHADER);
		shader->AddShader("Source/Laboratoare/Laborator8/Shaders/FragmentShader.glsl", GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}

	//Light & material properties
	{
		lightPosition = glm::vec3(-2.0f, 1.5f, 0);
		lightDirection = glm::vec3(0, 1, 0);
		materialShininess = 30;
		materialKd = 0.5;
		materialKs = 0.5;
	}
}

void Laborator8::FrameStart()
{
	// clears the color buffer (using the previously set color) and depth buffer
	glClearColor(0.3, 0.2, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::ivec2 resolution = window->GetResolution();
	// sets the screen area where to draw
	glViewport(0, 0, resolution.x, resolution.y);	
}

void Laborator8::Update(float deltaTimeSeconds)
{
	glPolygonMode(GL_FRONT_AND_BACK, polygonMode);

	// Pentru slow-motion
	if (efect == false) {
		Timp = Timp + 1;
		deltaTimeSeconds1 = deltaTimeSeconds;
	}
	else if (efect == true) {
		Timp += deltaTimeSeconds * timpIncetinire ;
		deltaTimeSeconds1 = deltaTimeSeconds / 4;
	}

	//Desnez sfera principala
	{
		glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(sfera.pozX, sfera.pozY, sfera.pozZ));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(0.3f));
		RenderSimpleMesh(meshes["sphere"], shaders["ShaderLab8"], modelMatrix);

	}

	animatie = 1;
	// Render ground
	{
		glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(-8, 0.01f, -8));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(0.08f));
		RenderSimpleMesh(meshes["plan"], shaders["ShaderLab8"], modelMatrix, glm::vec3(0,1,1));
	}
	animatie = 0;

	soare = 1;
	// Render the point light in the scene
	{
		glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, lightPosition);
		modelMatrix = glm::scale(modelMatrix, glm::vec3(0.1f));
		RenderSimpleMesh(meshes["sphere"], shaders["ShaderLab8"], modelMatrix, glm::vec3(1, 1, 0));
	}
	soare = 0;

	
	//Desenz picatura de apa
	if (biluta.pozY >= -0.2f && biluta.on == 1 ) {

		glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(biluta.pozX, biluta.pozY, biluta.pozZ));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(0.2f));
		RenderSimpleMesh(meshes["sphere"], shaders["ShaderLab8"], modelMatrix, glm::vec3(0, 1, 1));
	
		
		biluta.vitezaBila += G * deltaTimeSeconds1;
		biluta.pozY -= biluta.vitezaBila *deltaTimeSeconds1;
		
	
	}
	if (biluta.pozY <= -0.2f && biluta.on == 1) {
		
		biluta1.on = 1;
		biluta1.up = true;
		biluta.on = 0;
	}
	if (biluta1.pozY <= -0.27f ) {
		biluta2.on = 1;
		biluta2.up = true;
	}
	
	//Desenez biluta1
	if (biluta1.on == 1 && biluta1.pozY >= -0.4f) {
		glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(biluta1.pozX, biluta1.pozY, biluta1.pozZ));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(0.13f));
		RenderSimpleMesh(meshes["sphere"], shaders["ShaderLab8"], modelMatrix, glm::vec3(0, 1, 1));
			
		if (biluta1.pozY >= 1.2f) {
			biluta1.up = false;
			biluta1.vitezaBila = 0.0f;
		}

		if (biluta1.up == true) {
			biluta1.vitezaBila -= G * deltaTimeSeconds1;
			biluta1.pozY += biluta1.vitezaBila *deltaTimeSeconds1;
		}
		else if(biluta1.up == false){
			biluta1.vitezaBila += G * deltaTimeSeconds1;
			biluta1.pozY -= biluta1.vitezaBila *deltaTimeSeconds1;
		}
		
	}

	//Desenez biluta2
	if (biluta2.on == 1 && biluta2.pozY >= -0.4f) {
		glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(biluta2.pozX, biluta2.pozY, biluta2.pozZ));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(0.08f));
		RenderSimpleMesh(meshes["sphere"], shaders["ShaderLab8"], modelMatrix, glm::vec3(0, 1, 1));

		if (biluta2.pozY >= 1.2f) {
			biluta2.up = false;
			biluta2.vitezaBila = 0.0f;
		}

		if (biluta2.up == true) {
			biluta2.vitezaBila -= G * deltaTimeSeconds1;
			biluta2.pozY += biluta2.vitezaBila *deltaTimeSeconds1;
		}
		else if (biluta2.up == false) {
			biluta2.vitezaBila += G * deltaTimeSeconds1;
			biluta2.pozY -= biluta2.vitezaBila *deltaTimeSeconds1;
		}

	}

	if (biluta.pozY < 0) {

		if (efect == false) {
			lung += 0.22f;
		}
		else {
			lung += 0.22f * (deltaTimeSeconds * timpIncetinire) ;
		}
		
		biluta.cazut = 1;

		
		corX = (biluta.pozX + 8.3501f)* (float)(100 / 8);
		corZ = (biluta.pozZ + 8.3501f)* (float)(100 / 8);
	}

	
	
	if (A > 0.1f) {
		if (efect == false) {
			A -= 0.0079f;
		}
		else {
			A -= 0.0079f / 4;
		}
		
	}

}

void Laborator8::FrameEnd()
{
	DrawCoordinatSystem(camera->GetViewMatrix(), projectionMatrix);
}


void Laborator8::RenderSimpleMesh(Mesh *mesh, Shader *shader, const glm::mat4 & modelMatrix, const glm::vec3 &color)
{
	if (!mesh || !shader || !shader->GetProgramID())
		return;

	// render an object using the specified shader and the specified position
	glUseProgram(shader->program);



	glUniform1f(glGetUniformLocation(shader->program, "A"), A);
	// Set shader uniforms for light & material properties
	// TODO: Set light position uniform
	int light_position = glGetUniformLocation(shader->program, "light_position");
	glUniform3f(light_position, lightPosition.x, lightPosition.y, lightPosition.z);

	glUniform1f(glGetUniformLocation(shader->program, "animatie"), animatie);

	glUniform1f(glGetUniformLocation(shader->program, "Phi"), Phi);

	int light_direction = glGetUniformLocation(shader->program, "light_direction");
	glUniform3f(light_direction, lightDirection.x, lightDirection.y, lightDirection.z);

	// TODO: Set eye position (camera position) uniform
	glm::vec3 eyePosition = camera->position;
	int eye_position = glGetUniformLocation(shader->program, "eye_position");
	glUniform3f(eye_position, eyePosition.x, eyePosition.y, eyePosition.z);

	

	// TODO: Set material property uniforms (shininess, kd, ks, object color) 
	int material_shininess = glGetUniformLocation(shader->program, "material_shininess");
	glUniform1i(material_shininess, materialShininess);

	int material_kd = glGetUniformLocation(shader->program, "material_kd");
	glUniform1f(material_kd, materialKd);

	glUniform1f(glGetUniformLocation(shader->program, "corZ"), corZ);

	int material_ks = glGetUniformLocation(shader->program, "material_ks");
	glUniform1f(material_ks, materialKs);

	glUniform1f(glGetUniformLocation(shader->program, "W"), W);

	int object_color = glGetUniformLocation(shader->program, "object_color");
	glUniform3f(object_color, color.r, color.g, color.b);

	glUniform1f(glGetUniformLocation(shader->program, "corX"), corX);
	
	int so = glGetUniformLocation(shader->program, "so");
	glUniform1i(so, soare);


	glUniform1f(glGetUniformLocation(shader->program, "Timp"), Timp);

	// Bind model matrix
	GLint loc_model_matrix = glGetUniformLocation(shader->program, "Model");
	glUniformMatrix4fv(loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

	glUniform1f(glGetUniformLocation(shader->program, "lung"), lung);

	// Bind view matrix
	glm::mat4 viewMatrix = camera->GetViewMatrix();
	int loc_view_matrix = glGetUniformLocation(shader->program, "View");
	glUniformMatrix4fv(loc_view_matrix, 1, GL_FALSE, glm::value_ptr(viewMatrix));

	// Bind projection matrix
	
	int loc_projection_matrix = glGetUniformLocation(shader->program, "Projection");
	glUniformMatrix4fv(loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

	// Draw the object
	glBindVertexArray(mesh->GetBuffers()->VAO);
	glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_SHORT, 0);
}

// Documentation for the input functions can be found in: "/Source/Core/Window/InputController.h" or
// https://github.com/UPB-Graphics/Framework-EGC/blob/master/Source/Core/Window/InputController.h

void Laborator8::OnInputUpdate(float deltaTime, int mods)
{

	if (window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT))
	{
		float cameraSpeed = 2.0f;

		if (window->KeyHold(GLFW_KEY_W)) {
			camera->TranslateForward(cameraSpeed * deltaTime);
		}

		if (window->KeyHold(GLFW_KEY_A)) {
			camera->TranslateRight(-cameraSpeed * deltaTime);

		}

		if (window->KeyHold(GLFW_KEY_S)) {
			camera->TranslateForward(-cameraSpeed * deltaTime);

		}

		if (window->KeyHold(GLFW_KEY_D)) {
			camera->TranslateRight(cameraSpeed * deltaTime);


		}

		if (window->KeyHold(GLFW_KEY_Q)) {
			camera->TranslateUpword(-cameraSpeed * deltaTime);

		}

		if (window->KeyHold(GLFW_KEY_E)) {
			camera->TranslateUpword(cameraSpeed * deltaTime);
		}
	}

	float speed = 2;

	if (!window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT))
	{
		
		glm::vec3 up = glm::vec3(0, 1, 0);
		glm::vec3 right = camera->right;
		glm::vec3 forward = camera->forward;
		forward = glm::normalize(glm::vec3(forward.x, 0, forward.z));

		// Control light position using on W, A, S, D, E, Q
		if (window->KeyHold(GLFW_KEY_UP)) lightPosition += forward * deltaTime * speed;
		if (window->KeyHold(GLFW_KEY_LEFT)) lightPosition -= right * deltaTime * speed;
		if (window->KeyHold(GLFW_KEY_DOWN)) lightPosition -= forward * deltaTime * speed;
		if (window->KeyHold(GLFW_KEY_RIGHT)) lightPosition += right * deltaTime * speed;
		if (window->KeyHold(GLFW_KEY_M)) lightPosition += up * deltaTime * speed;
		if (window->KeyHold(GLFW_KEY_N)) lightPosition -= up * deltaTime * speed;


		if (window->KeyHold(GLFW_KEY_W)) sfera.pozZ -= deltaTime * speed;
		if (window->KeyHold(GLFW_KEY_S)) sfera.pozZ += deltaTime * speed;
		if (window->KeyHold(GLFW_KEY_A)) sfera.pozX -= deltaTime * speed;
		if (window->KeyHold(GLFW_KEY_D)) sfera.pozX += deltaTime * speed;


	}
	


}

void Laborator8::OnKeyPress(int key, int mods)
{
	// add key press event

	if (key == GLFW_KEY_Z)
	{
		switch (polygonMode)
		{
		case GL_POINT:
			polygonMode = GL_FILL;
			break;
		case GL_LINE:
			polygonMode = GL_POINT;
			break;
		default:
			polygonMode = GL_LINE;
			break;
		}
	}
	
	if (key == GLFW_KEY_SPACE ) {


		//A = 2.8f;
		A = 2.75f;

		//Cand apas space voi crea bilele , dar pentru inceput doar prima 
		// va aparea cand bila principala va atinge apa.
		biluta.pozX = sfera.pozX;
		biluta.pozY = sfera.pozY;
		biluta.pozZ = sfera.pozZ;
		biluta.vitezaBila = 0.0f;
		biluta.on = 1;
		biluta.cazut = 0;
		lung = 0.0f;
		Timp = 6;

		biluta1.pozX = sfera.pozX;
		biluta1.pozY = -0.2f;
		biluta1.pozZ = sfera.pozZ;
		biluta1.vitezaBila = 4.2f;
		biluta1.on = 0;

		biluta2.pozX = sfera.pozX;
		biluta2.pozY = -0.3f;
		biluta2.pozZ = sfera.pozZ;
		biluta2.vitezaBila = 4.6f;
		biluta2.on = 0;
	}

	if (key == GLFW_KEY_T) {

		if (efect == true) {
			efect = false;
		}
		else if (efect == false) {
			efect = true;
		}

	}


}

void Laborator8::OnKeyRelease(int key, int mods)
{
	// add key release event
}

void Laborator8::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
	if (window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT))
	{
		float sensivityOX = 0.001f;
		float sensivityOY = 0.001f;
	
		camera->RotateFirstPerson_OX(-deltaX * sensivityOX);
		camera->RotateFirstPerson_OY(-deltaY * sensivityOY);
	

	}
}

void Laborator8::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
	// add mouse button press event
}

void Laborator8::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
	// add mouse button release event
}

void Laborator8::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}

void Laborator8::OnWindowResize(int width, int height)
{
}
