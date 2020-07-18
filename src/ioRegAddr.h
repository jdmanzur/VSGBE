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
#define  IOREGADDR_H_

//Endereço do banco de ROM 0
const int  ROM0  =  0x0000;

//Endereço de início do banco de ROM
const int  ROMX  =  0x4000;

//Endereço de início da VRAM
const int  VRAM  =  0x8000;

//Endereço de início do banco de RAM
const int  RAMX  =  0xA000;

//Endereço de RAM interna
const int  RAM0  =  0xC000;

//Espelho da RAM interna
const int  ERAM  =  0xE000;

//Enderenço de início da OAM
const int  OAMS  =  0xFE00;

//Endereços Vazios
const int  EMP0  =  0xFEA0;

//Portas de entrada e saída
const int  IOPR  =  0xFF00;

//Endereços Vazios
const int  EMP1  =  0xFF4C;

//Endereço de RAM interna
const int  RAM1  =  0xFF80;

//Endereço dos registros especiais na memória
//Em certos aspectos, é melhor referenciar pelo nomes,

const int  P1   =   0xFF00;   
const int  SB   =   0xFF01;   
const int  SC   =   0xFF02;  
const int  DIV  =   0xFF04;   
const int  TIMA =   0xFF05;  
const int  TMA  =   0xFF06;   
const int  TAC  =   0xFF07;   
const int  IF   =   0xFF0F;   
const int  NR10 =   0xFF10;   
const int  NR11 =   0xFF11;   
const int  NR12 =   0xFF12;   
const int  NR13 =   0xFF13;   
const int  NR14 =   0xFF14;   
const int  NR21 =   0xFF16;   
const int  NR22 =   0xFF17;   
const int  NR23 =   0xFF18;   
const int  NR24 =   0xFF19;   
const int  NR30 =   0xFF1A;   
const int  NR31 =   0xFF1B;   
const int  NR32 =   0xFF1C;   
const int  NR33 =   0xFF1D;   
const int  NR34 =   0xFF1E;   
const int  NR41 =   0xFF20;   
const int  NR42 =   0xFF21;   
const int  NR43 =   0xFF22;   
const int  NR44 =   0xFF23;   
const int  NR50 =   0xFF24;   
const int  NR51 =   0xFF25;   
const int  NR52 =   0xFF26;   
//FF30 - FF3F (Wave Pattern RAM)
const int  LCDC  =  0xFF40;   
const int  STAT  =  0xFF41;   
const int  SCY   =  0xFF42;   
const int  SCX   =  0xFF43;   
const int  LY    =  0xFF44;   
const int  LYC   =  0xFF45;   
const int  DMA   =  0xFF46;   
const int  BGP   =  0xFF47;   
const int  OBP0  =  0xFF48;   
const int  OBP1  =  0xFF49;   
const int  WY    =  0xFF4A;   
const int  WX    =  0xFF4B;   
const int  IE    =  0xFFFF;   

#endif /* IOREGADDR_H_ */