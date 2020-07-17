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

#include <iostream>
#include <fstream>
#include <cstdint>
#include "ioRegAddr.h"
#include "video.h"
#include "ppu.h"



//Construtor
Ppu::Ppu(Mmu* mem_s) : video(640,560)
{
        //Leitura do espaço de memória
        this->mem = mem_s;

        //Flag de modo da ppu
        this->mode_flag = HBLANK;
        
        //Contador de Linhas
        this->line = 0;

        //Contador de ciclos de clock
        this->clk = 0;

        //Inicializa o video
        //this->video =  Video(640,576);

}

//Desenha uma linha do background no buffer
//TODO:Melhorar, Os nomes estão confusos 
void Ppu::drawnLineBG()
{
	//Deslocamento horizontal e vertical para scroll
        uint8_t scrollx = (*mem).read(SCX);
        uint8_t scrolly = (*mem).read(SCY);

        //tiles deslocados em relação ao scroll y
        uint8_t inc_tile_y = ((scrolly & 0xF8) >> 3);
         
        //Tiles deslocados em relação ao scroll x
        uint8_t inc_tile_x = ((scrollx & 0xF8) >> 3);
        
        //Colunas deslocadas dentro do tile pelo scroll x
        uint8_t inc_colu_x = (scrollx & 0x07);

        //Linhas deslocadas dentro do tile pelo scroll y
        uint8_t inc_line_y = (scrolly & 0x07);

        //No do primero tile a ser desenhado
        uint16_t n_tile = (((((int)this->line/8)*32) + ((int)inc_tile_y * 32) + ((((this->line & 0x07) + inc_line_y)>>3)*32) + inc_tile_x) & 0x03FF);

        //No da linha dentro sprite
       uint8_t sub_line = (((this->line & 0x07) + inc_line_y) & 0x07);

        //Linha de tiles para espelho horizontal
        uint8_t tile_line = n_tile/32;

        //Variável para guardar a linha decodificada
        uint8_t tile_data[8];

        //Desenha o tile no buffer
        //TODO:Verficar comportamento quando houver deslocamento horizontal
        for(int i = 0; i < 160; )
        {
			
		int c_start = 0;
		int c_end = 7;
			
                //Tiles para desenhar
                if(inc_colu_x &&(i == 0))
        	        c_start = inc_colu_x;

               
                //Pega os dados do tile
                this->GetTileLine((n_tile++),sub_line,tile_data);
                
                //Desenha a linha do tile no framebuffer
                this->video.drawnTileLine(tile_data,0,(*mem).read(BGP),this->line,(i),c_start,c_end);

                //Verifica se atingiu o fim da tela
                if((n_tile/32) > tile_line)
                        n_tile = (tile_line * 32);

                //Incrementa a coluna 
                i += ((c_end - c_start)+1);

        }

}

//Desenha uma linha do tile no buffer
//TODO:Melhorar
void Ppu::drawnLineOB()
{
        //Tamanho dos sprites : 0=8x8, 1=8x16
        uint8_t ob_size = ((*mem).read(LCDC) & 0x04) ? 16 : 8;

        //Número de sprites na linha atual
        uint8_t ob_num = 0;

        //Lista de sprites na linha
        uint8_t ob_list[10];

        //Veriável temporária para contar os sprites verificados
        uint8_t i = 0;

        //Dados da linhado sprite
        uint8_t obj_data[8];

        //Percorre toda a OAM e verifica quais sprites devem ser desenhados na linha
        while((i < 40) && (ob_num < 10))
        {
                //Posição X do sprite
                uint8_t obj_x = ((*mem).read(OAMS + (i*4) + 1) - 8);

                //Posição Y do sprite
                uint8_t obj_y = ((*mem).read(OAMS + (i*4)) - 16);

                //Endereço do sprite na OAM
                uint16_t oam_addr = (OAMS + (i * 4));

                //Palheta utilizada
                uint8_t obj_pal = (((*mem).read(oam_addr + 3) & 0x10) >> 4) ? (*mem).read(OBP1) : (*mem).read(OBP0);
                uint8_t pal_num = (((*mem).read(oam_addr + 3) & 0x10) >> 4) ? 2 : 1;

                //Verifica se o sprite está no campo de visão na tela
                if((obj_x < 160)&&(obj_y < 144)&&(this->line >= obj_y))
                        //Verifica se o sprite está contido na linha
                        if((this->line - obj_y) < ob_size)
                        {
                                //Adiciona o sprite à lista
                                ob_list[ob_num++] = i;

                                //Pega os dados do sprite
                                this->GetObjLine(i,(this->line - obj_y),obj_data);

                                //Desenha o sprite
                                //TODO:Criar uma função que leva em conta a palheta
                                video.drawnTileLine(obj_data,pal_num,obj_pal,this->line,obj_x,0,7);
                        }
   

                //Incrementa o sprite
                i++;

        }

        //std::cout << (int)ob_num << std::endl;

}

