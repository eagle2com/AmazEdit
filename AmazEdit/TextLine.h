#pragma once
#include "stdafx.h"
#include "font.h"
#include "ShapeRectangle.h"

class TextLine
{
public:
	TextLine();
	~TextLine();

	void draw(PackedFont& font, GLfloat x, GLfloat y);
	void update(PackedFont& font);

	std::string text;
	bool dirty;

private:
	GLuint VAO;
	GLuint VBO;

	// The line has changed and the VBO has to be updated
	
	std::vector<GLfloat> buffer_data;
};

