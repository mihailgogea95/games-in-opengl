#include "Laborator5.h"

#include <vector>
#include <string>
#include <iostream>
#include <Core/Engine.h>


#include <Component/SimpleScene.h>
#define PI 3.14159265
#define ZERO 0.0f
/*
	Tema 2 Grafica
	Gogea Mihail 334CC
*/


using namespace std;

// Clasa pentru proiectile
class Proiectil {

public:
	std::string nume;
	float pozX, pozY, pozZ;
	glm::vec3 forward;
	glm::vec3 position;
	float viteza;
	float distanta;
	float orientareGrenada;
	float timpGrenada;
	float panta;
	float semn;
};

//Retin numarul de inamici ca sa stiu ce putere le dau
int numarInamici = 0;
int numarGrenade = 0;

float aparitieGrenada = 5.0f;
float aparitiePistol = 5.0f;
float aparitiePusca = 5.0f;

// Pentru turnuri
float diferenta1;
float diferenta2;
float parteIntreaga;
float timpTurn;

// Aceasta clasa este folosita pentru turnuri,om, inamici
class Personaj {

public:
	std::string nume;
	float pozX, pozY, pozZ;
	glm::vec3 forward;
	glm::vec3 position;

	int arma;
	int distantaGrenada;

	int directie;
	bool rotire;
	int vieti;

	float timpMurit;
	float scalarMurit;
	
	float timpTurn;

};


Personaj persoana;

float rot = 0;
float rotireVieti = 0;

bool joc = true; // Daca jocul este in functionare sau nu.

int unghiZoom = 60;


Laborator5::Laborator5()
{
}

Laborator5::~Laborator5()
{
}

// Ma ajuta la rotirea omului.
float unghiOm;
float angleOm;

float aparitieEnemy = 5.0f;
float timp = 0.0f;

int vieti = 3;

//Pentru calculul distantei
float distantaTemp;
float putere1;
float putere2;

int i;

// Cele 3 vectori pentru inamici, proiectile si inamici care au murit.
vector<Personaj> vectorInamici;
vector<Proiectil> vectorProiectile;
vector<Personaj> vectorMorti;

Personaj OM;
Personaj turn[3];

// Creez aceste 2 mesh-uri pentru a fii desenate la minimapa.
Mesh* rosu = new Mesh("rosu");
Mesh* albastru = new Mesh("albastru");
std::vector<VertexFormat> vertices = {
	VertexFormat(glm::vec3(-1,0,1), glm::vec3(1,0,0)),
	VertexFormat(glm::vec3(1,0,-1), glm::vec3(1,0,0)),
	VertexFormat(glm::vec3(-1,0,-1), glm::vec3(1,0,0)),
	VertexFormat(glm::vec3(1,0,1), glm::vec3(1,0,0))
};
std::vector<VertexFormat> vertices1 = {
	VertexFormat(glm::vec3(-1,0,1), glm::vec3(0,1,1)),
	VertexFormat(glm::vec3(1,0,-1), glm::vec3(0,1,1)),
	VertexFormat(glm::vec3(-1,0,-1), glm::vec3(0,1,1)),
	VertexFormat(glm::vec3(1,0,1), glm::vec3(0,1,1))
};
std::vector<unsigned short> indices = { 0,1,2,  0,3,1  };



