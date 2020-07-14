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
#ifndef TIMER_H
#define TIMER_H

#include <cstdint>
#include "mmu.h"


class Timer
{

private:
    
    //Leitura do espaço de memória
    Mmu* mem;

    //Registro de divisão do timer
    uint16_t div_reg;

    //Módulo da divisão do clock anterior
    uint16_t mod;

public:

    //Construtor
    Timer(Mmu *mem_s);

    //Atualiza o estado do timer
    void update(uint8_t e_clk);

};

#endif