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
	Cursor *sel_end;	// end of selection (if any)
};


bool operator > (const Cursor& lh, const Cursor& rh);
bool operator < (const Cursor& lh, const Cursor& rh);

class FileBuffer
{
public:
	FileBuffer();
	~FileBuffer();
	void insert(unsigned char c);
	void erase();
	void backspace();
	void cursor_left(bool cancel_selection = true);
	void cursor_shift_left();

	void cursor_right(bool cancel_selection = true);
	void cursor_shift_right();

	void cursor_up(bool cancel_selection = true);
	void cursor_shift_up();

	void cursor_down(bool cancel_selection = true);
	void cursor_shift_down();

	void cursor_end();
	void cursor_beg();
	void draw(PackedFont& font);
	void add_line(const std::string& text);
	void enter();
	void ctrl_enter();
	void ctrl_shift_enter();
	
	void tab();
	bool load(const std::string& filename);
	void wheel_up();
	void wheel_down();

	std::list<TextLine> lines;
	int first_line = 0;
	ShapeRectangle cursor_shape;
	ShapeRectangle selection_shape;
	std::vector<Cursor> cursors;
};

