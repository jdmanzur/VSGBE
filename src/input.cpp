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

#include "input.h"
#include <assert.h>

//Construtor
InputCrt::InputCrt(Mmu *mem_s,SDL_Event* e)
{
    //Configura ponteiro para espaço de memória
    this->mem = mem_s;

    this->input_e = e;

    //Variável para indicar se usuário quer sair do programa
    this->app_quit = 0;

    //Inicializa o estado antigos da teclas
    this->p14_old = 1;
    this->p15_old = 1;

    //Inicializa os eventos do SDL
   //if(SDL_Init(SDL_INIT_EVENTS) < 0);
   assert(SDL_Init(SDL_INIT_EVENTS) >= 0);




}

//Destrutor
InputCrt::~InputCrt()
{
    //Desativa o SLD
    //SDL_Quit();
}

//Atualiza o estado da entrada do programa
void InputCrt::UpdateAppUI()
{

    //Atualiza os eventos
    //TODO: Colocar um Timer para melhor a velocidade do programa
    while ((SDL_PollEvent(input_e)) != 0)
        if ((*input_e).type == SDL_QUIT) 
        {
            this->app_quit = 1;
            std::cout << "Sair...." << std::endl;
        }

    //Leitura do estados da teclas
    const uint8_t* key_state = SDL_GetKeyboardState(NULL);

    //verifica qual tecla está selecionada
    //Teclas Up, Down, Left e Right
    bool p14 = ((*mem).read(AddrConst::P1) & 0x10) ? false : true;
    //Teclas A,B, Select e Start
    bool p15 = ((*mem).read(AddrConst::P1) & 0x20) ? false : true;

    //Variável para guardar o estdo das teclas
    uint8_t k_read;

    //Se conjuto de teclas 1 selecionado
    //TODO:Não está com o comportamento exato
    //TOD: Melhorar...
    if(p14)
    {
        //Bit 0 = Tecla esquerda
        uint8_t p10 = (!key_state[SDL_SCANCODE_RIGHT]);
        //Bit 1 = Tecla direita
        uint8_t p11 = ((!key_state[SDL_SCANCODE_LEFT]) << 1);
        //Bit 2 = Tecla cima
        uint8_t p12 = ((!key_state[SDL_SCANCODE_UP]) << 2);
        //Bit 3 = Tecla baixo
        uint8_t p13 = ((!key_state[SDL_SCANCODE_DOWN]) << 3);

        //Invert todos os bits (simulando circuito pull up)
        uint8_t k_read = ( p10 | p11 | p12 | p13);

        //Atualiza a memória
        (*mem).write(AddrConst::P1,k_read + ((!p14) << 4) + ((!p15) << 5));

       //Verifica interrupção de acionamento das teclas
       if(this->p14_old > (*mem).read(AddrConst::P1))
            (*mem).write(AddrConst::IF,((*mem).read(AddrConst::IF) | 0x10));

        this->p14_old = (*mem).read(AddrConst::P1);
              
    }
    else if(p15)
    {                
        //Bit 0 = Tecla esquerda
        uint8_t p10 = (!key_state[SDL_SCANCODE_A]);
        //Bit 1 = Tecla direita
        uint8_t p11 = ((!key_state[SDL_SCANCODE_S]) << 1);
        //Bit 2 = Tecla cima
        uint8_t p12 = ((!key_state[SDL_SCANCODE_Z]) << 2);
        //Bit 3 = Tecla baixo
        uint8_t p13 = ((!key_state[SDL_SCANCODE_X]) << 3);

        //Inverte todos os bits (simulando circuito pull up)
        uint8_t k_read = ( p10 | p11 | p12 | p13);

        //Atualiza a memória
        (*mem).write(AddrConst::P1,k_read + ((!p14) << 4) + ((!p15) << 5));

        //Verifica interrupção de acionamento das teclas
       if(this->p15_old > (*mem).read(AddrConst::P1))
            (*mem).write(AddrConst::IF,((*mem).read(AddrConst::IF) | 0x10));

        this->p15_old = (*mem).read(AddrConst::P1);

    }

    

     
   

}