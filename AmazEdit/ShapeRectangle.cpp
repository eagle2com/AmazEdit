#include "stdafx.h"
#include "ShapeRectangle.h"

ShapeRectangle::ShapeRectangle(GLfloat x, GLfloat y, GLfloat w, GLfloat h):
	x(x), y(y), w(w), h(h), dirty(true)
{
}

void ShapeRectangle::set_position(GLfloat x, GLfloat y) {
	this->x = x;
	this->y = y;
	model = glm::mat4();
	model = glm::translate(model, { x,y,0 });
}

void ShapeRectangle::set_size(GLfloat w, GLfloat h) {
	this->w = w;
	this->h = h;
	dirty = true;
}

void ShapeRectangle::set_fill_color(const glm::vec4& color) {
	fill_color = color;
}

void ShapeRectangle::draw() {
	if (dirty) update();
	shader->Use();

	glUniform4fv(glGetUniformLocation(shader->Program, "fill_color"), 1, glm::value_ptr(fill_color));
	glUniformMatrix4fv(glGetUniformLocation(shader->Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	glBindVertexArray(0);
}

void ShapeRectangle::update() {
	GLfloat vertices[8] = {
		0, h,
		0, 0,
		w, 0,
		w, h
	};

	if (VAO == 0 && VBO == 0) {
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
	}

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 8, &vertices, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	dirty = false;
}

ShapeRectangle::~ShapeRectangle()
{
	if (VAO) glDeleteBuffers(1, &VAO);
	if (VBO) glDeleteBuffers(1, &VBO);
}
