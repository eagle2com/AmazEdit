#include "stdafx.h"
#include "FileBuffer.h"


bool operator > (const Cursor& lh, const Cursor& rh) {
	if (lh.line_n > rh.line_n)
		return true;
	else if (lh.line_n == rh.line_n && lh.pos >= rh.pos)
		return true;
	else
		return false;
}

bool operator < (const Cursor& lh, const Cursor& rh) {
	return !(lh > rh);
}


FileBuffer::FileBuffer() : 
	cursor_shape(0.0, 0.0, 1.0, (GLfloat)CHAR_LINE_HEIGHT),
	selection_shape(0.0, 0.0, 1.0, (GLfloat)CHAR_LINE_HEIGHT)
{
	add_line("");
	cursor_shape.set_fill_color({ 1.0, 1.0, 1.0, 1.0 });
	selection_shape.set_fill_color({ 1.0, 1.0, 1.0, 0.2 });
}


FileBuffer::~FileBuffer()
{
}

void FileBuffer::insert(unsigned char c) {
	double time = glfwGetTime();
	for (auto& cursor : cursors) {
		cursor.line->text.insert(cursor.line->text.begin() + cursor.pos++, c);
		cursor.line->dirty = true;
		cursor.last_time = time;
		cursor.max_up_down = -1;
	}
}

void FileBuffer::erase() {
	double time = glfwGetTime();
	for (auto& cursor : cursors) {	
		if (cursor.pos >= cursor.line->text.length() - 1) {
			if (cursor.line_n < lines.size() - 1) {
				cursor.line++;
				const std::string text = cursor.line->text;
				cursor.line = lines.erase(cursor.line);
				cursor.line--;
				cursor.line->text.append(text);
				cursor.line->dirty = true;
			}
		}
		else {
			cursor.line->text.erase(cursor.line->text.begin() + cursor.pos);
			cursor.line->dirty = true;
		}
		
		cursor.last_time = time;
		cursor.max_up_down = -1;
	}
}

void FileBuffer::backspace() {
	double time = glfwGetTime();
	for (auto& cursor : cursors) {
		if (cursor.pos > 0) {
			cursor.line->text.erase(cursor.line->text.begin() + cursor.pos - 1);
			cursor.pos--;
			cursor.line->dirty = true;
		}
		else {
			if (cursor.line_n > 0) {
				std::string text = cursor.line->text;
				cursor.line = lines.erase(cursor.line);
				cursor.line--;
				int previous_length = cursor.line->text.length();
				cursor.line->text.append(text);
				cursor.pos = previous_length;
				cursor.line_n--;
				cursor.line->dirty = true;
			}
		}
		cursor.last_time = time;
		cursor.max_up_down = -1;
	}
}

void FileBuffer::cursor_left(bool cancel_selection) {
	double time = glfwGetTime();
	for (auto& cursor : cursors) {
		if (cursor.sel_end && cancel_selection) {
			delete cursor.sel_end;
			cursor.sel_end = nullptr;
		}
		cursor.pos--;
		if (cursor.pos < 0 && cursor.line_n > 0) {
			cursor.line_n--;
			cursor.line--;
			cursor.pos = cursor.line->text.length();
		}
		else if(cursor.pos < 0)
			cursor.pos = 0;
			

		cursor.last_time = time;
		cursor.max_up_down = -1;
	}
}

void FileBuffer::cursor_right(bool cancel_selection) {
	double time = glfwGetTime();
	for (auto& cursor : cursors) {
		if (cursor.sel_end && cancel_selection) {
			delete cursor.sel_end;
			cursor.sel_end = nullptr;
		}
		cursor.pos++;
		if (cursor.pos > cursor.line->text.length()) {
			if (cursor.line_n < lines.size() - 1) {
				cursor.pos = 0;
				cursor.line++;
				cursor.line_n++;
			}
			else
				cursor.pos = cursor.line->text.length();
		}
			

		cursor.last_time = time;
		cursor.max_up_down = -1;
	}
}

