/*
This file is part of VSGBE 
Copyright (c) 2020 Adriano Emidio

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
*/

#ifndef CPU_H_
#define CPU_H_

#include <cstdint>
#include "regFile.h"
#include "ioRegAddr.h"
#include "mmu.h"

//Estados da CPU
enum cpu_s {OFF = 1, RUN = 1, STOP = 2, HALT = 3};



//TODO: Melhor a decodificação das instruções
class SimpleZ80
{
private:

	//Ponteiros de funções com dois argumentos
	typedef void (SimpleZ80::*function_pointer) (uint8_t* opcode);

	//registers reg_file;
	bool IME;
	uint8_t cpu_state;
	uint8_t clk_elapsed;
	Mmu* mem;
	//Ponteiro para instruções da CPU
	//TODO:Talvez tonar isso estático...
	function_pointer instr_array[256] =
	{
			//0x00
			&SimpleZ80::nop,
			//0x01
			&SimpleZ80::ldr16d,
			//0x02
			&SimpleZ80::ldp16a,
			//0x03
			&SimpleZ80::incr16,
			//0x04
			&SimpleZ80::incr8,
			//0x05
			&SimpleZ80::decr8,
			//0x06
			&SimpleZ80::ldr8d,
			//0x07
			&SimpleZ80::rlca,
			//0x08
			&SimpleZ80::ldpsp,
			//0x09
			&SimpleZ80::ldhlr16,
			//0x0A
			&SimpleZ80::ldap16,
			//0x0B
			&SimpleZ80::decr16,
			//0x0C
			&SimpleZ80::incr8,
			//0x0D
			&SimpleZ80::decr8,
			//0x0E
			&SimpleZ80::ldr8d,
			//0x0F
			&SimpleZ80::rrca,
			//0x10
			&SimpleZ80::stop,
			//0x11
			&SimpleZ80::ldr16d,
			//0x12
			&SimpleZ80::ldp16a,
			//0x13
			&SimpleZ80::incr16,
			//0x14
			&SimpleZ80::incr8,
			//0x15
			&SimpleZ80::decr8,
			//0x16
			&SimpleZ80::ldr8d,
			//0x17
			&SimpleZ80::rla,
			//0x18
			&SimpleZ80::jre8,
			//0x19
			&SimpleZ80::ldhlr16,
			//0x1a
			&SimpleZ80::ldap16,
			//0x1b
			&SimpleZ80::decr16,
			//0x1c
			&SimpleZ80::incr8,
			//0x1d
			&SimpleZ80::decr8,
			//0x1e
			&SimpleZ80::ldr8d,
			//0x1f
			&SimpleZ80::rra,
			//0x20
			&SimpleZ80::jrnze8,
			//0x21
			&SimpleZ80::ldr16d,
			//0x22
			&SimpleZ80::ldp16a,
			//0x23
			&SimpleZ80::incr16,
			//0x24
			&SimpleZ80::incr8,
			//0x25
			&SimpleZ80::decr8,
			//0x26
			&SimpleZ80::ldr8d,
			//0x27
			&SimpleZ80::daa,
			//0x28
			&SimpleZ80::jrze8,
			//0x29
			&SimpleZ80::ldhlr16,
			//0x2a
			&SimpleZ80::ldap16,
			//0x2b
			&SimpleZ80::decr16,
			//0x2c
			&SimpleZ80::incr8,
			//0x2d
			&SimpleZ80::decr8,
			//0x2e
			&SimpleZ80::ldr8d,
			//0x2f
			&SimpleZ80::cpl,
			//0x30
			&SimpleZ80::jrnce8,
			//0x31
			&SimpleZ80::ldr16d,
			//0x32
			&SimpleZ80::ldp16a,
			//0x33
			&SimpleZ80::incr16,
			//0x34
			&SimpleZ80::incr8,
			//0x35
			&SimpleZ80::decr8,
			//0x36
			&SimpleZ80::ldr8d,
			//0x37
			&SimpleZ80::scf,
			//0x38
			&SimpleZ80::jrce8,
			//0x39
			&SimpleZ80::ldhlr16,
			//0x3a
			&SimpleZ80::ldap16,
			//0x3b
			&SimpleZ80::decr16,
			//0x3c
			&SimpleZ80::incr8,
			//0x3d
			&SimpleZ80::decr8,
			//0x3e
			&SimpleZ80::ldr8d,
			//0x3f
			&SimpleZ80::ccf,
			//0x40
			&SimpleZ80::ldr8r8,
			//0x41
			&SimpleZ80::ldr8r8,
			//0x42
			&SimpleZ80::ldr8r8,
			//0x43
			&SimpleZ80::ldr8r8,
			//0x44
			&SimpleZ80::ldr8r8,
			//0x45
			&SimpleZ80::ldr8r8,
			//0x46
			&SimpleZ80::ldr8r8,
			//0x47
			&SimpleZ80::ldr8r8,
			//0x48
			&SimpleZ80::ldr8r8,
			//0x49
			&SimpleZ80::ldr8r8,
			//0x4a
			&SimpleZ80::ldr8r8,
			//0x4b
			&SimpleZ80::ldr8r8,
			//0x4c
			&SimpleZ80::ldr8r8,
			//0x4d
			&SimpleZ80::ldr8r8,
			//0x4e
			&SimpleZ80::ldr8r8,
			//0x4f
			&SimpleZ80::ldr8r8,
			//0x50
			&SimpleZ80::ldr8r8,
			//0x51
			&SimpleZ80::ldr8r8,
			//0x52
			&SimpleZ80::ldr8r8,
			//0x53
			&SimpleZ80::ldr8r8,
			//0x54
			&SimpleZ80::ldr8r8,
			//0x55
			&SimpleZ80::ldr8r8,
			//0x56
			&SimpleZ80::ldr8r8,
			//0x57
			&SimpleZ80::ldr8r8,
			//0x58
			&SimpleZ80::ldr8r8,
			//0x59
			&SimpleZ80::ldr8r8,
			//0x5a
			&SimpleZ80::ldr8r8,
			//0x5b
			&SimpleZ80::ldr8r8,
			//0x5c
			&SimpleZ80::ldr8r8,
			//0x5d
			&SimpleZ80::ldr8r8,
			//0x5e
			&SimpleZ80::ldr8r8,
			//0x5f
			&SimpleZ80::ldr8r8,
			//0x60
			&SimpleZ80::ldr8r8,
			//0x61
			&SimpleZ80::ldr8r8,
			//0x62
			&SimpleZ80::ldr8r8,
			//0x63
			&SimpleZ80::ldr8r8,
			//0x64
			&SimpleZ80::ldr8r8,
			//0x65
			&SimpleZ80::ldr8r8,
			//0x66
			&SimpleZ80::ldr8r8,
			//0x67
			&SimpleZ80::ldr8r8,
			//0x68
			&SimpleZ80::ldr8r8,
			//0x69
			&SimpleZ80::ldr8r8,
			//0x6a
			&SimpleZ80::ldr8r8,
			//0x6b
			&SimpleZ80::ldr8r8,
			//0x6c
			&SimpleZ80::ldr8r8,
			//0x6d
			&SimpleZ80::ldr8r8,
			//0x6e
			&SimpleZ80::ldr8r8,
			//0x6f
			&SimpleZ80::ldr8r8,
			//0x70
			&SimpleZ80::ldr8r8,
			//0x71
			&SimpleZ80::ldr8r8,
			//0x72
			&SimpleZ80::ldr8r8,
			//0x73
			&SimpleZ80::ldr8r8,
			//0x74
			&SimpleZ80::ldr8r8,
			//0x75
			&SimpleZ80::ldr8r8,
			//0x76
			&SimpleZ80::halt,
			//0x77
			&SimpleZ80::ldr8r8,
			//0x78
			&SimpleZ80::ldr8r8,
			//0x79
			&SimpleZ80::ldr8r8,
			//0x7a
			&SimpleZ80::ldr8r8,
			//0x7b
			&SimpleZ80::ldr8r8,
			//0x7c
			&SimpleZ80::ldr8r8,
			//0x7d
			&SimpleZ80::ldr8r8,
			//0x7e
			&SimpleZ80::ldr8r8,
			//0x7f
			&SimpleZ80::ldr8r8,
			//0x80
			&SimpleZ80::addr8a,
			//0x81
			&SimpleZ80::addr8a,
			//0x82
			&SimpleZ80::addr8a,
			//0x83
			&SimpleZ80::addr8a,
			//0x84
			&SimpleZ80::addr8a,
			//0x85
			&SimpleZ80::addr8a,
			//0x86
			&SimpleZ80::addr8a,
			//0x87
			&SimpleZ80::addr8a,
			//0x88
			&SimpleZ80::adcr8a,
			//0x89
			&SimpleZ80::adcr8a,
			//0x8a
			&SimpleZ80::adcr8a,
			//0x8b
			&SimpleZ80::adcr8a,
			//0x8c
			&SimpleZ80::adcr8a,
			//0x8d
			&SimpleZ80::adcr8a,
			//0x8e
			&SimpleZ80::adcr8a,
			//0x8f
			&SimpleZ80::adcr8a,
			//0x90
			&SimpleZ80::subr8a,
			//0x91
			&SimpleZ80::subr8a,
			//0x92
			&SimpleZ80::subr8a,
			//0x93
			&SimpleZ80::subr8a,
			//0x94
			&SimpleZ80::subr8a,
			//0x95
			&SimpleZ80::subr8a,
			//0x96
			&SimpleZ80::subr8a,
			//0x97
			&SimpleZ80::subr8a,
			//0x98
			&SimpleZ80::sbcr8a,
			//0x99
			&SimpleZ80::sbcr8a,
			//0x9a
			&SimpleZ80::sbcr8a,
			//0x9b
			&SimpleZ80::sbcr8a,
			//0x9c
			&SimpleZ80::sbcr8a,
			//0x9d
			&SimpleZ80::sbcr8a,
			//0x9e
			&SimpleZ80::sbcr8a,
			//0x9f
			&SimpleZ80::sbcr8a,
			//0xa0
			&SimpleZ80::andr8a,
			//0xa1
			&SimpleZ80::andr8a,
			//0xa2
			&SimpleZ80::andr8a,
			//0xa3
			&SimpleZ80::andr8a,
			//0xa4
			&SimpleZ80::andr8a,
			//0xa5
			&SimpleZ80::andr8a,
			//0xa6
			&SimpleZ80::andr8a,
			//0xa7
			&SimpleZ80::andr8a,
			//0xa8
			&SimpleZ80::xorr8a,
			//0xa9
			&SimpleZ80::xorr8a,
			//0xaa
			&SimpleZ80::xorr8a,
			//0xab
			&SimpleZ80::xorr8a,
			//0xac
			&SimpleZ80::xorr8a,
			//0xad
			&SimpleZ80::xorr8a,
			//0xae
			&SimpleZ80::xorr8a,
			//0xaf
			&SimpleZ80::xorr8a,
			//0xb0
			&SimpleZ80::orr8a,
			//0xb1
			&SimpleZ80::orr8a,
			//0xb2
			&SimpleZ80::orr8a,
			//0xb3
			&SimpleZ80::orr8a,
			//0xb4
			&SimpleZ80::orr8a,
			//0xb5
			&SimpleZ80::orr8a,
			//0xb6
			&SimpleZ80::orr8a,
			//0xb7
			&SimpleZ80::orr8a,
			//0xb8
			&SimpleZ80::cprr8a,
			//0xb9
			&SimpleZ80::cprr8a,
			//0xba
			&SimpleZ80::cprr8a,
			//0xbb
			&SimpleZ80::cprr8a,
			//0xbc
			&SimpleZ80::cprr8a,
			//0xbd
			&SimpleZ80::cprr8a,
			//0xbe
			&SimpleZ80::cprr8a,
			//0xbf
			&SimpleZ80::cprr8a,
			//0xc0
			&SimpleZ80::retnz,
			//0xc1
			&SimpleZ80::popr16,
			//0xc2
			&SimpleZ80::jpnzr16d,
			//0xc3
			&SimpleZ80::jp16d,
			//0xc4
			&SimpleZ80::callnzd,
			//0xc5
			&SimpleZ80::pushr16,
			//0xc6
			&SimpleZ80::addd8a,
			//0xc7
			&SimpleZ80::rst,
			//0xc8
			&SimpleZ80::retz,
			//0xc9
			&SimpleZ80::ret,
			//0xca
			&SimpleZ80::jpz16d,
			//0xcb
			&SimpleZ80::prefixcb,
			//0xcc
			&SimpleZ80::callzd,
			//0xcd
			&SimpleZ80::calld,
			//0xce
			&SimpleZ80::adcd8a,
			//0xcf
			&SimpleZ80::rst,
			//0xd0
			&SimpleZ80::retnc,
			//0xd1
			&SimpleZ80::popr16,
			//0xd2
			&SimpleZ80::jpnc16d,
			//0xd3
			&SimpleZ80::nop,
			//0xd4
			&SimpleZ80::callncd,
			//0xd5
			&SimpleZ80::pushr16,
			//0xd6
			&SimpleZ80::subd8a,
			//0xd7
			&SimpleZ80::rst,
			//0xd8
			&SimpleZ80::retc,
			//0xd9
			&SimpleZ80::reti,
			//0xda
			&SimpleZ80::jpc16d,
			//0xdb
			&SimpleZ80::nop,
			//0xdc
			&SimpleZ80::callcd,
			//0xdd
			&SimpleZ80::nop,
			//0xde
			&SimpleZ80::sbcd8a,
			//0xdf
			&SimpleZ80::rst,
			//0xe0
			&SimpleZ80::ldhd8a,
			//0xe1
			&SimpleZ80::popr16,
			//0xe2
			&SimpleZ80::ldpca,
			//0xe3
			&SimpleZ80::nop,
			//0xe4
			&SimpleZ80::nop,
			//0xe5
			&SimpleZ80::pushr16,
			//0xe6
			&SimpleZ80::andd8a,
			//0xe7
			&SimpleZ80::rst,
			//0xe8
			&SimpleZ80::addspd8,
			//0xe9
			&SimpleZ80::jpphl,
			//0xea
			&SimpleZ80::ldpd16a,
			//0xeb
			&SimpleZ80::nop,
			//0xec
			&SimpleZ80::nop,
			//0xed
			&SimpleZ80::nop,
			//0xee
			&SimpleZ80::xord8a,
			//0xef
			&SimpleZ80::rst,
			//0xf0
			&SimpleZ80::ldhad8,
			//0xf1
			&SimpleZ80::popr16,
			//0xf2
			&SimpleZ80::ldapc,
			//0xf3
			&SimpleZ80::di,
			//0xf4
			&SimpleZ80::nop,
			//0xf5
			&SimpleZ80::pushr16,
			//0xf6
			&SimpleZ80::ord8a,
			//0xf7
			&SimpleZ80::rst,
			//0xf8
			&SimpleZ80::ldhlspd8,
			//0xf9
			&SimpleZ80::ldsphl,
			//0xfa
			&SimpleZ80::ldapd16,
			//0xfb
			&SimpleZ80::ei,
			//0xfc
			&SimpleZ80::nop,
			//0xfd
			&SimpleZ80::nop,
			//0xfe
			&SimpleZ80::cpdd8a,
			//0xff
			&SimpleZ80::rst



	};

