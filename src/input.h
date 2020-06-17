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
    SDL_Event input_e;

public:

    //Variável para usuário sair da aplicação
    bool app_quit;

    //Construtor
    InputCrt(Mmu *mem_s);

    //Destrutor
    ~InputCrt();


    //Atualiza o estado da entrada do programa
    void UpdateAppUI();

};



#endif