#include <iostream>
#include <fstream>
#include <exception>
#include <string>
#include <iomanip>
#include "chip8.hpp"
#include "font.hpp"
#include "debug.hpp"

chip8::chip8()
	:	_pc(MEMORY_ROM_ADDR), _memory(), _stack(), _keypad(),
		_rand_gen(std::chrono::system_clock::now().time_since_epoch().count()),
		_rand_byte(std::uniform_int_distribution<uint8_t>(0, 255U))
{
	this->_init_OP_PTRs();
	memset(this->_video, 0, sizeof(this->_video));
}

chip8::chip8(const char *rom_filename) : _memory(), _stack(), _keypad()
{
	this->_rom_filename = rom_filename;
	this->_pc = MEMORY_ROM_ADDR;

	this->_rand_byte = std::uniform_int_distribution<uint8_t>(0, 255U);

	std::memcpy(&this->_memory[MEMORY_FONT_ADDR], fontset, FONTSET_SIZE);
	memset(this->_video, 0, sizeof(this->_video));
	DBG (
		dbg::print_memory(this->_memory, MEMORY_SIZE);
	)
	this->_init_OP_PTRs();
}

chip8::~chip8() {};

void	chip8::load_rom(const char *rom_filename)
{
	if (rom_filename == nullptr)
		rom_filename = this->_rom_filename;
	if (rom_filename == nullptr)
		throw "Couldn't open ROM file\n";

	std::ifstream file(rom_filename, std::ios::binary | std::ios::ate);

	if (file.is_open())
	{
		std::streampos size = file.tellg();
		if (size > MAX_ROM_SIZE)
		{
			std::string error = "ROM file is too large. Max size is ";
			error += std::to_string(MAX_ROM_SIZE);
			error += " bytes";
			throw error.c_str();
		}

		char *buff = new char[size];

		file.seekg(0, std::ios::beg);
		file.read(buff, size);
		file.close();

		for (size_t i = 0; i < size; ++i)
			this->_memory[MEMORY_ROM_ADDR + i] = buff[i];
		
		delete[] buff;

		DBG (
			std::cerr << size << std::endl;
		)
	}
	else
		throw "Couldn't open ROM file. No such file or directory";
		
	DBG (
		dbg::print_memory(this->_memory, sizeof(this->_memory));
	)
}

void	chip8::_init_OP_PTRs()
{	
	this->OP_PTR_DXXX[0x1] = &chip8::OP_1NNN;
	this->OP_PTR_DXXX[0x2] = &chip8::OP_2NNN;
	this->OP_PTR_DXXX[0x3] = &chip8::OP_3XKK;
	this->OP_PTR_DXXX[0x4] = &chip8::OP_4XKK;
	this->OP_PTR_DXXX[0x5] = &chip8::OP_5XY0;
	this->OP_PTR_DXXX[0x6] = &chip8::OP_6XKK;
	this->OP_PTR_DXXX[0x7] = &chip8::OP_7XKK;
	this->OP_PTR_DXXX[0xA] = &chip8::OP_ANNN;
	this->OP_PTR_DXXX[0xB] = &chip8::OP_BNNN;
	this->OP_PTR_DXXX[0xC] = &chip8::OP_CXKK;
	this->OP_PTR_DXXX[0xD] = &chip8::OP_DXYN;

	this->OP_PTR_8XXD[0x0] = &chip8::OP_8XY0;	// naming problem conflict with 0xD!!
	this->OP_PTR_8XXD[0x1] = &chip8::OP_8XY1;
	this->OP_PTR_8XXD[0x2] = &chip8::OP_8XY2;
	this->OP_PTR_8XXD[0x3] = &chip8::OP_8XY3;
	this->OP_PTR_8XXD[0x4] = &chip8::OP_8XY4;
	this->OP_PTR_8XXD[0x5] = &chip8::OP_8XY5;
	this->OP_PTR_8XXD[0x6] = &chip8::OP_8XY6;
	this->OP_PTR_8XXD[0x7] = &chip8::OP_8XY7;
	this->OP_PTR_8XXD[0xE] = &chip8::OP_8XYE;

	this->OP_PTR_00ED[0x0] = &chip8::OP_00E0;
	this->OP_PTR_00ED[0xE] = &chip8::OP_00EE;

	this->OP_PTR_EXDD[0x1] = &chip8::OP_EXA1;
	this->OP_PTR_EXDD[0xE] = &chip8::OP_EX9E;

	this->OP_PTR_FXDD[0x3] = &chip8::OP_FX33;
	this->OP_PTR_FXDD[0x7] = &chip8::OP_FX07;
	this->OP_PTR_FXDD[0x8] = &chip8::OP_FX18;
	this->OP_PTR_FXDD[0x9] = &chip8::OP_FX29;
	this->OP_PTR_FXDD[0xA] = &chip8::OP_FX0A;
	this->OP_PTR_FXDD[0XE] = &chip8::OP_FX1E;

	this->OP_PTR_FXD5[0x1] = &chip8::OP_FX15;
	this->OP_PTR_FXD5[0x5] = &chip8::OP_FX55;
	this->OP_PTR_FXD5[0x6] = &chip8::OP_FX65;
}

