# ifndef CHIP8_HPP
# define CHIP8_HPP

#include <string>
#include <cstdint>
#include <random>
#include <chrono>

#define MEMORY_SIZE 		4096
#define	MAX_ROM_SIZE		3000
#define VIDEO_WIDTH			64	
#define VIDEO_HEIGHT		32
#define MEMORY_ROM_ADDR		0x200
#define MEMORY_FONT_ADDR	0x050

# ifdef DEBUG
	# define DBG(X) do { X } while (0);
# else
	# define DBG(X)
# endif

/*	0x000-0x1FF: Never write or read to that area (except for 0x050-0x0A0: storage space for the 16 built-in chars)
	0x200-0xFFF: Instructions from the ROM will be stored starting at 0x200. Anything left is free to use	*/

class chip8
{
	private:
		uint16_t		_opcode;

		unsigned char	_memory[MEMORY_SIZE];
		
		unsigned char	_registers[16];
		uint16_t		_index;
		uint16_t		_pc;
		unsigned char	_sp;
		uint16_t		_stack[16];
		
		uint8_t			_delay_timer;
		
		unsigned char	_keypad[16];

		const char *	_rom_filename;

		std::default_random_engine				_rand_gen;
		std::uniform_int_distribution<uint8_t> _rand_byte;	
		
		// OPCODES
		void	OP_NOP();
	
		void	OP_00E0();
		void	OP_00EE();
		void	OP_1NNN();
		void	OP_2NNN();
		void	OP_3XKK();
		void	OP_4XKK();
		void	OP_5XY0();
		void	OP_6XKK();
		void	OP_7XKK();
		void	OP_8XY0();
		void	OP_8XY1();
		void	OP_8XY2();
		void	OP_8XY3();
		void	OP_8XY4();
		void	OP_8XY5();
		void	OP_8XY6();
		void	OP_8XY7();
		void	OP_8XYE();
		void	OP_9XY0();
		void	OP_ANNN();
		void	OP_BNNN();
		void	OP_CXKK();
		void	OP_DXYN();
		void	OP_EX9E();
		void	OP_EXA1();
		void	OP_FX07();
		void	OP_FX0A();
		void	OP_FX15();
		void	OP_FX18();
		void	OP_FX1E();
		void	OP_FX29();
		void	OP_FX33();
		void	OP_FX55();
		void	OP_FX65();

		// TABLE POINTERS
		// (opcode >= 0x1XXX && opcode <= 0x7XXX) || (opcode >= 0x9XXX && opcode <= 0xDXXX)
		void	(chip8::*OP_PTR_DXXX[16])();
		void	(chip8::*OP_PTR_8XXD[16])();
		void	(chip8::*OP_PTR_00ED[16])();
		void	(chip8::*OP_PTR_EXDD[16])();
		void	(chip8::*OP_PTR_FXDD[16])();
		void	(chip8::*OP_PTR_FXD5[16])();

		// utils
		void	_init_OP_PTRs();

	public:
		chip8();
		chip8(const char *rom_filename);
		~chip8();
		void	load_rom(const char *rom_filename = nullptr);

		uint32_t		_video[VIDEO_WIDTH * VIDEO_HEIGHT]; // TODO: should be private, add a getter
		// // HUMAN-READABLE OPCODES ALIASES
		// inline void	OP_CLS() { OP_00E0(); };
		// inline void	OP_RET() { OP_00EE(); };
		// inline void	OP_JP_addr() { OP_1NNN(); };
		// inline void	OP_CALL_addr() { OP_2NNN(); };
		// inline void	OP_SKIP_Vx_eq_byte() { OP_3XKK(); };
		// inline void	OP_SKIP_Vx_ne_byte() { OP_4XKK(); };
		// inline void	OP_SKIP_Vx_eq_Vy() { OP_5XY0(); };
		// inline void	OP_SET_Vx_byte() { OP_6XKK(); };
		// inline void	OP_ADD_Vx_byte() { OP_7XKK(); };
		// inline void	OP_SET_Vx_Vy() { OP_8XY0(); };
		// inline void	OP_SET_Vx_Vx_or_Vy() { OP_8XY1(); };
		// inline void	OP_SET_Vx_Vx_and_Vy() { OP_8XY2(); };
		// inline void	OP_SET_Vx_Vx_xor_Vy() { OP_8XY3(); };
		// inline void	OP_ADD_Vx_Vy__SET_Vf_carry() { OP_8XY4(); };
		// inline void	OP_SET_Vx_Vx_minus_Vy__SET_Vf_not_borrow() { OP_8XY5(); };
		// inline void	OP_SET_Vx_Vx_SHR_1__SET_Vf_lst_bit_Vx() { OP_8XY6(); };
		// inline void	OP_SET_Vx_Vy_minus_Vx__SET_Vf_not_borrow() { OP_8XY7(); };
		// inline void	OP_SET_Vx_SHL_1__SET_Vf_lst_bit_Vx() { OP_8XYE(); };
		// inline void	OP_SKIP_Vx_ne_Vy() { OP_9XY0(); };
		// inline void OP_SET_I_NNN() { OP_ANNN(); };
		// inline void OP_JMP_NNN_plus_V0() { OP_BNNN(); };
		// inline void OP_SET_Vx_RAND_and_byte() { OP_CXKK(); };
		
		// Fetch, Decode, Execute! (turn on/ tune in/ drop out)
		void	cycle();

		void	set_key(uint8_t key, bool value);

		// Debug
		void	print_registers();
};

# endif
