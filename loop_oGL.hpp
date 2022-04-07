# ifndef LOOP_HPP
# define LOOP_HPP

# define GLFW_INCLUDE_GLCOREARB
# define GL_SILENCE_DEPRECATION

# ifdef __APPLE__
	# define GL_DO_NOT_WARN_IF_MULTI_GL_VERSION_HEADERS_INCLUDED
#endif

#include <GL/glew.h> 
#include <GLFW/glfw3.h>

# ifdef __APPLE__
	# define __gl_h_
#endif

class game_window
{
	private:
		chip8		&_chip;
		GLFWwindow*	_window;

	public:
		game_window(chip8 &chip) : _chip(chip) {};
		~game_window();
		int	main_loop();

		static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
		void	key_callback_impl(int key, int action);
		void	draw_pixels();
};

# endif