/*
**	OPCODES list
*/

// CLS
void	chip8::OP_00E0() {
	DBG ( std::cerr << __PRETTY_FUNCTION__ << std::endl; )
	std::memset(this->_video, 0, sizeof(this->_video));
}

// RET
void	chip8::OP_00EE() {
	DBG ( std::cerr << __PRETTY_FUNCTION__ << std::endl; )
	--this->_sp;
	this->_pc = this->_stack[this->_sp];
}

// JUMP to byte
void	chip8::OP_1NNN() {
	DBG ( std::cerr << __PRETTY_FUNCTION__ << std::endl; )
	this->_pc = this->_opcode & 0x0FFFu;
}

// 
void	chip8::OP_2NNN() {
	DBG (std::cerr << __PRETTY_FUNCTION__ << std::endl; )
	this->_stack[this->_sp] = this->_pc;
	++this->_sp; 
	this->_pc = this->_opcode & 0x0FFFu; 
}

void	chip8::OP_3XKK() {
	DBG ( std::cerr << __PRETTY_FUNCTION__ << std::endl; )
	uint8_t Vx = 0x0Fu & (this->_opcode >> 8u);
	uint8_t byte = 0xFFu & this->_opcode;
	if (this->_registers[Vx] == byte)
		this->_pc += 2;
}

void	chip8::OP_4XKK() {
	DBG ( std::cerr << __PRETTY_FUNCTION__ << std::endl; )
	uint8_t Vx = 0x0Fu & (this->_opcode >> 8u);
	uint8_t byte = 0xFFu & this->_opcode;
	if (this->_registers[Vx] != byte)
		this->_pc += 2;
}

void	chip8::OP_5XY0() {
	DBG ( std::cerr << __PRETTY_FUNCTION__ << std::endl; )
	uint8_t Vx = 0x0F & (this->_opcode >> 8u);
	uint8_t Vy = (0xF0 & (this->_opcode)) >> 8u;

	if (this->_registers[Vx] == this->_registers[Vy])
		this->_pc += 2;
}

void	chip8::OP_6XKK() {
	DBG ( std::cerr << __PRETTY_FUNCTION__ << std::endl; )
	std::cerr << std::hex << (this->_opcode >> 8u);
	uint8_t byte = 0xFFu & this->_opcode;
	uint8_t	Vx = 0x0Fu & (this->_opcode >> 8u);

	this->_registers[Vx] = byte;
}

void	chip8::OP_7XKK() {
	DBG ( std::cerr << __PRETTY_FUNCTION__ << std::endl; )
	uint8_t byte = 0xFFu & this->_opcode;
	uint8_t	Vx = 0x0Fu & (this->_opcode >> 8u);

	this->_registers[Vx] += byte;
}

