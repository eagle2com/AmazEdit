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

	/*cout << std::setprecision(3) << std::setw(3);
	double time = glfwGetTime();
	int minutes = (int)time;
	int decimal = (time - minutes)*100;
	cout << minutes << ":" << decimal << "> line updated" << endl;*/

	dirty = false;
}