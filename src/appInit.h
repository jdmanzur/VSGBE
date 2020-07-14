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

#ifndef APPINIT_H_
#define APPINIT_H_

#include <stdio.h>
#include <cstdint>
#include <iostream>
#include <fstream>

#include "mmu.h"


//Método que carrega a bios e a rom na memória
void loadBiosROM(char bios_name[],char rom_name[], char* b_array, char* r_array);


int getFilesize(char file[]);


#endif /* APPINIT_H_ */