void	chip8::OP_8XY0() {
	DBG ( std::cerr << __PRETTY_FUNCTION__ << std::endl; )
	uint8_t Vx = 0x0Fu & (this->_opcode >> 8u);
	uint8_t Vy = 0x0Fu & (this->_opcode >> 4u);

	this->_registers[Vx] = this->_registers[Vy];
}

void	chip8::OP_8XY1() {
	DBG ( std::cerr << __PRETTY_FUNCTION__ << std::endl; )
	uint8_t Vx = 0x0Fu & (this->_opcode >> 8u);
	uint8_t Vy = 0x0Fu & (this->_opcode >> 4u);

	this->_registers[Vx] |= this->_registers[Vy];
}

void	chip8::OP_8XY2() {
	DBG ( std::cerr << __PRETTY_FUNCTION__ << std::endl; )
	uint8_t Vx = 0x0Fu & (this->_opcode >> 8u);
	uint8_t Vy = 0x0Fu & (this->_opcode >> 4u);

	this->_registers[Vx] &= this->_registers[Vy];
}

void	chip8::OP_8XY3() {
	DBG ( std::cerr << __PRETTY_FUNCTION__ << std::endl; )
	uint8_t Vx = 0x0Fu & (this->_opcode >> 8u);
	uint8_t Vy = 0x0Fu & (this->_opcode >> 4u);

	this->_registers[Vx] ^= this->_registers[Vy];
}

void	chip8::OP_8XY4() {
	DBG ( std::cerr << __PRETTY_FUNCTION__ << std::endl; )
	uint8_t Vx = 0x0Fu & (this->_opcode >> 8u);
	uint8_t Vy = 0x0Fu & (this->_opcode >> 4u);

	uint16_t	sum = (this->_registers[Vx] + this->_registers[Vy]);

	if (sum > 255)
	{
		this->_registers[0xF] = 1;
	}
	else
		sum = 0;
	this->_registers[Vx] = sum;
}

void	chip8::OP_8XY5() {
	DBG ( std::cerr << __PRETTY_FUNCTION__ << std::endl; )
	uint8_t Vx = 0x0Fu & (this->_opcode >> 8u);
	uint8_t Vy = 0x0Fu & (this->_opcode >> 4u);

	if (this->_registers[Vx] > this->_registers[Vy])
		this->_registers[0xF] = 1;
	else
		this->_registers[0xF] = 0;
	this->_registers[Vx] -= this->_registers[Vy];
}

void	chip8::OP_8XY6() {
	DBG ( std::cerr << __PRETTY_FUNCTION__ << std::endl; )
	uint8_t Vx = 0x0Fu & (this->_opcode >> 8u);
	uint8_t Vy = 0x0Fu & (this->_opcode >> 4u);

	if (Vx & 0x1)
		this->_registers[0xF] = 1;
	else
		this->_registers[0x0] = 0;
	this->_registers[Vx] >>= 1;
}

void	chip8::OP_8XY7() {
	DBG ( std::cerr << __PRETTY_FUNCTION__ << std::endl; )
	uint8_t Vx = 0x0Fu & (this->_opcode >> 8u);
	uint8_t Vy = 0x0Fu & (this->_opcode >> 4u);

	if (this->_registers[Vx] > this->_registers[Vy])
		this->_registers[0xF] = 1;
	else
		this->_registers[0xF] = 0;
	this->_registers[Vx] -= this->_registers[Vy];
}

void	chip8::OP_8XYE() {
	DBG ( std::cerr << __PRETTY_FUNCTION__ << std::endl; )
	uint8_t Vx = 0x0Fu & (this->_opcode >> 8u);
	uint8_t Vy = 0x0Fu & (this->_opcode >> 4u);

	this->_registers[0xF] = Vx & 1;
	this->_registers[Vx] <<= 1;
}

