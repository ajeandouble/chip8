#include <iostream>
#include "chip8.hpp"
#include "loop.hpp"

int	main(int ac, char **av)
{
	if (ac != 2)
	{
		std::cout << "Usage : " << av[0] << " rom_file" << std::endl;
		exit(0);
	}
	try {
		chip8 chip(av[1]);
		chip.load_rom();
		game_window	gw(chip);
		gw.main_loop();
	}
	catch(const char *msg) {
		std::cerr << msg << std::endl;
		std::cerr << msg << std::endl;
		return (-0x2a);
	}
	return (0);
}
