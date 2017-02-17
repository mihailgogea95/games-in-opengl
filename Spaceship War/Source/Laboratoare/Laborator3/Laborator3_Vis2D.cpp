#include "Laborator3_Vis2D.h"

#include <vector>
#include <iostream>
#include <ctime>

#include <Core/Engine.h>
#include "Transform2D.h"
#include "Object2D.h"
#include <math.h>
#define PI 3.14159265

// Gogea Mihail 334CC

using namespace std;

float navaX = 3.0f, navaY = 3.0f;
float cx, cy, angularStep;
float mX, mY; // Cordonatele mousului
int vieti = 3; // Vietile navei
float stingereEcran = 0; // Lumina rosie de la sfarsit

float distantaMinima = 0.01f;
int vitezaProiectil = 8; // viteza proiectilului
float timpAparitie = 2; //Maxim timp de aparitie
float minimTimpAparitie = 0.5f;

//Timpul pentru inamicii
float timp = 0;

float tempx1, tempy1, tempx2, tempy2; // Variabile auxiliare ce vor fi necesare pe parcurs

int numarInamiciMorti = 0; // retin numarul de inamici omorati


// Clasa construita pentru obiectele ce vor aparea pe ecran ( nave si proiectile)
class GameObject {

public:
	std::string nume;		// retin tipul obiectului
	float curentX, curentY, panta, semn; // retin pozitia , panta traiectoriei si semnu traiectoriei
	float viteza;	
	float scaleX;	
	float scaleY;
	float scaleColor;
	int protectie;
	float rotireProtectie;
	int dublare;
};


void AppendToVec(vector<GameObject> & vec, vector<GameObject>::iterator & i, GameObject obj) {
	const size_t diff = i - vec.begin();
	vec.push_back(obj);
	i = vec.begin() + diff;
}

bool joc = true;
float deltaTimeSeconds1;

std::vector<GameObject> vectorProiectil;
std::vector<GameObject> vectorInamici;

Laborator3_Vis2D::Laborator3_Vis2D()
{
}

Laborator3_Vis2D::~Laborator3_Vis2D()
{
}

void Laborator3_Vis2D::Init()
{
	camera->SetPosition(glm::vec3(0, 0, 50));
	camera->SetRotation(glm::vec3(0, 0, 0));
	camera->Update();
	cameraInput->SetActive(false);

	logicSpace.x = 0;		// logic x
	logicSpace.y = 0;		// logic y
	logicSpace.width = 6;	// logic width
	logicSpace.height = 6;	// logic height

	// initialize angularStep
	 angularStep = 0;

	 //Initializez generatorul de numere random
	 srand(time( NULL ));


	glm::vec3 corner = glm::vec3(0.001, 0.001, 0);
	length = 0.18f;				// Marimea navei mele
	lengthProiectil = 0.05f;	// Marimea proiectilului
	lengthInamic1 = length ;	// Marimea Inamicului de tip 1
	lengthInamic2 = length * 1.6;	// Marimea inamicului de tip2
	lengthInamic3 = 0.20f; // distanta navainamic 3 si protectia
	lengthVIATA = 0.16f; // dimensiune bonus viata

	// compute coordinates of square center
	cx = corner.x ;
	cy = corner.y;

	// Adaug nava principala
	Mesh* square1 = Object2D::CreateSquare("square1", corner, length, glm::vec3(1, 0.5f, 0),0);
	AddMeshToList(square1);

	// Adaug tipul inamicului1
	Mesh* inamic1 = Object2D::CreateSquare("inamic1", corner, lengthInamic1, glm::vec3(0, 0, 1), 0);
	AddMeshToList(inamic1);

	// Adaug tipul inamicului2
	Mesh* inamic2 = Object2D::CreateSquare("inamic2", corner, lengthInamic2, glm::vec3(1, 1, 0), 0);
	AddMeshToList(inamic2);

	// Adaug tipul inamicului2 dupa ce este lovit o data
	Mesh* inamic2Mort = Object2D::CreateSquare("inamic2Mort", corner, lengthInamic2, glm::vec3(1, 0, 0), 0);
	AddMeshToList(inamic2Mort);

	// Adaug tipul de proiectil
	Mesh* proiectil = Object2D::CreateSquare("proiectil", corner, lengthProiectil, glm::vec3(1, 1, 0), 1);
	AddMeshToList(proiectil);

	// Adaug tipul de dreptunghi pentru a afisa numarul de vieti
	Mesh* dreptunghi = Object2D::CreateSquare("dreptunghi", corner, 0.2f, glm::vec3(1, 1, 0), 2);
	AddMeshToList(dreptunghi);

	// Adaug tipul de protectie
	Mesh* protectie = Object2D::CreateSquare("protectie", corner, 0.04f, glm::vec3(0.9f, 0.65f, 0.47f), 1);
	AddMeshToList(protectie);

	// Adaug tipul inamicului3
	Mesh* inamic3 = Object2D::CreateSquare("inamic3", corner, lengthInamic1, glm::vec3(0, 0, 1), 0);
	AddMeshToList(inamic3);

	// Adaug tipul inamicului4
	Mesh* inamic4 = Object2D::CreateSquare("inamic4", corner, lengthInamic2, glm::vec3(1, 0.7f, 0.6f), 0);
	AddMeshToList(inamic4);

	// Adaug tipul de proiectil
	Mesh* bonus = Object2D::CreateSquare("bonus", corner, lengthVIATA, glm::vec3(1, 0.4f, 0.7f), 1);
	AddMeshToList(bonus);

}