void	chip8::OP_9XY0() {
	DBG ( std::cerr << __PRETTY_FUNCTION__ << std::endl; )
	uint8_t Vx = 0x0F & (this->_opcode >> 8u);
	uint8_t Vy = 0x0Fu & (this->_opcode >> 4u);

	if (this->_registers[Vx] != this->_registers[Vy])
		this->_pc += 2;
}

void	chip8::OP_ANNN() {
	DBG ( std::cerr << __PRETTY_FUNCTION__ << std::endl; )
	uint16_t	addr = this->_opcode & 0x0FFFu;

	this->_index = addr; 
}

void	chip8::OP_BNNN() {
	DBG ( std::cerr << __PRETTY_FUNCTION__ << std::endl; )
	uint16_t	addr = this->_opcode & 0xFFFu;

	this->_pc = this->_registers[0] + addr;
}

void	chip8::OP_CXKK() {
	DBG ( std::cerr << __PRETTY_FUNCTION__ << std::endl; )
	uint8_t Vx = 0xFu & (this->_opcode >> 8u);
	uint8_t	byte = this->_opcode & 0xFFu;

	this->_registers[Vx] = this->_rand_byte(this->_rand_gen) & byte;
}

void	chip8::OP_DXYN() {
	DBG ( std::cerr << __PRETTY_FUNCTION__ << std::endl; )
	uint8_t Vx = 0x0Fu & (this->_opcode >> 8u);
	uint8_t Vy = 0x0Fu & (this->_opcode >> 4u);
	uint8_t	height = this->_opcode & 0x0Fu;

	uint8_t xPos = this->_registers[Vx] % VIDEO_WIDTH;
	uint8_t yPos = this->_registers[Vy] % VIDEO_HEIGHT;

	this->_registers[0xF] = 0;

	for (size_t row = 0; row < height; ++row)
	{
		uint8_t sprite_byte = this->_memory[this->_index + row];

		for (size_t col = 0; col < 8; ++col)
		{
			// 128 == 0x80 == 10000000 -> first bit
			uint8_t			sprite_pixel = sprite_byte & (128 >> col); 
			unsigned int	i_screen_pixel = (yPos + row) * VIDEO_WIDTH + (xPos + col);

			if (sprite_pixel)
			{
				if (this->_video[i_screen_pixel] == 0xFFFFFFFFu)
					this->_registers[0xF] = 1;
				
				this->_video[i_screen_pixel] ^= 0xFFFFFFFFu;
			}
		}
	}
}

void	chip8::OP_EX9E() {
	DBG ( std::cerr << __PRETTY_FUNCTION__ << std::endl; )
	uint8_t Vx = 0xF & (this->_opcode >> 8u);
	uint8_t	key = this->_registers[Vx];

	if (this->_keypad[key])
		++this->_pc;
}

void	chip8::OP_EXA1() {
	DBG (
		std::cerr << __PRETTY_FUNCTION__ << std::endl;
	)
	
	uint8_t Vx = 0xF & (this->_opcode >> 8u);
	uint8_t	key = this->_registers[Vx];

	if (this->_keypad[key])
		this->_pc += 2;
}

void	chip8::OP_FX07() {
	DBG ( std::cerr << __PRETTY_FUNCTION__ << std::endl; )
	uint8_t Vx = 0xF & (this->_opcode >> 8u);

	this->_registers[Vx] = this->_delay_timer;
}

