#pragma once
#include "GLObject.h"
#include <GL/glew.h>

class Shader : public GLObject
{
private:
	const char* filename;


public:
	Shader(const char* filename) {
		this->filename = filename;
	}

	void bind(int slot) {

	}

	void initialize() {
		
	}

	void release() {

	}
};