void FileBuffer::cursor_shift_right() {
	for (auto& cursor : cursors) {
		if (cursor.sel_end == nullptr) {
			cursor.sel_end = new Cursor(cursor);
		}
	}
	cursor_right(false);
}

void FileBuffer::cursor_shift_left() {
	for (auto& cursor : cursors) {
		if (cursor.sel_end == nullptr) {
			cursor.sel_end = new Cursor(cursor);
		}
	}
	cursor_left(false);
}


void FileBuffer::cursor_end() {
	double time = glfwGetTime();
	for (auto& cursor : cursors) {
		cursor.pos = cursor.line->text.length();
		cursor.last_time = time;
		cursor.max_up_down = -1;
	}
}

void FileBuffer::cursor_beg() {
	double time = glfwGetTime();
	for (auto& cursor : cursors) {
		cursor.pos = 0;
		cursor.last_time = time;
		cursor.max_up_down = -1;
	}
}

void FileBuffer::wheel_up() {
	first_line -= 3;
	if (first_line < 0)
		first_line = 0;
}

void FileBuffer::wheel_down() {
	first_line += 3;
	if (first_line > lines.size())
		first_line = lines.size();
}


void FileBuffer::draw(PackedFont& font) {
	GLfloat x_offset = 3;
	GLfloat y_offset = WINDOW_HEIGHT - CHAR_LINE_HEIGHT;
	auto it = lines.begin();
	
	for (int i = 0; i < first_line; i++, it++) {}
	auto first = it;
	for (int i = 0; i < WINDOW_HEIGHT/CHAR_LINE_HEIGHT && it != lines.end(); i++, it++) {}
	auto last = it;

	for (it = first; it != last; it++) {
		it->draw(font, x_offset, y_offset);
		y_offset -= CHAR_LINE_HEIGHT;
	}

	for (auto& cursor : cursors) {
		double time = glfwGetTime();
		double color = 0.5 + 0.5*cos(M_PI*(time - cursor.last_time));
		cursor_shape.set_fill_color({ 1.0, 1.0, 1.0, color });
		cursor_shape.set_position(
			x_offset + CHAR_ADVANCE * cursor.pos,
			WINDOW_HEIGHT - CHAR_LINE_HEIGHT*(cursor.line_n+1 - first_line) - 3); // @HARDCODE: use character advance and size

		cursor_shape.draw();

		if (cursor.sel_end) {
			const Cursor& c1 = std::min(cursor, *cursor.sel_end);
			const Cursor& c2 = std::max(cursor, *cursor.sel_end);
			auto line = c1.line;
			int pos1 = 0;
			int pos2 = 0;
			for (int i = c1.line_n; i <= c2.line_n; i++) {
				pos1 = 0;
				pos2 = line->text.length();

				if (i == c1.line_n) {
					pos1 = c1.pos;
				}
				
				if (i == c2.line_n) {
					pos2 = c2.pos;
				}
				
				selection_shape.set_position(
					x_offset + CHAR_ADVANCE * pos1,
					WINDOW_HEIGHT - CHAR_LINE_HEIGHT*(i + 1 - first_line) - 3);

				selection_shape.set_size(CHAR_ADVANCE*(pos2 - pos1), CHAR_LINE_HEIGHT);
				selection_shape.draw();
				line++;
			}
		}
	}
}

void FileBuffer::cursor_up(bool cancel_selection) {
	double time = glfwGetTime();
	for (auto& cursor : cursors) {
		if (cursor.sel_end && cancel_selection) {
			delete cursor.sel_end;
			cursor.sel_end = nullptr;
		}
		if (cursor.line_n > 0) {
			cursor.line_n--;
			cursor.line--;
			if (cursor.max_up_down >= 0)
				cursor.pos = cursor.max_up_down;
			else
				cursor.max_up_down = cursor.pos;

			if (cursor.pos > cursor.line->text.length()) {
				cursor.pos = cursor.line->text.length();
			}
		}
		cursor.last_time = time;
	}
}