// 2D visualization matrix
glm::mat3 Laborator3_Vis2D::VisualizationTransf2D(const LogicSpace & logicSpace, const ViewportSpace & viewSpace)
{
	float sx, sy, tx, ty;
	sx = viewSpace.width / logicSpace.width;
	sy = viewSpace.height / logicSpace.height;
	tx = viewSpace.x - sx * logicSpace.x;
	ty = viewSpace.y - sy * logicSpace.y;

	return glm::transpose(glm::mat3(
		sx, 0.0f, tx,
		0.0f, sy, ty,
		0.0f, 0.0f, 1.0f));
}

// uniform 2D visualization matrix (same scale factor on x and y axes)
glm::mat3 Laborator3_Vis2D::VisualizationTransf2DUnif(const LogicSpace & logicSpace, const ViewportSpace & viewSpace)
{
	float sx, sy, tx, ty, smin;
	sx = viewSpace.width / logicSpace.width;
	sy = viewSpace.height / logicSpace.height;
	if (sx < sy)
		smin = sx;
	else
		smin = sy;
	tx = viewSpace.x - smin * logicSpace.x + (viewSpace.width - smin * logicSpace.width) / 2;
	ty = viewSpace.y - smin * logicSpace.y + (viewSpace.height - smin * logicSpace.height) / 2;

	return glm::transpose(glm::mat3(
		smin, 0.0f, tx,
		0.0f, smin, ty,
		0.0f, 0.0f, 1.0f));
}

void Laborator3_Vis2D::SetViewportArea(const ViewportSpace & viewSpace, glm::vec3 colorColor, bool clear)
{
	glViewport(viewSpace.x, viewSpace.y, viewSpace.width, viewSpace.height);

	glEnable(GL_SCISSOR_TEST);
	glScissor(viewSpace.x, viewSpace.y, viewSpace.width, viewSpace.height);

	// Clears the color buffer (using the previously set color) and depth buffer
	glClearColor(colorColor.r, colorColor.g, colorColor.b, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_SCISSOR_TEST);

	camera->SetOrthographic((float)viewSpace.x, (float)(viewSpace.x + viewSpace.width), (float)viewSpace.y, (float)(viewSpace.y + viewSpace.height), 0.1f, 400);
	camera->Update();
}

void Laborator3_Vis2D::FrameStart()
{
	// Clears the color buffer (using the previously set color) and depth buffer
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

}

