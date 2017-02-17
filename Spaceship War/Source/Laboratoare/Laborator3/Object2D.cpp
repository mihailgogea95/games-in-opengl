#include "Object2D.h"

#include <Core/Engine.h>

Mesh* Object2D::CreateSquare(std::string name, glm::vec3 leftBottomCorner, float length, glm::vec3 color, int tip)
{


	// Tipul de nava proprie 0
	glm::vec3 corner = leftBottomCorner;

	std::vector<VertexFormat> vertices =
	{
		VertexFormat(corner, color),
		VertexFormat(corner + glm::vec3(length, -length, 0), color),
		VertexFormat(corner + glm::vec3(length, length, 0), color),
		VertexFormat(corner + glm::vec3(-length, length, 0), color),
		VertexFormat(corner + glm::vec3(-length , -length , 0), color)
	};

	Mesh* square = new Mesh(name);
	std::vector<unsigned short> indices = { 0, 1, 2, 4, 0, 3 };
	
	
	// Tipul de proiectil 1
	std::vector<VertexFormat> verticesProiectil =
	{
		VertexFormat(corner, color),
		VertexFormat(corner + glm::vec3(length, -length, 0), color),
		VertexFormat(corner + glm::vec3(length, length, 0), color),
		VertexFormat(corner + glm::vec3(-length, length, 0), color),
		VertexFormat(corner + glm::vec3(-length , -length , 0), color)
	};

	std::vector<unsigned short> indicesProiectil = { 0, 1, 2, 4, 0, 3, 0, 2, 3, 0, 4, 1 };


	// Scorul din dreapta in forma de dreptunghi
	std::vector<VertexFormat> verticesScor =
	{
		VertexFormat(corner, color),
		VertexFormat(corner + glm::vec3(length/2, 0, 0), color),
		VertexFormat(corner + glm::vec3(length/2, length, 0), color),
		VertexFormat(corner + glm::vec3(0, length, 0), color)
	};

	
	std::vector<unsigned short> indicesScor = { 0, 1, 2, 3, 0, 2 };

	
	// In functie de ultimul argument voi crea obiectul cerut
	if (tip == 0) {
		square->InitFromData(vertices, indices);
	}
	else if (tip == 1) {
		square->InitFromData(verticesProiectil, indicesProiectil);
	}
	else if (tip == 2) {
		square->InitFromData(verticesScor, indicesScor);
	}
	
	return square;
}