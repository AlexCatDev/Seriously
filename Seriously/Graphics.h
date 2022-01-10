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
		glm::vec2 Position;
		glm::vec2 TexCoord;
		glm::vec2 RotationOrigin;
		glm::vec4 Color;
		float Rotation;
		float TextureSlot;
	};

	Shader* shader;
	VertexArray vertexArray;

	std::map<Texture*, int> lol;

	int bindSlotIndex = 0;

	//hva
	int getTextureSlot(const Texture* texture) {
		if (texture == nullptr)
			texture = &Texture::Square;

		auto p = lol.find((Texture*)texture);
		
		//Not in the list
		if (p == lol.end()) {
			int slotToAdd = bindSlotIndex;

			bindSlotIndex++;

			lol.insert(std::pair<Texture*, int>((Texture*)texture, slotToAdd));

			return slotToAdd;
		}

		return p->second;
	}

	void convertToRadians(float& degrees) {
		#define PI 3.141592653589793238463

		degrees = degrees * 0.017453292519943295769236907684886;
		//degrees = (degrees * PI) / 180;
	}

	void mapRange(float& value, float fromSource, float toSource, float fromTarget, float toTarget) {
		value = (value - fromSource) / (toSource - fromSource) * (toTarget - fromTarget) + fromTarget;
	}

	float mapRange(float value, float fromSource, float toSource, float fromTarget, float toTarget) {
		return value = (value - fromSource) / (toSource - fromSource) * (toTarget - fromTarget) + fromTarget;
	}

#define mapR(dest, valueSource, fromSource, toSource, fromTarget, toTarget) dest = (valueSource - fromSource) / (toSource - fromSource) * (toTarget - fromTarget) + fromTarget

public:
	PrimitiveBatch<Vertex>* Batcher;
	glm::mat4 Projection;

	Graphics()
	{
		Batcher = new PrimitiveBatch<Vertex>(400000, 600000);
		//shader = new Shader("Shaders/Default.vert", "Shaders/Default.frag");
		shader = new Shader("C:\\Users\\user\\Desktop\\Default.vert", "C:\\Users\\user\\Desktop\\Default.frag");
		
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

	void DrawRectangle(glm::vec2& position, glm::vec2& size, glm::vec4& color, const Texture* const texture, glm::vec4 textureRect, bool uvNormalized = true, float rotation = 0) {
		#define texWidth textureRect.z
		#define texHeight textureRect.w

		auto quad = Batcher->WriteQuad();

		auto textureSlot = getTextureSlot(texture);

		glm::vec2 rotationOrigin = position;

		quad[0].Position = position;
		quad[0].Rotation = rotation;
		quad[0].TextureSlot = textureSlot;
		quad[0].Color = color;
		quad[0].RotationOrigin = rotationOrigin;
		quad[0].TexCoord = glm::vec2(textureRect.x, textureRect.y);

		quad[1].Position = glm::vec2(position.x + size.x, position.y);
		quad[1].Rotation = rotation;
		quad[1].TextureSlot = textureSlot;
		quad[1].Color = color;
		quad[1].RotationOrigin = rotationOrigin;
		quad[1].TexCoord = glm::vec2(textureRect.x + texWidth, textureRect.y);

		quad[2].Position = position + size;
		quad[2].Rotation = rotation;
		quad[2].TextureSlot = textureSlot;
		quad[2].Color = color;
		quad[2].RotationOrigin = rotationOrigin;
		quad[2].TexCoord = glm::vec2(textureRect.x + texWidth, textureRect.y + texHeight);

		quad[3].Position = glm::vec2(position.x, position.y + size.y);
		quad[3].Rotation = rotation;
		quad[3].TextureSlot = textureSlot;
		quad[3].Color = color;
		quad[3].RotationOrigin = rotationOrigin;
		quad[3].TexCoord = glm::vec2(textureRect.x, textureRect.y + texHeight);
	}

	void DrawEllipse(glm::vec2& position, float startAngle, float endAngle, float outerRadius, float innerRadius, glm::vec4& color, const Texture* texture = nullptr, int segments = 50, bool wrapUV = true, const glm::vec4* textureRect = nullptr)
	{
#define texX textureRect->x
#define texY textureRect->y
#define texWidth textureRect->z
#define texHeight textureRect->w

		convertToRadians(startAngle);
		convertToRadians(endAngle);

		int slot = getTextureSlot(texture);

		glm::vec2 first, second;

		glm::vec2 firstUV{ 0,0 }, secondUV{ 1, 1};

		float theta = startAngle;
		float stepTheta = (endAngle - startAngle) / (segments - 1);

		float cos, sin;
		
		int vertexCount = segments * 2;

		auto vertices = Batcher->WriteTriangleStrip(vertexCount);

		for (int i = 0; i < vertexCount; i++)
		{
			cos = std::cosf(theta);
			sin = std::sinf(theta);

			first.x = position.x + cos * outerRadius;
			first.y = position.y + sin * outerRadius;

			second.x = position.x + cos * innerRadius;
			second.y = position.y + sin * innerRadius;

			if (wrapUV)
			{
				if (textureRect != nullptr)
				{
					firstUV.x = mapRange(cos * outerRadius, -outerRadius, outerRadius, texX, texX + texWidth);
					firstUV.y = mapRange(sin * outerRadius, -outerRadius, outerRadius, texY, texY + texHeight);

					secondUV.x = mapRange(cos * innerRadius, -outerRadius, outerRadius, texX, texX + texWidth);
					secondUV.y = mapRange(sin * innerRadius, -outerRadius, outerRadius, texY, texY + texHeight);
				}
				else
				{
					/*
					mapR(firstUV.x, cos * outerRadius, 0, outerRadius, 0.5f, 1);
					mapR(firstUV.y, sin * outerRadius, 0, outerRadius, 0.5f, 1);

					mapR(secondUV.x, cos * innerRadius, 0, outerRadius, 0.5f, 1);
					mapR(secondUV.y, sin * innerRadius, 0, outerRadius, 0.5f, 1);
					*/
					
					firstUV.x = mapRange(cos * outerRadius, 0, outerRadius, 0.5f, 1);
					firstUV.y = mapRange(sin * outerRadius, 0, outerRadius, 0.5f, 1);

					secondUV.x = mapRange(cos * innerRadius, 0, outerRadius, 0.5f, 1);
					secondUV.y = mapRange(sin * innerRadius, 0, outerRadius, 0.5f, 1);
					
				}
			}

			vertices[i].Position = first;
			vertices[i].Color = color;
			vertices[i].TextureSlot = slot;
			vertices[i].Rotation = 0;
			vertices[i].TexCoord = firstUV;

			++i;

			vertices[i].Position = second;
			vertices[i].Color = color;
			vertices[i].TextureSlot = slot;
			vertices[i].Rotation = 0;
			vertices[i].TexCoord = secondUV;

			theta += stepTheta;
		}
	}

	void Render() {
		//Fix use unordered map
		//fix?, declare a std::vector or class member array and keep reusing it
		int* slots = new int[bindSlotIndex];
		
		auto begin = lol.begin();
		for (int i = 0; i < bindSlotIndex; i++)
		{
			begin->first->Bind(i);
			std::advance(begin, i);
			slots[i] = i;
		}

		shader->Bind();

		shader->SetMatrix("u_Projection", Projection);
		shader->SetIntArray("u_Textures", slots, bindSlotIndex);

		Batcher->Render();
		vertexArray.Bind();

		bindSlotIndex = 0;

		lol.clear();
		delete[] slots;
	}

	~Graphics() {
		delete shader;
		delete Batcher;
	}
};