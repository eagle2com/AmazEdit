// AmazEdit.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include "shader.h"
#include "font.h"
#include "TextLine.h"
#include "FileBuffer.h"

// enable optimus
extern "C" {
	//_declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;	// uncomment to select nvidia card
}

PackedFont main_font;

std::string line;

void APIENTRY glDebugOutput(GLenum source,
                            GLenum type,
                            GLuint id,
                            GLenum severity,
                            GLsizei/* length*/,
                            const GLchar *message,
                            void* /*userParam*/)
{
    // ignore non-significant error/warning codes
   if(id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

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

FileBuffer file_buffer;

void key_callback(GLFWwindow* window, int key, int /*scancode*/, int action, int /*mods*/) {
    if(action == GLFW_PRESS || action == GLFW_REPEAT) {
        if(key == GLFW_KEY_ESCAPE)  {
            glfwSetWindowShouldClose(window, true);
        }
        else if(key == GLFW_KEY_BACKSPACE) {
			file_buffer.backspace();
        }
		else if (key == GLFW_KEY_DELETE) {
			file_buffer.erase();
		}
		else if (key == GLFW_KEY_LEFT) {
			file_buffer.cursor_left();
		}
		else if (key == GLFW_KEY_RIGHT) {
			file_buffer.cursor_right();
		}
		else if (key == GLFW_KEY_UP) {
			file_buffer.key_up();
		}
		else if (key == GLFW_KEY_DOWN) {
			file_buffer.key_down();
		}
		else if (key == GLFW_KEY_HOME) {
			file_buffer.cursor_beg();
		}
		else if (key == GLFW_KEY_END) {
			file_buffer.cursor_end();
		}
		else if (key == GLFW_KEY_ENTER) {
			file_buffer.enter();
		}
		else if (key == GLFW_KEY_TAB) {
			file_buffer.tab();
		}
    }
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {

}

void character_callback(GLFWwindow* /*window*/, unsigned int codepoint)
{

	//cout << codepoint << endl;
	if(codepoint < 255)
		file_buffer.insert(codepoint);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	if (yoffset > 0) {
		file_buffer.wheel_up();
	}

	if (yoffset < 0) {
		file_buffer.wheel_down();
	}
}

// The MAIN function, from here we start our application and run the Game loop
int main(int, const char**)
{
	// Init GLFW
	glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

	GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "AmazEdit", nullptr, nullptr); // Windowed
	glfwMakeContextCurrent(window);

	// Initialize GLEW to setup the OpenGL Function pointers
	glewExperimental = GL_TRUE;
	glewInit();

    int major, minor, rev;
    glfwGetVersion(&major, &minor, &rev);
    cout << "OpenGL " << major << "." << minor << endl;
	
#ifdef _DEBUG
    GLint flags; glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
    if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
    {
        cout << "OpenGL debug enabled" << endl;
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback((GLDEBUGPROC)glDebugOutput, nullptr);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
    }
#endif

	// Define the viewport dimensions
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

	// Set OpenGL options
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

   // cout << "Compiling shaders ";
	// Compile and setup the shader
	Shader shader("shaders/text.vs", "shaders/text.frag");
	Shader shape_shader("shaders/shape.vs", "shaders/shape.fs");

	glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(WINDOW_WIDTH), 0.0f, static_cast<GLfloat>(WINDOW_HEIGHT));
	shader.Use();
	glUniformMatrix4fv(glGetUniformLocation(shader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

	shape_shader.Use();
	glUniformMatrix4fv(glGetUniformLocation(shape_shader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

	main_font.load("C:/Windows/fonts/consola.ttf", 12);
	main_font.shader = &shader;

	file_buffer.shape_shader = &shape_shader;
	file_buffer.load("FileBuffer.cpp");

    // Game loop
    bool first_render = true;
	
    glfwSetKeyCallback(window, key_callback);
    glfwSetCharCallback(window, character_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetScrollCallback(window, scroll_callback);

    glfwSwapInterval(1);

	while (!glfwWindowShouldClose(window))
	{
       /* if(first_render) {
            glfwWaitEventsTimeout(1/60.0);
        }
        else
            glfwWaitEventsTimeout(1);*/

		glfwPollEvents();

        glClearColor(65/255.0f, 73/255.0f, 86/255.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

		file_buffer.draw(main_font);

        glfwSwapBuffers(window);
	}

	glfwTerminate();
	return 0;
}