void FileBuffer::cursor_shift_up() {
	for (auto& cursor : cursors) {
		if (cursor.sel_end == nullptr) {
			cursor.sel_end = new Cursor(cursor);
		}
	}
	cursor_up(false);
}

void FileBuffer::cursor_shift_down() {
	for (auto& cursor : cursors) {
		if (cursor.sel_end == nullptr) {
			cursor.sel_end = new Cursor(cursor);
		}
	}
	cursor_down(false);
}


void FileBuffer::cursor_down(bool cancel_selection) {
	double time = glfwGetTime();
	for (auto& cursor : cursors) {
		if (cursor.sel_end && cancel_selection) {
			delete cursor.sel_end;
			cursor.sel_end = nullptr;
		}
		if (cursor.line_n < lines.size() - 1) {
			cursor.line_n++;
			cursor.line++;
			if (cursor.max_up_down >= 0)
				cursor.pos = cursor.max_up_down;
			else
				cursor.max_up_down = cursor.pos;

			if (cursor.pos > cursor.line->text.length()) {
				cursor.pos = cursor.line->text.length();
			}
		}
		cursor.last_time = time;
	}
}

void FileBuffer::tab() {
	double time = glfwGetTime();
	for (auto& cursor : cursors) {
		cursor.line->text.insert(cursor.pos, "    ");
		cursor.pos += 4;
		cursor.line->dirty = true;
		cursor.last_time = time;
	}
}

void FileBuffer::enter() {
	double time = glfwGetTime();
	for (auto& cursor : cursors) {
		std::string text = cursor.line->text.substr(cursor.pos);
		cursor.line->text.erase(cursor.pos, cursor.line->text.length() - cursor.pos);
		cursor.line->dirty = true;
		cursor.line = lines.insert(++cursor.line, TextLine());
		cursor.line->text = text;
		cursor.line->dirty = true;
		cursor.line_n++;
		cursor.pos = 0;
		cursor.last_time = time;
	}
}

void FileBuffer::ctrl_enter() {
	double time = glfwGetTime();
	for (auto& cursor : cursors) {
		cursor.line = lines.insert(++cursor.line, TextLine());
		cursor.line->text = "";
		cursor.line->dirty = true;
		cursor.line_n++;
		cursor.pos = 0;
		cursor.last_time = time;
	}
}

void FileBuffer::ctrl_shift_enter() {
	double time = glfwGetTime();
	for (auto& cursor : cursors) {
		cursor.line = lines.insert(cursor.line, TextLine());
		cursor.line->text = "";
		cursor.line->dirty = true;
		cursor.line_n;
		cursor.pos = 0;
		cursor.last_time = time;
	}
}

void FileBuffer::add_line(const std::string& text) {
	  
	if (lines.empty()) {
		lines.push_back(TextLine());
		cursors.clear();
		cursors.push_back(Cursor());
		cursors[0].last_time = 0;
		cursors[0].line = lines.begin();
		cursors[0].line_n = 0;
		cursors[0].pos = text.length();
		cursors[0].line->text = text;
		cursors[0].line->dirty = true;
	}
	else {
		cursors[0].line = lines.insert(++cursors[0].line, TextLine());
		cursors[0].line->text = text;
		cursors[0].line->dirty = true;
		cursors[0].line_n++;
		cursors[0].pos = text.length();
	}
}

bool FileBuffer::load(const std::string& filename) {
	lines.clear();
	std::ifstream file;
	file.open(filename);

	std::string line;
	while (std::getline(file, line)) {
		size_t pos = line.find('\t');
		while (pos != line.npos) {
			line.erase(pos, 1);
			line.insert(pos, "    ");
			pos = line.find('\t', pos + 4);
		}
		add_line(line);
	}

	cursors[0].line = lines.begin();
	cursors[0].pos = 0;
	cursors[0].line_n = 0;

	return true;
}