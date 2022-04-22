#pragma once

//Include all the header files or some shit?=

#include "GLBuffer.h"
#include "PrimitiveBatch.h"
#include "Shader.h"
#include "Texture.h"
#include "vec2.hpp"
#include "vec4.hpp"

class Graphics {
private:
	struct Vertex {
		glm::vec2 Position { 0,0 };
		glm::vec2 TexCoord { 0,0 };
		glm::vec2 RotationOrigin { 0,0 };
		glm::vec4 Color { 0,0,0,0 };
		float Rotation = 0;
		float TextureSlot = 0;
	};

	Shader shader;

	int slots[16]{0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};

	std::map<Texture*, int> lol;

	int bindSlotIndex = 0;

	void convertToRadians(float& degrees) {
		degrees = degrees * 0.017453292519943295769236907684886;
	}

	void mapRange(float& value, float fromSource, float toSource, float fromTarget, float toTarget) {
		value = (value - fromSource) / (toSource - fromSource) * (toTarget - fromTarget) + fromTarget;
	}

	float mapRange(float value, float fromSource, float toSource, float fromTarget, float toTarget) {
		return value = (value - fromSource) / (toSource - fromSource) * (toTarget - fromTarget) + fromTarget;
	}

public:
	PrimitiveBatch<Vertex> Batcher;
	glm::mat4 Projection;

	Graphics() :
		shader("C:\\Users\\user\\Desktop\\Default.vert", "C:\\Users\\user\\Desktop\\Default.frag"),
		Batcher(400000, 600000,
		{
			{ GL_FLOAT, 2, false },
			{ GL_FLOAT, 2, false },
			{ GL_FLOAT, 2, false },
			{ GL_FLOAT, 4, false },
			{ GL_FLOAT, 1, false },
			{ GL_FLOAT, 1, false }
		}),
		Projection() { }

	int GetTextureSlot(const Texture* texture) {
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

	void DrawRectangle(glm::vec2& position, glm::vec2& size, glm::vec4& color, const Texture* const texture, glm::vec4 textureRect, bool uvNormalized = true, float rotation = 0) {
		#define texWidth textureRect.z
		#define texHeight textureRect.w

		auto quad = Batcher.WriteQuad();

		auto textureSlot = GetTextureSlot(texture);

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

		int slot = GetTextureSlot(texture);

		glm::vec2 first, second;

		glm::vec2 firstUV{ 0,0 }, secondUV{ 1, 1};

		float theta = startAngle;
		float stepTheta = (endAngle - startAngle) / (segments - 1);

		float cos, sin;
		
		int vertexCount = segments * 2;

		auto vertices = Batcher.WriteTriangleStrip(vertexCount);

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
			vertices[i].RotationOrigin = { 0, 0 };

			++i;

			vertices[i].Position = second;
			vertices[i].Color = color;
			vertices[i].TextureSlot = slot;
			vertices[i].Rotation = 0;
			vertices[i].TexCoord = secondUV;
			vertices[i].RotationOrigin = { 0, 0 };

			theta += stepTheta;
		}
	}

	void Render() {
		//Fix use unordered map

		auto begin = lol.begin();
		for (int i = 0; i < bindSlotIndex; i++)
		{
			begin->first->Bind(i);
			std::advance(begin, i);
			slots[i] = i;
		}

		shader.Bind();

		shader.SetMatrix("u_Projection", Projection);
		shader.SetIntArray("u_Textures", slots, bindSlotIndex);

		Batcher.Render();

		bindSlotIndex = 0;

		lol.clear();
	}
};