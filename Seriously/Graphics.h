#pragma once

//Include all the header files or some shit?=

#include "GLBuffer.h"
#include "PrimitiveBatch.h"
#include "Shader.h"
#include "Texture.h"
#include "VertexArray.h"
#include "vec2.hpp"
#include "vec4.hpp"

class Graphics {
private:
	struct Vertex {
		glm::vec2 Postion;
		glm::vec2 TexCoord;
		glm::vec2 RotationOrigin;
		glm::vec4 Color;
		float Rotation;
		float TextureSlot;
	};

	PrimitiveBatch<Vertex>* batch;
	Shader* shader;
	VertexArray vertexArray;

public:
	Graphics()
	{
		
		//Position
		vertexArray.Add<float>(2);
		//TexCoord
		vertexArray.Add<float>(2);
		//Rotation Origin
		vertexArray.Add<float>(2);
		//Color
		vertexArray.Add<float>(4);
		//Rotation
		vertexArray.Add<float>(1);
		//TextureSlot
		vertexArray.Add<float>(1);
	}

	~Graphics() {
		delete shader;
		delete batch;
	}
};