	//Ponteiro para instruções com prefixo 0xCB
	function_pointer cb_array[32] =
	{
			//0x00 a 0x07
			&SimpleZ80::rlcr8,
			//0x08 a 0x0F
			&SimpleZ80::rrcr8,
			//0x10 a 0x17
			&SimpleZ80::rlr8,
			//0x18 a 0x1F
			&SimpleZ80::rrr8,
			//0x20 a 0x27
			&SimpleZ80::slar8,
			//0x28 a 0x2F
			&SimpleZ80::srar8,
			//0x30 a 0x37
			&SimpleZ80::swapr8,
			//0x38 a 0x3F
			&SimpleZ80::srlr8,
			//0x40 a 0x47
			&SimpleZ80::bitr8,
			//0x48 a 0x4F
			&SimpleZ80::bitr8,
			//0x50 a 0x57
			&SimpleZ80::bitr8,
			//0x58 a 0x5F
			&SimpleZ80::bitr8,
			//0x60 a 0x67
			&SimpleZ80::bitr8,
			//0x68 a 0x6F
			&SimpleZ80::bitr8,
			//0x70 a 0x77
			&SimpleZ80::bitr8,
			//0x78 a 0x7F
			&SimpleZ80::bitr8,
			//0x80 a 0x87
			&SimpleZ80::resr8,
			//0x88 a 0x8F
			&SimpleZ80::resr8,
			//0x90 a 0x97
			&SimpleZ80::resr8,
			//0x98 a 0x9F
			&SimpleZ80::resr8,
			//0xA0 a 0xA7
			&SimpleZ80::resr8,
			//0xA8 a 0xAF
			&SimpleZ80::resr8,
			//0xB0 a 0xB7
			&SimpleZ80::resr8,
			//0xB8 a 0xBF
			&SimpleZ80::resr8,
			//0xC0 a 0xC7
			&SimpleZ80::setr8,
			//0xC8 a 0xCF
			&SimpleZ80::setr8,
			//0xD0 a 0xD7
			&SimpleZ80::setr8,
			//0xD8 a 0xDF
			&SimpleZ80::setr8,
			//0xE0 a 0xE7
			&SimpleZ80::setr8,
			//0xE8 a 0xEF
			&SimpleZ80::setr8,
			//0xF0 a 0xF7
			&SimpleZ80::setr8,
			//0xF8 a 0xFF
			&SimpleZ80::setr8

	};

public:

