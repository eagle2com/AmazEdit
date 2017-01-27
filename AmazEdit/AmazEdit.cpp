// AmazEdit.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include "shader.h"

// enable optimus
extern "C" {
	//_declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;	// uncomment to select nvidia card
}


struct Character {
	GLuint TextureID;   // ID handle of the glyph texture
	glm::ivec2 Size;    // Size of glyph
	glm::ivec2 Bearing;  // Offset from baseline to left/top of glyph
	GLuint Advance;    // Horizontal offset to advance to next glyph
};



struct PackedCharacter {
    GLuint advance;
    glm::ivec2 bearing;
    glm::ivec2 size;
    glm::ivec2 uv1;
};

struct PackedFont {
  PackedCharacter chars[255];
  GLuint texture_id;
  glm::ivec2 tex_size;
};

PackedFont main_font;

std::map<GLchar, Character> Characters;
GLuint VAO, VBO;

const int WIDTH = 800;
const int HEIGHT = 600;

std::string line;

void RenderText(Shader &shader, std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color);

void APIENTRY glDebugOutput(GLenum source,
                            GLenum type,
                            GLuint id,
                            GLenum severity,
                            GLsizei length,
                            const GLchar *message,
                            void *userParam)
{
    // ignore non-significant error/warning codes
    //if(id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

    std::cout << "---------------" << std::endl;
    std::cout << "Debug message (" << id << "): " <<  message << std::endl;

    switch (source)
    {
        case GL_DEBUG_SOURCE_API:             std::cout << "Source: API"; break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   std::cout << "Source: Window System"; break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER: std::cout << "Source: Shader Compiler"; break;
        case GL_DEBUG_SOURCE_THIRD_PARTY:     std::cout << "Source: Third Party"; break;
        case GL_DEBUG_SOURCE_APPLICATION:     std::cout << "Source: Application"; break;
        case GL_DEBUG_SOURCE_OTHER:           std::cout << "Source: Other"; break;
    } std::cout << std::endl;

    switch (type)
    {
        case GL_DEBUG_TYPE_ERROR:               std::cout << "Type: Error"; break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cout << "Type: Deprecated Behaviour"; break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  std::cout << "Type: Undefined Behaviour"; break;
        case GL_DEBUG_TYPE_PORTABILITY:         std::cout << "Type: Portability"; break;
        case GL_DEBUG_TYPE_PERFORMANCE:         std::cout << "Type: Performance"; break;
        case GL_DEBUG_TYPE_MARKER:              std::cout << "Type: Marker"; break;
        case GL_DEBUG_TYPE_PUSH_GROUP:          std::cout << "Type: Push Group"; break;
        case GL_DEBUG_TYPE_POP_GROUP:           std::cout << "Type: Pop Group"; break;
        case GL_DEBUG_TYPE_OTHER:               std::cout << "Type: Other"; break;
    } std::cout << std::endl;

    switch (severity)
    {
        case GL_DEBUG_SEVERITY_HIGH:         std::cout << "Severity: high"; break;
        case GL_DEBUG_SEVERITY_MEDIUM:       std::cout << "Severity: medium"; break;
        case GL_DEBUG_SEVERITY_LOW:          std::cout << "Severity: low"; break;
        case GL_DEBUG_SEVERITY_NOTIFICATION: std::cout << "Severity: notification"; break;
    } std::cout << std::endl;
    std::cout << std::endl;
}


void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if(action == GLFW_PRESS || action == GLFW_REPEAT) {
        if(key == GLFW_KEY_ESCAPE)  {
            glfwSetWindowShouldClose(window, true);
        }
        else if(key == GLFW_KEY_BACKSPACE) {
            if(!line.empty())
                line.pop_back();
        }
    }
}

void character_callback(GLFWwindow* window, unsigned int codepoint)
{
    line.push_back(codepoint);
}