void	chip8::OP_FX0A() {
	DBG ( std::cerr << __PRETTY_FUNCTION__ << std::endl; )
	uint8_t Vx = 0xF & (this->_opcode >> 8u);
	if (this->_keypad[0])
	{
		this->_registers[Vx] = 0;
	}
	else if (this->_keypad[1])
	{
		this->_registers[Vx] = 1;
	}
	else if (this->_keypad[2])
	{
		this->_registers[Vx] = 2;
	}
	else if (this->_keypad[3])
	{
		this->_registers[Vx] = 3;
	}
	else if (this->_keypad[4])
	{
		this->_registers[Vx] = 4;
	}
	else if (this->_keypad[5])
	{
		this->_registers[Vx] = 5;
	}
	else if (this->_keypad[6])
	{
		this->_registers[Vx] = 6;
	}
	else if (this->_keypad[7])
	{
		this->_registers[Vx] = 7;
	}
	else if (this->_keypad[8])
	{
		this->_registers[Vx] = 8;
	}
	else if (this->_keypad[9])
	{
		this->_registers[Vx] = 9;
	}
	else if (this->_keypad[10])
	{
		this->_registers[Vx] = 10;
	}
	else if (this->_keypad[11])
	{
		this->_registers[Vx] = 11;
	}
	else if (this->_keypad[12])
	{
		this->_registers[Vx] = 12;
	}
	else if (this->_keypad[13])
	{
		this->_registers[Vx] = 13;
	}
	else if (this->_keypad[14])
	{
		this->_registers[Vx] = 14;
	}
	else if (this->_keypad[15])
	{
		this->_registers[Vx] = 15;
	}
	else
	{
		this->_pc -= 2;
	}
}

void	chip8::OP_FX15() {
	DBG ( std::cerr << __PRETTY_FUNCTION__ << std::endl; )
	uint8_t	Vx = 0xFu & this->_opcode >> 8u;
	uint8_t digit = this->_registers[Vx];

	this->_index = MEMORY_FONT_ADDR + (5 * digit);
}

// To implement later!
void	chip8::OP_FX18() {
	DBG ( std::cerr << __PRETTY_FUNCTION__ << std::endl; )
	uint8_t	Vx = 0xFu & (this->_opcode >> 8u);

	// this->_sounder_timer = this->_registers[Vx]
}

void	chip8::OP_FX1E() {
	DBG ( std::cerr << __PRETTY_FUNCTION__ << std::endl; )
	uint8_t	Vx = 0xF & (this->_opcode >> 8u);

	this->_index += this->_registers[Vx];
}

void	chip8::OP_FX29()
{
	DBG ( std::cerr << __PRETTY_FUNCTION__ << std::endl; )
	uint8_t	Vx = 0xF & (this->_opcode >> 8u);
	uint8_t digit = this->_registers[Vx];

	this->_index = MEMORY_FONT_ADDR + (5 * digit);
}

void	chip8::OP_FX33()
{
	DBG ( std::cerr << __PRETTY_FUNCTION__ << std::endl; )
	uint8_t	Vx = 0xF & (this->_opcode >> 8u);
	uint8_t value = this->_registers[Vx];

	// Units
	this->_memory[this->_index + 2] = value % 10;
	value /= 10;

	// Tens
	this->_memory[this->_index + 1] = value % 10;
	value /= 10;

	// Hundreds
	this->_memory[this->_index] = value % 10;
}

void	chip8::OP_FX55()
{
	DBG ( std::cerr << __PRETTY_FUNCTION__ << std::endl; )
	uint8_t Vx = (this->_opcode >> 8u) & 0xFu;

	for (uint8_t i = 0; i <= Vx; ++i)
		this->_memory[this->_index + i] = this->_registers[i];
}

void 	chip8::OP_FX65()
{
	DBG ( std::cerr << __PRETTY_FUNCTION__ << std::endl; )
	uint8_t Vx = (this->_opcode >> 8u) & 0xFu;

	for (uint8_t i = 0; i <= Vx; ++i)
		this->_registers[i] = this->_memory[this->_index + i];
}

