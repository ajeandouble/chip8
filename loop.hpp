# ifndef LOOP_HPP
# define LOOP_HPP

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_timer.h>
#include "chip8.hpp"

class game_window
{
	private:
		chip8		&_chip;

	public:
		game_window(chip8 &chip) : _chip(chip) {};
		~game_window();
		int	main_loop();

		void			process_key(int key, int action);
		SDL_Window		*_window;
		SDL_Renderer	*_renderer;
		SDL_Texture		*_texture;
};

# endif
