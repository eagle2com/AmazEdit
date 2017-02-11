#pragma once
#include "stdafx.h"
#include "shader.h"

std::ostream& operator << (std::ostream& out, const glm::ivec2 v);

struct PackedCharacter {
	GLuint advance;
	glm::ivec2 bearing;
	glm::ivec2 size;
	glm::vec2 uv1;
	glm::vec2 uv2;
};

struct PackedFont {
	PackedCharacter chars[256];
	GLuint texture_id;
	glm::ivec2 tex_size;
	Shader* shader;

	PackedFont();
	bool load(const std::string& filename, int size);
};