// The MAIN function, from here we start our application and run the Game loop
int main(int arch, const char** argv)
{
    cout << "a: " << (int)'a' << endl;
    cout << "Initializing OpenGL ";
	// Init GLFW
	glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "LearnOpenGL", nullptr, nullptr); // Windowed
	glfwMakeContextCurrent(window);

	// Initialize GLEW to setup the OpenGL Function pointers
	glewExperimental = GL_TRUE;
	glewInit();

    cout << "[OK]" << endl;

    int major, minor, rev;
    glfwGetVersion(&major, &minor, &rev);
    cout << "OpenGL " << major << "." << minor << endl;

    GLint flags; glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
    if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
    {
        cout << "OpenGL debug enabled" << endl;
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback((GLDEBUGPROC)glDebugOutput, nullptr);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
    }

	// Define the viewport dimensions
	glViewport(0, 0, WIDTH, HEIGHT);

	// Set OpenGL options
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

   // cout << "Compiling shaders ";
	// Compile and setup the shader
	Shader shader("shaders/text.vs", "shaders/text.frag");
	glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(WIDTH), 0.0f, static_cast<GLfloat>(HEIGHT));
	shader.Use();
	glUniformMatrix4fv(glGetUniformLocation(shader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

   // cout << "[OK]" << endl;

   // cout << "Compiling fonts ";
	// FreeType
	FT_Library ft;
	// All functions return a value different than 0 whenever an error occurred
	if (FT_Init_FreeType(&ft))
		std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;

	// Load font as face
	FT_Face face;
    if (FT_New_Face(ft, "/usr/share/fonts/TTF/Hack-Regular.ttf", 0, &face))
		std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;

	// Set size to load glyphs as
	FT_Set_Pixel_Sizes(face, 0, 12);

	// Disable byte-alignment restriction
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    /*
    // Load full ascii set
    for (GLubyte c = 0; c < 128; c++)
	{
		// Load character glyph 
		
        if (FT_Load_Char(face, c, FT_LOAD_RENDER | FT_LOAD_TARGET_LIGHT))
		{
            //sFT_Render_Glyph(face->glyph, FT_RENDER_MODE_LCD_V);
			std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
			continue;
		}
		// Generate texture
		GLuint texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RED,
			face->glyph->bitmap.width,
			face->glyph->bitmap.rows,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			face->glyph->bitmap.buffer
        );
		// Set texture options
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// Now store character for later use
		
		Character character = {
			texture,
			glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
			glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
			face->glyph->advance.x
		};
		Characters.insert(std::pair<GLchar, Character>(c, character));
    }*/

    // compute full texture size, 16x16 characters
    int max_x = 0;
    int sum_y = 0;
    for(int y = 0; y < 16; y++) {
        int max_y = 0;
        int sum_x = 0;
        for(int x = 0; x < 16; x++) {
            GLubyte n = x + 16*y;
            if (FT_Load_Char(face, n, FT_LOAD_RENDER | FT_LOAD_TARGET_LIGHT))
            {
                //FT_Render_Glyph(face->glyph, FT_RENDER_MODE_LCD_V);
                std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
                continue;
            }
            sum_x += face->glyph->bitmap.width;
            if(face->glyph->bitmap.rows > max_y)
                max_y = face->glyph->bitmap.rows;
        }

        if(sum_x > max_x)
            max_x = sum_x;

        sum_y += max_y;
    }

   // cout << "texture needed: " << max_x << "x" << sum_y << endl;
    GLubyte *empty_texture = new GLubyte[max_x*sum_y];
    memset(empty_texture, 1, max_x*sum_y);

    glBindTexture(GL_TEXTURE_2D, 0);

    main_font.tex_size = {max_x, sum_y};
    glGenTextures(1, &main_font.texture_id);
    glBindTexture(GL_TEXTURE_2D, main_font.texture_id);
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RED,
        max_x,
        sum_y,
        0,
        GL_RED,
        GL_UNSIGNED_BYTE,
        0
    );

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    sum_y = 0;
    char n = 0;
    for(int y = 0; y < 8; y++) {
        int max_y = 0;
        int sum_x = 0;
        for(int x = 0; x < 16; x++) {
            n = x + 16*y;
            if (FT_Load_Char(face, n, FT_LOAD_RENDER | FT_LOAD_TARGET_LIGHT))
            {
                //FT_Render_Glyph(face->glyph, FT_RENDER_MODE_LCD_V);
                std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
                continue;
            }
            glTexSubImage2D(GL_TEXTURE_2D, 0, sum_x, sum_y, face->glyph->bitmap.width, face->glyph->bitmap.rows,
                            GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);
            main_font.chars[n].advance = face->glyph->advance.x;
            main_font.chars[n].size = {face->glyph->bitmap.width, face->glyph->bitmap.rows};
            main_font.chars[n].bearing = {face->glyph->bitmap_left, face->glyph->bitmap_top};
            main_font.chars[n].uv1 = {sum_x, sum_y};

            sum_x += face->glyph->bitmap.width;
            if(face->glyph->bitmap.rows > max_y)
                max_y = face->glyph->bitmap.rows;
        }

        if(sum_x > max_x)
            max_x = sum_x;

        sum_y += max_y;
    }

	glBindTexture(GL_TEXTURE_2D, 0);
	// Destroy FreeType once we're finished
	FT_Done_Face(face);
	FT_Done_FreeType(ft);

	// Configure VAO/VBO for texture quads
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

  //  cout << "[OK]" << endl;
    //int n = 0;
	auto render_time = std::chrono::high_resolution_clock::now();
	auto p_render_time = std::chrono::high_resolution_clock::now();
	// Game loop
    bool first_render = true;

    glfwSetKeyCallback(window, key_callback);
    glfwSetCharCallback(window, character_callback);

    glfwSwapInterval(0);

	while (!glfwWindowShouldClose(window))
	{
        if(first_render) {
            glfwWaitEventsTimeout(1/60.0);
        }
        else
            glfwWaitEventsTimeout(1);

		render_time = std::chrono::high_resolution_clock::now();
        //if (render_time - p_render_time > 16ms) {
			p_render_time = render_time;
			// Clear the colorbuffer
			//glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
			glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);

            //RenderText(shader, "a", 0.0f, HEIGHT - 40, 1.0f, glm::vec3(0.0f, 0.0f,0.0f));
            RenderText(shader, "int main(int argc, char** argv) {", 0.0f, HEIGHT - 40, 1.0f, glm::vec3(0.0f, 0.0f,0.0f));
            //RenderText(shader, "FT_Set_Pixel_Sizes(face, 13, 12);", 25.0f, 25.0f, 1.0f, glm::vec3(0.5f, 1.0f, 0.2f));
            //RenderText(shader, line.c_str(), 0, 10, 1.0f, glm::vec3(0.0, 0.0f, 0.0f));
            //cout << n++ << endl;
			// Swap the buffers
            first_render = false;
			glfwSwapBuffers(window);
        //}
	}

    delete [] empty_texture;

	glfwTerminate();
	return 0;
}

