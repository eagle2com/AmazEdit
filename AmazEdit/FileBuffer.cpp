#include "stdafx.h"
#include "FileBuffer.h"


FileBuffer::FileBuffer() : cursor_shape(0, -3, 1, CHAR_LINE_HEIGHT)
{
	add_line("");
	cursor_shape.set_fill_color({ 1.0, 1.0, 1.0, 1.0 });
}


FileBuffer::~FileBuffer()
{
}

void FileBuffer::insert(unsigned char c) {
	for (auto& cursor : cursors) {
		cursor.line->text.insert(cursor.line->text.begin() + cursor.pos, c);
		cursor.line->dirty = true;
	}
}

void FileBuffer::erase() {
	for (auto& cursor : cursors) {	
		cursor.line->text.erase(cursor.line->text.begin() + cursor.pos);
		cursor.line->dirty = true;
	}
	//if (current_line != lines.end()) current_line->erase();
}

void FileBuffer::backspace() {
	for (auto& cursor : cursors) {
		if (cursor.pos > 0) {
			cursor.line->text.erase(cursor.line->text.begin() + cursor.pos - 1);
			cursor.pos--;
			cursor.line->dirty = true;
		}
	}
}

void FileBuffer::cursor_left() {
	
}

void FileBuffer::cursor_right() {

}

void FileBuffer::cursor_set(int pos) {
	//if (current_line != lines.end()) current_line->cursor_set(pos);
}

void FileBuffer::cursor_end() {
	//if (current_line != lines.end()) current_line->cursor_end();
}

void FileBuffer::cursor_beg() {
	//if (current_line != lines.end()) current_line->cursor_beg();
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
	GLfloat y_offset = WINDOW_HEIGHT - CHAR_LINE_HEIGHT;
	auto it = lines.begin();
	
	for (int i = 0; i < first_line; i++, it++) {}
	auto first = it;
	for (int i = 0; i < WINDOW_HEIGHT/CHAR_LINE_HEIGHT && it != lines.end(); i++, it++) {}
	auto last = it;

	for (it = first; it != last; it++) {
		it->draw(font, 3, y_offset);
		y_offset -= CHAR_LINE_HEIGHT;
	}
}

void FileBuffer::key_up() {
	/*int cursor_pos = current_line->cursor;
	current_line->cursor_set(-1);

	if (current_line != lines.begin())
		current_line--;

	current_line->cursor_set(cursor_pos);
	current_line->cursor_last_time = glfwGetTime();*/
}

void FileBuffer::key_down() {
	/*int cursor_pos = current_line->cursor;
	current_line->cursor_set(-1);
	
	current_line++;
	if (current_line == lines.end())
		current_line--;
	current_line->cursor_set(cursor_pos);
	current_line->cursor_last_time = glfwGetTime();*/
}

void FileBuffer::tab() {
	for (auto& cursor : cursors) {
		cursor.line->text.insert(cursor.pos, "    ");
		cursor.line->dirty = true;
	}
}

void FileBuffer::enter() {
	/*std::string text = current_line->text.substr(current_line->cursor);
	current_line->text.erase(current_line->cursor, current_line->text.length() - current_line->cursor);
	add_line(text, 0);*/
}

void FileBuffer::add_line(const std::string& text) {
	  
	if (lines.empty()) {
		lines.push_back(TextLine());
		cursors.clear();
		cursors.push_back(Cursor());
		cursors[0].last_time = 0;
		cursors[0].line = lines.begin();
		cursors[0].pos = 0;
		cursors[0].line->set(text);
	}
	else {
		cursors[0].line = lines.insert(cursors[0].line, TextLine());
		cursors[0].line->set(text);
	}
}

bool FileBuffer::load(const std::string& filename) {
	/*lines.clear();
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

	current_line = lines.begin();
	current_line->cursor_end();*/

	return true;
}