void Laborator3_Vis2D::Update(float deltaTimeSeconds)
{

	glm::ivec2 resolution = window->GetResolution();

	// Sets the screen area where to draw - the left half of the window
	viewSpace = ViewportSpace(0, 0, resolution.x, resolution.y);
	
	// Pentru animatia de la sfarsit in care se face ecranul rosu
	if (vieti == 0) {
		if (stingereEcran < 1) {
			stingereEcran += deltaTimeSeconds;
		}
		else {
			stingereEcran = 1;
		}
	}
	
	SetViewportArea(viewSpace, glm::vec3(stingereEcran,0,0), true);
	
	// Compute the 2D visualization matrix
	visMatrix = glm::mat3(1);
	visMatrix *= VisualizationTransf2D(logicSpace, viewSpace);

	DrawScene(visMatrix, deltaTimeSeconds);

	// Aparitia inamiciilor pe ecran
	timp += deltaTimeSeconds;

	if (timp > timpAparitie && vieti != 0) {
		
		if (timp > minimTimpAparitie) {
			
			timpAparitie -= deltaTimeSeconds*3;
		}
		
		GameObject navaInamic;

		// Introduc pe ecran , random tipuri de inamici
		int random = (rand() % 4);

		if (random == 0) {
			navaInamic.nume = "tip1";
		}
		else if(random == 1){
			navaInamic.nume = "tip2";
		}
		else if (random == 2) {
			navaInamic.nume = "tip4";
			navaInamic.dublare = 1;
		}
		else {
			navaInamic.nume = "tip3";
			navaInamic.protectie = 1;
			navaInamic.rotireProtectie = 1;
		}
		
		// Viteza noului inamic
		navaInamic.viteza = rand() % 7 + 3;

		//alege random pozitia inamicului pe cordonata X
		navaInamic.curentX = ((float)rand() / 32767) * logicSpace.width;
		float pozitieY = sqrtf(pow(logicSpace.width / 2, 2) - pow(navaInamic.curentX - logicSpace.width / 2, 2)) ;

		if ((rand() % 2) == 0) {
			navaInamic.curentY = pozitieY + logicSpace.height / 2;
		}
		else {
			navaInamic.curentY = (-1) * pozitieY + logicSpace.height / 2;
		}
		
		// Introduc noul inamic in vectoru cu inamicii
		vectorInamici.push_back(navaInamic);

		//Setez timpul din nou la 0
		timp = 0;
	}
}

void Laborator3_Vis2D::FrameEnd()
{

}