	registers reg_file;

	//Construtor
	SimpleZ80(Mmu* mem_arr);

	//Execução
	void runInstruction();

	//Executa um opcode
	void runOp(uint8_t *opcode);

	//No de clock levados pela última instrução
	uint16_t getClkElapsed();

	//Verifica se houve interrupção
	void Checkinterrupt();

	//Lista de instruções da CPU

	//NOP - sem operação
	//opcode - 0x00
	void nop(uint8_t* opcode);

	//LD r16,d16 - Carrega constante int16 para registro de 16 bits
	//Opcodes - 0x01, 0x11, 0x21, 0x31
	void ldr16d(uint8_t* opcode);

	//LD (reg16),A - Carrega o end. de memória apontado por registro com valor de A
	//Opcodes - 0x02, 0x12, 0x22, 0x32
	void ldp16a(uint8_t* opcode);

	//INC reg16 - Incrementa o registro de 16 bits
	//Opcodes - 0x03, 0x13, 0x23, 0x33
	void incr16(uint8_t* opcode);

	//INC reg8 - Incrementa o registro de 8 bits
	//Opcodes - 0x04, 0x0C, 0x14, 0x1C, 0x24, 0x2C, 0x34, 0x3C
	void incr8(uint8_t* opcode);

	//DEC reg8 - Decrementa o registro de 8 bits
	//Opcodes - 0x05, 0x0D, 0x15, 0x1D, 0x25, 0x2D, 0x35, 0x3D
	void decr8(uint8_t* opcode);