// Desenez sferele in jurul omului
void Laborator5::DesenareVieti(const glm::vec3 vector, float delta,float distX, float distY, float distZ) {

	glm::mat4 modelMatrix = glm::mat4(1);
	modelMatrix = glm::translate(modelMatrix, camera->GetTargetPosition());
	modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 0.5f, 0));
	modelMatrix = glm::rotate(modelMatrix, delta, vector);
	modelMatrix = glm::scale(modelMatrix, glm::vec3(0.1f));
	modelMatrix = glm::translate(modelMatrix, glm::vec3(distX, distY, distZ));
	RenderMesh(meshes["sphere"], shaders["VertexColor"], modelMatrix);


}
void Laborator5::Init()
{
	renderCameraTarget = false;

	rosu->InitFromData(vertices, indices);
	albastru->InitFromData(vertices1, indices);

	AddMeshToList(rosu);
	AddMeshToList(albastru);


	camera = new Laborator::Camera();
	camera->Set(glm::vec3(0, -1.05f, 3.5f), glm::vec3(0, 1, 0), glm::vec3(0, 1, 0));


	{
		Mesh* mesh = new Mesh("box");
		mesh->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "box.obj");
		meshes[mesh->GetMeshID()] = mesh;
	}

	{
		Mesh* mesh = new Mesh("sphere");
		mesh->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "sphere.obj");
		meshes[mesh->GetMeshID()] = mesh;
	}

	{
		Mesh* mesh = new Mesh("om");
		mesh->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "Steve.obj");
		meshes[mesh->GetMeshID()] = mesh;
	}

	{
		Mesh* mesh = new Mesh("monster");
		mesh->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "MonsterLowPoly.obj");
		meshes[mesh->GetMeshID()] = mesh;
	}
	{
		Mesh* mesh = new Mesh("tower");
		mesh->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "tower.obj");
		meshes[mesh->GetMeshID()] = mesh;
	}


	// Pentru minimap fac o noua camera care va privii de sus
	cameraMinimap = new Laborator::Camera();
	cameraMinimap->Set(glm::vec3(0, 16, 0.01f), glm::vec3(0, 1, 0), glm::vec3(0, 1, 0));

	// Arma initiala a Omului
	OM.arma = 1;

	//Pozitile initiale ale turnurilor
	turn[0].pozX = 0.0f;
	turn[0].pozY = 0.0f;
	turn[0].pozZ = 1.0f;
	turn[0].timpTurn = 0.0f;
	
	turn[1].pozX = 1.0f;
	turn[1].pozY = 0.0f;
	turn[1].pozZ = -2.0f;
	turn[1].timpTurn = 0.0f;

	turn[2].pozX = -3.0f;
	turn[2].pozY = 0.0f;
	turn[2].pozZ = 2.0f;
	turn[2].timpTurn = 0.0f;

	//Retin intro alta camera , camera principala pentru revenire atunci cand desenez minimap.
	cameraRezerva = camera;
}

void Laborator5::FrameStart()
{
	// clears the color buffer (using the previously set color) and depth buffer
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	projectionMatrix = glm::perspective(RADIANS(60), window->props.aspectRatio, 0.01f, 200.0f);
}

