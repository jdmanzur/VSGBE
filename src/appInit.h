/*
 * appInit.h
 *
 *  Created on: 6 de mai de 2020
 *      Author: adriano
 */

#ifndef APPINIT_H_
#define APPINIT_H_

#include <stdio.h>
#include <cstdint>
#include <iostream>
#include <fstream>

#include "mmu.h"

//Método temporário para carregar na memória
void loadBios(char* mem);

//Método temporário para a ROM na memória
void loadROM(char* mem);

//Método que carrega a bios e a rom na memória
void loadBiosROM(char bios_name[],char rom_name[], char* b_array, char* r_array);

//Método temporário para carregar o logo
void loadLogo(uint8_t* mem);

int getFilesize(char file[]);


#endif /* APPINIT_H_ */