	//LD reg8,d8 - Carrega o registro de 8 bits com constante
	//Opcodes - 0x06, 0x0E, 0x16, 0x1E, 0x26, 0x2E, 0x36, 0x3E
	void ldr8d(uint8_t* opcode);

	//RLCA - Rotaciona o registro A para esquerda.
	//Opcode - 0x07
	void rlca(uint8_t* opcode);

	//LD (d16),SP - Carrega o valor de memória apontado por constante com o valor de SP
	//Opcode - 0x08
	void ldpsp(uint8_t* opcode);

	//ADD HL,reg16 - Soma o valor de reg16 com HL
	//Opcodes - 0x09, 0x19, 0x29, 0x39
	void ldhlr16(uint8_t* opcode);

	//LD A,(reg16) - Carrega o resgitro A com o end. de memória apontado por reg16
	//Opcodes - 0x0A, 0x1A, 0x2A, 0x3A
	void ldap16(uint8_t* opcode);

	//DEC reg16 - Decrementa o registro de 16 bits
	//Opcodes - 0x0B, 0x1B, 0x2B, 0x3B
	void decr16(uint8_t* opcode);

	//RRCA - Rotaciona o registro A para direita.
	//Opcode - 0x0F
	void rrca(uint8_t* opcode);

	//STOP 0 - Para a CPU
	//Opcode - 0x10
	void stop(uint8_t* opcode);

