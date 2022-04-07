#include <iostream>
#include <iomanip>
#include <cmath>
#include "debug.hpp"

void	dbg::print_memory(const unsigned char *memory, size_t mem_size)
{
	#define WIDTH 32
	size_t addr_width = ( log(mem_size) / log(16) );
	std::cerr << "-MEMORY " << std::string(WIDTH * 3 + 3 + addr_width - strlen("-MEMORY "), '-') << std::endl;
	for (int i = 0; i < mem_size; ++i)
	{
		if (i % (WIDTH) == 0)
		{
			if (i != 0)
				std::cerr << std::endl;
			std::cerr << std::dec;
			std::cerr << "0x" << std::setfill('0') << std::setw((int)addr_width) << std::hex << i << "  ";
		}
		std::string n_pad = i % (WIDTH) == 0 ? "" : " ";
		n_pad += (memory[i] <= 0xF) ? "0" : "";
		std::cerr << n_pad << std::hex << static_cast<unsigned int>(memory[i]); 
	}
	std::cerr << std::endl << std::string(WIDTH * 3 + 3 + addr_width, '-') << std::endl;
	std::cerr << std::dec;
}