//Retorna uma linha do sprite
void Ppu::GetObjLine(uint8_t obj_n,uint8_t line,uint8_t* obj_data)
{

        //Tamanho dos sprites : 0=8x8, 1=8x16
        bool ob_size = ((*mem).read(LCDC) & 0x04) ? 1 : 0;

        //Endereço do sprite na OAM
        uint16_t oam_addr = (OAMS + (obj_n * 4));

        //Secção da ram que guarda os dados dos sprites
        //Verifica qual a Banco de dados de sprites utilizado
	uint16_t data_addr = 0x8000 + ((*mem).read(oam_addr + 2) * 16);

        //Muda o endereço, se os sprites for 8x16
        if(ob_size) 
                data_addr &= 0xFFFE;
       
        //Decodificação das Flags
        //Verifica rotação eixo y
        bool y_flip = (((*mem).read(oam_addr + 3) & 0x40) >> 6);
        //Verifica rotação eixo x
        bool x_flip = (((*mem).read(oam_addr + 3) & 0x20) >> 5);

        //Calcula o novo endereço para rotação no eixo y
        //TODO: Adicinar suporte para 8x16
        if(y_flip)
                data_addr = (data_addr + 14) - (line * 2);
        else
                data_addr += (line * 2);

        
        //Percorre as colunas dentro do sprite
        if(x_flip)
		{
                for(int j = 0; j < 8; j++)
				{	
					
					//TODO:Melhorar, código pouco legível
					if(j == 0)
					{
						obj_data[j] = (((*mem).read(data_addr)&1))|(((*mem).read(data_addr+1) & 1)<<1);
					}
					else
					{
					
						obj_data[j] = (((*mem).read(data_addr)&(1<<j))>>j)|(((*mem).read(data_addr+1)&(1<<j))>>(j-1));
					}

                }
         }        
        else
        {
			        for(int j = 0; j < 8; j++)
			        {
						//TODO:Melhorar, código pouco legível
						if(j == 0)
						{
							obj_data[7-j] = (((*mem).read(data_addr)&1))|(((*mem).read(data_addr+1) & 1)<<1);
						}
						else
						{
					
							obj_data[7-j] = (((*mem).read(data_addr)&(1<<j))>>j)|(((*mem).read(data_addr+1)&(1<<j))>>(j-1));
						}

                        
                        //obj_data[7-j] = (((*mem).read(data_addr)&(1<<j))>>j)|(((*mem).read(data_addr+1)&(1<<j))>>(j-1));
                        
                    }
        }



}

//Retorna uma linha do Tile
void Ppu::GetTileLine(uint16_t tile_n,uint8_t line,uint8_t* tile_data)
{
        //Secção da ram que guarda os códigos de tiles
        //Verifica qual a Banco de códigos de tiles utilizado
        //TODO:Substituir o No 0x08 por máscara p/ bit 3
	uint16_t code_addr = ((*mem).read(LCDC) & 0x08) ? 0x9C00 : 0x9800;

	//Secção da ram que guarda os dados dos tiles
        //Verifica qual a Banco de dados de tiles utilizado
        //TODO:Substituir o No 0x10 por máscara p/ bit 4
	uint16_t data_addr = ((*mem).read(LCDC) & 0x10) ? 0x8000 : 0x9000;

        uint16_t tile_addr;
       
        //Endereço do tile
        if((data_addr == 0x9000)&&((*mem).read(code_addr + tile_n) > 127))
                tile_addr = data_addr - ((256 -(*mem).read(code_addr + tile_n))*16);
        else
                tile_addr = data_addr + ((*mem).read(code_addr + tile_n)*16);

        //Endereço Base
        uint16_t base = tile_addr + (line * 2);

        //Percorre as colunas dentro do tile
        for(int j = 0; j < 8; j++)
        {
                //TODO:Melhorar, código pouco legível
                if(j == 0)
                {
					tile_data[7-j] = (((*mem).read(base)&1))|(((*mem).read(base+1) & 1)<<1);
				}
				else
                {
					
					tile_data[7-j] = (((*mem).read(base)&(1<<j))>>j)|(((*mem).read(base+1)&(1<<j))>>(j-1));
				}
        }



        //if((this->getClk()%500) == 0)
        //      std::cout << "Data: " << std::hex << data_addr << " Code: " << code_addr << std::endl;
        //        std::cout << std::hex << (int)(*mem).read(LCDC) << std::endl;
   
}

