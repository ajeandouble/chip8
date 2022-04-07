#include <iostream>
#include "loop.hpp"

game_window::~game_window()
{

}

void	game_window::process_key(int key, int action)
{
	std::cerr << "KEY UP" << action << " " << SDL_KEYUP << " " << SDL_KEYDOWN << std::endl << std::flush;
	action = action == SDL_KEYDOWN ? 1 : 0;
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

		case 'q':
			this->_chip.set_key(0x4 ,action);
			break ;
		case 'w':
			this->_chip.set_key(0x5 ,action);
			break ;
		case 'e':
			this->_chip.set_key(0x6 ,action);
			break ;
		case 'r':
			this->_chip.set_key(0xD ,action);
			break ;
	
		case 'a':
			this->_chip.set_key(0x7 ,action);
			break ;
		case 's':
			this->_chip.set_key(0x8 ,action);
			break ;
		case 'd':
			this->_chip.set_key(0x9 ,action);
			break ;
		case 'f':
			this->_chip.set_key(0xE ,action);
			break ;

		case 'z':
			this->_chip.set_key(0xA ,action);
			break ;
		case 'x':
			this->_chip.set_key(0x0 ,action);
			break ;
		case 'c':
			this->_chip.set_key(0xB ,action);
			break ;
		case 'v':
			this->_chip.set_key(0xF ,action);
			break ;
	}
}

int	game_window::main_loop()
{
	// std::cerr << "test" << std::flush << std::endl; 
	SDL_Init(SDL_INIT_VIDEO);
	
	this->_window = SDL_CreateWindow("chip8 interpreter", 0, 0, VIDEO_WIDTH * 10, VIDEO_HEIGHT * 10, SDL_WINDOW_SHOWN);
	this->_renderer = SDL_CreateRenderer(this->_window, -1, SDL_RENDERER_ACCELERATED);

	this->_texture = SDL_CreateTexture(this->_renderer, SDL_PIXELFORMAT_RGBA8888,SDL_TEXTUREACCESS_STREAMING,
		VIDEO_WIDTH, VIDEO_HEIGHT);
	int videoPitch = sizeof(uint32_t) * VIDEO_WIDTH;

	auto lastCycleTime = std::chrono::high_resolution_clock::now();
	bool	close = false;
	SDL_Event	e;
	while (!close)
	{  
		while (SDL_PollEvent(&e))
		{
			if (e.type == SDL_QUIT)
				close = true;
			else if (e.type == SDL_KEYDOWN || e.type == SDL_KEYUP)
				this->process_key(e.key.keysym.sym, e.type);
    	}
		auto currentTime = std::chrono::high_resolution_clock::now();
		float dt = std::chrono::duration<float, std::chrono::milliseconds::period>(currentTime - lastCycleTime).count();

		int delay = 0;

		if (dt > delay) // TODO: no magic number
		{

			lastCycleTime = currentTime;
			
			SDL_UpdateTexture(this->_texture, nullptr, this->_chip._video, videoPitch);
			SDL_RenderClear(this->_renderer);
			SDL_RenderCopy(this->_renderer, this->_texture, nullptr, nullptr);
			SDL_RenderPresent(this->_renderer);	

			// calculates to 60 fps
			// SDL_Delay(1000 / 60);
			this->_chip.cycle();
			DBG (
				static unsigned int counter = 0;
				std::cerr << "counter= " << counter++ << std::endl << std::flush;
				if (counter > 340)
					delay = 6000;
			)
			DBG ( this->_chip.print_registers(); )
		}
	}
	return (0);
}
