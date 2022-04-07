#include <iostream>

#include "chip8.hpp"
#include "loop.hpp"

void	game_window::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	static_cast<game_window *>(glfwGetWindowUserPointer(window))->key_callback_impl(key, action);
}

void	game_window::key_callback_impl(int key, int action)
{
	if(action == GLFW_PRESS)
        if(key == GLFW_KEY_ESCAPE)
            glfwSetWindowShouldClose(this->_window, GL_TRUE);

	enum ACTION_VALUE
	{
		KEY_UP,
		KEY_DOWN,
		KEY_PRESS
	};
	if (action == KEY_PRESS)
		action = KEY_UP;
	switch (key)
	{
		case '1':
			this->_chip.set_key(0x1 ,action);
			break ;
		case '2':
			this->_chip.set_key(0x2 ,action);
			break ;
		case '3':
			this->_chip.set_key(0x3 ,action);
			break ;
		case '4':
			this->_chip.set_key(0xC ,action);
			break ;

		case 'Q':
			this->_chip.set_key(0x4 ,action);
			break ;
		case 'W':
			this->_chip.set_key(0x5 ,action);
			break ;
		case 'E':
			this->_chip.set_key(0x6 ,action);
			break ;
		case 'R':
			this->_chip.set_key(0xD ,action);
			break ;
	
		case 'A':
			this->_chip.set_key(0x7 ,action);
			break ;
		case 'S':
			this->_chip.set_key(0x8 ,action);
			break ;
		case 'D':
			this->_chip.set_key(0x9 ,action);
			break ;
		case 'F':
			this->_chip.set_key(0xE ,action);
			break ;

		case 'Z':
			this->_chip.set_key(0xA ,action);
			break ;
		case 'X':
			this->_chip.set_key(0x0 ,action);
			break ;
		case 'C':
			this->_chip.set_key(0xB ,action);
			break ;
		case 'V':
			this->_chip.set_key(0xF ,action);
			break ;
	}
}

void	game_window::draw_pixels()
{

}

int	game_window::main_loop()
{
	if (!glfwInit())
	{
		throw "Cannot initialize glfw\n";
		return (-0x2a);
	}

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2); 
    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); 
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    this->_window = glfwCreateWindow(VIDEO_WIDTH * 10, VIDEO_HEIGHT * 10, "chip8", NULL, NULL);
	glfwSetWindowUserPointer(this->_window, this);
	glfwSetKeyCallback(this->_window, this->key_callback);
    glfwMakeContextCurrent(this->_window);	 
	if (!this->_window) 
    {
        glfwTerminate();
		throw("Couldn't create window\n");
    }

	glewExperimental = GL_TRUE;
    glewInit();
	DBG (
		const GLubyte* renderer = glGetString(GL_RENDERER); // get renderer string
		const GLubyte* version = glGetString(GL_VERSION); // version as a string
		std::cerr << "Renderer: " << renderer << std::endl;
		std::cerr << "OpenGL version supported " << version << std::endl;
	)
	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"

 	// Event loop
    float points[] = {
		0.0f,  0.1f,  0.0f,
		0.1f, -0.1f,  0.0f,
		-0.1f, -0.1f,  0.0f
	};
    float points2[] = {
		0.0f,  0.5f,  0.0f,
		0.5f, -0.5f,  0.1f,
		-0.1f, -0.4f,  0.0f
	};
	GLuint vbo = 0;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(float), points, GL_STATIC_DRAW);

	GLuint vbo2 = 0;
	glGenBuffers(1, &vbo2);
	glBindBuffer(GL_ARRAY_BUFFER, vbo2);
	glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(float), points2, GL_STATIC_DRAW);

	GLuint vao = 0;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	const char* vertex_shader =
		"#version 400\n"
		"in vec3 vp;"
		"void main() {"
		"  gl_Position = vec4(vp, 0.5);"
		"}";
	const char* fragment_shader =
		"#version 400\n"
		"out vec4 frag_colour;"
		"void main() {"
		"  frag_colour = vec4(0.5, 0.4, 0.5, 1.0);"
		"}";
	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, &vertex_shader, NULL);
	glCompileShader(vs);
	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, &fragment_shader, NULL);
	glCompileShader(fs);
	GLuint shader_programme = glCreateProgram();
	glAttachShader(shader_programme, fs);
	glAttachShader(shader_programme, vs);
	glLinkProgram(shader_programme);
    while(!glfwWindowShouldClose(this->_window)) 
    {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glUseProgram(shader_programme);
		glBindVertexArray(vao);
		// draw points 0-3 from the currently bound VAO with current in-use shader
		glDrawArrays(GL_TRIANGLES, 0, 3);
		// update other events like input handling 
		glfwPollEvents();
		// put the stuff we've been drawing onto the display
		glfwSwapBuffers(this->_window);
    }

    glfwTerminate();
	return (0);
}

game_window::~game_window()
{
	glfwDestroyWindow(this->_window);
}
