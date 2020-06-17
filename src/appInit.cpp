/*
 * appInit.cpp
 *
 *  Created on: 6 de mai de 2020
 *      Author: adriano
 */

using namespace std;


#include "appInit.h"

//Método temporário para carregar na memória
void loadBios(char* mem)
{
	//Local da bios
	char local[] = "/home/adriano/Documentos/gb/bios.gb";

	//Guarda o tamanho do arquivo
	streampos size;
	
  	ifstream file ("/home/adriano/Documentos/gb/bios.gb", ios::in|ios::binary|ios::ate);
  	
	//Verifica se o arquivo lido corretamente
	if (file.is_open())
  	{
		//Tamanho do arquuivo
    	size = file.tellg();
		//Inicializa do inicio
    	file.seekg (0, ios::beg);
		//Lê todo conteudo do arquivo
    	file.read (mem, size);

		//Fecha o arquivo
    	file.close();

  }



}

//Método temporário para a ROM na memória
void loadROM(char* mem)
{
	//Local da bios
	char local[] = "/home/adriano/Documentos/gb/bios.gb";

	//Guarda o tamanho do arquivo
	streampos size;
	
  	ifstream file ("/home/adriano/Documentos/gb/Amida (J).gb", ios::in|ios::binary|ios::ate);
	//ifstream file ("/home/adriano/Documentos/gb/cpu_instrs/individual/11-op a,(hl).gb", ios::in|ios::binary|ios::ate);
  	
	//Verifica se o arquivo lido corretamente
	if (file.is_open())
  	{

		//Aloca vetor temporário para arquivo
		uint8_t* file_array = new uint8_t[size];


		//Tamanho do arquuivo
    	size = file.tellg();
		//Inicializa do inicio
    	file.seekg (0, ios::beg);
		//Lê todo conteudo do arquivo
    	file.read (mem, size);

		//Fecha o arquivo
    	file.close();

		//Desaloca vetor temporário para arquivo
		delete [] file_array;

  }




}

//Método que carrega a bios e a rom na memória
void loadBiosROM(char bios_name[],char rom_name[],char* b_array, char* r_array)
{


	//Stream para leitura da bios
  	ifstream bios_file (bios_name, ios::in|ios::binary|ios::ate);

	//Guarda o tamanho do arquivo da bios
	streampos bios_size;

	//Verifica se o arquivo da rom lido corretamente
	if (bios_file.is_open())
  	{

		//Tamanho do arquuivo
    	bios_size = bios_file.tellg();

		//Inicializa do inicio
    	bios_file.seekg (0, ios::beg);

		//Lê todo conteudo do arquivo
    	bios_file.read (b_array, bios_size);

		//Fecha o arquivo
    	bios_file.close();

  }	



	//Guarda o tamanho do arquivo da rom
	streampos rom_size;

	//Stream para leitura da rom
  	ifstream rom_file (rom_name, ios::in|ios::binary|ios::ate);

  	
	//Verifica se o arquivo da rom lido corretamente
	if (rom_file.is_open())
  	{

		//Tamanho do arquuivo
    	rom_size = rom_file.tellg();

		//Inicializa do inicio
    	rom_file.seekg (0, ios::beg);

		//Lê todo conteudo do arquivo
    	rom_file.read (r_array, rom_size);

		//Fecha o arquivo
    	rom_file.close();


  }

}

void loadLogo(uint8_t* mem)
{
	uint8_t logo[48] = {0xce, 0xed, 0x66, 0x66, 0xcc, 0x0d, 0x00, 0x0b, 0x03, 0x73, 0x00, 0x83, 0x00, 0x0c, 0x00, 0x0d, 0x00, 0x08, 0x11, 0x1f, 0x88, 0x89, 0x00, 0x0e, 0xdc, 0xcc, 0x6e, 0xe6, 0xdd, 0xdd, 0xd9, 0x99, 0xbb, 0xbb, 0x67, 0x63, 0x6e, 0x0e, 0xec, 0xcc, 0xdd, 0xdc, 0x99, 0x9f, 0xbb, 0xb9, 0x33, 0x3e};

	for(int i = 0; i < 48; i++)
	{
		mem[0x104+i] = logo[i];
	}

}

int getFilesize(char file[])
{
	//Stream para leitura da bios
  	ifstream file_s (file, ios::in|ios::binary|ios::ate);

	//Verifica se o arquivo da rom lido corretamente
	if (file_s.is_open())
  	{

		//Tamanho do arquuivo
    	int tam  = file_s.tellg();

		return (int)tam;
	
	}

	return 0;

}