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
#include <cstdint>
#include "mmu.h"


//Construtor para carregar a bios e cartucho
//TODO:Verificar e tratar exceções
Mmu::Mmu(uint8_t* bios_a, uint8_t* rom_a, uint32_t rom_s)
{
    //Decofica os dados do cabeçalho da ROM
    readROMHeader(rom_a);

    //Inicializa modo de operação do MBC
    if(this->c_type == MBC1)
        //Inicializa no modo 16Mbit de ROM/8KByte de RAM
        this->mbc1_t = 0;

    //Aloca memória para bancos de ROM
    this->b_rom = new uint8_t[rom_s - ROM_BANK_SIZE];

    //Aloca memória para os bancos de RAM
    if(this->ram_size > 0)
        this->b_ram = new uint8_t[this->ram_size];

    //Carrega a bios
    for(int i = 0; i < BIOS_SIZE; i++)
        this->bios[i] = bios_a[i];

    //Carrega a ROM
    for(int i = 0; i < rom_s; i++)
    {
        if(i < ROM_BANK_SIZE)
            this->f_rom[i] = rom_a[i];
        else
            this->b_rom[i - ROM_BANK_SIZE] = rom_a[i];
    }

    //Coloca a BIOS como primeira página
    this->fst_page = this->bios;

    //Seleciona o primeiro banco de ROM
    this->rom_sel = 0;


}

//Construtor para carregar apenas o cartucho
Mmu::Mmu(uint8_t* rom)
{
    //Carrega a ROM
    for(int i = 0; i < ROM_SIZE; i++)
    {
        if(i < ROM_BANK_SIZE)
            this->f_rom[i] = rom[i];
        else
            this->b_rom[i - ROM_BANK_SIZE] = rom[i];
    }
    //Coloca a ROM como primeira página
    this->fst_page = this->f_rom;

}

//Destrutor da Classe
Mmu::~Mmu()
{

/*
    //Desaloca a rom
    if(this->b_rom != NULL)
        delete [] this->b_rom;
    
    //Desaloca a ram
    if(this->b_ram != NULL)
        delete [] this->b_ram;

    */

}

//Lê da memória
uint8_t Mmu::read(uint16_t addr)
{
    //Verifica qual região da RAM está sendo acessada

    //BIOS e Vetores de inteuupção
    if(addr < BIOS_SIZE)
    {
        return this->fst_page[addr];
    }
    //Banco #0 da ROM 
    else if((addr >= BIOS_SIZE)&&(addr < AddrConst::ROMX))
    {
        return this->f_rom[addr];
    }
    //Banco de ROM selecionável
    else if((addr >= AddrConst::ROMX)&&(addr < AddrConst::VRAM))
    {
        return this->b_rom[(addr - AddrConst::ROMX)+(this->rom_sel * ROM_BANK_SIZE)];
    }
    //Video RAM
    else if ((addr >= AddrConst::VRAM)&&(addr < AddrConst::RAMX))
    {
        return this->v_ram[(addr - AddrConst::VRAM)];
    }
    //Banco de RAM Externa selecionável
    else if((addr >= AddrConst::RAMX)&&(addr < AddrConst::RAM0))
    {

        //Verfica se há RAM externa presente
        if(this->ram_size > 0)
            return this->b_ram[(addr - AddrConst::RAMX) + (RAM_BANK_SIZE * ram_sel)];

        return 0;
    }
    //Banco 0 de RAM interna
    else if((addr >= AddrConst::RAM0)&&(addr < AddrConst::ERAM))
    {
        return this->i_ram0[(addr - AddrConst::RAM0)];
    }
    //Espelho da RAM interna
    else if((addr >= AddrConst::ERAM)&&(addr < AddrConst::OAMS))
    {
        return this->i_ram0[(addr - AddrConst::ERAM)];
    }
    //Memória de atributo de sprites
    else if((addr >= AddrConst::OAMS)&&(addr < 0x10000))
    {
        return this->ioram[(addr - AddrConst::OAMS)];
    }

    return 0;
}

//Escreve na memória
void Mmu::write(uint16_t addr,uint8_t data)
{
    //Verifica qual região da RAM está sendo acessada

    //Habilita RAM externa
    if(addr < 0x2000)
    {


    }
    //Seleção de banco de ROM
    else if((addr >= 0x2000)&&(addr < 0x4000))
    {


        //Caso controlador de memória estiver presente muda o banco de ROM
        if(this->c_type == MBC1)
        {
            uint16_t bank = (data & 0x1F);

            //O controlador não suporta o mapeamento do banco 0
            if(bank == 0)
                bank++;

            this->rom_sel = (bank - 1);
        }
        else if(this->c_type == MBC3)
        {
            uint16_t bank = (data & 0x7F);

            //O controlador não suporta o mapeamento do banco 0
            if(bank == 0)
                bank++;

            this->rom_sel = (bank - 1);
        }


    }
    //Banco de RAM selecionável
    else if((addr >= 0x4000)&&(addr < 0x6000))
    {
        //Caso controlador de memória estiver presente muda o banco de RAM
        if(this->c_type == MBC1)
        {
            if(mbc1_t == 1)
                this->ram_sel = ((data & 0x03) - 1);
            else
                this->rom_sel = ((data & 0x03) << 5);
            
        }

    }
    //Modo de operação do MBC
    else if ((addr >= 0x6000)&&(addr < 0x8000))
    {
        //Seleciona modo de operação do controlador de memória
        mbc1_t = (data & 0x01);
       
    }
    //Video RAM
    else if ((addr >= AddrConst::VRAM)&&(addr < AddrConst::RAMX))
    {
        this->v_ram[(addr - AddrConst::VRAM)] = data;
    }
    //Banco de RAM Externa selecionável
    else if((addr >= AddrConst::RAMX)&&(addr < AddrConst::RAM0))
    {
        //Verfica se há RAM externa presente
        if(this->ram_size > 0)
            this->b_ram[(addr - AddrConst::RAMX) + (RAM_BANK_SIZE * ram_sel)] =  data;

    }
    //Banco 0 de RAM interna
    else if((addr >= AddrConst::RAM0)&&(addr < AddrConst::ERAM))
    {
        this->i_ram0[(addr - AddrConst::RAM0)] = data;
    }
    //Espelho da RAM interna
    else if((addr >= AddrConst::ERAM)&&(addr < AddrConst::OAMS))
    {
        this->i_ram0[(addr - AddrConst::ERAM)] = data;
    }
    //Memória de atributo de sprites e io
    else if((addr >= AddrConst::OAMS)&&(addr < 0x10000))
    {
        this->ioram[(addr - AddrConst::OAMS)] = data;

        //Verifica se a escrita foi no dma
        if(addr == AddrConst::DMA)
            //Chama o DMA
            this->dma();

        // Verifica se a bios foi desabilitada
        if((addr == 0xFF50)&&(data == 1))
        //if(addr == 0xFF50)
            //Desabilita a BIOS
            this->fst_page = this->f_rom;
    }
   

}