void RenderText(Shader &shader, std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color)
{
	// Activate corresponding render state	
	shader.Use();
	glUniform3f(glGetUniformLocation(shader.Program, "textColor"), color.x, color.y, color.z);
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(VAO);

	// Iterate through all characters
	std::string::const_iterator c;
	for (c = text.begin(); c != text.end(); c++)
	{
         char ch_c = *c;
        /*Character ch = Characters[ch_c];

		GLfloat xpos = x + ch.Bearing.x * scale;
		GLfloat ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

		GLfloat w = ch.Size.x * scale;
		GLfloat h = ch.Size.y * scale;
		// Update VBO for each character
        GLfloat vertices2[6][4] = {
			{ xpos,     ypos + h,   0.0, 0.0 },
			{ xpos,     ypos,       0.0, 1.0 },
			{ xpos + w, ypos,       1.0, 1.0 },

			{ xpos,     ypos + h,   0.0, 0.0 },
			{ xpos + w, ypos,       1.0, 1.0 },
			{ xpos + w, ypos + h,   1.0, 0.0 }
        };*/

        PackedCharacter ch = main_font.chars[ch_c];

        GLfloat xpos = x + ch.bearing.x * scale;
        GLfloat ypos = y - (ch.size.y - ch.bearing.y) * scale;

        GLfloat w = ch.size.x * scale;
        GLfloat h = ch.size.y * scale;

        float x1 = main_font.chars[ch_c].uv1.x/(float)main_font.tex_size.x;
        float y1 = main_font.chars[ch_c].uv1.y/(float)main_font.tex_size.y;

        float x2 = (main_font.chars[ch_c].uv1.x + main_font.chars[ch_c].size.x)/(float)main_font.tex_size.x;
        float y2 = (main_font.chars[ch_c].uv1.y + main_font.chars[ch_c].size.y)/(float)main_font.tex_size.y;

       GLfloat vertices[6][4] = {
            { xpos,     ypos + h,   x1, y1 },
            { xpos,     ypos,       x1, y2 },
            { xpos + w, ypos,       x2, y2 },

            { xpos,     ypos + h,   x1, y1 },
            { xpos + w, ypos,       x2, y2 },
            { xpos + w, ypos + h,   x2, y1 },
        };


		// Render glyph texture over quad
        //glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        glBindTexture(GL_TEXTURE_2D, main_font.texture_id);

		// Update content of VBO memory
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // Be sure to use glBufferSubData and not glBufferData

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		// Render quad
		glDrawArrays(GL_TRIANGLES, 0, 6);
		// Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (ch.advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}