void Laborator3_Vis2D::DrawScene(glm::mat3 visMatrix , float deltaTimeSeconds)
{
	// Pentru a bloca toate obiectele pe ecran la sfarsitul jocului.
	if (vieti == 0) {
		deltaTimeSeconds1 = 0;
	}
	else {
		deltaTimeSeconds1 = deltaTimeSeconds;
	}
	
	modelMatrix = visMatrix * Transform2D::Translate(navaX, navaY);
	
	float panta; // panta navei
	panta = (float)((float)(mY - navaY) / (float)(mX - navaX)); // panta navei in functie de pozitionarea mausului
	
	float unghi;
	unghi = atan(panta) + PI/2; // calculez unghiul in radiani
	//Pentru a roti nava in functie de maus si cand nu mai sunt vieti sa ramana blocat pe ecran
	if(vieti != 0){
		modelMatrix *= Transform2D::Translate(cx, cy);
		modelMatrix *= Transform2D::Rotate(unghi);
		modelMatrix *= Transform2D::Translate(-cx, -cy);
	}
	
	// Desenez nava principala pe ecran.
	RenderMesh2D(meshes["square1"], shaders["VertexColor"], modelMatrix);

	// Adaug scorul in dreapta sus care poate fii maxim 3.
	if (vieti != 0) {
		if (vieti == 3) {
			
			modelMatrix = visMatrix * Transform2D::Translate((float)(logicSpace.width - 1.0f), (float)(logicSpace.height - 0.4f));
			RenderMesh2D(meshes["dreptunghi"], shaders["VertexColor"], modelMatrix);
			modelMatrix = visMatrix * Transform2D::Translate((float)(logicSpace.width - 0.8f), (float)(logicSpace.height - 0.4f));
			RenderMesh2D(meshes["dreptunghi"], shaders["VertexColor"], modelMatrix);
			modelMatrix = visMatrix * Transform2D::Translate((float)(logicSpace.width - 0.6f), (float)(logicSpace.height - 0.4f));
			RenderMesh2D(meshes["dreptunghi"], shaders["VertexColor"], modelMatrix);
		}else if (vieti == 2) {

			modelMatrix = visMatrix * Transform2D::Translate((float)(logicSpace.width - 0.8f), (float)(logicSpace.height - 0.4f));
			RenderMesh2D(meshes["dreptunghi"], shaders["VertexColor"], modelMatrix);
			modelMatrix = visMatrix * Transform2D::Translate((float)(logicSpace.width - 0.6f), (float)(logicSpace.height - 0.4f));
			RenderMesh2D(meshes["dreptunghi"], shaders["VertexColor"], modelMatrix);
		}else if (vieti == 1) {

			modelMatrix = visMatrix * Transform2D::Translate((float)(logicSpace.width - 0.6f), (float)(logicSpace.height - 0.4f));
			RenderMesh2D(meshes["dreptunghi"], shaders["VertexColor"], modelMatrix);
		}
		
	}

	//Adaug inamicii
	std::vector<GameObject>::iterator it1; // Iterator pentru inamici
	std::vector<GameObject>::iterator itProiectil; // Iterator pentru proiectil
	
	for (it1 = vectorInamici.begin(); it1 != vectorInamici.end();) {

		// Scalarea unei nave de tip2 cand este lovita odata si trebuie sa scada in marime
		if ((*it1).nume == "tip2Mort") {
			if ((*it1).scaleX > 0.5f) {
				(*it1).scaleX -= deltaTimeSeconds1 * 1.5f ;
				(*it1).scaleY -= deltaTimeSeconds1 * 1.5f;
			}
		}
		

		int esteMort = 0;
		for (itProiectil = vectorProiectil.begin(); itProiectil != vectorProiectil.end();) {

			// Calculez cu formula distantei , distanta dintre proiectil si inamic
			float distanta = sqrtf(pow((*itProiectil).curentX - (*it1).curentX, 2) + pow((*itProiectil).curentY - (*it1).curentY, 2));

			if (distanta < lengthInamic1 && (((*it1).nume == "tip1") || ((*it1).nume == "tip2Mort") || (((*it1).nume == "tip3") && ((*it1).protectie == 0)) ) ) {

				numarInamiciMorti++;
				cout << "Score: " << numarInamiciMorti << endl;
				itProiectil = vectorProiectil.erase(itProiectil);
				
				// Pentru bonusul de viata
				if (numarInamiciMorti % 5 == 0 && vieti < 3) {
					(*it1).nume = "VIATA";
				}
				else {
					esteMort = 1;  // Setez ca mort pentru a sti sa nu mai continui mai departe
				}

			}else if (distanta < lengthInamic2 && (*it1).nume == "tip2") {
				
				numarInamiciMorti++;
				cout <<"Score: " <<  numarInamiciMorti << endl;
				// Sterg inamicul de tip2 , urmand ca apoi sa apara in locul lui un inamic de tip1 rosu,
				// deci ii schimb tipul , dublandui si viteza.
				itProiectil = vectorProiectil.erase(itProiectil);
				(*it1).nume = "tip2Mort";
				(*it1).scaleX = 1;
				(*it1).scaleY = 1;
				(*it1).viteza *= 2;
			}
			else if (distanta < lengthInamic2 && (*it1).nume == "tip4") {		// Tipul 4 este cel cand moare se va transforma in 2 inamici mai mici.
				numarInamiciMorti++;
				cout << "Score: " << numarInamiciMorti << endl;
				itProiectil = vectorProiectil.erase(itProiectil);
				(*it1).nume = "tip1";

				GameObject navaInamic;

				navaInamic.nume = "tip1";
				navaInamic.curentX = (*it1).curentX - lengthInamic2;
				navaInamic.curentY = (*it1).curentY - lengthInamic2;
				navaInamic.viteza = (*it1).viteza;

				AppendToVec(vectorInamici, it1, navaInamic);
				

				(*it1).curentX += lengthInamic2;
				(*it1).curentY += lengthInamic2;

				 

			}
			else if (distanta < lengthInamic3 && (*it1).nume == "tip3" && ((*it1).protectie == 1)) {		// Acest tip este cel cu protectie
				itProiectil = vectorProiectil.erase(itProiectil);
				(*it1).protectie = 0;
			}
			else {
				++itProiectil; // trec la urmatorul proiectil
			}
		}
		// Daca este mort il voi scoate din vectoru cu inamici ca sa nu il mai desenez pe ecran
		if (esteMort == 1) {
			it1 = vectorInamici.erase(it1);
			continue; // Continui iteratile
		}

		// Calculez distanta dintre nava si inamic
		float distNavIn = sqrtf(pow((*it1).curentX - navaX, 2) + pow((*it1).curentY - navaY, 2));

		

		if ((distNavIn > lengthInamic1 && (((*it1).nume == "tip1") || ((*it1).nume == "VIATA") || ((*it1).nume == "tip2Mort"))) || ((distNavIn > lengthInamic3) && ((*it1).nume == "tip3") && ((*it1).protectie == 0)) ||
				(distNavIn > lengthInamic2 && ((*it1).nume == "tip2" || (*it1).nume == "tip4")) || (distNavIn > (1.7*lengthInamic3 + lengthInamic1) && (*it1).nume == "tip3" && ((*it1).protectie == 1))) {
				
			unghi = asin((navaY - (*it1).curentY) / distNavIn);
			unghi += PI/2;

			// Pentru fiecare inamic ii calculez traiectoria X si Y
			if ((*it1).nume != "VIATA") {
				if (navaX - (*it1).curentX < 0) {
					unghi = (-1) * unghi;
				}

				if (abs((*it1).curentY - navaY) > distantaMinima) {

					if ((*it1).curentY > navaY) {

						(*it1).curentY -= (deltaTimeSeconds1 * (*it1).viteza) / 8;
					}
					else {
						(*it1).curentY += (deltaTimeSeconds1 * (*it1).viteza) / 8;
					}
				}

				if (abs((*it1).curentX - navaX) > distantaMinima) {

					if ((*it1).curentX > navaX) {

						(*it1).curentX -= (deltaTimeSeconds1 * (*it1).viteza) / 8;
					}
					else {
						(*it1).curentX += (deltaTimeSeconds1 * (*it1).viteza) / 8;
					}
				}
			}
			

			// Desenez pe ecran inamicii 
			modelMatrix = visMatrix * Transform2D::Translate((*it1).curentX, (*it1).curentY);
			modelMatrix *= Transform2D::Translate(cx, cy);
			if ((*it1).nume == "VIATA") {
				modelMatrix *= Transform2D::Rotate(135 * Engine::GetElapsedTime() );			// Rotirea bonusului de viata
			}
			else {
				modelMatrix *= Transform2D::Rotate(unghi);
			}
			
			modelMatrix *= Transform2D::Translate(-cx, -cy);

			if ((*it1).nume == "tip2Mort") {					// Scalarea atunci cand inamicul de tip2 moare si devine mai mic
				modelMatrix *= Transform2D::Translate(cx, cy);
				modelMatrix *= Transform2D::Scale((*it1).scaleX, (*it1).scaleY);
				modelMatrix *= Transform2D::Translate(-cx, -cy);
			}

			if ((*it1).nume == "tip1") {
				RenderMesh2D(meshes["inamic1"], shaders["VertexColor"], modelMatrix);
			}else if ((*it1).nume == "tip2Mort") {
						
				RenderMesh2D(meshes["inamic2Mort"], shaders["VertexColor"], modelMatrix);
			}
			else if((*it1).nume == "tip4"){
				RenderMesh2D(meshes["inamic4"], shaders["VertexColor"], modelMatrix);
			}
			else if ((*it1).nume == "VIATA") {
				RenderMesh2D(meshes["bonus"], shaders["VertexColor"], modelMatrix);

			}else if ((*it1).nume == "tip3") {									// Rotirea protectiei
				(*it1).rotireProtectie += 20 * deltaTimeSeconds;
				
				if ((*it1).rotireProtectie >= 360) {
					(*it1).rotireProtectie -= 360;
					
				}
				// Daca protectia este activata atunci voi desena 4 patrate rotinduse cu o viteza mare in jurul navei de tip3.
				if ((*it1).protectie == 1 && vieti != 0) {
					RenderMesh2D(meshes["inamic1"], shaders["VertexColor"], modelMatrix);

					modelMatrix *= Transform2D::Translate(cx, cy);
					modelMatrix *= Transform2D::Rotate((*it1).rotireProtectie);
					modelMatrix *= Transform2D::Translate(lengthInamic3, lengthInamic3);
					modelMatrix *= Transform2D::Translate(-cx, -cy);
					RenderMesh2D(meshes["protectie"], shaders["VertexColor"], modelMatrix);

					modelMatrix *= Transform2D::Translate(cx, cy);
					modelMatrix *= Transform2D::Translate(-2 * lengthInamic3, -2 * lengthInamic3);
					RenderMesh2D(meshes["protectie"], shaders["VertexColor"], modelMatrix);

					modelMatrix *= Transform2D::Translate(0, 2* lengthInamic3);
					RenderMesh2D(meshes["protectie"], shaders["VertexColor"], modelMatrix);

					modelMatrix *= Transform2D::Translate(2* lengthInamic3, -2 * lengthInamic3);
					RenderMesh2D(meshes["protectie"], shaders["VertexColor"], modelMatrix);
				}
				else {
					RenderMesh2D(meshes["inamic1"], shaders["VertexColor"], modelMatrix);
				}
				

			}
			else {
						RenderMesh2D(meshes["inamic2"], shaders["VertexColor"], modelMatrix);
			}
					
			++it1;

		}else if((*it1).nume == "VIATA" && distNavIn < lengthVIATA) {

			if(vieti < 3)
				vieti++;
			it1 = vectorInamici.erase(it1);		// Sterg inamicul insemnand ca sa ciocnit cu nava
			
		}
		else {
			if ((*it1).nume == "VIATA") {
				it1 = vectorInamici.erase(it1);
				if (vieti < 3) {
					vieti++;
				}
			}
			else {
				it1 = vectorInamici.erase(it1);		// Sterg inamicul insemnand ca sa ciocnit cu nava
				vieti--;
			}
			
		}
	}
		
	glm::ivec2 resolution = window->GetResolution();
	// Creez proiectilele din originea navei.
	// Adaug proiectilele (trag cu proiectile) si directile lor pe OX si OY
	std::vector<GameObject>::iterator it;
	for (it = vectorProiectil.begin(); it != vectorProiectil.end();) {

		if ((*it).curentY > 0 && (*it).curentY < resolution.y && (*it).curentX > 0 && (*it).curentX < resolution.x) {	// Ma asigur ca sunt in spatiul logic
			
			// Desenez proiectilele pe ecran 
			modelMatrix = visMatrix * Transform2D::Translate((*it).curentX , (*it).curentY );
			modelMatrix *= Transform2D::Translate(cx, cy);
			modelMatrix *= Transform2D::Rotate(atan((float)(*it).panta));
			modelMatrix *= Transform2D::Translate(-cx, -cy);

			RenderMesh2D(meshes["proiectil"], shaders["VertexColor"], modelMatrix);
			
			// Le calculez traiectoria cu ajutorul pantei
			float parteIntreaga = abs((*it).panta);
			float oldValue, dist;

			if (parteIntreaga < 1) {
				oldValue = (*it).curentX;
				(*it).curentX += deltaTimeSeconds1 * (*it).semn * vitezaProiectil;
				dist = (*it).curentX - oldValue ;
				(*it).curentY = (*it).panta * dist + (*it).curentY;
			}
			else {
				oldValue = (*it).curentY;
				(*it).curentY += deltaTimeSeconds1 * (*it).semn * vitezaProiectil;
				dist = (*it).curentY - oldValue ;
				(*it).curentX = dist / (*it).panta + (*it).curentX;
			}
			++it;
		}
		else {
			it = vectorProiectil.erase(it);
		}
	}
	
	
	
	
}

