#pragma once
#include "stdafx.h"
#include "shader.h"

class ShapeRectangle
{
public:
	ShapeRectangle(GLfloat x, GLfloat y, GLfloat w, GLfloat h);
	~ShapeRectangle();
	void set_position(GLfloat x, GLfloat y);
	void set_size(GLfloat w, GLfloat h);
	void draw();
	void update();
	void set_fill_color(const glm::vec4& color);
	Shader* shader;

private:
	GLuint VAO = 0;
	GLuint VBO = 0;
	GLfloat x, y, w, h;
	glm::mat4 model;
	bool dirty = true;
	glm::vec4 fill_color;
};