	//RLA - Rotaciona o registro A para esquerda com carry.
	//Opcode - 0x17
	void rla(uint8_t* opcode);

	//JR e8 - Salto incondicional relativo pela constante com sinal e8
	//Opcode - 0x18
	void jre8(uint8_t* opcode);

	//RRA - Rotaciona o registro A para direita com carry.
	//Opcode - 0x1F
	void rra(uint8_t* opcode);

	//JR NZ,e8 - Salto condicional  relativo pela constante com sinal e8
	//Opcode - 0x20
	void jrnze8(uint8_t* opcode);

	//DAA - Ajuste decimal do reg. A para representação BCD após uma instrução aritmética
	//Opcode - 0x27
	void daa(uint8_t* opcode);

	//JR Z,e8 - Salto condicional  relativo pela constante com sinal e8
	//Opcode - 0x28
	void jrze8(uint8_t* opcode);

	//CPL - Faz o complemento do acumulador A = ~A
	//Opcode - 0x2F
	void cpl(uint8_t* opcode);

	//JR NC,e8 - Salto condicional  relativo pela constante com sinal e8
	//Opcode - 0x30
	void jrnce8(uint8_t* opcode);

	//SCF - Coloca o bit carry em 1
	//Opcode - 0x37
	void scf(uint8_t* opcode);

