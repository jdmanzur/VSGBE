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

#include <iostream>
#include <fstream>
#include <cstdint>
#include <SDL2/SDL.h>
#include <assert.h>
#include "mmu.h"
#include "cpu.h"
#include "appInit.h"
#include "ppu.h"
#include "input.h"

using namespace std;



int main(int argc, char *argv[]) {

	//Variáveis temporárias para ram e bios

	//Carrega a bios e a rom
	//loadBios((char*)bios);
	//loadROM((char*)rom);

	assert(argc >= 3);

	SDL_Event e;

	//Local da bios
	//char bios_l[] = "/home/adriano/Documentos/gb/bios.gb";

	//Local da rom
	//char rom_l[] = "/home/adriano/Documentos/gb/tetris.gb";

	//Horrível, melhorar
	uint8_t* bios = new uint8_t[getFilesize(argv[1])];
	uint8_t* rom =  new uint8_t[getFilesize(argv[2])];
	//uint8_t* bios = (uint8_t*)malloc(getFilesize(bios_l)*sizeof(uint8_t));
	//uint8_t* rom =  (uint8_t*)malloc(getFilesize(rom_l)*sizeof(uint8_t));


	//uint8_t bios[256];
	//uint8_t rom[32768];

	//Carrega a ROM e a BIOS
	loadBiosROM(argv[1],argv[2],(char*)bios,(char*)rom);

	//Objeto Espaço de memória
	//Mmu ram(bios,rom,getFilesize(rom_l));
	Mmu ram(bios,rom,getFilesize(argv[2]));



	//Librera a ram
	delete [] bios;
	delete [] rom;


	//Carrega o espaço de memória
	//loadBiosROM(ram,bios_l,rom_l);

	//Instância da CPU
	SimpleZ80 cpu(&ram);

	//Instância da PPU
	Ppu ppu(&ram);

	//Instância da Entardas
	InputCrt entrada(&ram,&e);

/*	for(int i = 0; i < 256; i++)
		cout << hex << (int)ram[i] << endl;


	uint8_t opcode1[2] = {0x0E,0x64};
	uint8_t opcode2 = 0x0D;

	cpu.runOp(opcode1);
	cpu.runOp(&opcode2);

	cout << cpu.reg_file.C() << endl;
*/

	uint16_t sys_clk = 0;

	bool fim = 1;

	//Flag para o controlador de vídeo 
    uint8_t lcd_on;


	//Zera todos os botões 
	ram[0xFF00] = 0xFF;

	//Mundança manual do timer
	ram[0xFF05] = 0x00;
	ram[0xFF04] = 0x00;

	//cpu.reg_file.PC = 0x100;

	while(fim)
	{
		//Roda uma instrução da CPU
		cpu.runInstruction();


		uint16_t pc = cpu.reg_file.PC;

		//cout << "PC: "<< hex << pc << " Intru: " << (int)ram[pc]  <<  endl;
		//cout << "B: " << hex << (int)cpu.reg_file.B() << endl;

		//Rotina para enganar o programa e dizer que os botões não estão apetados
		if((cpu.reg_file.PC == 0x97))
		{
			//Zera todos os botões 
			ram[0xFF80] = 0x00;
		}	

		//Atualiza o clock da PPU
		ppu.update(cpu.getClkElapsed());

		//Atualiza as entradas
		entrada.UpdateAppUI();

		//Mundança manual do timer
		ram.write(0xFF05,(ram.read(0xFF05) + cpu.getClkElapsed()));
		ram.write(0xFF04,(ram.read(0xFF04) + cpu.getClkElapsed()));

		//Zera todos os botões 
		//ram[0xFF80] = 0x00;

		//Verifica se o usuário desligou
		fim = (!entrada.app_quit);


	}


	return 0;
}
