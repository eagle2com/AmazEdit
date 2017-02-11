#include "stdafx.h"
#include "TextLine.h"

TextLine::TextLine()
{
	dirty = true;
	VAO = 0;
	VBO = 0;

	
}


TextLine::~TextLine()
{
	if(VAO) glDeleteBuffers(1, &VAO);
	if(VBO) glDeleteBuffers(1, &VBO);
}

void TextLine::draw(PackedFont& font, GLfloat x, GLfloat y) {
	if (dirty) update(font);

	font.shader->Use();
	glUniform3f(glGetUniformLocation(font.shader->Program, "textColor"), 1.0f, 1.0f, 1.0f);

	glm::mat4 model;
	model = glm::translate(model, { x,y,0 });
	glUniformMatrix4fv(glGetUniformLocation(font.shader->Program, "model"), 1, GL_FALSE, glm::value_ptr(model));

	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(VAO);

	glBindTexture(GL_TEXTURE_2D, font.texture_id);
	glDrawArrays(GL_TRIANGLES, 0, 6*text.length());
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);

	double time = glfwGetTime();

	// draw cursor
	//if (cursor >= 0) {

		double color = 0.5 + 0.5*cos(M_PI*(time - cursor_last_time));
		if (cursor < 0)
			color = 0;
		cursor_shape.set_fill_color({ 1.0, 1.0, 1.0, color });
		cursor_shape.set_position(x + CHAR_ADVANCE * cursor, y - 3); // @HARDCODE: use character advance and size
	//}
	cursor_shape.draw();
}

 void TextLine::set(const std::string& text) {
	this->text = text;
	dirty = true;
}

void TextLine::insert(unsigned char c) {
	if (cursor >= 0) {
		text.insert(text.begin() + cursor, c);
		dirty = true;
		cursor++;
	}
}

void TextLine::insert(const std::string & str)
{
	if (cursor >= 0) {
		text.insert(cursor, str);
		dirty = true;
		cursor += str.length();
	}
}

void TextLine::backspace() {
	if (!text.empty() && cursor - 1 >= 0) {
		text.erase(cursor - 1, 1);
		dirty = true;
		cursor--;
	}
}

void TextLine::erase() {
	if (!text.empty() && cursor >= 0) {
		text.erase(cursor, 1);
		dirty = true;
	}
}

void TextLine::cursor_set(int pos) {
	if (pos > (int)text.length())
		pos = text.length();

	cursor = pos;
	cursor_last_time = glfwGetTime();
}

void TextLine::cursor_left() {
	cursor--;
	if (cursor < 0)
		cursor = 0;
	cursor_last_time = glfwGetTime();
}

void TextLine::cursor_right() {
	cursor++;
	if (cursor > (int)text.length())
		cursor = text.length();
	cursor_last_time = glfwGetTime();
}

void TextLine::cursor_end() {
	cursor = text.length();
	cursor_last_time = glfwGetTime();
}

void TextLine::cursor_beg() {
	cursor = 0;
	cursor_last_time = glfwGetTime();
}

void TextLine::update(PackedFont& font) {
	// Configure VAO/VBO for texture quads
	if (VAO == 0 && VBO == 0) {
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
	}
	
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	buffer_data.clear();
	buffer_data.reserve(6 * 4 * text.length());
	GLfloat x = 0;
	GLfloat y = 0;

	for (unsigned char c : text) {
		PackedCharacter& ch = font.chars[c];

		const GLfloat xpos = x + ch.bearing.x;
		const GLfloat ypos = y - (ch.size.y - ch.bearing.y);

		const GLfloat w = (GLfloat)ch.size.x;
		const GLfloat h = (GLfloat)ch.size.y;

		buffer_data.push_back(xpos);
		buffer_data.push_back(ypos + h);
		buffer_data.push_back(ch.uv1.x);
		buffer_data.push_back(ch.uv1.y);
			
		buffer_data.push_back(xpos);
		buffer_data.push_back(ypos);
		buffer_data.push_back(ch.uv1.x);
		buffer_data.push_back(ch.uv2.y);
				
		buffer_data.push_back(xpos + w);
		buffer_data.push_back(ypos);
		buffer_data.push_back(ch.uv2.x);
		buffer_data.push_back(ch.uv2.y);

		buffer_data.push_back(xpos + w);
		buffer_data.push_back(ypos);
		buffer_data.push_back(ch.uv2.x);
		buffer_data.push_back(ch.uv2.y);
				  
		buffer_data.push_back(xpos + w);
		buffer_data.push_back(ypos + h);
		buffer_data.push_back(ch.uv2.x);
		buffer_data.push_back(ch.uv1.y);

		buffer_data.push_back(xpos);
		buffer_data.push_back(ypos + h);
		buffer_data.push_back(ch.uv1.x);
		buffer_data.push_back(ch.uv1.y);

		x += (ch.advance >> 6);
	}

	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4 * text.length(), buffer_data.data(), GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4* sizeof(GLfloat), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	
	cursor_last_time = glfwGetTime();

	dirty = false;
}