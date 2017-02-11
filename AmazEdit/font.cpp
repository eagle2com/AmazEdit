#include "stdafx.h"
#include "font.h"

PackedFont::PackedFont() {

}

bool PackedFont::load(const std::string& filename, int size) {
	FT_Library ft;
	// All functions return a value different than 0 whenever an error occurred
	if (FT_Init_FreeType(&ft))
		std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;

	FT_Face face;
	if (FT_New_Face(ft, filename.c_str(), 0, &face))
		std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;

	// Set size to load glyphs as
	FT_Set_Pixel_Sizes(face, 0, size);



	// Disable byte-alignment restriction
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	FT_Int32 flags = FT_LOAD_RENDER | FT_LOAD_TARGET_LIGHT;

	// compute full texture size, 16x16 characters
	uint32_t max_x = 0;
	uint32_t sum_y = 0;
	int n = 0;
	for (int y = 0; y < 16; y++) {
		uint32_t max_y = 0;
		uint32_t sum_x = 0;
		for (int x = 0; x < 16; x++) {
			n = x + 16 * y;
			if (FT_Load_Char(face, n, flags))
			{
				cout << "ERROR::FREETYTPE: Failed to load Glyph" << endl;
				continue;
			}
			sum_x += face->glyph->bitmap.width;

			if (face->glyph->bitmap.rows > max_y)
				max_y = face->glyph->bitmap.rows;
		}

		if (sum_x > max_x)
			max_x = sum_x;

		sum_y += max_y;
	}

	

	this->tex_size = { max_x, sum_y };
	glGenTextures(1, &this->texture_id);
	glBindTexture(GL_TEXTURE_2D, this->texture_id);
	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		GL_RED,
		max_x,
		sum_y,
		0,
		GL_RED,
		GL_UNSIGNED_BYTE,
		nullptr
	);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	std::stringstream ss;

	sum_y = 0;

	for (int y = 0; y < 16; y++) {
		uint32_t max_y = 0;
		uint32_t sum_x = 0;
		for (int x = 0; x < 16; x++) {
			n = x + 16 * y;
			int nreal = n;
			if (FT_Load_Char(face, nreal, flags))
			{
				cout << "ERROR::FREETYTPE: Failed to load Glyph" << endl;
				continue;
			}

			if (face->glyph->bitmap.buffer) {
				glTexSubImage2D(GL_TEXTURE_2D, 0, sum_x, sum_y, face->glyph->bitmap.width, face->glyph->bitmap.rows,
					GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);

				/*ss.str("");
				ss << "font_" << n << ".bmp";
				FILE* file = fopen(ss.str().c_str(), "wb");
				fwrite(&face->glyph->bitmap.width, sizeof(int), 1, file);
				fwrite(&face->glyph->bitmap.rows, sizeof(int), 1, file);
				fwrite(face->glyph->bitmap.buffer, face->glyph->bitmap.width*face->glyph->bitmap.rows, 1, file);
				fclose(file);*/
			}

			chars[n].advance = face->glyph->advance.x;
			chars[n].size = { face->glyph->bitmap.width, face->glyph->bitmap.rows };
			chars[n].bearing = { face->glyph->bitmap_left, face->glyph->bitmap_top };

			float x1 = sum_x / (float)tex_size.x;
			float y1 = sum_y / (float)tex_size.y;

			chars[n].uv1 = { x1, y1 };

			float x2 = (sum_x + chars[n].size.x) / (float)tex_size.x;
			float y2 = (sum_y + chars[n].size.y) / (float)tex_size.y;

			chars[n].uv2 = { x2, y2 };

			sum_x += face->glyph->bitmap.width;
			if (face->glyph->bitmap.rows > max_y)
				max_y = face->glyph->bitmap.rows;
		}

		if (sum_x > max_x)
			max_x = sum_x;

		sum_y += max_y;
	}

	cout << "loaded " << n << " glyphs" << endl;

	glBindTexture(GL_TEXTURE_2D, 0);
	// Destroy FreeType once we're finished
	FT_Done_Face(face);
	FT_Done_FreeType(ft);
	cout << "texture size: " << tex_size << endl;

	return true;
}

std::ostream& operator << (std::ostream& out, const glm::ivec2 v) {
	out << "( " << v.x << " " << v.y << " )";
	return out;
}