void Laborator5::Update(float deltaTimeSeconds)
{

	
	glm::ivec2 resolution = window->GetResolution();
	
	glViewport(0, 0, resolution.x, resolution.y);
	
	// Timpul pentru a trage turnurile
	turn[0].timpTurn += deltaTimeSeconds / 2;
	turn[1].timpTurn += deltaTimeSeconds / 2;
	turn[2].timpTurn += deltaTimeSeconds / 2;

	glm::mat4 modelMatrix;
	//Daca omul mai este in viata.
	if (joc == true) {

		

		timp += deltaTimeSeconds;
		aparitiePistol += deltaTimeSeconds * 2;
		aparitieGrenada += deltaTimeSeconds * 2;
		aparitiePusca += deltaTimeSeconds * 2 ;

		// Introduc inamicii in vector
		if (timp > aparitieEnemy) {

			numarInamici++;

			if(aparitieEnemy >= 2.0f)
				aparitieEnemy -= deltaTimeSeconds * 3;

			Personaj persoana;
			persoana.nume = "Enemy";
			persoana.pozX = -8.0f;
			persoana.pozY = 0.0f;
			persoana.pozZ = -8.0f;
			persoana.directie = 1;
			
			if (numarInamici <= 3)
				persoana.vieti = 4;
			else if (numarInamici > 3 && numarInamici <= 6)
				persoana.vieti = 6;
			else if (numarInamici > 6 && numarInamici <= 10)
				persoana.vieti = 7;
			else if (numarInamici > 10)
				persoana.vieti = 8;

			persoana.timpMurit = 0.0f;
			persoana.scalarMurit = 0.3f;
			vectorInamici.push_back(persoana);

			timp = 0;

		}

				
		//Desenez proiectilele
		std::vector<Proiectil>::iterator itp;
		std::vector<Personaj>::iterator it1;

		for (itp = vectorProiectile.begin(); itp != vectorProiectile.end();) {



			if ((*itp).nume == "turn") {

				// Le calculez traiectoria cu ajutorul pantei
				parteIntreaga = abs((*itp).panta);
				float oldValue, dist;

				if (parteIntreaga < 1) {
					oldValue = (*itp).position[0];
					(*itp).position[0] += deltaTimeSeconds * (*itp).semn * (*itp).viteza;
					dist = (*itp).position[0] - oldValue;
					(*itp).position[2] = (*itp).panta * dist + (*itp).position[2];
					(*itp).position[1] -= deltaTimeSeconds;
				}
				else {
					oldValue = (*itp).position[2];
					(*itp).position[2] += deltaTimeSeconds * (*itp).semn * (*itp).viteza;
					dist = (*itp).position[2] - oldValue;
					(*itp).position[0] = dist / (*itp).panta + (*itp).position[0];
					(*itp).position[1] -= deltaTimeSeconds ;
				}
				(*itp).distanta -= deltaTimeSeconds * (*itp).viteza;
			}
			else {
				glm::vec3 dir = glm::normalize((*itp).forward);
				(*itp).position += dir * deltaTimeSeconds * (*itp).viteza;
				// Scad distanta proiectilului.
				(*itp).distanta -= deltaTimeSeconds * (*itp).viteza;
			}

			// Fac animatia grenazii (bolta) 
			if ((*itp).nume == "grenada") {
				if ((*itp).distanta >= 4.5f) {
					(*itp).orientareGrenada += deltaTimeSeconds / 2;
				}
				else if ((*itp).distanta <= 4.5f && (*itp).distanta >= 3.8f) {
					(*itp).orientareGrenada += deltaTimeSeconds / 6;
				}
				else if ((*itp).distanta >= 2.8f && (*itp).distanta <= 3.8f) {
					(*itp).orientareGrenada -= deltaTimeSeconds / 6;
				}
				else if ((*itp).distanta <= 2.8f) {
					(*itp).orientareGrenada -= deltaTimeSeconds / 2;
				}
				
				// Omor cand explodeaza grenada
				if ((*itp).distanta <= 0.05f) {
					for (it1 = vectorInamici.begin(); it1 != vectorInamici.end();) {

						//Calculez distanta din jurul grenadei atunci cand se loveste de pamant
						putere1 = pow((*itp).position[0] - (*it1).pozX, 2);
						putere2 = pow((*itp).position[2] - (*it1).pozZ, 2);
						distantaTemp = sqrtf(putere1 + putere2);
						if (distantaTemp < 3.0f) {

							vectorMorti.push_back((*it1));
							it1 = vectorInamici.erase(it1);

						}
						else {
							++it1;
						}

					}
				}

			}

			//Desenez proiectilele
			modelMatrix = glm::mat4(1);
			if ((*itp).nume == "turn") {
				//Desenez traiectoria
				modelMatrix = glm::translate(modelMatrix, glm::vec3((*itp).position[0], (*itp).position[1], (*itp).position[2]));
				//Marimea unui proiectil
				modelMatrix = glm::scale(modelMatrix, glm::vec3(0.15f));

				RenderMesh(meshes["sphere"], shaders["VertexNormal"], modelMatrix);
			}
			else
				if ((*itp).nume == "pistol") {
				modelMatrix = glm::translate(modelMatrix, glm::vec3((*itp).position[0], 0.3f, (*itp).position[2]));
				//Marimea unui proiectil ca
				modelMatrix = glm::scale(modelMatrix, glm::vec3(0.12f));

				RenderMesh(meshes["sphere"], shaders["VertexNormal"], modelMatrix);
			}
			else
				if ((*itp).nume == "pusca") {
				modelMatrix = glm::translate(modelMatrix, glm::vec3((*itp).position[0], 0.3f, (*itp).position[2]));
				modelMatrix = glm::scale(modelMatrix, glm::vec3(0.08f));
				RenderMesh(meshes["sphere"], shaders["VertexNormal"], modelMatrix);
			}
			else {
				modelMatrix = glm::translate(modelMatrix, glm::vec3((*itp).position[0], (*itp).orientareGrenada , (*itp).position[2]));
				modelMatrix = glm::scale(modelMatrix, glm::vec3(0.2f));
				RenderMesh(meshes["sphere"], shaders["VertexNormal"], modelMatrix);
			}
			if ((*itp).nume == "grenada") {
				numarGrenade = numarGrenade + 1;
			}
			
			distantaTemp = (*itp).distanta;
			if (distantaTemp <= ZERO ) {
				itp = vectorProiectile.erase(itp);
			}
			else {
				++itp;
			}

		}


		//Desenez Inamicii
		
		for (it1 = vectorInamici.begin(); it1 != vectorInamici.end();) {

			//Coliziunile inamici cu proiectile
			for (itp = vectorProiectile.begin(); itp != vectorProiectile.end();) {
				
				//Cand proiectilul intra in inamic
				putere1 = pow((*itp).position[0] - (*it1).pozX, 2);
				putere2 = pow((*itp).position[2] - (*it1).pozZ, 2);

				distantaTemp = sqrtf(putere1 + putere2);

				if (distantaTemp < 0.2f) {

					if ((*itp).nume == "grenada") {
						(*it1).vieti = 0;
						
						//Sterg proiectilul
						itp = vectorProiectile.erase(itp);

					}
					else if((*itp).nume == "pusca"){
						(*it1).vieti -= 1;
						itp = vectorProiectile.erase(itp);
					//	vectorMorti.push_back(*it1);
					}
					else if((*itp).nume == "pistol") {
						(*it1).vieti -= 2;
						itp = vectorProiectile.erase(itp);
					//	vectorMorti.push_back(*it1);
					}
					else if ((*itp).nume == "turn") {
						(*it1).vieti -= 1;
						itp = vectorProiectile.erase(itp);
					}
				}
				else {
					++itp;
				}


			}
			if ((*it1).vieti <= 0 ) {
				// Introduc in vector de animatie
				vectorMorti.push_back(*it1);
				it1 = vectorInamici.erase(it1);
				continue;
			}

			// Desenz inamicii pe ecran
			// Sa stiu pe ce traiectorie sunt inamicii (Parcurgerea in Z)
			if ((*it1).directie == 1 && (*it1).pozX >= 8.0f) {
				(*it1).directie = 2;

			}
			else if ((*it1).directie == 2 && (*it1).pozX <= -8.0f && (*it1).pozZ >= 8.0f) {
				(*it1).directie = 3;
			}
			else if ((*it1).pozX >= 8.0f && (*it1).pozZ >= 8.0f) {
				vieti--;
				it1 = vectorInamici.erase(it1);
				
			}

			if (vieti == 0) {
				OM.timpMurit = 0.0f;
				OM.scalarMurit = 0.08f;
				joc = false;
			}

			// Directia lor de orientare si deplasarea
			if ((*it1).directie == 1) {
				rot = (float)(90);
				(*it1).pozX += deltaTimeSeconds * 2;
			}
			else if ((*it1).directie == 2) {
				(*it1).pozX -= deltaTimeSeconds * 2;
				(*it1).pozZ += deltaTimeSeconds * 2;
				(*it1).rotire = true;
				rot = (float)(-45);
			}
			else if ((*it1).directie == 3) {
				(*it1).pozX += deltaTimeSeconds * 2;
				rot = (float)(90);
			}


			modelMatrix = glm::mat4(1);
			modelMatrix = glm::translate(modelMatrix, glm::vec3((*it1).pozX, (*it1).pozY, (*it1).pozZ));
			modelMatrix = glm::scale(modelMatrix, glm::vec3(0.3f));
			modelMatrix = glm::translate(modelMatrix, glm::vec3(0, +1.2f, +0.0f));

			modelMatrix = glm::rotate(modelMatrix, RADIANS(rot), glm::vec3(0, 1, 0));
			RenderMesh(meshes["monster"], shaders["VertexNormal"], modelMatrix);


			++it1;
		}

		std::vector<Personaj>::iterator itm;
		// Desenez animatia cand moare un inamic
		for (itm = vectorMorti.begin(); itm != vectorMorti.end();) {

			if((*itm).timpMurit <= 3.0f){

				(*itm).timpMurit += deltaTimeSeconds ;
				(*itm).scalarMurit -= deltaTimeSeconds/12 ;

				modelMatrix = glm::mat4(1);
				modelMatrix = glm::translate(modelMatrix, glm::vec3((*itm).pozX, (*itm).pozY, (*itm).pozZ));
				//Scade scalarea cand moare
				modelMatrix = glm::scale(modelMatrix, glm::vec3((*itm).scalarMurit));
				modelMatrix = glm::rotate(modelMatrix, (*itm).timpMurit, glm::vec3(1, 0, 0));
				RenderMesh(meshes["monster"], shaders["VertexColor"], modelMatrix);
				
				++itm;
			}
			else {
				itm = vectorMorti.erase(itm);
			}

		}


		rotireVieti += deltaTimeSeconds * 3;
		
		// Desenez omul si vietile lui.
		if (renderCameraTarget && OM.arma != 2)
		{

			//Desenez vietile omului.
			if (vieti == 3) {
				DesenareVieti(glm::vec3(0, 1, 0), rotireVieti, 3, 0, 0);
				DesenareVieti(glm::vec3(1, 0, 0), rotireVieti, 0, 3, 0);
				DesenareVieti(glm::vec3(0, 0, 1), rotireVieti, 3, 0, 0);
			}
			else if (vieti == 2) {
				DesenareVieti(glm::vec3(0, 1, 0), rotireVieti, 3, 0, 0);
				DesenareVieti(glm::vec3(1, 0, 0), rotireVieti, 0, 3, 0);
			}
			else if (vieti == 1) {
				DesenareVieti(glm::vec3(0, 1, 0), rotireVieti, 3, 0, 0);
			}

			//Desenez omul
			modelMatrix = glm::mat4(1);
			modelMatrix = glm::translate(modelMatrix, camera->GetTargetPosition());
			OM.position = camera->GetTargetPosition();

			modelMatrix = glm::scale(modelMatrix, glm::vec3(0.08f));
			modelMatrix = glm::rotate(modelMatrix, RADIANS((float)((180 / 2) / (PI / 2) * (unghiOm + (PI / 2)))), glm::vec3(0, 1, 0));
			RenderMesh(meshes["om"], shaders["VertexNormal"], modelMatrix);
		}

		// Desenez turnuri si preiau pozitile la inamici

		for (i = 0; i < 3; i++) {
			modelMatrix = glm::mat4(1);
			modelMatrix = glm::translate(modelMatrix, glm::vec3(turn[i].pozX, turn[i].pozY, turn[i].pozZ));
			modelMatrix = glm::scale(modelMatrix, glm::vec3(0.0013f));
			RenderMesh(meshes["tower"], shaders["VertexNormal"], modelMatrix);
		}
		

		//Sa vad ce inamici sunt aproape de turnuri ca turnurile sa inceapa sa traga
		for (it1 = vectorInamici.begin(); it1 != vectorInamici.end();) {
		

			for (i = 0; i < 3; i++) {
				if ((sqrtf(pow((*it1).pozX - turn[i].pozX, 2) + pow((*it1).pozZ - turn[i].pozZ, 2)) < 4.2f) && turn[i].timpTurn >= 0.9f) {

					turn[i].timpTurn = 0.0f;
					diferenta1 = turn[i].pozZ - (*it1).pozZ;
					diferenta2 = turn[i].pozX - (*it1).pozX;

					Proiectil temp;
					temp.nume = "turn";
					temp.position[0] = turn[i].pozX;
					temp.position[1] = 0.5f;
					temp.position[2] = turn[i].pozZ;
					temp.viteza = 12.0f;
					temp.distanta = 5.0f;

					temp.panta = diferenta1 / diferenta2;

					parteIntreaga = abs(temp.panta);

					if ((*it1).pozZ > turn[i].pozZ && parteIntreaga > 1) {
						temp.semn = 1;
					}
					else if ((*it1).pozX > turn[i].pozX && parteIntreaga < 1) {
						temp.semn = 1;
					}
					else {
						temp.semn = -1;
					}

					vectorProiectile.push_back(temp);
				}
			}
			

			++it1;
		}

		DrawCoordinatSystem(camera->GetViewMatrix(), projectionMatrix);

		// Pentru a afisa minimapa
		if (OM.arma != 2) {
			glViewport(resolution.x * 6.95f / 10, 0, resolution.x * 3.2f / 10, resolution.y * 3.2f / 10);
			camera = cameraMinimap;

			//Desenez cu rosu inamici pe minimapa
			for (it1 = vectorInamici.begin(); it1 != vectorInamici.end(); ) {

				modelMatrix = glm::mat4(1);
				modelMatrix = glm::translate(modelMatrix, glm::vec3((*it1).pozX, (*it1).pozY, (*it1).pozZ));
				modelMatrix = glm::scale(modelMatrix, glm::vec3(0.5f));
				RenderMesh(meshes["rosu"], shaders["VertexColor"], modelMatrix);
				++it1;
			}

			// Desenez sfere albe la proiectile
			for (itp = vectorProiectile.begin(); itp != vectorProiectile.end(); ) {

				modelMatrix = glm::mat4(1);
				modelMatrix = glm::translate(modelMatrix, glm::vec3((*itp).position[0], 0, (*itp).position[2]));
				modelMatrix = glm::scale(modelMatrix, glm::vec3(0.37f));
				RenderMesh(meshes["sphere"], shaders["VertexColor"], modelMatrix);

				++itp;
			}


			// Desenez albastru acolo unde e Omul pentru Minimapa.
			modelMatrix = glm::mat4(1);

			modelMatrix = glm::translate(modelMatrix, OM.position);
			
			modelMatrix = glm::scale(modelMatrix, glm::vec3(0.4f));
			RenderMesh(meshes["albastru"], shaders["VertexColor"], modelMatrix);


			for (i = 0; i < 3; i++) {
				modelMatrix = glm::mat4(1);
				modelMatrix = glm::translate(modelMatrix, glm::vec3(turn[i].pozX, turn[i].pozY, turn[i].pozZ));
				modelMatrix = glm::scale(modelMatrix, glm::vec3(2.0f));
				RenderMesh(meshes["sphere"], shaders["VertexNormal"], modelMatrix);
			}

			projectionMinimap = glm::perspective(RADIANS(50), window->props.aspectRatio, 0.01f, 200.0f);
			DrawCoordinatSystem(cameraMinimap->GetViewMatrix(), projectionMinimap);

			//Inapoiez la camera principala
			camera = cameraRezerva;
		}				
	}
else {

	//Omu a murit si ii voi face animatia de sfarsit

	if (OM.timpMurit <= 5.0f) {
		OM.timpMurit += deltaTimeSeconds;
		OM.scalarMurit -= deltaTimeSeconds / 140;

		//Desenez omul
		modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, camera->GetTargetPosition());
		modelMatrix = glm::scale(modelMatrix, glm::vec3(OM.scalarMurit));
		modelMatrix = glm::rotate(modelMatrix, OM.timpMurit / 4.0f, glm::vec3(0, 0, 1));
		RenderMesh(meshes["om"], shaders["VertexColor"], modelMatrix);
	}

}
		
}

