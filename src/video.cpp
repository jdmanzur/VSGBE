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

#include "video.h"
#include <assert.h>

//Construtor
Video::Video(int width,int height)
{
        //Variável para dizer se o sdl foi inicializado corretamente
        this->sdl_ok = 1;

        //Inicializa tudo do SLD
        //TODO: Inicilaizar só a parte vídeo
        if(SDL_Init(SDL_INIT_VIDEO) < 0)
            this->sdl_ok = 0;
        
        //Inicializa a janela
        this->app_win = SDL_CreateWindow("VSGBE ALPHA",SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,width,height,SDL_WINDOW_SHOWN);
       
        //Verifica se a janela foi criada corretamente
        if(this->app_win == NULL)
            this->sdl_ok = 0;
                 
        //inicializa o renderer para janela do aplicativo
        this->frame_rend = SDL_CreateRenderer(this->app_win,-1,SDL_RENDERER_ACCELERATED);

        //Cria uma textura para ser utilizada como frame buffer
        //TODO: Remover nº mágicos do tamanho do buffer
        //this->frame_surf = SDL_CreateRGBSurface(0, 160, 144, 32, 0xff000000, 0x00ff0000, 0x0000ff00, 0x0000ff00)
        //this->frame_surf = SDL_CreateTexture(this->frame_rend,SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, 160, 144 )
        this->BG_texture = SDL_CreateTexture(this->frame_rend,SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, 160, 144);
        
        //Inicializa o contador de pixeis do buffer de escrita
        this->pixel_d = 0;

        //Buffer para atualizar o backgorund
        //TODO: Remover Números mágicos
        //this->BG_buff = [0x7F007F00 for pixel in range(23040)]
        for(uint64_t x = 0; x < 30000; x++)
            this->BG_buff[x] = 0x007F007F;

        this->fps_lasttime = SDL_GetTicks();
        this-> fps_frames = 0;
}


//Desenha uma linha no buffer
//TODO:Verficar condição quando Houver deslocamonto Horizontal
void Video::drawnTileLine(uint8_t* tile_data,uint8_t bgp,uint8_t line,uint8_t col,uint8_t start,uint8_t end)
{    

//Vetor para guardar a palheta de cores
    uint32_t p[4];
        
    //Decodificação da palheta
    for(int i = 0; i < 8; i+=2)
    {
        //Decodifica a palheta de cores
        uint8_t dec = ((bgp & (3 << i)) >> i);

        //Verde mais escuro (Preto);
        if (dec == 3)
            //p[i/2] = 0x000F380F;
            p[i/2] = 0x00000000;
        //Verde escuro
        else if (dec == 2)
            //p[i/2] = 0x00306230;
            p[i/2] = 0x00555555;
        //Verde médio
        else if (dec == 1)
            //p[i/2] = 0x008bac0f;
            p[i/2] = 0x00888888;
        //Verde claro
        else if (dec == 0)
            //p[i/2] = 0x00CADC9F;
            p[i/2] = 0x00CCCCCC;
        
        }

    //TODO:Verficar condição quando Houver deslocamonto Horizontal
    for(int x = start; x <= end; x++)
        this->BG_buff[(x) + (line*160) + (col)] = p[tile_data[x]];
    
}
    
//TODO:Melhorar isso...
//Converte as cores para RGB
void Video::decodeRGBA(uint8_t* tile_data,uint8_t bgp,uint32_t* line_rgba)
{

    //Vetor para guardar a palheta de cores
    uint32_t p[4];
        
    //Decodificação da palheta
    for(int i = 0; i < 8; i+=2)
    {
        //Decodifica a palheta de cores
        uint8_t dec = ((bgp & (3 << i)) >> i);

        //Verde mais escuro (Preto);
        if (dec == 3)
            p[i/2] = 0x000F380F;
        //Verde escuro
        else if (dec == 2)
            p[i/2] = 0x00306230;
        //Verde médio
        else if (dec == 1)
            p[i/2] = 0x008bac0f;
        //Verde claro
        else if (dec == 0)
            p[i/2] = 0x009bbc0f;
        
        }

        //Realiza um ou entre cada bit das duas posições da matriz
        //TODO: Deixar isso mais claro...
        for(int i = 0; i < 0x10000; i++)
        {
            //Converte os dois bytes para bits
            line_rgba[i] = p[tile_data[i]];

        }

}

//Escreve diretamente no buffer
void Video::writeTileBuffer(uint8_t data[8][8],uint16_t i,uint16_t j,uint8_t bgp)
{

    //Vetor para guardar a palheta de cores
    uint32_t p[4];
        
    //Decodificação da palheta
    for(int i = 0; i < 8; i+=2)
    {
        //Decodifica a palheta de cores
        uint8_t dec = ((bgp & (3 << i)) >> i);

        //Verde mais escuro (Preto);
        if (dec == 3)
            p[i/2] = 0x000F380F;
        //Verde escuro
        else if (dec == 2)
            p[i/2] = 0x00306230;
        //Verde médio
        else if (dec == 1)
            p[i/2] = 0x008bac0f;
        //Verde claro
        else if (dec == 0)
            p[i/2] = 0x009bbc0f;
        
        }


    for(int x = 0; x < 8; x++)
        for(int y = 0; y < 8; y ++)
            this->BG_buff[(y+i) + ((x+j)*256)] = p[data[x][y]];

}
    
//Rotina que transfere buffer para o renderer e mostra na tela
void Video::drawnTextures()
{
        //Limpa o render
        SDL_RenderClear(this->frame_rend);

        //Atualiza a textura do background com o buffer
        //TODO: Remover os No mágicos de tamanho da linha
        SDL_UpdateTexture(this->BG_texture,NULL,this->BG_buff,160*sizeof(uint32_t));

         //Coloca a textura do background no renderer
        SDL_RenderCopy(this->frame_rend,this->BG_texture,NULL,NULL);
        
        //Apresenta o renderer na tela
        SDL_RenderPresent(this->frame_rend);

        //Calcula o FPS no último segundo

        this->fps_frames++;

        if(this->fps_lasttime < (SDL_GetTicks() - 1000))
        {
            this->fps_lasttime = SDL_GetTicks();
            this->fps_current = fps_frames;
            fps_frames = 0;
        }

        //std::cout << "FPS: " << this->fps_current << std::endl;
}
