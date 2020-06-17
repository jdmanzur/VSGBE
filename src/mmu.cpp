#include <iostream>
#include <cstdint>
#include "mmu.h"


//Construtor para carregar a bios e cartucho
//TODO:Verificar e tratar exceções
Mmu::Mmu(uint8_t* bios_a, uint8_t* rom_a, uint32_t rom_s)
{

    //Carrega a bios
    for(int i = 0; i < BIOS_SIZE; i++)
        this->bios[i] = bios_a[i];

    //Carrega a ROM
    for(int i = 0; i < ROM_SIZE; i++)
    {
        if(i < ROM_BANK_SIZE)
            this->f_rom[i] = rom_a[i];
        else
            this->b_rom[i - ROM_BANK_SIZE] = rom_a[i];
    }

    //Coloca a BIOS como primeira página
    this->fst_page = this->bios;


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
    else if((addr >= BIOS_SIZE)&&(addr < ROMX))
    {
        return this->f_rom[addr];
    }
    //Banco de ROM selecionável
    else if((addr >= ROMX)&&(addr < VRAM))
    {
        return this->b_rom[(addr - ROMX)];
    }
    //Video RAM
    else if ((addr >= VRAM)&&(addr < RAMX))
    {
        return this->v_ram[(addr - VRAM)];
    }
    //Banco de RAM Externa selecionável
    else if((addr >= RAMX)&&(addr < RAM0))
    {
        return 0;
    }
    //Banco 0 de RAM interna
    else if((addr >= RAM0)&&(addr < ERAM))
    {
        return this->i_ram0[(addr - RAM0)];
    }
    //Espelho da RAM interna
    else if((addr >= ERAM)&&(addr < OAMS))
    {
        return this->i_ram0[(addr - ERAM)];
    }
    //Memória de atributo de sprites
    else if((addr >= OAMS)&&(addr < 0x10000))
    {
        return this->ioram[(addr - OAMS)];
    }

    return 0;
}

//Escreve na memória
void Mmu::write(uint16_t addr,uint8_t data)
{
    //Verifica qual região da RAM está sendo acessada

    //BIOS e Vetores de inteuupção
    if(addr < BIOS_SIZE)
    {

    }
    //Banco #0 da ROM 
    else if((addr >= BIOS_SIZE)&&(addr < ROMX))
    {

    }
    //Banco de ROM selecionável
    else if((addr >= ROMX)&&(addr < VRAM))
    {

    }
    //Video RAM
    else if ((addr >= VRAM)&&(addr < RAMX))
    {
        this->v_ram[(addr - VRAM)] = data;
    }
    //Banco de RAM Externa selecionável
    else if((addr >= RAMX)&&(addr < RAM0))
    {

    }
    //Banco 0 de RAM interna
    else if((addr >= RAM0)&&(addr < ERAM))
    {
        this->i_ram0[(addr - RAM0)] = data;
    }
    //Espelho da RAM interna
    else if((addr >= ERAM)&&(addr < OAMS))
    {
        this->i_ram0[(addr - ERAM)] = data;
    }
    //Memória de atributo de sprites e io
    else if((addr >= OAMS)&&(addr < 0x10000))
    {
        this->ioram[(addr - OAMS)] = data;

        //Verifica se a escrita foi no dma
        if(addr == DMA)
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