	//JR C,e8 - Salto condicional relativo pela constante com sinal e8
	//Opcode - 0x38
	void jrce8(uint8_t* opcode);

	//CCF - Faz o complemento do carry CY = ~CY
	//Opcode - 0x3F
	void ccf(uint8_t* opcode);

	//LD reg8,reg8
	//Opcodes - 0x40 a 0x75 e 0x77 a 0x7F
	void ldr8r8(uint8_t* opcode);

	//HALT - Pausa a CPU e espera por interrupção
	//Opcode - 0x76
	void halt(uint8_t* opcode);

	//ADD reg8 - Soma o registro de 8 bit ao registro A
	//Opcodes - 0x80 a 0x87
	void addr8a(uint8_t* opcode);

	// ADC reg8 - Soma o registro de 8 bit ao registro A com carry
	//Opcodes - 0x88 a 0x8F
	void adcr8a(uint8_t* opcode);

	//SUB reg8 - Subtrai o registro de 8 bit ao registro A
	//Opcodes - 0x90 a 0x97
	void subr8a(uint8_t* opcode);

	//SBC reg8 - Subtrai o registro de 8 bit ao registro A com carry
	//Opcodes - 0x98 a 0x9F
	void sbcr8a(uint8_t* opcode);

	//AND reg8 - Realiza a operação "E" bit a bit entre reg8 e A
	//Opcodes - 0xA0 a 0xA7
	void andr8a(uint8_t* opcode);

	//XOR reg8 - Realiza a operação "Ou exclusivo" bit a bit entre reg8 e A
	//Opcodes - 0xA8 a 0xAF
	void xorr8a(uint8_t* opcode);

	//OR reg8 - Realiza a operação "Ou" bit a bit entre reg8 e A
	//Opcodes - 0xB0 a 0xB7
	void orr8a(uint8_t* opcode);

	//CP reg8 - Subtrai o reg 8 do A, ajusta os flags, mas não grava os resulatdos
	//Opcodes - 0xB8 a 0xBF
	void cprr8a(uint8_t* opcode);

	//RET NZ - Retorno de chamada de sub-rotina condicional
	//Opcode - 0xC0
	void retnz(uint8_t* opcode);

	//POP reg16 - Retira o valor da pilha e grava no registro de 16 bits
	//Opcodes - 0xC1, 0xD1, 0xE1, 0xF1
	void popr16(uint8_t* opcode);

	//JP NZ,d16 - Salto condicional para o end. absoluto dado pela constante
	//Opcode - 0xC2
	void jpnzr16d(uint8_t* opcode);

	//JP d16 - Salto incondicional para o end. absoluto dado pela constante
	//Opcode - 0xC3
	void jp16d(uint8_t* opcode);

	//CALL NZ,d16 - Chamada de subrotina condicional
	//Opcode - 0xC4
	void callnzd(uint8_t* opcode);