void Laborator5::FrameEnd()
{
	
}

void Laborator5::RenderMesh(Mesh * mesh, Shader * shader, const glm::mat4 & modelMatrix)
{
	if (!mesh || !shader)
		return;

	shader->Use();
	glUniformMatrix4fv(shader->loc_view_matrix, 1, false, glm::value_ptr(camera->GetViewMatrix()));
	glUniformMatrix4fv(shader->loc_projection_matrix, 1, false, glm::value_ptr(projectionMatrix));
	glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

	mesh->Render();
}


void Laborator5::OnInputUpdate(float deltaTime, int mods)
{
	// move the camera only if MOUSE_RIGHT button is pressed
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

		
	}



}

void Laborator5::OnKeyPress(int key, int mods)
{
	
	if (key == GLFW_KEY_T)
	{
		renderCameraTarget = !renderCameraTarget;
	}
	
	if (key == GLFW_KEY_1) {
		if (OM.arma == 2) {

			camera->TranslateForward(-3.0f);
			
		}
		OM.arma = 1;
	}
		

	if (key == GLFW_KEY_2) {
		if (OM.arma != 2) {

			camera->TranslateForward(3.0f);
			
		}
		OM.arma = 2;
		
	}
		

	if (key == GLFW_KEY_3) {
		if (OM.arma == 2) {

		}
		OM.arma = 3;
	}
		

	if (key == GLFW_KEY_P)
		projectionMatrix = glm::perspective(RADIANS(60), window->props.aspectRatio, 0.01f, 200.0f);

	
	if (key == GLFW_KEY_O)
		projectionMatrix = glm::ortho(-5.0f, 5.0f, -5.0f, 5.0f, 0.0f , 200.0f);
}