//Realiza o DMA
void Mmu::dma()
{
	//Verifica o endereço
    //TODO: Substiuir endereços    
    if(this->read(0xFF46))
	{
            //O endereço de tranasferência é: DMA * 0x100
            uint16_t addr = this->read(0xFF46) << 8;
   
            //Laço para cópia
            for(uint8_t x = 0; x < 0xA0; x++)
                //Copia o dado para o endereço 0xFE00 + passo
                //TODO: Colocar alguma constante no lugar de 0xFE000
                this->write((0xFE00+x),this->read(addr+x));
	}
}

//Decodifica os dados da rom
void Mmu::readROMHeader(uint8_t* rom)
{
    //Verica o tipo de cartucho
    this->g_type = rom[0x147];

    //Tipo de controlador
    if (g_type == 0x00)
        this->c_type = MBC0;
    else if((g_type == 0x01)||(g_type == 0x02)||(g_type == 0x03))
        this->c_type = MBC1;
    else if((g_type == 0x05)||(g_type == 0x06))
        this->c_type = MBC2;
    else if((g_type == 0x0F)||(g_type == 0x10)||(g_type == 0x11)||(g_type == 0x12)||(g_type == 0x13))
        this->c_type = MBC3;
    else if ((g_type == 0x19)||(g_type == 0x1A)||(g_type == 0x1B)||(g_type == 0x1C)||(g_type == 0x1D)||(g_type == 0x1E))
       this-> c_type = MBC5;

    //Tamanho da ram
    if(rom[0x149] == 0)
    {
        //Tamanho da RAM em bytes
        this->ram_size = 0;

        //No de bancos de RAM
        this->n_ram = 0;

    }
    else if(rom[0x149] == 1)
    {
        //Tamanho da RAM em bytes
        this->ram_size = 2048;

        //No de bancos de RAM
        this->n_ram = 1;

    }    
    else if(rom[0x149] == 2)
    {
        //Tamanho da RAM em bytes
        this->ram_size = 8192;

        //No de bancos de RAM
        this->n_ram = 1;

    }
    else if(rom[0x149] == 3)
    {
        //Tamanho da RAM em bytes
        this->ram_size = 32768;

        //No de bancos de RAM
        this->n_ram = 4;

    }
    else if(rom[0x149] == 4)
    {
        //Tamanho da RAM em bytes
        this->ram_size = 131072;

        //No de bancos de RAM
        this->n_ram = 16;

    }

    //Tamanho da ROM
    if(rom[0x148] == 0)
    {
        //Tamanho da RAM em bytes
        this->rom_size = 32768;

        //No de bancos de RAM
        this->n_rom = 2;

    }
    else if(rom[0x148] == 1)
    {
        //Tamanho da RAM em bytes
        this->rom_size = 65536;

        //No de bancos de RAM
        this->n_rom = 4;

    }    
    else if(rom[0x148] == 2)
    {
        //Tamanho da RAM em bytes
        this->rom_size = 131072;

        //No de bancos de RAM
        this->n_rom = 8;

    }
    else if(rom[0x148] == 3)
    {
        //Tamanho da RAM em bytes
        this->rom_size = 262144;

        //No de bancos de RAM
        this->n_rom = 16;

    }
    else if(rom[0x148] == 4)
    {
        //Tamanho da ROM em bytes
        this->rom_size = 524288;

        //No de bancos de ROM
        this->n_rom = 32;

    }
    else if(rom[0x148] == 5)
    {
        //Tamanho da ROM em bytes
        this->rom_size = 1024000;

        //No de bancos de ROM
        this->n_rom = 64;

    }    
    else if(rom[0x148] == 6)
    {
        //Tamanho da RAM em Kbytes
        this->rom_size = 2048000;

        //No de bancos de ROM
        this->n_rom = 128;

    }
    else if(rom[0x148] == 0x52)
    {
        //Tamanho da ROM em Kbytes
        this->rom_size = 1126400;

        //No de bancos de ROM
        this->n_rom = 4;

    }
    else if(rom[0x148] == 0x53)
    {
        //Tamanho da ROM em Kbytes
        this->rom_size = 1228800;

        //No de bancos de ROM
        this->n_rom = 80;

    }
    else if(rom[0x148] == 0x54)
    {
        //Tamanho da RAM em Kbytes
        this->rom_size = 1536000;

        //No de bancos de ROM
        this->n_rom = 96;

    }
    


}
