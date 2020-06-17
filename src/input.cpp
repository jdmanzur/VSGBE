#include "input.h"
#include <assert.h>

//Construtor
InputCrt::InputCrt(Mmu *mem_s)
{
    //Configura ponteiro para espaço de memória
    this->mem = mem_s;

    //Variável para indicar se usuário quer sair do programa
    this->app_quit = 0;

    //Inicializa os eventos do SDL
    if(SDL_Init(SDL_INIT_EVENTS) != 0);
        assert("Erro");




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
    while ((SDL_PollEvent(&input_e)) != 0)
        if (input_e.type == SDL_QUIT) 
        {
            this->app_quit = 1;
            std::cout << "Sair...." << std::endl;
        }
    //Leitura do estados da teclas
    const uint8_t* key_state = SDL_GetKeyboardState(NULL);

    //verifica qual tecla está selecionada
    //Teclas Up, Down, Left e Right
    bool p14 = ((*mem).read(P1) & 0x10) ? false : true;
    //Teclas A,B, Select e Start
    bool p15 = ((*mem).read(P1) & 0x20) ? false : true;

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
        (*mem).write(P1,k_read + ((!p14) << 4) + ((!p15) << 5));

        //std::cout << std::hex << (int)k_read << std::endl;
              
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
        (*mem).write(P1,k_read + ((!p14) << 4) + ((!p15) << 5));

    }

    

     
   

}