	//PUSH reg16 - Coloca o valor do registro de 16 bits na pilha
	//Opcodes - 0xC5, 0xD5, 0xE5, 0xF5
	void pushr16(uint8_t* opcode);

	//ADD A,d8 - Soma a constante ao registro A
	//Opcode - 0xC6
	void addd8a(uint8_t* opcode);

	//RST y*8h - Reinicia a CPU no end. y * 0x08
	//Opcode - 0xC7, 0xCF, 0xD7, 0xDF, 0xE7, 0xEF, 0xF7, 0xFF
	void rst(uint8_t* opcode);

	//RET Z - Retorno de chamada de sub-rotina condicional
	//Opcodes - 0xC8
	void retz(uint8_t* opcode);

	//RET - Retorno de chamada de sub-rotina incondicional
	//Opcodes - 0xC9
	void ret(uint8_t* opcode);

	//JP Z,d16 - Salto condicional para o end. absoluto dado pela constante
	//Opcodes - 0xCA
	void jpz16d(uint8_t* opcode);

	//PREFIX CB - Conjunto de intruções do prefixo CB
	//Opcodes - 0xCB
	void prefixcb(uint8_t* opcode);

	//CALL Z,d16 - Chamada de subrotina condicional
	//Opcode - 0xCC
	void callzd(uint8_t* opcode);

	//CALL d16 - Chamada de subrotina incondicional
	//Opcode - 0xCD
	void calld(uint8_t* opcode);

	//ADC A,d8
	//Opcode - 0xCE - Soma a constante ao registro A com o carry
	void adcd8a(uint8_t* opcode);

	//RET NC - Retorno de chamada de sub-rotina condicional
	//Opcodes - 0xD0
	void retnc(uint8_t* opcode);

	//JP NC,d16 - Salto condicional para o end. absoluto dado pela constante
	//Opcodes - 0xD2
	void jpnc16d(uint8_t* opcode);

	//Sem Implementação - Executar NOP
	//Opcodes - 0xD3

	//CALL NC,d16 - Chamada de subrotina condicional
	//Opcode - 0xD4
	void callncd(uint8_t* opcode);

	//SUB d8
	//Opcode - 0xD6  - Subtrai a constante ao registro A
	void subd8a(uint8_t* opcode);

	//RET C - Retorno de chamada de sub-rotina condicional
	//Opcodes - 0xD8
	void retc(uint8_t* opcode);

	//RETI - Retorno de interrupção incondicional
	//Opcodes - 0xD9
	void reti(uint8_t* opcode);

	//JP C,d16 - Salto condicional para o end. absoluto dado pela constante
	//Opcodes - 0xDA
	void jpc16d(uint8_t* opcode);

	//Sem Implementação - Executar NOP
	//Opcodes - 0xDB

	//CALL C,d16 - Chamada de subrotina condicional
	//Opcode - 0xDC
	void callcd(uint8_t* opcode);

	//Sem Implementação - Executar NOP
	//Opcodes - 0xDD

	//SBC A,d8 - Subtrai a constante ao registro A com o carry
	//Opcode - 0xDE
	void sbcd8a(uint8_t* opcode);

	///LDH (d8),A - Carrega o dado de A na região de 0xFF00 a 0xFFFF dado pela constante
	//Opcode - 0xE0
	void ldhd8a(uint8_t* opcode);

	//LD (C),A - Carrega o dado de A na região de 0xFF00 a 0xFFFF dado por C
	//Opcode - 0xE2
	void ldpca(uint8_t* opcode);

	//Sem Implementação - Executar NOP
	//Opcodes - 0xE3

	//Sem Implementação - Executar NOP
	//Opcodes - 0xE4

	//AND d8 - Operação "E" bit a bit entre constante e registro A
	//Opcode - 0xE6
	void andd8a(uint8_t* opcode);

	//ADD SP,d8 - Soma  a constante ao registro SP
	//Opcode - 0xE8
	void addspd8(uint8_t* opcode);

	//JP (HL) - Salto incondicional para o endereço apontado por HL
	//Opcode - 0xE9
	void jpphl(uint8_t* opcode);

