#include <iostream>
#include <cstdint>
#include "mmu.h"


//Construtor para carregar a bios e cartucho
//TODO:Verificar e tratar exceções
Mmu::Mmu(uint8_t* bios, uint8_t* rom)
{
    //Carrega a bios
    for(int i = 0; i < BIOS_SIZE; i++)
        this->bios[i] = bios[i];

    //Carrega a ROM
    for(int i = 0; i < ROM_SIZE; i++)
        this->mem[i] = rom[i];

    //Coloca a BIOS como primeira página
    this->fst_page = this->bios;
}

//Construtor para carregar apenas o cartucho
Mmu::Mmu(uint8_t* rom)
{
    //Carrega a ROM
    for(int i = 0; i < ROM_SIZE; i++)
        this->mem[i] = rom[i];

    //Coloca a ROM como primeira página
    this->fst_page = this->mem;

}

// Sobrecarga do operador [] para acessar a ram como um único vetor
Proxy Mmu::operator[](uint16_t index)
{
    return Proxy(*this,index);
}

//Lê da memória
uint8_t Mmu::read(uint16_t addr)
{
    //Verifica se Carrega da BIOS ou ROM
    if(addr < BIOS_SIZE)
        return this->fst_page[addr];
    else
        return this->mem[addr];
}

//Escreve na memória
void Mmu::write(uint16_t addr,uint8_t data)
{
    //Não pode escrver na área da ROM
    if(addr >= ROM_SIZE)
        this->mem[addr] = data;

    //Verifica se a escrita foi no dma
    if(addr == 0xFF46)
        //Chama o DMA
        this->dma();

    //Verifica se a bios foi desabilitada
    if((addr == 0xFF50)&&(this->mem[0xFF50] == 1))
        //Desabilita a BIOS
        this->fst_page = this->mem;
    
}

//Realiza o DMA
void Mmu::dma()
{
	//Verifica o endereço
    //TODO: Substiuir endereços    
    if(this->mem[0xFF46])
	{
            //O endereço de tranasferência é: DMA * 0x100
            uint16_t addr = this->mem[0xFF46] << 8;
   
            //Laço para cópia
            for(uint8_t x = 0; x < 0xA0; x++)
                //Copia o dado para o endereço 0xFE00 + passo
                //TODO: Colocar alguma constante no lugar de 0xFE000
                this->mem[0xFE00+x] = this->mem[addr+x];
	}
}

