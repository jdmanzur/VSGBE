#ifndef PPU_H_
#define PPU_H_

#include <cstdint>
#include "video.h"
#include "mmu.h"

//Nº máx de sprites
#define SPRITE_NUM = 40

//TODO: Arrumar uma maneira melhor de fazer isso...
enum PpuState {HBLANK = 0,VBLANK = 1,OAM = 2,RENDER = 3};

    
class Ppu
{
private:
        //Leitura do espaço de memória
        Mmu* mem;

        //Flag de modo da ppu
        uint8_t mode_flag;

        //Contador de Linhas
        uint8_t line;

        //Contador de Pixels
        uint16_t dot;

        //Contador de ciclos de clock
        uint32_t clk;
        
        //Estáncia para o vídeo
        Video video;

public:

	//Construtor
	Ppu(Mmu *mem_s);

	//Desenha uma linha do background no buffer 
	void drawnLineBG();

        //Desenha uma linha de sprites no buffer
        void drawnLineOB();

        //Desenha um tile na tela
        void drawnTile(uint16_t tile_num, uint8_t x, uint8_t y);

        //Desenha todos os tiles no framebuffer
        void drawnAllTiles();

	//Atualiza o estado da PPU
	void update(uint8_t e_clk);

	//Copia o buffer para tela
 	void showScreen();

        //Retorna o valor do clock da PPU
        void setClk(uint16_t data);

        //Retorna uma linha do sprite
        void GetObjLine(uint8_t obj_n,uint8_t line,uint8_t* obj_data);

        //Retorna uma linha do tile
        void GetTileLine(uint16_t tile_n,uint8_t line,uint8_t* tile_data);

        //Atribui valor ao clock da PPU
        uint32_t getClk();

        //Incrementa o clock da PPU em x unidades
        void incClk(uint16_t n_clk);

        //Incrementa o contador de pixeis
        void ModeUpdate();



};


#endif /* PPU_H_ */
