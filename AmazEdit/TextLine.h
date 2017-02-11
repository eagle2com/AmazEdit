#pragma once
#include "stdafx.h"
#include "font.h"
#include "ShapeRectangle.h"

class TextLine
{
public:
	TextLine();
	~TextLine();

	void set(const std::string& text);
	void insert(unsigned char c);
	void insert(const std::string& str);
	void erase();
	void backspace();
	void cursor_left();
	void cursor_right();
	void cursor_set(int pos);
	void cursor_end();
	void cursor_beg();

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

