/*
 * regFile.h
 *
 *  Created on: 6 de mai de 2020
 *      Author: adriano
 */

#ifndef REGFILE_H_
#define REGFILE_H_

#include <cstdint>

//Máscara para acessar os bit do registro flags
enum f_bit {z = 0x80, n = 0x40, h = 0x20, c = 0x10 };

//Código dependente da "endianess" do sistema
struct RegisterFile {
	/*
	Registros represenatdos em 16 bits
	pos. 0 - BC
	pos. 1 - DE
	pos. 2 - HL
	pos. 3 - SP
	pos. 4 - AF
	*/
	uint16_t reg16[5];

	//Converter o ponteiro para 8 bits
	uint8_t* p8 = (uint8_t*)reg16;

	/*
	Ponteiro para registros de 8 bits
	pos. 0 - B
	pos. 1 - C
	pos. 2 - D
	pos. 3 - E
	pos. 4 - H
	pos. 5 - L
	pos. 6 - (HL)
	pos. 7 - A
	pos. 8 - F

	*/
	//TODO:Possivelmente perigoso, arrumar uma maneira melhor de fazer..
	uint8_t* reg8[8] = {p8+1,p8,p8+3,p8+2,p8+5,p8+4,p8+8,p8+9,};

	//Contador de programa
	uint16_t PC;
};
//Acesso aos registros
struct registers : public RegisterFile
{
	//Set
	//Registros acessados como 8 bits
	uint8_t& B() { return (*reg8[0]); }
	uint8_t& C() { return (*reg8[1]); }
	uint8_t& D() { return (*reg8[2]); }
	uint8_t& E() { return (*reg8[3]); }
	uint8_t& H() { return (*reg8[4]); }
	uint8_t& L() { return (*reg8[5]); }
	uint8_t& F() { return (*reg8[6]); }
	uint8_t& A() { return (*reg8[7]); }


	//Registros acessados como 16 bits
	uint16_t& AF() { return reg16[4]; }
	uint16_t& BC() { return reg16[0]; }
	uint16_t& DE() { return reg16[1]; }
	uint16_t& HL() { return reg16[2]; }
	uint16_t& SP() { return reg16[3]; }

	//Bits dos Flags
	void setFlagBit(int bit) { (this->F()) |= bit; }
	void clrFlagBit(int bit) { (this->F()) &= (~bit); }

	//Get
    //Registros acessados com 8 bits
	const uint8_t& B() const { return (*reg8[0]); }
   	const uint8_t& C() const { return (*reg8[1]); }
    const uint8_t& D() const { return (*reg8[2]); }
    const uint8_t& E() const { return (*reg8[3]); }
    const uint8_t& H() const { return (*reg8[4]); }
    const uint8_t& L() const { return (*reg8[5]); }
    const uint8_t& F() const { return (*reg8[6]); }
    const uint8_t& A() const { return (*reg8[7]); }

    //Registros acessados como 16 bits
	const uint16_t& AF() const { return reg16[4]; }
	const uint16_t& BC() const { return reg16[0]; }
	const uint16_t& DE() const { return reg16[1]; }
	const uint16_t& HL() const { return reg16[2]; }
	const uint16_t& SP() const { return reg16[3]; }

    //Bits dos Flags
    bool getFlagBit(int bit) { return (bool)((*reg8[6]) & bit); }

};


#endif /* REGFILE_H_ */
