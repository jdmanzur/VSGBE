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

#ifndef MMU_H_
#define MMU_H_

#include <iostream>
#include <SDL2/SDL.h>
#include <cstdint>
#include "ioRegAddr.h"

//Constantes para o tamanho da rom e da BIOS
//TODO: Isso depois tem que ser mudado para suportar outros tamanhos
#define BIOS_SIZE	0x0100
#define ROM_SIZE	0x8000

//Tamanhos dos bancos de ROM e RAM
#define ROM_BANK_SIZE	0x4000
#define RAM_BANK_SIZE	0x2000
#define RAM_LST_SIZE	0x0080
#define ORAM_SIZE		0x00A0
#define IORAM_SIZE		0x0200

//Tipos de controladores de memória
//MBC0 = Sem controlador, apenas ROM
enum MBCType {MBC0 = 0,MBC1 = 1,MBC2 = 2,MBC3 = 3,MBC5 = 4};


//Classe que implementa o controlador de memória
//TODO: Implementar os diversos controladores de memórias
class Mmu
{
private:

	//Matriz para bios e dado do jogo
	//TODO: Encontrar uma melhor maneira de se fazer isso.
	uint8_t bios[BIOS_SIZE];

	//8Kbytes de RAM de vídeo
	uint8_t v_ram[RAM_BANK_SIZE];

	//Vetor que representa o banco 0
	uint8_t f_rom[ROM_BANK_SIZE];

	//RAM de sistema banco 0
	uint8_t i_ram0[RAM_BANK_SIZE];

	//RAM do atributo de Sprite e io
	uint8_t ioram[IORAM_SIZE];

	//Ponteiro para mapeamento da BIOS no end. de memória
	uint8_t* fst_page;

	//Ponteiro para o banco de ROM selecionável
	//uint8_t* b_rom;
	uint8_t* b_rom;

	//Ponteiro para o banco de RAM selecionável
	uint8_t* b_ram;
		
	//Tipo de controlador cartucho
	uint8_t c_type;

	//Tipo de jogo
	uint8_t g_type;

	//Tamanho de RAM externa
	uint32_t ram_size;

	//Tamanho total da ROM externa
	uint32_t rom_size;

	//Registros do controlador de menória

	//Tipo de memória no MBC1
	uint8_t mbc1_t;

	//Número total de bancos de ROM
	uint16_t n_rom;

	//Número total de bancos de ROM
	uint16_t n_ram;

	//Banco de ROM selecionado
	uint16_t rom_sel;

	//Banco de RAM selecionado
	uint16_t ram_sel;


	//Classe que faz a intermediação de escrita nos endereços de memória
	class Proxy 
	{
		//Ponteiro para o dado manipulado
		Mmu &mmu;
		
		//Gravar o índice da memoria para acessar
    	uint16_t index;

	public:
    	Proxy(Mmu& mmu, uint16_t index) : mmu(mmu), index(index){}
    		
		//Realiza a escrita na memória através da sobrecarga de operador
		void operator= (uint8_t valor)
		{

			//std::cout << "Escreveu na memória" << std::endl;
        	mmu.write(index,valor);
	       		
		}	
    		
		//Realiza a leitura da memória
		operator uint8_t() const
		{
			//std::cout << "Leu da memória" << std::endl;
			return mmu.read(index);
		}
	
	};
	

public:

	//Construtor para carregar a bios e cartucho
	Mmu(uint8_t* bios_a, uint8_t* rom_a, uint32_t rom_s);

	//Construtor para carregar apenas o cartucho
	Mmu(uint8_t* rom);

	//Destrutor da Classe
	~Mmu();

	// Sobrecarga do operador [] para acessar a ram como um único vetor
	//Proxy operator[](uint16_t index);
	Proxy operator[](int index)
	{
    	return Proxy(*this,index);
	}

	//Lê da memória
	uint8_t read(uint16_t addr);

	//Escreve na memória
	void write(uint16_t addr,uint8_t data);

	//Realiza o DMA
	void dma();

	//Decodifica os dados da rom
	void readROMHeader(uint8_t* rom);

};


#endif//MMU_H//
