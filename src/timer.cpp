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

#include "timer.h"

//Construtor
Timer::Timer(Mmu *mem_s)
{
    //Inicializa ponteiro para o espaço de memória
    this->mem = mem_s;

    //Limpa o registro de divisão
    this->div_reg = 0; 

    //Inicializa o módulo da divisão
    this->mod = 0;

}

//Atualiza o estado do timer
void Timer::update(uint8_t e_clk)
{
    //Incrementa o registro de divisão
    this->div_reg += e_clk;

    //Atulaiza o registro DIV na memória de sistema
    (*mem).write(DIV,(uint8_t)(div_reg >> 8));

    //Váriavel se o timer está ligado
    bool t_on = ((*mem).read(TAC) & 0x04) >> 2;

    //Seleção de frequência do clock
    uint8_t f_sel = ((*mem).read(TAC) & 0x03);

    //TODO:Verificar se o funcionamento do timer no hardware real é dessa maneira...
    //Verifica se o timer está ligado
    if(t_on)
    {
        //Valor anterior do timer
        uint8_t t_old = (*mem).read(TIMA);

        //Tempo passado desde a última isntrução
        uint8_t delta_t = (e_clk + mod);

        switch (f_sel)
        {
        case 1:
            //Freq / 2^4 (262.144 KHz)

            //Atualiza o timer
            (*mem).write(TIMA,((delta_t/0x10) + t_old));

            //Atualiza o módulo
            this->mod = (delta_t%0x10);            

            break;
        
        case 2:
            //Freq / 2^6 (65.536 KHz)
                        
            //Atualiza o timer
            (*mem).write(TIMA,((delta_t/0x40) + t_old));

            //Atualiza o módulo
            this->mod = (delta_t%0x40);  

            break;

        case 3:
            //Freq / 2^8 (16.384 KHz)
            
            //Atualiza o timer
            (*mem).write(TIMA,((delta_t/0x100) + t_old));

            //Atualiza o módulo
            this->mod = (delta_t%0x100);  
            
            break;
        
        default:
            //Freq / 2^10 (4.096 KHz)

            //Atualiza o timer
            (*mem).write(TIMA,((delta_t/0x400) + t_old));

            //Atualiza o módulo
            this->mod = (delta_t%0x400);  

            break;
        }

        //Verifica o overflow do timer
        if(t_old > (*mem).read(TIMA))
        {
            //Carrega o valor do timer
            (*mem).write(TIMA,((*mem).read(TMA) + t_old));

            //Habilita a interrupção
            (*mem).write(IF,((*mem).read(IF) | 0x04));
        }

    }


}