	//LD (d16),A - Carrega o dado de A no end. apontado pela constante
	//Opcode - 0xEA
	void ldpd16a(uint8_t* opcode);

	//Sem Implementação - Executar NOP
	//Opcodes - 0xEB

	//Sem Implementação - Executar NOP
	//Opcodes - 0xEC

	//Sem Implementação - Executar NOP
	//Opcodes - 0xED

	//XOR d8 - Operação "Ou exclusivo" bit a bit entre constante e registro A
	//Opcode - 0xEE
	void xord8a(uint8_t* opcode);

	//LDH A,(d8) - Carrega o dado região de 0xFF00 a 0xFFFF dado pela constante em A
	//Opcode - 0xF0
	void ldhad8(uint8_t* opcode);

	//LD A,(C) Carrega o dado da região de 0xFF00 a 0xFFFF dado por C em A
	//Opcode - 0xF2
	void ldapc(uint8_t* opcode);

	//DI - Desabilita as interrupções
	//Opcode - 0xF3
	void di(uint8_t* opcode);

	//Sem Implementação - Executar NOP
	//Opcodes - 0xF4

	//OR d8 - Operação "OU" bit a bit entre constante e registro A
	//Opcode - 0xF6
	void ord8a(uint8_t* opcode);

	//LD HL,SP+r8 - Carrega o valor de SP + constante com sinal em HL
	//Opcode - 0xF8
	void ldhlspd8(uint8_t* opcode);

	//LD SP,HL - Carrega o valor de HL em SP
	//Opcode - 0xF9
	void ldsphl(uint8_t* opcode);

	//LD A,(a16) - Carrega o valor apontado pela constante em A
	//Opcode - 0xFA
	void ldapd16(uint8_t* opcode);

	//EI - Habilita as interrupções
	//Opcode - 0xFB
	void ei(uint8_t* opcode);

	//Sem Implementação - Executar NOP
	//Opcodes - 0xFC

	//Sem Implementação - Executar NOP
	//Opcodes - 0xFD

	//CP d8 - Subtrai a constante ao registro A, mas não grava o resultado
	//Opcode -0xFE
	void cpdd8a(uint8_t* opcode);

	//Instruções com prefixo CB

	//RLC reg8 - Rotaciona o registro de 8 bit para esquerda
	//Opcodes - 0x00 a 0x07
	void rlcr8(uint8_t* opcode);

	//RRC reg8 - Rotaciona o registro de 8 bit para direita
	//Opcodes - 0x08 a 0x0F
	void rrcr8(uint8_t* opcode);

	//RL reg8 - Rotaciona o registro de 8 bit para esquerda com carry
	//Opcodes - 0x10 a 0x17
	void rlr8(uint8_t* opcode);

	//RR reg8 - Rotaciona o registro de 8 bit para direita com carry
	//Opcodes - 0x18 a 0x1F
	void rrr8(uint8_t* opcode);

	//SLA reg8 - Deslocamento aritimético dos bits do registro de 8 bit para esquerda
	//Opcodes - 0x20 a 0x27
	void slar8(uint8_t* opcode);

	//SRA reg8 - Deslocamento aritimético dos bits do registro de 8 bit para direira
	//Opcodes - 0x28 a 0x2F
	void srar8(uint8_t* opcode);

	//SWAP reg8 - Troca os 4 bits mais significativos com os 4 menos significativos
	//Opcodes - 0x30 a 0x37
	void swapr8(uint8_t* opcode);

	//SRL reg8 - Deslocamento lógico dos bits do registro de 8 bit para esquerda
	//Opcodes - 0x38 a 0x37
	void srlr8(uint8_t* opcode);

	//BIT b,reg8 - Testa o bit b no registro r8, seta a flag zero se o bit não está setado.
	//Opcodes - 0x40 a 0x7F
	void bitr8(uint8_t* opcode);

	//RES b,reg8 - Coloca o bit b em 0
	//Opcodes - 0x80 a 0xBF
	void resr8(uint8_t* opcode);

	//SET b,reg8 - Coloca o bit b em 1
	//Opcodes - 0xC0 a 0xFF
	void setr8(uint8_t* opcode);


};

#endif /* CPU_H_ */