void Laborator3_Vis2D::OnInputUpdate(float deltaTime, int mods)
{

	if (vieti == 0) {
		deltaTimeSeconds1 = 0;
	}
	else {
		deltaTimeSeconds1 = deltaTime;
	}

	if (KEY_W == true) {

		navaY += deltaTimeSeconds1 * 2.5;
	}
	if (KEY_A == true) {
		navaX -= deltaTimeSeconds1 * 2.5;
	}
	if (KEY_S == true) {
		navaY -= deltaTimeSeconds1 * 2.5;
	}
	if (KEY_D == true) {
		navaX += deltaTimeSeconds1 * 2.5;
	}
	
}

void Laborator3_Vis2D::OnKeyPress(int key, int mods)
{
	if (key == GLFW_KEY_W) {
		KEY_W = true;
	}
	if (key == GLFW_KEY_A) {
		KEY_A = true;
	}
	if (key == GLFW_KEY_S) {
		KEY_S = true;
	}
	if (key == GLFW_KEY_D) {
		KEY_D = true;
	}
	
};

void Laborator3_Vis2D::OnKeyRelease(int key, int mods)
{
	if (key == GLFW_KEY_W) {
		KEY_W = false;
	}
	if (key == GLFW_KEY_A) {
		KEY_A = false;
	}
	if (key == GLFW_KEY_S) {
		KEY_S = false;
	}
	if (key == GLFW_KEY_D) {
		KEY_D = false;
	}
	
};