//Desenha todos os tiles no framebuffer
void Ppu::drawnAllTiles()
{

        for(int i = 0; i < 0x20; i++)
                for(int j = 0; j < 0x20; j++)
                {
                        this->drawnTile(i+(j*32),(i*8),(j*8)); 
                }


}

void Ppu::drawnTile(uint16_t tile_num, uint8_t x, uint8_t y)
{
        //Secção da ram que guarda os códigos de tiles
        //Verifica qual a Banco de códigos de tiles utilizado
        //TODO:Substituir o No 0x08 por máscara p/ bit 3
	uint16_t code_addr = ((*mem).read(LCDC) & 0x08) ? 0x9C00 : 0x9800;

	//Secção da ram que guarda os dados dos tiles
        //Verifica qual a Banco de dados de tiles utilizado
        //TODO:Substituir o No 0x10 por máscara p/ bit 4
	uint16_t data_addr = ((*mem).read(LCDC) & 0x10) ? 0x8000 : 0x8800;

        //Endereço do tile
        uint16_t tile_addr = data_addr + ((*mem).read(code_addr + tile_num)*16);

        //Varivel para guardar o tile
        uint8_t tile[8][8];

        //Decodofica as linhas
        for(int i = 0; i < 16; i+=2)
        {

                //Endereço Base
                uint16_t base = tile_addr + i;

                //Percorre as colunas dentro do tile
                for(int j = 0; j < 8; j++)
                        tile[i/2][7-j] = (((*mem)[base]&(1<<j))>>j)|(((*mem)[base+1]&(1<<j))>>(i-j));

        }

        /*
        for(int x = 0; x < 8; x++)
        {
                for(int y = 0; y < 8; y++)
                        std::cout << (int)tile[x][y] << " ";

                std::cout << std::endl;

        }

        */
        video.writeTileBuffer(tile,x,y,(*mem).read(BGP));

}

//Atualiza o estado da PPU
void Ppu::update(uint8_t e_clk)
{
	//Variveis do estado do controlador de vídeo
        //Flag para o background 
        uint8_t bg_on = ((*mem).read(LCDC) & 0x01);
        //Flag para os sprites 
        uint8_t ob_on = (((*mem).read(LCDC) & 0x02) >> 1);
        //Flag para a janela 
        uint8_t win_on = (((*mem).read(LCDC) & 0x20) >> 4);
        //Flag para o controlador de vídeo 
        uint8_t lcd_on = (((*mem).read(LCDC) & 0x80) >> 6);

        //Atualiza o clock
	this->incClk(e_clk);

        //Linha anterior
        int old_line = this->line;

        //Atualiza o No da linha
        this->line = (this->clk / 456);

        //Verifica se vídeo está ativo
        if(lcd_on)
	{

		//Atualiza o background, caso esteja habilitado  
		if((this->line != old_line))
                { 
                        //this->line = new_line;
                        
                        if(this->line < 144)
                        {
                                this->drawnLineBG();
                                //this->drawnAllTiles();
                                this->drawnLineOB();
                        }
                        else if (this->line == 144)
                        {
                                this->showScreen();
                        }
                }
	
	}

        //Atualiza  o registro contador de linhas
        (*mem).write(LY,this->line);

        //Atualiza os registros da PPU
        this->ModeUpdate();

        //Atualiza a Flag de LCY
        if((*mem).read(LY) == (*mem).read(LYC))
                (*mem).write(STAT,((*mem).read(STAT) | 0x04));
        else
                (*mem).write(STAT,((*mem).read(STAT) & 0xFB));   
        
        
}

