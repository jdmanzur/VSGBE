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

#ifndef IOREGADDR_H_
#define IOREGADDR_H_

//Endereço do banco de ROM 0
const uint16_t ROM0  =  0x0000;

//Endereço de início do banco de ROM
const uint16_t ROMX  =  0x4000;

//Endereço de início da VRAM
const uint16_t VRAM  =  0x8000;

//Endereço de início do banco de RAM
const uint16_t RAMX  =  0xA000;

//Endereço de RAM interna
const uint16_t RAM0  =  0xC000;

//Espelho da RAM interna
const uint16_t ERAM  =  0xE000;

//Enderenço de início da OAM
const uint16_t OAMS  =   0xFE00;

//Endereços Vazios
const uint16_t EMP0  =  0xFEA0;

//Portas de entrada e saída
const uint16_t IOPR  =  0xFF00;

//Endereços Vazios
const uint16_t EMP1 =   0xFF4C;

//Endereço de RAM interna
const uint16_t RAM1  =  0xFF80;

//Endereço dos registros especiais na memória
//Em certos aspectos, é melhor referenciar pelo nomes,

const uint16_t P1  =    0xFF00; 
const uint16_t SB   =   0xFF01;  
const uint16_t SC   =   0xFF02;  
const uint16_t DIV   =  0xFF04;   
const uint16_t TIMA  =  0xFF05;   
const uint16_t TMA   =  0xFF06;   
const uint16_t TAC   =  0xFF07;   
const uint16_t IF    =  0xFF0F;   
const uint16_t NR10  =  0xFF10;   
const uint16_t NR11  =  0xFF11;   
const uint16_t NR12  =  0xFF12;   
const uint16_t NR13  =  0xFF13;   
const uint16_t NR14  =  0xFF14;   
const uint16_t NR21  =  0xFF16;   
const uint16_t NR22  =  0xFF17;   
const uint16_t NR23  =  0xFF18;   
const uint16_t NR24  =  0xFF19;   
const uint16_t NR30  =  0xFF1A;   
const uint16_t NR31  =  0xFF1B;   
const uint16_t NR32  =  0xFF1C;   
const uint16_t NR33  =  0xFF1D;   
const uint16_t NR34  =  0xFF1E;   
const uint16_t NR41  =  0xFF20;   
const uint16_t NR42  =  0xFF21;   
const uint16_t NR43  =  0xFF22;   
const uint16_t NR44  =  0xFF23;   
const uint16_t NR50  =  0xFF24;   
const uint16_t NR51  =  0xFF25;  
const uint16_t NR52  =  0xFF26;   
//FF30 - FF3F (Wave Pattern RAM)
const uint16_t LCDC  =  0xFF40;   
const uint16_t STAT  =  0xFF41;   
const uint16_t SCY   =  0xFF42;   
const uint16_t SCX   =  0xFF43;   
const uint16_t LY    =  0xFF44;   
const uint16_t LYC   =  0xFF45;   
const uint16_t DMA   =  0xFF46;   
const uint16_t BGP   =  0xFF47;   
const uint16_t OBP0  =  0xFF48;   
const uint16_t OBP1  =  0xFF49;   
const uint16_t WY    =  0xFF4A;   
const uint16_t WX    =  0xFF4B;   
const uint16_t IE    =  0xFFFF;   

#endif /* IOREGADDR_H_ */