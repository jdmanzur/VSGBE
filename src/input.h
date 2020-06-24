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

#ifndef INPUT_H_
#define INPUT_H_

#include <cstdint>
#include <iostream>
#include <SDL2/SDL.h>
#include "mmu.h"
#include "ioRegAddr.h"

class InputCrt
{
private:
    //Ponteiro para o espaço de memória
    Mmu* mem;

    //Variável de eventos para SDL
    SDL_Event* input_e;

public:

    //Variável para usuário sair da aplicação
    bool app_quit;

    //Construtor
    InputCrt(Mmu *mem_s,SDL_Event* e);

    //Destrutor
    ~InputCrt();


    //Atualiza o estado da entrada do programa
    void UpdateAppUI();

};



#endif