//Copia o buffer para tela
void Ppu::showScreen()
{
	//Flag para o controlador de vídeo 
        uint8_t lcd_on = (((*mem).read(LCDC) & 0x80) >> 6);
        
        //Atualiza o display
       // if(lcd_on)
        this->video.drawnTextures();
}

//Retorna o valor do clock da PPU
uint32_t Ppu::getClk()
{
        return this->clk;
}

//Atribui valor ao clock da PPU
void Ppu::setClk(uint16_t data) 
{
        this->clk = data; 

        //Caso o valor do clock for maior que No de clks de uma tela
        //TODO:Substituir os No mágicos
        if(this->clk > 70224)
                this->clk -= 70224;
}

//Incrementa o clock da PPU em x unidades
void Ppu::incClk(uint16_t n_clk)
{
        //Incrementa
        this->clk += n_clk; 

        //Se caso tenha alcaçando o fim da tela
        //TODO:Substituir os No mágicos
        if(this->clk > 70224)
                this->clk -= 70224;
}

//Incrementa o contador de pixeis
void Ppu::ModeUpdate()
{
        //Incrementa o contador de pixel
        this->dot = (this->clk%455);

        //Estado antigo da PPU
        uint8_t old_mode = this->mode_flag;

        //Verifica se interrupções habilitadas
        //Interrupção de V BLANK
        bool v_blank_on = ((*mem).read(IE) & 0x01);
        //Interrupção Selecionável LCDC
        bool lcdc_on = ((*mem).read(IE) & 0x02);

        //Interrupções do registro STAT
        //HBLANK - Começo do modo 0
        bool stat3 = ((*mem).read(STAT) & 0x08);
        //VBLANK- Começo do modo 1 (adicional à INT 40)        
        bool stat4 = ((*mem).read(STAT) & 0x10);
        //OAM - Início do modo 2 e modo 1        
        bool stat5 = ((*mem).read(STAT) & 0x20);
        //LY=LYC       
        bool stat6 = ((*mem).read(STAT) & 0x40);


        //Atualiza o estado da PPU
        if(this->line < 144)
        {
                if((dot >= 0)&&(dot < 80))
                {
                        this->mode_flag = OAM;

                        //Altera o modo na memória
                        (*mem).write(STAT,(((*mem).read(STAT) & 0xFC) | 0x02));
                }
                else if((dot >= 80)&&(dot < 200))
                {
                        this->mode_flag = RENDER;

                        //Altera o modo na memória
                        (*mem).write(STAT,(((*mem).read(STAT) & 0xFC) | 0x03));
                }
                else if(dot >= 200)
                {
                        this->mode_flag = HBLANK;

                        //Altera o modo na memória
                        (*mem).write(STAT,(((*mem).read(STAT)& 0xFC)));
                }
        }
        else
        {
                this->mode_flag = VBLANK;

                //Altera o modo na memória
                (*mem).write(STAT,(((*mem).read(STAT) & 0xFC) | 0x01));

        }

        //Verifica se o modo foi atualizado
        if(this->mode_flag != old_mode)
        {
                //Verfica se a interrupção de V BLANK está ocorrendo
                if(this->mode_flag == VBLANK)
                        (*mem).write(IF,((*mem).read(IF) | 0x01));

                //Verifica as interrupções stat
                else
                {
                        //HBLANK - Começo do modo 0
                        if(stat3 && (this->mode_flag == HBLANK))
                        {
                                //Habilia a flag de interrupção
                                (*mem).write(IF,((*mem).read(IF) | 0x02));

                        }
                        //VBLANK- Começo do modo 1 (adicional à INT 40) 
                        else if(stat4 && (this->mode_flag == VBLANK))
                        {
                                //Habilia a flag de interrupção
                                (*mem).write(IF,((*mem).read(IF) | 0x02));

                        }
                        //OAM - Início do modo 2 e modo 1   
                        else if(stat5 && ((this->mode_flag == OAM)||(this->mode_flag == VBLANK)))
                        {
                                //Habilia a flag de interrupção
                                (*mem).write(IF,((*mem).read(IF) | 0x02));

                        }
                        //LY=LYC 
                        else if (stat6 && ((*mem).read(LY) == (*mem).read(LYC)))
                        {
                                //Habilia a flag de interrupção
                                (*mem).write(IF,((*mem).read(IF) | 0x02));

                        }



                }
                
        }



}
