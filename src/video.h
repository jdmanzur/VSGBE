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

public:
	//Construtor
 	Video(int width,int height);

    //Desenha uma linha na buffer
    void drawnTileLine(uint8_t* tile_data, uint8_t bgp,uint8_t line,uint8_t col,uint8_t start,uint8_t end);

    //Escreve diretamente no buffer
    void writeTileBuffer(uint8_t data[8][8],uint16_t,uint16_t,uint8_t bgp);

    //Converte as cores para RGB
    void decodeRGBA(uint8_t* tile_data,uint8_t bgp,uint32_t* line_rgba);
     
    //Rotina que transfere buffer para o renderer e mostra na tela
    void drawnTextures();
};

#endif /* VIDEO_H_*/