void Laborator5::OnKeyRelease(int key, int mods)
{
	// add key release event
}

void Laborator5::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{

	if (window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT))
	{
		float sensivityOX = 0.001f;
		float sensivityOY = 0.001f;

		
		renderCameraTarget = true; 
				
		if (OM.arma == 2) {
			camera->RotateFirstPerson_OX(-deltaX * sensivityOX);
			camera->RotateThirdPerson_OY(-deltaY * sensivityOY);
		}
		else {
			camera->RotateThirdPerson_OX(-deltaX * sensivityOX);
			camera->RotateThirdPerson_OY(-deltaY * sensivityOY);
		}
		
		unghiOm = unghiOm + (-1) * deltaX * sensivityOY ;

	}
}

void Laborator5::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
	// add mouse button press event
	int clickLeft = 1;

	if (button == clickLeft) {
		
		// Atunci creez un nou proiectil
		Proiectil obj;

		if (OM.arma == 1 && aparitiePistol >= 1.0f) {

			aparitiePistol = 0.0f;
			obj.nume = "pistol";
			obj.viteza = 9.5f;
			obj.distanta = 12.0f;
			obj.forward = camera->forward;
			obj.position = camera->position;

			glm::vec3 pozitionarea = glm::normalize(obj.forward);
			obj.position += pozitionarea * 2.2f;
			vectorProiectile.push_back(obj);
		}
		else if (OM.arma == 2 && aparitiePusca >= 2.0f) {
			aparitiePusca = 0.0f;
			obj.nume = "pusca";
			obj.viteza = 15.0f;
			obj.distanta = 20.0f;
			camera->TranslateForward(-5.0f);
			obj.forward = camera->forward;
			obj.position = camera->position;
			camera->TranslateForward(5.0f);

			glm::vec3 pozitionarea = glm::normalize(obj.forward);
			obj.position += pozitionarea * 2.2f;
			vectorProiectile.push_back(obj);
		}
		else if(OM.arma == 3 && aparitieGrenada >= 3.0f){
			aparitieGrenada = 0.0f;
			obj.nume = "grenada";
			obj.viteza = 2.0f;
			obj.distanta = 6.0f;
			obj.orientareGrenada = 0.3f;
			obj.timpGrenada = 0.0f;
			obj.forward = camera->forward;
			obj.position = camera->position;

			glm::vec3 pozitionarea = glm::normalize(obj.forward);
			obj.position += pozitionarea * 2.2f;
			vectorProiectile.push_back(obj);
		}

		if (obj.nume == "grenada") {
			numarGrenade = numarGrenade + 1;
		}
		
	}

}

void Laborator5::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
	// add mouse button release event
}

void Laborator5::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}

void Laborator5::OnWindowResize(int width, int height)
{
}
