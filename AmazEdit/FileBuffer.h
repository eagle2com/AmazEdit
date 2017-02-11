#pragma once
#include "stdafx.h"
#include "TextLine.h"
#include "font.h"
#include "ShapeRectangle.h"

struct Cursor {
	int pos;
	std::list<TextLine>::iterator line;
	int line_n;
	double last_time;
	int max_up_down;
};

class FileBuffer
{
public:
	FileBuffer();
	~FileBuffer();
	void insert(unsigned char c);
	void erase();
	void backspace();
	void cursor_left();
	void cursor_right();
	void cursor_end();
	void cursor_beg();
	void draw(PackedFont& font);
	void add_line(const std::string& text);
	void enter();
	void key_up();
	void key_down();
	void tab();
	bool load(const std::string& filename);
	void wheel_up();
	void wheel_down();

	Shader* shape_shader;

	std::list<TextLine> lines;
	int first_line = 0;
	ShapeRectangle cursor_shape;
	std::vector<Cursor> cursors;
};