// Fetch, Decode, Execute
void	chip8::cycle()
{
	uint16_t	*op_ptr;
	uint8_t		byte1;
	uint8_t		byte2;

	DBG ( std::cerr << "pc= " << this->_pc << std::endl; )
	op_ptr = reinterpret_cast<uint16_t *>(this->_memory + this->_pc);

	// stream is big endian, our platform is lil endian (no offense)
	byte1 = *op_ptr & 0xFFu;
	byte2 = (*op_ptr & 0xFF00u) >> 8u;
	this->_opcode = (byte1 << 8u) + byte2;
	this->_pc += 2;
	DBG (
		std::cout << std::hex << "pc: " << std::hex << this->_pc << std::flush << std::endl;
		std::cerr << std::hex << (int)byte1 << " " << std::hex << (int)byte2 << std::endl;
		std::cerr << "Big-endian opcode: " << std::hex << this->_opcode << std::endl;
		std::cerr << std::dec;
	)
	uint16_t	first_digit = (this->_opcode & 0xF000u) >> 12u;
	uint16_t	last_digit = (this->_opcode & 0x000Fu);
	uint16_t	first_3_digits = (this->_opcode & 0xFFF0u) >> 4u;
	uint16_t	last_2_digits = this->_opcode & 0x00FFu;
	uint16_t	third_digit = (this->_opcode & 0x00F0) >> 4u;

	DBG (
		std::cerr << "first_digit = " << std::hex << (int)first_digit << ", ";
		std::cerr << "last_digit = " << std::hex << (int)last_digit << ", ";
		std::cerr << "first_3_digits = " << std::hex << (int)first_3_digits << ", ";
		std::cerr << "last_2_digits = " << std::hex << (int)last_2_digits;
		std::cerr << std::endl << std::dec;
	)

	// execute instructions
	if ((first_digit >= 0x1 && first_digit <= 0x7) || (first_digit >= 0xA && first_digit <= 0xD))
	{
		if (OP_PTR_DXXX[first_digit])	// ADD TESTS
			((*this).*(OP_PTR_DXXX[first_digit]))();
	}
	else if (0x8u == first_digit)
	{
		if (OP_PTR_8XXD[last_digit])
			((*this).*(OP_PTR_8XXD[last_digit]))();
	}
	else if (0x00Eu == first_3_digits)
	{
		if (OP_PTR_00ED[last_digit])
			((*this).*(OP_PTR_00ED[last_digit]))();
	}
	else if (0xEu == first_digit)
	{
		if (OP_PTR_EXDD[last_digit])
			((*this).*(OP_PTR_EXDD[last_digit]))();
	}
	else if (0xFu == first_digit && 0x5u != last_digit)
	{
		if (OP_PTR_FXDD[last_digit])
			((*this).*(OP_PTR_FXDD[last_digit]))();
	}
	else if (0xFu == first_digit && 0x5u == last_digit)
	{
		if (OP_PTR_FXD5[third_digit])
			((*this).*(OP_PTR_FXD5[third_digit]))();
	}

	if (this->_delay_timer > 0)
		--this->_delay_timer;

	// if (this->_sound_timer)
	// 	--this->_sound_timer;

	DBG (
		// dbg::print_memory(this->_memory, sizeof(this->_memory));
	)
}

void	chip8::set_key(uint8_t key, bool value)
{
	if (key < 0 || key > 0xF)
		return ;

	this->_keypad[key] = value;

	DBG (
		std::cerr << "--------" << std::endl;
		for (size_t i = 0; i < 16; i++)
		{
			if (i != 0 && i % 4 == 0)
				std::cerr << std::endl;
			std::cerr << (this->_keypad[i] ? "X" : ".") << " ";
		}
		std::cerr << "\n--------" << std::endl << std::flush;
	)
}

void chip8::chip8::print_registers()
{
    std::cerr << "SP: " << (int)this->_sp << "Index: " << this->_index << std::endl;
    std::cerr << "PC: " << (int)this->_pc << "Index: " << this->_index << std::endl;
	for (size_t i = 0; i < sizeof(this->_registers); ++i)
	{
		if (i != 0 && i % 4 == 0)
			std::cerr << std::endl;
		std::cerr << "F" << i << "=" << static_cast<int>(this->_registers[i]) << "\t";
	}
}