void Laborator3_Vis2D::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
	// Iau cordonatele mausului si le modific astfel incat sa fie o valoare in functie de logicSpace
	glm::ivec2 resolution = window->GetResolution();
	mX =((float)mouseX / resolution.x) * logicSpace.width;
	mY = logicSpace.height -  ((float)mouseY / resolution.y) * logicSpace.height;

};

void Laborator3_Vis2D::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
	// add mouse button press event

	glm::ivec2 resolution = window->GetResolution();

	// Creez un nou proiectil
	GameObject temp;

	// Ma asigur ca mausul a fost apasat in interiorul ferestrei, iar cordonatei
	// Y trebuie sa o scad din rezolutia(Y) deoarece este invers preluata (adica jos)
	if (mouseX < resolution.x && (resolution.y - mouseY) < resolution.y) {
		tempx2 = ((float)mouseX / resolution.x) * logicSpace.width;
		tempy2 = logicSpace.height - ((float)mouseY / resolution.y) * logicSpace.height;
	}
	float diferentaOY = navaY - tempy2;
	float diferentaOX = navaX - tempx2;

	float panta = diferentaOY / diferentaOX;

	// Pozitile curente
	temp.curentX = navaX;
	temp.curentY = navaY;

	temp.panta = panta;

	// Partea intreaga a pantei
	float pantaIntreaga = abs(panta);

	if (tempy2 > navaY && pantaIntreaga > 1) {
		temp.semn = 1;
	}else if(tempx2 > navaX && pantaIntreaga < 1){
		temp.semn = 1;
	}
	else {
		temp.semn = -1;
	}

	vectorProiectil.push_back(temp);
};

void Laborator3_Vis2D::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
	// add mouse button release event
}

void Laborator3_Vis2D::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}