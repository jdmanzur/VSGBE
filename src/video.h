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

#ifndef VIDEO_H_
#define VIDEO_H_

#include <cstdint>
#include <iostream>
#include <SDL2/SDL.h>

class Video
{
private:
    //Variável para dizer se o sdl foi inicializado corretamente
    bool sdl_ok;

    //Janela para exibição da PPU
    SDL_Window* app_win;

    //Render para PPU
    SDL_Renderer* frame_rend;

    //Textura com a imagem de saída da PPU
    SDL_Texture* BG_texture;
    
    //Textura para controles touch screen
    SDL_Texture* BT_texture;
    
    //Superfície para as imagens dos botões
    SDL_Surface* BT_surface;

    //Contador de pixeis do buffer de escrita
    //TODO:Pensar em uma forma mais elegante de fazer essa função
    uint8_t pixel_d;

    //Buffer para guardar os dados da textura
    //TODO: Remover Números mágicos
    //uint32_t BG_buff[22400];
    uint32_t BG_buff[30000];

    //Variaveis para se medir o framerate
    uint32_t fps_lasttime;
    uint32_t fps_current;
    uint32_t fps_frames;
    

    //Constante para palheta
/*
    const uint32_t pal_c[3][4] = {{0x00CCCCCC,0x00888888,0x00555555,0x00000000},
                                 {0x00CADC9F,0x008bac0f,0x00306230,0x00000000},
                                 {0x00CADC9F,0x008bac0f,0x00306230,0x00000000}};
*/
    const uint32_t pal_c[3][4] = {{0x00EEEEEE,0x00FFCBC1,0x005FBFB8,0x00000000},
                                 {0x00FFFFFF,0x00D0C4A0,0x006EB5FF,0x00000000},
                                 {0x00FFFFFF,0x00DCD3FF,0x00FF9CEE,0x00000000}};

public:
	//Construtor
 	Video(int width,int height);

    //Desenha uma linha na buffer
    void drawnTileLine(uint8_t* tile_data,uint8_t tile_type,uint8_t bgp,uint8_t line,uint8_t col,uint8_t start,uint8_t end);
     
    //Rotina que transfere buffer para o renderer e mostra na tela
    void drawnTextures();
};

#endif /* VIDEO_H_*/
