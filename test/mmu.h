#ifndef MMU_H_
#define MMU_H_

#include <iostream>
#include <cstdint>

//Constantes para o tamanho da rom e da BIOS
//TODO: Isso depois tem que ser mudado para suportar outros tamanhos
#define BIOS_SIZE	0x100
#define ROM_SIZE	0x8000


//Classe que implementa o controlador de memória
//TODO: Implementar os diversos controladores de memórias
class Mmu
{
private:

	//Matriz para bios e dado do jogo
	//TODO: Encontrar uma melhor maneira de se fazer isso.
	uint8_t bios[BIOS_SIZE];

	//Vetor que representara o espaço de endereço
	uint8_t mem[0x10000];

	//Ponteiro para mapeamento da BIOS no end. de memória
	uint8_t* fst_page;


	

public:

	//Classe que faz a intermediação de escrita nos endereços de memória
	class Proxy 
	{
		//Ponteiro para o dado manipulado
		Mmu &mmu;
		
		//Gravar o índice da memoria para acessar
    	uint16_t index;

	public:
    		Proxy(Mmu& mmu, uint16_t index) : mmu(mmu), index(index){}
    		
		//Realiza a escrita na memória através da sobrecarga de operador
		void operator= (uint8_t valor)
		{

			//std::cout << "Escreveu na memória" << std::endl;
        	mmu.write(index,valor);
	       		
		}	
    		
		//Realiza a leitura da memória
		operator uint8_t() const
		{
			//std::cout << "Leu da memória" << std::endl;
			return mmu.read(index);
		}
	
	};

	//Construtor para carregar a bios e cartucho
	Mmu(uint8_t* bios, uint8_t* rom);

	//Construtor para carregar apenas o cartucho
	Mmu(uint8_t* rom);

	// Sobrecarga do operador [] para acessar a ram como um único vetor
	Proxy operator[](uint16_t index);

	//Lê da memória
	uint8_t read(uint16_t addr);

	//Escreve na memória
	void write(uint16_t addr,uint8_t data);

	//Realiza o DMA
	void dma();


};


#endif//MMU_H//
