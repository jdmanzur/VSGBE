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

#include "cpu.h"

//TODO: Veficar todas operações com (HL) e trocar if por ponteiro
//Construtor
SimpleZ80::SimpleZ80(Mmu* mem_arr)
{
	//Inicializa as vaviráveis internas
	IME = 0;
	clk_elapsed = 0;
	cpu_state  = RUN;
	this->mem = mem_arr;

}



//Executa uma instrução
void SimpleZ80::runInstruction()
{
	//Verifica interrupções
	this->Checkinterrupt();
	//Carrega Instrução (Fecth)
    uint8_t opcode[] = {(*mem).read(reg_file.PC), (*mem).read(reg_file.PC + 1), (*mem).read(reg_file.PC + 2) };
    //Decodifica e excuta
    (this->*instr_array[opcode[0]])(opcode);

}

//Executa um opcode
void SimpleZ80::runOp(uint8_t *opcode)
{
	//Decodifica e excuta
	(this->*instr_array[opcode[0]])(opcode);
}

//No de clock levados pela última instrução
uint16_t SimpleZ80::getClkElapsed() { return this->clk_elapsed; }

//Verifica se houve interrupção
void SimpleZ80::Checkinterrupt()
{
	//Verifica os bis de isntrução

	//V BLANK
	bool vblank_if = ((*mem).read(IF) & 0x01);

	//LCLD
	bool lcld_if = ((*mem).read(IF) & 0x02);

	//P10-P13 Terminal Negative Edge
	bool joy_if = ((*mem).read(IF) & 0x10);

	//Caso tenha ocorrido a interrupção de V BLANK
	if(vblank_if && this->IME)
	{
		//Coloca o valor da próxima instrução na pilha
		(*mem).write(--this->reg_file.SP(),((this->reg_file.PC & 0xFF00)>>8));
		(*mem).write(--this->reg_file.SP(),(this->reg_file.PC & 0x00FF));

		//Executa o salto para o endereço de interrupção
		this->reg_file.PC = 0x40;

		//Desabilita a interrupção
		(*mem).write(IF,((*mem).read(IF) & 0xFE));

	}else if (lcld_if && this->IME)
	{
		//Coloca o valor da próxima instrução na pilha
		(*mem).write(--this->reg_file.SP(),((this->reg_file.PC & 0xFF00)>>8));
		(*mem).write(--this->reg_file.SP(),(this->reg_file.PC & 0x00FF));

		//Executa o salto para o endereço de interrupção
		this->reg_file.PC = 0x48;

		//Desabilita a interrupção
		(*mem).write(IF,((*mem).read(IF) & 0xFD));		
	}
	

}


//NOP - sem operação
//opcode - 0x00
void SimpleZ80::nop(uint8_t* opcode)
{
	//Atualiza o contador de programa
    this->reg_file.PC += 1;

    //Atualiza o clock
     this->clk_elapsed = 4;

}

//LD r16,d16 - Carrega constante int16 para registro de 16 bits
//Opcodes - 0x01, 0x11, 0x21, 0x31
void SimpleZ80::ldr16d(uint8_t* opcode)
{
	//varivel temporária para indentificar o subgrupo da instrução
    uint8_t reg = (opcode[0] & 0x30) >> 4;

	//Carrega o valor no registro
	this->reg_file.reg16[reg] = ((opcode[2] << 8) + opcode[1]);

	//Atualiza o contador de programa
	this->reg_file.PC += 3;

	//Atualiza o clock
	this->clk_elapsed = 12;

}

//LD (reg16),A - Carrega o end. de memória apontado por registro com valor de A
//Opcodes - 0x02, 0x12, 0x22, 0x32
void SimpleZ80::ldp16a(uint8_t* opcode)
{
	//varivel temporária para indentificar o registro
    uint8_t reg = (opcode[0] & 0x30) >> 4;

    //LD (HL+),A
    if(reg == 2)
    {
    	//Carrega o valor de A na memória
    	(*mem).write(this->reg_file.HL(),this->reg_file.A());

    	//Incrementa o registro
    	this->reg_file.HL()++;

    }
    //LD (HL-),A
    else if(reg == 3)
	{
    	//Carrega o valor de A na memória
    	(*mem).write(this->reg_file.HL(),this->reg_file.A());

    	//Decrementa o registro
    	this->reg_file.HL()--;

	}
    else
    {
    	//Carrega o valor de A na memória
    	(*mem).write(this->reg_file.reg16[reg],this->reg_file.A());

    }


	//Atualiza o contador de programa
	this->reg_file.PC += 1;

	//Atualiza o clock
	this->clk_elapsed = 8;
}

//INC reg16 - Incrementa o registro de 16 bits
//Opcodes - 0x03, 0x13, 0x23, 0x33
void SimpleZ80::incr16(uint8_t* opcode)
{
	//varivel temporária para indentificar o registro
	uint8_t reg = (opcode[0] & 0x30) >> 4;

	//Incrementa o registro
   	this->reg_file.reg16[reg]++;

	//Atualiza o contador de programa
   	this->reg_file.PC += 1;

	//Atualiza o clock
	this->clk_elapsed = 8;
}

//INC reg8 - Incrementa o registro de 8 bits
//Opcodes - 0x04, 0x0C, 0x14, 0x1C, 0x24, 0x2C, 0x34, 0x3C
void SimpleZ80::incr8(uint8_t* opcode)
{
	//Limpa os Flgas
	this->reg_file.F() &= 0x1F;

	//varivel temporária para indentificar o registro de destino
	uint8_t y = (opcode[0] & 0x38) >> 3;

	if(y == 6)
	{
		//Atualiza o half carry
		if ((((*mem).read(this->reg_file.HL()) & 0x0F)+1) == 0x10)
				this->reg_file.setFlagBit(h);

		//Incrementa o end. de memória
		(*mem).write(this->reg_file.HL(),((*mem).read(this->reg_file.HL()) + 1));

		//Atualiza os flag Zero
		if((*mem).read(this->reg_file.HL()) == 0)
			this->reg_file.setFlagBit(z);



		//Atualiza o clock
		this->clk_elapsed = 12;

	}
	else
	{
		//Atualiza o half carry
		if ((((*this->reg_file.reg8[y]) & 0x0F)+1) == 0x10)
			this->reg_file.setFlagBit(h);

		//Incrementa o registro
		(*this->reg_file.reg8[y])++;

		//Atualiza os flag Zero
		if((*this->reg_file.reg8[y]) == 0)
			this->reg_file.setFlagBit(z);

		//Atualiza o clock
		this->clk_elapsed = 4;

	}

	//Atualiza o contador de programa
   	this->reg_file.PC += 1;


}

//DEC reg8 - Decrementa o registro de 8 bits
//Opcodes - 0x05, 0x0D, 0x15, 0x1D, 0x25, 0x2D, 0x35, 0x3D
void SimpleZ80::decr8(uint8_t* opcode)
{
	//Limpa os flags
	this->reg_file.F() &= 0x10;
	this->reg_file.setFlagBit(n);

	//Registro de origem
	uint8_t reg_o = (opcode[0] & 0x38) >> 3;

	if(reg_o == 6)
	{

		//Atualiza o half carry
		if(((int)(((*mem).read(this->reg_file.HL())) & 0x0F)-1) < 0x00)
			this->reg_file.setFlagBit(h);
	
		//Decrementa o registro
		(*mem).write(this->reg_file.HL(),((*mem).read(this->reg_file.HL()) - 1));

		//Verifica o zero
		if((*mem).read(this->reg_file.HL()) == 0)
			this->reg_file.setFlagBit(z);

   		//Atualiza o clock
    	this->clk_elapsed = 12;
	}
	else
	{
		//Ponteiro para qual registro decrementar
		uint8_t *p_reg = (this->reg_file.reg8[reg_o]);

		//Atualiza o half carry
		if(((int)((*p_reg) & 0x0F)-1) < 0x00)
			this->reg_file.setFlagBit(h);
	
		//Decrementa o registro
		(*p_reg)--;

		//Verifica o zero
		if((*p_reg) == 0)
			this->reg_file.setFlagBit(z);

   		//Atualiza o clock
    	this->clk_elapsed = 4;	
	}

    //Atualiza o contador de programa
    this->reg_file.PC += 1;

}

//LD reg8,d8 - Carrega o registro de 8 bits com constante
//Opcodes - 0x06, 0x0E, 0x16, 0x1E, 0x26, 0x2E, 0x36, 0x3E
void SimpleZ80::ldr8d(uint8_t* opcode)
{
	//varivel temporária para indentificar o subgrupo da instrução
	uint8_t y = (opcode[0] & 0x38) >> 3;

	if(y == 6)
	{

		//Carrega o valor no end. de memória
		(*mem).write(this->reg_file.HL(),opcode[1]);

		//Atualiza o clock
		this->clk_elapsed = 12;

	}
	else
	{
		//Carrega o valor no registro
		(*this->reg_file.reg8[y])  = opcode [1];


		//Atualiza o clock
		this->clk_elapsed = 8;

	}

	//Atualiza o contador de programa
   	this->reg_file.PC += 2;
}

//RLCA - Rotaciona o registro A para esquerda.
//Opcode - 0x07
void SimpleZ80::rlca(uint8_t* opcode)
{
	//Limpa o Flags
	this->reg_file.F() = 0;

	//Verifica o bit de overflow
	if (this->reg_file.A() & 0x80)
		this->reg_file.setFlagBit(c);

	//Executa RLCA
	this->reg_file.A() = (this->reg_file.A() << 1);

	//Copia o carry para bit 0
	this->reg_file.A() |= this->reg_file.getFlagBit(c);

	//Atualiza o contador de programa
	this->reg_file.PC += 1;

	//Atualiza o clock
	this->clk_elapsed = 4;


}

//LD (d16),SP - Carrega o valor de memória apontado por constante com o valor de SP
//Opcode - 0x08
void SimpleZ80::ldpsp(uint8_t* opcode)
{
	//Calcula o end. alvo
	uint16_t addr = (opcode[1] | (opcode[2] << 8));

	//Armazena o dado da pilha na memória
	(*mem).write(addr,(this->reg_file.SP() & 0x00FF));
	(*mem).write((addr + 1),((this->reg_file.SP() & 0xFF00 ) >> 8));

	//Atualiza o contador de programa
	this->reg_file.PC += 3;

	//Atualiza o clock
	this->clk_elapsed = 20;

}

//ADD HL,reg16 - Soma o valor de reg16 com HL
//Opcodes - 0x09, 0x19, 0x29, 0x39
void SimpleZ80::ldhlr16(uint8_t* opcode)
{
	//Limpa os Flgas
	this->reg_file.F() &= 0x8F;

	//varivel temporária para indentificar o registro
	uint8_t reg = (opcode[0] & 0x30) >> 4;

	//Verifica o half carry
	if(((this->reg_file.HL()&0xFFF ) + (this->reg_file.reg16[reg]&0xFFF)) >= 0x1000)
		this->reg_file.setFlagBit(h);

	//verifica o carry
	if((uint32_t)(this->reg_file.HL() + this->reg_file.reg16[reg]) > 0xFFFF)
		this->reg_file.setFlagBit(c);

	//Efetua a soma
	this->reg_file.HL() += this->reg_file.reg16[reg];

	//Atualiza o contador de programa
	this->reg_file.PC += 1;

	//Atualiza o clock
	this->clk_elapsed = 20;

}

//LD A,(reg16) - Carrega o resgitro A com o end. de memória apontado por reg16
//Opcodes - 0x0A, 0x1A, 0x2A, 0x3A
void SimpleZ80::ldap16(uint8_t* opcode)
{
	//varivel temporária para indentificar o registro
    uint8_t reg = (opcode[0] & 0x30) >> 4;

    //LD A,(HL+)
    if(reg == 2)
    {
    	//Carrega o valor de A na memória
    	this->reg_file.A() = (*mem).read(this->reg_file.HL());

    	//Incrementa o registro
    	this->reg_file.HL()++;

    }
    //LD A,(HL-)
    else if(reg == 3)
	{
    	//Carrega o valor de A na memória
    	this->reg_file.A() = (*mem).read(this->reg_file.HL());

    	//Decrementa o registro
    	this->reg_file.HL()--;

	}
    else
    {
    	//Carrega o valor de A na memória
    	this->reg_file.A() = (*mem).read(this->reg_file.reg16[reg]);

    }


	//Atualiza o contador de programa
	this->reg_file.PC += 1;

	//Atualiza o clock
	this->clk_elapsed = 8;
}

//DEC reg16 - Decrementa o registro de 16 bits
//Opcodes - 0x0B, 0x1B, 0x2B, 0x3B
void SimpleZ80::decr16(uint8_t* opcode)
{
	//varivel temporária para indentificar o registro
	uint8_t reg = (opcode[0] & 0x30) >> 4;

	//Decrementa 0 registro
	this->reg_file.reg16[reg]--;

	//Atualiza o contador de programa
	this->reg_file.PC += 1;

	//Atualiza o clock
	this->clk_elapsed = 8;

}

//RRCA - Rotaciona o registro A para direita.
//Opcode - 0x0F
void SimpleZ80::rrca(uint8_t* opcode)
{
	//Limpa o Flags
	this->reg_file.F() = 0;

	//Verifica o primeiro bit para overflow
	if (this->reg_file.A() & 0x01)
		//Seta o flag
		this->reg_file.setFlagBit(c);

	//Executa RLCA
	this->reg_file.A() = (this->reg_file.A() >> 1);

	//Copia o carry para bit 7
	this->reg_file.A() |= (this->reg_file.getFlagBit(c) << 7);

	//Atualiza o contador de programa
	this->reg_file.PC += 1;

	//Atualiza o clock
	this->clk_elapsed = 4;


}

//STOP 0 - Para a CPU
//Opcode - 0x10
void SimpleZ80::stop(uint8_t* opcode)
{
	//Para a CPU
	this->cpu_state = STOP;

	//Parte desnecessária, mantida só para lembrar que a intrução STOP usa dois bytes da memória

	//Atualiza o contador de programa
	this->reg_file.PC += 2;

	//Atualiza o clock
	this->clk_elapsed = 4;

}

//RLA - Rotaciona o registro A para esquerda com carry.
//Opcode - 0x17
void SimpleZ80::rla(uint8_t* opcode)
{
	//Variável temporária para guardar o carry
	bool c_old = (this->reg_file.getFlagBit(c));

	//Limpa o Flags
	this->reg_file.F() = 0;

	//Verifica o primeiro bit para overflow
	if (this->reg_file.A() & 0x80)
		//Seta o flag
		this->reg_file.setFlagBit(c);

	//Executa RRA
	this->reg_file.A() = (this->reg_file.A() << 1);

	//Copia o carry para bit 7
	this->reg_file.A() |= c_old;

	//Atualiza o contador de programa
	this->reg_file.PC += 1;

	//Atualiza o clock
	this->clk_elapsed = 4;

}

//JR e8 - Salto incondicional relativo pela constante com sinal e8
//Opcode - 0x18
void SimpleZ80::jre8(uint8_t* opcode)
{
			//Variável para guardar o end. relativo
			int addr = 0;
			
			//Calcula o endereço relativo
	 	 	 addr = (opcode[1] > 127) ? (opcode[1] - 256) : addr = opcode[1];

            //Executa JR r8
            this->reg_file.PC += addr;

            //Atualiza o contador de programa
            this->reg_file.PC += 2;

            //Atualiza o clock
            this->clk_elapsed = 12;
}

//RRA - Rotaciona o registro A para direita com carry.
//Opcode - 0x1F
void SimpleZ80::rra(uint8_t* opcode)
{
	//Variável temporária para guardar o carry
	bool c_old = (this->reg_file.getFlagBit(c));

	//Limpa o Flags
	this->reg_file.F() = 0;

	//Verifica o primeiro bit para overflow
	if (this->reg_file.A() & 0x01)
		//Seta o flag
		this->reg_file.setFlagBit(c);

	//Executa RRA
	this->reg_file.A() = (this->reg_file.A() >> 1);

	//Copia o carry para bit 7
	this->reg_file.A() |= //Copia o carry para bit 7
		this->reg_file.A() |= (c_old << 7);

	//Atualiza o contador de programa
	this->reg_file.PC += 1;

	//Atualiza o clock
	this->clk_elapsed = 4;

}

//JR NZ,e8 - Salto condicional  relativo pela constante com sinal e8
//Opcode - 0x20
void SimpleZ80::jrnze8(uint8_t* opcode)
{
	//Verifica o flag
	if(!this->reg_file.getFlagBit(z))
	{
		//Calcula o endereço relativo
		int addr = (opcode[1] > 127) ? (opcode[1] - 256) : addr = opcode[1];

		//Executa o salto
		this->reg_file.PC += addr;

		//Atualiza o clock
		this->clk_elapsed = 12;


	}
	else
		//Não ocorreu o salto
		//Atualiza o clock
        this->clk_elapsed = 8;


    //Atualiza o contador de programa
    this->reg_file.PC += 2;

}

//DAA - Ajuste decimal do reg. A para representação BCD após uma instrução aritmética
//Opcode - 0x27
void SimpleZ80::daa(uint8_t* opcode)
{

	//TODO: Melhorar, essa implementação está horrivel..

	//divide o registro em parte alta e baixa
	uint8_t al = this->reg_file.A() & 0x0F;
	uint8_t ah = (this->reg_file.A() & 0xF0) >> 4;

	//Guarda o valor antigo das flags C e H
	bool c_old = this->reg_file.getFlagBit(c);
	bool h_old = this->reg_file.getFlagBit(h);

	//limpa os flags
	this->reg_file.clrFlagBit(c);
	this->reg_file.clrFlagBit(h);
	this->reg_file.clrFlagBit(z);

	//Verifica o resultado da operação anterior
	if(this->reg_file.getFlagBit(n))
	{

		//Verifica a parte baixa
		if (h_old || (al > 9))
			this->reg_file.A() += 0x06;

		//Verifica a parte alta
		if(c_old || (ah > 9) || ((ah == 9) || (al > 9)))
		{
			this->reg_file.A() += 0x60;
			this->reg_file.setFlagBit(c);
		}
	}
	else
	{

		//Verifica a parte baixa
		if(h_old && (al > 5))
			this->reg_file.A() = ((this->reg_file.A() - 0x06) & 0xFF);

		//Verifica a parte alta
		if(c_old && (ah > 6 || (h_old && (ah > 5))))
		{
			this->reg_file.A() = ((this->reg_file.A() - 0x60) & 0xFF);
			this->reg_file.setFlagBit(c);

		}
	}

	if(this->reg_file.A() == 0)
		this->reg_file.setFlagBit(z);

	//Atualiza o contador de programa
	this->reg_file.PC += 1;

	//Atualiza o clock
	this->clk_elapsed = 4;

}

//JR Z,e8 - Salto condicional  relativo pela constante com sinal e8
//Opcode - 0x28
void SimpleZ80::jrze8(uint8_t* opcode)
{
	//Verifica o flag
	if(this->reg_file.getFlagBit(z))
	{
		//Calcula o endereço relativo
		int addr = (opcode[1] > 127) ? (opcode[1] - 256) : addr = opcode[1];

		//Executa o salto
		this->reg_file.PC += addr;

		//Atualiza o clock
		this->clk_elapsed = 12;


	}
	else
		//Não ocorreu o salto
		//Atualiza o clock
        this->clk_elapsed = 8;


    //Atualiza o contador de programa
    this->reg_file.PC += 2;
}

//CPL - Faz o complemento do acumulador A = ~A
//Opcode - 0x2F
void SimpleZ80::cpl(uint8_t* opcode)
{

	//Atualiza os flags
	this->reg_file.F() |= 0x60;

	//Complementa o registro A
	this->reg_file.A() = ~this->reg_file.A();

	//Atualiza o clock
	this->clk_elapsed = 4;

	//Atualiza o contador de programa
	this->reg_file.PC += 1;
}

//JR NC,e8 - Salto condicional  relativo pela constante com sinal e8
//Opcode - 0x30
void SimpleZ80::jrnce8(uint8_t* opcode)
{
	//Verifica o flag
	if(!this->reg_file.getFlagBit(c))
	{
		//Calcula o endereço relativo
		int addr = (opcode[1] > 127) ? (opcode[1] - 256) : addr = opcode[1];

		//Executa o salto
		this->reg_file.PC += addr;

		//Atualiza o clock
		this->clk_elapsed = 12;


	}
	else
		//Não ocorreu o salto
		//Atualiza o clock
        this->clk_elapsed = 8;


    //Atualiza o contador de programa
    this->reg_file.PC += 2;
}

//SCF - Coloca o bit carry em 1
//Opcode - 0x37
void SimpleZ80::scf(uint8_t* opcode)
{
	//Atualiza os flags
	this->reg_file.F() &= 0x80;

	//Seta o carry
	this->reg_file.setFlagBit(c);

	//Atualiza o clock
	this->clk_elapsed = 4;

	//Atualiza o contador de programa
	this->reg_file.PC += 1;


}
//JR C,e8 - Salto condicional relativo pela constante com sinal e8
//Opcode - 0x38
void SimpleZ80::jrce8(uint8_t* opcode)
{
	//Verifica o flag
	if(this->reg_file.getFlagBit(c))
	{
		//Calcula o endereço relativo
		int addr = (opcode[1] > 127) ? (opcode[1] - 256) : addr = opcode[1];

		//Executa o salto
		this->reg_file.PC += addr;

		//Atualiza o clock
		this->clk_elapsed = 12;


	}
	else
		//Não ocorreu o salto
		//Atualiza o clock
        this->clk_elapsed = 8;


    //Atualiza o contador de programa
    this->reg_file.PC += 2;
}

//CCF - Faz o complemento do carry CY = ~CY
//Opcode - 0x3F
void SimpleZ80::ccf(uint8_t* opcode)
{

	//TODO: Melhorar esse código

	//Variavel temporária
	bool c_old = this->reg_file.getFlagBit(c);

	//Atualiza os flags
	this->reg_file.F() &= 0x80;

	//Seta o carry
	if(c_old)
		this->reg_file.clrFlagBit(c);
	else
		this->reg_file.setFlagBit(c);

	//Atualiza o clock
	this->clk_elapsed = 4;

	//Atualiza o contador de programa
	this->reg_file.PC += 1;

}

//LD reg8,reg8
//Opcodes - 0x40 a 0x75 e 0x77 a 0x7F
void SimpleZ80::ldr8r8(uint8_t* opcode)
{
	//Registro de destino
	uint8_t dest = (opcode[0] & 0x38) >> 3;

	//Registro de origem
    uint8_t orig = opcode[0] & 0x07;

    //Caso o destino de origem seja 6 (HL)
    if(orig == 6)
    	//Carrega o valor de um registro no outro
    	(*this->reg_file.reg8[dest]) = (*mem).read(this->reg_file.HL());
    //Caso o destino de destino seja 6 (HL)
    else if (dest == 6)
    	//Carrega o valor de um registro no outro
    	(*mem).write(this->reg_file.HL(),(*this->reg_file.reg8[orig]));
    else
    	//Carrega o valor de um registro no outro
    	(*this->reg_file.reg8[dest]) = (*this->reg_file.reg8[orig]);


    //Atualiza o clock
    this->clk_elapsed = 4;

    //Atualiza o contador de programa
    this->reg_file.PC += 1;


}

//HALT - Pausa a CPU e espera por interrupção
//Opcode - 0x76
void SimpleZ80::halt(uint8_t* opcode)
{
	//TODO: Verificar o comportamento em bugs em relação ao estado da EMI
	this->cpu_state = HALT;

	//Atualiza o clock
	this->clk_elapsed = 4;

	//Atualiza o contador de programa
	this->reg_file.PC += 1;

}

//ADD reg8 - Soma o registro de 8 bit ao registro A
//Opcodes - 0x80 a 0x87
void SimpleZ80::addr8a(uint8_t* opcode)
{
	//Limpa os flags
	this->reg_file.F() = 0x00;

	//Registro de origem
	uint8_t reg_o = opcode[0] & 0x07;

	//Valor a ser somado com a
	uint8_t val = (reg_o == 6) ? (*mem).read(this->reg_file.HL()) : (*this->reg_file.reg8[reg_o]);

	//Verifica o half carry
	if((this->reg_file.A()&0x0F) + (val&0x0F) >= 0x10)
		this->reg_file.setFlagBit(h);

	//Verifica o carry
	if(((int)(reg_file.A() + val)) > 0xFF)
		this->reg_file.setFlagBit(c);

	//Efetua a Soma
	this->reg_file.A() += val;

	//Verifica o zero
	if(this->reg_file.A() == 0)
		this->reg_file.setFlagBit(z);

   //Atualiza o clock
    this->clk_elapsed = (reg_o == 6) ? 8 : 4;

    //Atualiza o contador de programa
    this->reg_file.PC += 1;

}

// ADC reg8 - Soma o registro de 8 bit ao registro A com carry
//Opcodes - 0x88 a 0x8F
void SimpleZ80::adcr8a(uint8_t* opcode)
{
	//Guarda o carry antigo
	uint8_t c_old = (uint8_t)this->reg_file.getFlagBit(c);

	//Limpa os flags
	this->reg_file.F() = 0x00;

	//Registro de origem
	uint8_t reg_o = opcode[0] & 0x07;

	//Valor a ser somado com a
	uint8_t val = (reg_o == 6) ? (*mem).read(this->reg_file.HL()) : (*this->reg_file.reg8[reg_o]);

	//Verifica o half carry
	if(((this->reg_file.A() & 0x0F)+ c_old + (val & 0x0F)) >= 0x10)
		this->reg_file.setFlagBit(h);

	//Verifica o carry
	if(((int)(reg_file.A() + val + c_old)) > 0xFF)
		this->reg_file.setFlagBit(c);

	//Efetua a Soma
	this->reg_file.A() += (val + c_old);

	//Verifica o zero
	if(this->reg_file.A() == 0)
		this->reg_file.setFlagBit(z);

   //Atualiza o clock
    this->clk_elapsed = (reg_o == 6) ? 8 : 4;

    //Atualiza o contador de programa
    this->reg_file.PC += 1;
}

//SUB reg8 - Subtrai o registro de 8 bit ao registro A
//Opcodes - 0x90 a 0x97
void SimpleZ80::subr8a(uint8_t* opcode)
{
	//Limpa os flags
	this->reg_file.F() = 0x40;

	//Registro de origem
	uint8_t reg_o = opcode[0] & 0x07;

	//Valor a ser somado com a
	uint8_t val = (reg_o == 6) ? (*mem).read(this->reg_file.HL()) : (*this->reg_file.reg8[reg_o]);

	//Verifica o half carry
	if((int)((this->reg_file.A()&0x0F) - (val&0x0F)) < 0x00)
		this->reg_file.setFlagBit(h);

	//Verifica o carry
	if(val > this->reg_file.A())
		this->reg_file.setFlagBit(c);

	//Efetua a Subtração
	this->reg_file.A() -= val;

	//Verifica o zero
	if(this->reg_file.A() == 0)
		this->reg_file.setFlagBit(z);

   //Atualiza o clock
    this->clk_elapsed = (reg_o == 6) ? 8 : 4;

    //Atualiza o contador de programa
    this->reg_file.PC += 1;
}

//SBC reg8 - Subtrai o registro de 8 bit ao registro A com carry
//Opcodes - 0x98 a 0x9F
void SimpleZ80::sbcr8a(uint8_t* opcode)
{
	//Guarda o resultado antigo do carry
	uint8_t c_old = (uint8_t)this->reg_file.getFlagBit(c);

	//Limpa os flags
	this->reg_file.F() = 0x40;

	//Registro de origem
	uint8_t reg_o = opcode[0] & 0x07;

	//Valor a ser somado com a
	uint8_t val = (reg_o == 6) ? (*mem).read(this->reg_file.HL()) : (*this->reg_file.reg8[reg_o]);

	//Verifica o half carry
	if(((val & 0x0F) + c_old)  >  (this->reg_file.A()&0x0F))
		this->reg_file.setFlagBit(h);

	//Verifica o carry
	if((val + c_old) > this->reg_file.A())
		this->reg_file.setFlagBit(c);

	//Efetua a Soma
	this->reg_file.A() -= val + c_old;

	//Verifica o zero
	if(this->reg_file.A() == 0)
		this->reg_file.setFlagBit(z);

   //Atualiza o clock
    this->clk_elapsed = (reg_o == 6) ? 8 : 4;

    //Atualiza o contador de programa
    this->reg_file.PC += 1;
}

//AND reg8 - Realiza a operação "E" bit a bit entre reg8 e A
//Opcodes - 0xA0 a 0xA7
void SimpleZ80::andr8a(uint8_t* opcode)
{
	//Limpa os flags
	this->reg_file.F() = 0x20;

	//Registro de origem
	uint8_t reg_o = opcode[0] & 0x07;

	//Valor a ser somado com a
	uint8_t val = (reg_o == 6) ? (*mem).read(this->reg_file.HL()) : (*this->reg_file.reg8[reg_o]);

	//Efetua a operação
	this->reg_file.A() &= val;

	//Verifica o zero
	if(this->reg_file.A() == 0)
		this->reg_file.setFlagBit(z);

   //Atualiza o clock
    this->clk_elapsed = (reg_o == 6) ? 8 : 4;

    //Atualiza o contador de programa
    this->reg_file.PC += 1;
}

//XOR reg8 - Realiza a operação "Ou exclusivo" bit a bit entre reg8 e A
//Opcodes - 0xA8 a 0xAF
void SimpleZ80::xorr8a(uint8_t* opcode)
{
	//Limpa os flags
	this->reg_file.F() = 0x00;

	//Registro de origem
	uint8_t reg_o = opcode[0] & 0x07;

	//Valor a ser somado com a
	uint8_t val = (reg_o == 6) ? (*mem).read(this->reg_file.HL()) : (*this->reg_file.reg8[reg_o]);

	//Efetua a operação
	this->reg_file.A() ^= val;

	//Verifica o zero
	if(this->reg_file.A() == 0)
		this->reg_file.setFlagBit(z);

   //Atualiza o clock
    this->clk_elapsed = (reg_o == 6) ? 8 : 4;

    //Atualiza o contador de programa
    this->reg_file.PC += 1;
}

//OR reg8 - Realiza a operação "Ou" bit a bit entre reg8 e A
//Opcodes - 0xB0 a 0xB7
void SimpleZ80::orr8a(uint8_t* opcode)
{
	//Limpa os flags
	this->reg_file.F() = 0x00;

	//Registro de origem
	uint8_t reg_o = opcode[0] & 0x07;

	//Valor a ser somado com a
	uint8_t val = (reg_o == 6) ? (*mem).read(this->reg_file.HL()) : (*this->reg_file.reg8[reg_o]);

	//Efetua a operação
	this->reg_file.A() |= val;

	//Verifica o zero
	if(this->reg_file.A() == 0)
		this->reg_file.setFlagBit(z);

   //Atualiza o clock
    this->clk_elapsed = (reg_o == 6) ? 8 : 4;

    //Atualiza o contador de programa
    this->reg_file.PC += 1;
}

//CP reg8 - Subtrai o reg 8 do A, ajusta os flags, mas não grava os resulatdos
//Opcodes - 0xB8 a 0xBF
void SimpleZ80::cprr8a(uint8_t* opcode)
{
	//Limpa os flags
	this->reg_file.F() = 0x40;

	//Registro de origem
	uint8_t reg_o = opcode[0] & 0x07;
	
	//Valor a ser somado com a
	uint8_t val = (reg_o == 6) ? (*mem).read(this->reg_file.HL()) : (*this->reg_file.reg8[reg_o]);

	//Verifica o half carry
	if((int)((this->reg_file.A()&0x0F) - (val&0x0F)) < 0x00)
		this->reg_file.setFlagBit(h);

	//Verifica o carry
	if(val > this->reg_file.A())
		this->reg_file.setFlagBit(c);

	//Efetua a Subtração
	 uint8_t temp = this->reg_file.A() - val;

	//Verifica o zero
	if(temp == 0)
		this->reg_file.setFlagBit(z);

   //Atualiza o clock
    this->clk_elapsed = (reg_o == 6) ? 8 : 4;

    //Atualiza o contador de programa
    this->reg_file.PC += 1;

}

//RET NZ - Retorno de chamada de sub-rotina condicional
//Opcode - 0xC0
void SimpleZ80::retnz(uint8_t* opcode)
{
	if(!this->reg_file.getFlagBit(z))
	{
		//Endereço do topo da pilha
		uint16_t addr = this->reg_file.SP();

		//Pop para SP
		this->reg_file.PC = (((*mem).read(addr+1) << 8) + (*mem).read(addr));

		//Incrementa a pilha
		this->reg_file.SP() += 2;

		//Cliclos de clock
		this->clk_elapsed = 20;

	}
	else
	{
		//Cliclos de clock
		this->clk_elapsed = 16;

		//Atualiza o contador de programa
		this->reg_file.PC += 1;
	}


}

//POP reg16 - Retira o valor da pilha e grava no registro de 16 bits
//Opcodes - 0xC1, 0xD1, 0xE1, 0xF1
void SimpleZ80::popr16(uint8_t* opcode)
{
	//Registro de destino
	uint8_t dest = ((opcode[0] & 0x30) >> 4);

	//Obs: No caso de push e pop o destino é AF, no lugar de SP
	if(dest == 3) 
	{
		//Destino  = AF
		
		//Carrega o valor da pilha no registro
		this->reg_file.A() = (*mem).read(this->reg_file.SP() + 1);
		this->reg_file.F() = (*mem).read(this->reg_file.SP()) & 0xF0;
	
	}
	else
	{
		//Carrega o valor da pilha no registro
		this->reg_file.reg16[dest] = (((*mem).read(this->reg_file.SP()+1) << 0x08) + (*mem).read(this->reg_file.SP()));
	}
	

	//Atualiza a pilha
	this->reg_file.SP() += 2;

	//Cliclos de clock
	this->clk_elapsed = 12;

	//Atualiza o contador de programa
	this->reg_file.PC += 1;


}

//JP NZ,d16 - Salto condicional para o end. absoluto dado pela constante
//Opcode - 0xC2
void SimpleZ80::jpnzr16d(uint8_t* opcode)
{
	//Verifica o flags
	if(!this->reg_file.getFlagBit(z))
	{
		//Carrega a constante no contador de programa
		this->reg_file.PC = ((opcode[2] << 8)+ opcode[1]);

		//Cliclos de clock
		this->clk_elapsed = 16;

	}
	else
	{
		//Cliclos de clock
		this->clk_elapsed = 12;

		//Atualiza o contador de programa
		this->reg_file.PC += 3;

	}

}

//JP d16 - Salto incondicional para o end. absoluto dado pela constante
//Opcode - 0xC3
void SimpleZ80::jp16d(uint8_t* opcode)
{
	//Carrega a constante no contador de programa
	this->reg_file.PC = ((opcode[2] << 8) + opcode[1]);

	//Cliclos de clock
	this->clk_elapsed = 16;
}

//CALL NZ,d16 - Chamada de subrotina condicional
//Opcode - 0xC4
void SimpleZ80::callnzd(uint8_t* opcode)
{
	//Atualiza o contador de programa
	this->reg_file.PC += 3;

	if(!this->reg_file.getFlagBit(z))
	{
		//Calcula o endereço de reset
		uint16_t addr = ((opcode[2] << 8) + opcode[1]);

		//Coloca o valor da próxima instrução na pilha
		(*mem).write(--this->reg_file.SP(),((this->reg_file.PC & 0xFF00)>>8));
		(*mem).write(--this->reg_file.SP(),(this->reg_file.PC & 0x00FF));

		//Executa o salto
		this->reg_file.PC = addr;

		//Cliclos de clock
		this->clk_elapsed = 20;
	}
	else
	{
		//Cliclos de clock
		this->clk_elapsed = 12;

	}
}

//PUSH reg16 - Coloca o valor do registro de 16 bits na pilha
//Opcodes - 0xC5, 0xD5, 0xE5, 0xF5
void SimpleZ80::pushr16(uint8_t* opcode)
{
	//Registro de origem
	uint8_t orig = (opcode[0] & 0x30) >> 4;

	//Obs: No caso de push e pop o destino é AF, no lugar de SP
	if(orig == 3) orig++;

	//Carrega o valor do registro na pilha
	(*mem).write(--this->reg_file.SP(),((this->reg_file.reg16[orig] & 0xFF00)>>8));
	(*mem).write(--this->reg_file.SP(),(this->reg_file.reg16[orig] & 0x00FF));

	//Cliclos de clock
	this->clk_elapsed = 16;

	//Atualiza o contador de programa
	this->reg_file.PC += 1;
}

//ADD A,d8 - Soma a constante ao registro A
//Opcode - 0xC6
void SimpleZ80::addd8a(uint8_t* opcode)
{
	//Limpa os flags
	this->reg_file.F() = 0x00;

	//Verifica o half carry
	if((this->reg_file.A()&0x0F) + (opcode[1]&0x0F) >= 0x10)
		this->reg_file.setFlagBit(h);

	//Verifica o carry
	if(((int)(reg_file.A() + opcode[1])) > 0xFF)
		this->reg_file.setFlagBit(c);

	//Efetua a Soma
	this->reg_file.A() += opcode[1];

	//Verifica o zero
	if(this->reg_file.A() == 0)
		this->reg_file.setFlagBit(z);

   //Atualiza o clock
    this->clk_elapsed =  8;

    //Atualiza o contador de programa
    this->reg_file.PC += 2;

}

//RST y*8h - Reinicia a CPU no end. y * 0x08
//Opcode - 0xC7, 0xCF, 0xD7, 0xDF, 0xE7, 0xEF, 0xF7, 0xFF
void SimpleZ80::rst(uint8_t* opcode)
{
	//Atualiza o contador de programa
	this->reg_file.PC += 1;

	//Calcula o endereço de reset
	uint8_t addr = (opcode[0] & 0x38);

	//Coloca o valor da próxima instrução na pilha
	(*mem).write(--this->reg_file.SP(),((this->reg_file.PC & 0xFF00)>>8));
	(*mem).write(--this->reg_file.SP(),(this->reg_file.PC & 0x00FF));

	//Executa o reset
	this->reg_file.PC = addr;

	//Cliclos de clock
	this->clk_elapsed = 16;

}

//RET Z - Retorno de chamada de sub-rotina condicional
//Opcodes - 0xC8
void SimpleZ80::retz(uint8_t* opcode)
{
	if(this->reg_file.getFlagBit(z))
	{
		//Endereço do topo da pilha
		uint16_t addr = this->reg_file.SP();

		//Pop para SP
		this->reg_file.PC = (((*mem).read(addr+1) << 8) + (*mem).read(addr));

		//Cliclos de clock
		this->clk_elapsed = 20;

		//Increnta a pilha
		this->reg_file.SP() += 2;

	}
	else
	{
		//Cliclos de clock
		this->clk_elapsed = 16;

		//Atualiza o contador de programa
		this->reg_file.PC += 1;
	}
}

//RET - Retorno de chamada de sub-rotina incondicional
//Opcodes - 0xC9
void SimpleZ80::ret(uint8_t* opcode)
{
	//Endereço do topo da pilha
	uint16_t addr = this->reg_file.SP();

	//Pop para SP
	this->reg_file.PC = (((*mem).read(addr+1) << 8) + (*mem).read(addr));

	//Cliclos de clock
	this->clk_elapsed = 16;

	//Increnta a pilha
	this->reg_file.SP() += 2;


}

//JP Z,d16 - Salto condicional para o end. absoluto dado pela constante
//Opcodes - 0xCA
void SimpleZ80::jpz16d(uint8_t* opcode)
{
	//Verifica o flags
	if(this->reg_file.getFlagBit(z))
	{
		//Carrega a constante no contador de programa
		this->reg_file.PC = ((opcode[2] << 8)+ opcode[1]);

		//Cliclos de clock
		this->clk_elapsed = 16;

	}
	else
	{
		//Cliclos de clock
		this->clk_elapsed = 12;

		//Atualiza o contador de programa
		this->reg_file.PC += 3;

	}
}

//PREFIX CB - Conjunto de intruções do prefixo CB
//Opcodes - 0xCB
void SimpleZ80::prefixcb(uint8_t* opcode)
{
	//Decodifica instrução com prefixo 0xCB
	(this->*cb_array[opcode[1] >> 3])(opcode);

}

//CALL Z,d16 - Chamada de subrotina condicional
//Opcode - 0xCC
void SimpleZ80::callzd(uint8_t* opcode)
{
	//Atualiza o contador de programa
	this->reg_file.PC += 3;

	if(this->reg_file.getFlagBit(z))
	{
		//Calcula o endereço de reset
		uint8_t addr = ((opcode[2] << 8) + opcode[1]);

		//Coloca o valor da próxima instrução na pilha
		(*mem).write(--this->reg_file.SP(),((this->reg_file.PC & 0xFF00)>>8));
		(*mem).write(--this->reg_file.SP(),(this->reg_file.PC & 0x00FF));

		//Executa o salto
		this->reg_file.PC = addr;

		//Cliclos de clock
		this->clk_elapsed = 20;
	}
	else
	{
		//Cliclos de clock
		this->clk_elapsed = 12;

	}
}

//CALL d16 - Chamada de subrotina incondicional
//Opcode - 0xCD
void SimpleZ80::calld(uint8_t* opcode)
{
	//Atualiza o contador de programa
	this->reg_file.PC += 3;

	//Calcula o endereço de reset
	uint16_t addr = ((opcode[2] << 8) + opcode[1]);

	//Coloca o valor da próxima instrução na pilha
	(*mem).write(--this->reg_file.SP(),((this->reg_file.PC & 0xFF00)>>8));
	(*mem).write(--this->reg_file.SP(),(this->reg_file.PC & 0x00FF));

	//Executa o salto
	this->reg_file.PC = addr;

	//Cliclos de clock
	this->clk_elapsed = 20;


}

//ADC A,d8
//Opcode - 0xCE - Soma a constante ao registro A com o carry
void SimpleZ80::adcd8a(uint8_t* opcode)
{
	//Guarda o valor do carry antigo
	uint8_t c_old = (uint8_t)this->reg_file.getFlagBit(c);

	//Limpa os flags
	this->reg_file.F() = 0x00;

	//Verifica o half carry
	if(((this->reg_file.A() & 0x0F) + (opcode[1] & 0x0F) + c_old)  >= 0x10)
		this->reg_file.setFlagBit(h);

	//Verifica o carry
	if((int)(this->reg_file.A() + opcode[1] + c_old) > 0xFF)
		this->reg_file.setFlagBit(c);

	//Efetua a Soma
	this->reg_file.A() += (opcode[1] + c_old);

	//Verifica o zero
	if(this->reg_file.A() == 0)
		this->reg_file.setFlagBit(z);

   //Atualiza o clock
    this->clk_elapsed =  8;

    //Atualiza o contador de programa
    this->reg_file.PC += 2;
}

//RET NC - Retorno de chamada de sub-rotina condicional
//Opcodes - 0xD0
void SimpleZ80::retnc(uint8_t* opcode)
{
	if(!this->reg_file.getFlagBit(c))
	{
		//Endereço do topo da pilha
		uint16_t addr = this->reg_file.SP();

		//Pop para SP
		this->reg_file.PC = (((*mem).read(addr+1) << 8) + (*mem).read(addr));

		//Cliclos de clock
		this->clk_elapsed = 20;

		//Increnta a pilha
		this->reg_file.SP() += 2;

	}
	else
	{
		//Cliclos de clock
		this->clk_elapsed = 16;

		//Atualiza o contador de programa
		this->reg_file.PC += 1;
	}
}

//JP NC,d16 - Salto condicional para o end. absoluto dado pela constante
//Opcodes - 0xD2
void SimpleZ80::jpnc16d(uint8_t* opcode)
{
	//Verifica o flags
	if(!this->reg_file.getFlagBit(c))
	{
		//Carrega a constante no contador de programa
		this->reg_file.PC = ((opcode[2] << 8)+ opcode[1]);

		//Cliclos de clock
		this->clk_elapsed = 16;

	}
	else
	{
		//Cliclos de clock
		this->clk_elapsed = 12;

		//Atualiza o contador de programa
		this->reg_file.PC += 3;

	}
}

//CALL NC,d16 - Chamada de subrotina condicional
//Opcode - 0xD4
void SimpleZ80::callncd(uint8_t* opcode)
{
	//Atualiza o contador de programa
	this->reg_file.PC += 3;

	if(!this->reg_file.getFlagBit(c))
	{
		//Calcula o endereço de reset
		uint8_t addr = ((opcode[2] << 8) + opcode[1]);

		//Coloca o valor da próxima instrução na pilha
		(*mem).write(--this->reg_file.SP(),((this->reg_file.PC & 0xFF00)>>8));
		(*mem).write(--this->reg_file.SP(),(this->reg_file.PC & 0x00FF));

		//Executa o salto
		this->reg_file.PC = addr;

		//Cliclos de clock
		this->clk_elapsed = 20;
	}
	else
	{
		//Cliclos de clock
		this->clk_elapsed = 12;

	}
}

//SUB d8
//Opcode - 0xD6  - Subtrai a constante ao registro A
void SimpleZ80::subd8a(uint8_t* opcode)
{
	//Limpa os flags
	this->reg_file.F() = 0x40;

	//Registro de origem
	uint8_t reg_o = opcode[0] & 0x07;

	//Verifica o half carry
	if((int)((this->reg_file.A()&0x0F) - (opcode[1]&0x0F)) < 0x00)
		this->reg_file.setFlagBit(h);

	//Verifica o carry
	if(opcode[1] > this->reg_file.A())
		this->reg_file.setFlagBit(c);

	//Efetua a Subtração
	this->reg_file.A() -= opcode[1];

	//Verifica o zero
	if(this->reg_file.A() == 0)
		this->reg_file.setFlagBit(z);

   //Atualiza o clock
    this->clk_elapsed = 8;

    //Atualiza o contador de programa
    this->reg_file.PC += 2;
}

//RET C - Retorno de chamada de sub-rotina condicional
//Opcodes - 0xD8
void SimpleZ80::retc(uint8_t* opcode)
{
	if(this->reg_file.getFlagBit(c))
	{
		//Endereço do topo da pilha
		uint16_t addr = this->reg_file.SP();

		//Pop para SP
		this->reg_file.PC = (((*mem).read(addr+1) << 8) + (*mem).read(addr));

		//Cliclos de clock
		this->clk_elapsed = 10;

		//Increnta a pilha
		this->reg_file.SP() += 2;

	}
	else
	{
		//Cliclos de clock
		this->clk_elapsed = 8;

		//Atualiza o contador de programa
		this->reg_file.PC += 1;
	}
}

//RETI - Retorno de interrupção incondicional
//Opcodes - 0xD9
void SimpleZ80::reti(uint8_t* opcode)
{
	//Endereço do topo da pilha
	uint16_t addr = this->reg_file.SP();

	//Pop para SP
	this->reg_file.PC = (((*mem).read(addr+1) << 8) + (*mem).read(addr));

	//Liga o IME
	this->IME = 1;

	//Cliclos de clock
	this->clk_elapsed = 8;

	//Increnta a pilha
	this->reg_file.SP() += 2;
}

//JP C,d16 - Salto condicional para o end. absoluto dado pela constante
//Opcodes - 0xDA
void SimpleZ80::jpc16d(uint8_t* opcode)
{
	//Verifica o flags
	if(this->reg_file.getFlagBit(c))
	{
		//Carrega a constante no contador de programa
		this->reg_file.PC = ((opcode[2] << 8)+ opcode[1]);

		//Cliclos de clock
		this->clk_elapsed = 16;

	}
	else
	{
		//Cliclos de clock
		this->clk_elapsed = 12;

		//Atualiza o contador de programa
		this->reg_file.PC += 3;

	}
}

//CALL C,d16 - Chamada de subrotina condicional
//Opcode - 0xDC
void SimpleZ80::callcd(uint8_t* opcode)
{
	//Atualiza o contador de programa
	this->reg_file.PC += 3;

	if(this->reg_file.getFlagBit(c))
	{
		//Calcula o endereço de reset
		uint8_t addr = ((opcode[2] << 8) + opcode[1]);

		//Coloca o valor da próxima instrução na pilha
		(*mem).write(--this->reg_file.SP(),((this->reg_file.PC & 0xFF00)>>8));
		(*mem).write(--this->reg_file.SP(),(this->reg_file.PC & 0x00FF));

		//Executa o salto
		this->reg_file.PC = addr;

		//Cliclos de clock
		this->clk_elapsed = 20;
	}
	else
	{
		//Cliclos de clock
		this->clk_elapsed = 12;

	}
}

//SBC A,d8 - Subtrai a constante ao registro A com o carry
//Opcode - 0xDE
void SimpleZ80::sbcd8a(uint8_t* opcode)
{
	//Guarda o resultado antigo do carry
	uint8_t c_old = (uint8_t)this->reg_file.getFlagBit(c);

	//Limpa os flags
	this->reg_file.F() = 0x40;

	//Registro de origem
	uint8_t reg_o = opcode[0] & 0x07;

	//Verifica o half borrow
	if(((opcode[1] & 0x0F) + c_old ) > (this->reg_file.A() & 0x0F))
		this->reg_file.setFlagBit(h);

	//Verifica o borrow
	if((opcode[1] + c_old) > this->reg_file.A())
		this->reg_file.setFlagBit(c);

	//Efetua a Soma
	this->reg_file.A() -= opcode[1] + (int)c_old;

	//Verifica o zero
	if(this->reg_file.A() == 0)
		this->reg_file.setFlagBit(z);

   //Atualiza o clock
    this->clk_elapsed = 8;

    //Atualiza o contador de programa
    this->reg_file.PC += 2;
}

///LDH (d8),A - Carrega o dado de A na região de 0xFF00 a 0xFFFF dado pela constante
//Opcode - 0xE0
void SimpleZ80::ldhd8a(uint8_t* opcode)
{
	//Carrega o registro A no endereço alto da memória
	(*mem).write((0xFF00 + opcode[1]),this->reg_file.A());

	//Atualiza o clock
	this->clk_elapsed = 12;

	//Atualiza o contador de programa
	this->reg_file.PC += 2;
}

//LD (C),A - Carrega o dado de A na região de 0xFF00 a 0xFFFF dado por C
//Opcode - 0xE2
void SimpleZ80::ldpca(uint8_t* opcode)
{
	//Carrega o registro A no endereço alto da memória
	(*mem).write((0xFF00 + this->reg_file.C()),this->reg_file.A());

	//Atualiza o clock
	this->clk_elapsed = 12;

	//Atualiza o contador de programa
	this->reg_file.PC += 1;
}

//AND d8 - Operação "E" bit a bit entre constante e registro A
//Opcode - 0xE6
void SimpleZ80::andd8a(uint8_t* opcode)
{
	//Limpa os flags
	this->reg_file.F() = 0x20;

	//Efetua a operação
	this->reg_file.A() &= opcode[1];

	//Verifica o zero
	if(this->reg_file.A() == 0)
		this->reg_file.setFlagBit(z);

   //Atualiza o clock
    this->clk_elapsed = 8;

    //Atualiza o contador de programa
    this->reg_file.PC += 2;
}

//ADD SP,d8 - Soma  a constante ao registro SP
//Opcode - 0xE8
void SimpleZ80::addspd8(uint8_t* opcode)
{
	//Limpa os Flgas
	this->reg_file.F() &= 0x00;

	//Verifica o half carry
	if((this->reg_file.SP() & 0x0F ) + (opcode[1] & 0x0F) >= 0x10)
		this->reg_file.setFlagBit(h);

	//verifica o carry
	if(((this->reg_file.SP()&0xFF) + opcode[1]) > 0xFF)
		this->reg_file.setFlagBit(c);
	
	//Soma a constante no registro SP
	this->reg_file.SP() += (int8_t)opcode[1];

	//Atualiza o clock
	this->clk_elapsed = 16;

	//Atualiza o contador de programa
	this->reg_file.PC += 2;

}

//JP (HL) - Salto incondicional para o endereço apontado por HL
//Opcode - 0xE9
void SimpleZ80::jpphl(uint8_t* opcode)
{

	//Carrega o valor de HL no PC
	this->reg_file.PC = this->reg_file.HL();

	//Atualiza o clock
	this->clk_elapsed = 16;

}

//LD (d16),A - Carrega o dado de A no end. apontado pela constante
//Opcode - 0xEA
void SimpleZ80::ldpd16a(uint8_t* opcode)
{

	//Carrega o valor de A no end. de memória
	(*mem).write(((opcode[2]<<8)+opcode[1]),this->reg_file.A());

	//Atualiza o clock
	this->clk_elapsed = 16;

	//Atualiza o contador de programa
	this->reg_file.PC += 3;
}

//XOR d8 - Operação "Ou exclusivo" bit a bit entre constante e registro A
//Opcode - 0xEE
void SimpleZ80::xord8a(uint8_t* opcode)
{
	//Limpa os flags
	this->reg_file.F() = 0x00;

	//Efetua a operação
	this->reg_file.A() ^= opcode[1];

	//Verifica o zero
	if(this->reg_file.A() == 0)
		this->reg_file.setFlagBit(z);

   //Atualiza o clock
    this->clk_elapsed = 8;

    //Atualiza o contador de programa
    this->reg_file.PC += 2;
}

//LDH A,(d8) - Carrega o dado região de 0xFF00 a 0xFFFF dado pela constante em A
//Opcode - 0xF0
void SimpleZ80::ldhad8(uint8_t* opcode)
{
	//Carrega o registro A no endereço alto da memória
	this->reg_file.A() = (*mem).read(0xFF00 + opcode[1]);

	//Atualiza o clock
	this->clk_elapsed = 12;

	//Atualiza o contador de programa
	this->reg_file.PC += 2;
}

//LD A,(C) Carrega o dado da região de 0xFF00 a 0xFFFF dado por C em A
//Opcode - 0xF2
void SimpleZ80::ldapc(uint8_t* opcode)
{
	//Carrega o registro A no endereço alto da memória
	this->reg_file.A() = (*mem).read(0xFF00 + this->reg_file.C());

	//Atualiza o clock
	this->clk_elapsed = 12;

	//Atualiza o contador de programa
	this->reg_file.PC += 1;
}

//DI - Desabilita as interrupções
//Opcode - 0xF3
void SimpleZ80::di(uint8_t* opcode)
{
	//Desliga o IME
	this->IME = 0;

	//Atualiza o clock
	this->clk_elapsed = 4;

	//Atualiza o contador de programa
	this->reg_file.PC += 1;
}

//OR d8 - Operação "OU" bit a bit entre constante e registro A
//Opcode - 0xF6
void SimpleZ80::ord8a(uint8_t* opcode)
{
	//Limpa os flags
	this->reg_file.F() = 0x00;

	//Efetua a operação
	this->reg_file.A() |= opcode[1];

	//Verifica o zero
	if(this->reg_file.A() == 0)
		this->reg_file.setFlagBit(z);

   //Atualiza o clock
    this->clk_elapsed = 8;

    //Atualiza o contador de programa
    this->reg_file.PC += 2;

}

//LD HL,SP+r8 - Carrega o valor de SP + constante com sinal em HL
//Opcode - 0xF8
void SimpleZ80::ldhlspd8(uint8_t* opcode)
{

	//Limpa os Flgas
	this->reg_file.F() &= 0x00;

	//Verifica o half carry
	if((this->reg_file.SP() & 0x0F ) + (opcode[1] & 0x0F) >= 0x10)
		this->reg_file.setFlagBit(h);

	//verifica o carry
	if(((this->reg_file.SP()&0xFF) + opcode[1]) > 0xFF)
		this->reg_file.setFlagBit(c);

	//Soma o valor de SP e guarda em HL
	this->reg_file.HL() = (uint16_t)(this->reg_file.SP() + (int8_t)opcode[1]);

	//Atualiza o clock
	this->clk_elapsed = 12;

	//Atualiza o contador de program
	this->reg_file.PC += 2;
}

//LD SP,HL - Carrega o valor de HL em SP
//Opcode - 0xF9
void SimpleZ80::ldsphl(uint8_t* opcode)
{
	//Carrega o valor de HL em SP
	this->reg_file.SP() = this->reg_file.HL();

	//Atualiza o clock
	this->clk_elapsed = 8;

	//Atualiza o contador de programa
	this->reg_file.PC += 1;
}

//LD A,(a16) - Carrega o valor apontado pela constante em A
//Opcode - 0xFA
void SimpleZ80::ldapd16(uint8_t* opcode)
{
	//Carrega o valor de A no end. de memória
	this->reg_file.A() = (*mem).read((opcode[2]<<8)+opcode[1]);

	//Atualiza o clock
	this->clk_elapsed = 16;

	//Atualiza o contador de programa
	this->reg_file.PC += 3;
}

//EI - Habilita as interrupções
//Opcode - 0xFB
void SimpleZ80::ei(uint8_t* opcode)
{
	//Liga o IME
	this->IME = 1;

	//Atualiza o clock
	this->clk_elapsed = 4;

	//Atualiza o contador de programa
	this->reg_file.PC += 1;
}

//CP d8 - Subtrai a constante ao registro A, mas não grava o resultado
//Opcode -0xFE
void SimpleZ80::cpdd8a(uint8_t* opcode)
{
	//Limpa os flags
	this->reg_file.F() = 0x40;

	//Verifica o half carry
	if((int)((this->reg_file.A()&0x0F) - (opcode[1]&0x0F)) < 0x00)
		this->reg_file.setFlagBit(h);

	//Verifica o carry
	if(opcode[1] > this->reg_file.A())
		this->reg_file.setFlagBit(c);

	//Efetua a Soma
	 uint8_t temp = this->reg_file.A() - opcode[1];

	//Verifica o zero
	if(temp == 0)
		this->reg_file.setFlagBit(z);

   //Atualiza o clock
    this->clk_elapsed = 8;

    //Atualiza o contador de programa
    this->reg_file.PC += 2;
}

//Instruções com prefixo CB

//RLC reg8 - Rotaciona o registro de 8 bit para esquerda
//Opcodes - 0x00 a 0x07
void SimpleZ80::rlcr8(uint8_t* opcode)
{
	//Limpa o Flags
	this->reg_file.F() = 0;

	//Registro de destino
	uint8_t dest = opcode[1] & 0x07;

	//Verifica se é a memória ou registro
	if(dest == 6)
	{
		//Verifica o bit de overflow
		if ((*mem).read(this->reg_file.HL()) & 0x80)
			this->reg_file.setFlagBit(c);

		//Executa RLC
		(*mem).write(this->reg_file.HL(),((*mem).read(this->reg_file.HL()) << 1));

		//Copia o carry para bit 0
		(*mem).write(this->reg_file.HL(),(this->reg_file.getFlagBit(c) | (*mem).read(this->reg_file.HL())));

		//Verifica se o valor é zero
		if((*mem).read(this->reg_file.HL()) == 0)
			this->reg_file.setFlagBit(z);

		//Atualiza o clock
		this->clk_elapsed = 16;

	}else
	{
		//Ponteiro para qual registro rotacionar
		uint8_t *p_reg = (this->reg_file.reg8[dest]);

		//Verifica o bit de overflow
		if ((*p_reg) & 0x80)
			this->reg_file.setFlagBit(c);

		//Executa RLC
		(*p_reg)  = ((*p_reg) << 1);

		//Copia o carry para bit 0
		(*p_reg)  |= this->reg_file.getFlagBit(c);

		//Verifica se o valor é zero
		if((*p_reg) == 0)
			this->reg_file.setFlagBit(z);



		//Atualiza o clock
		this->clk_elapsed = 8;
		
	}
	
	//Atualiza o contador de programa
	this->reg_file.PC += 2;


}

//RRC reg8 - Rotaciona o registro de 8 bit para direita
//Opcodes - 0x08 a 0x0F
void SimpleZ80::rrcr8(uint8_t* opcode)
{
	//Limpa o Flags
	this->reg_file.F() = 0;

	//Registro de destino
	uint8_t dest = opcode[1] & 0x07;

	if(dest == 6)
	{

		//Verifica o bit de overflow
		if ((*mem).read(this->reg_file.HL()) & 0x01)
			this->reg_file.setFlagBit(c);

		//Executa RRC
		(*mem).write(this->reg_file.HL(),((*mem).read(this->reg_file.HL())>> 1));

		//Copia o carry para bit 0
		(*mem).write(this->reg_file.HL(),(((uint8_t)this->reg_file.getFlagBit(c)) << 7) | (*mem).read(this->reg_file.HL()));

		//Verifica se o valor é zero
		if((*mem).read(this->reg_file.HL()) == 0)
			this->reg_file.setFlagBit(z);

		//Atualiza o clock
		this->clk_elapsed = 16;	
	}
	else
	{
		//Ponteiro para qual registro rotacionar
		uint8_t *p_reg = (this->reg_file.reg8[dest]);

		//Verifica o bit de overflow
		if ((*p_reg) & 0x01)
			this->reg_file.setFlagBit(c);

		//Executa RRC
		(*p_reg)  = ((*p_reg) >> 1);

		//Copia o carry para bit 0
		(*p_reg)  |= ((uint8_t)this->reg_file.getFlagBit(c)) << 7;

		//Verifica se o valor é zero
		if((*p_reg) == 0)
			this->reg_file.setFlagBit(z);

		//Atualiza o clock
		this->clk_elapsed = 8;	
	}
	
	//Atualiza o contador de programa
	this->reg_file.PC += 2;

}

//RL reg8 - Rotaciona o registro de 8 bit para esquerda com carry
//Opcodes - 0x10 a 0x17
void SimpleZ80::rlr8(uint8_t* opcode)
{
	//Variável temporária para guardar o carry
	uint8_t c_old = (uint8_t)this->reg_file.getFlagBit(c);

	//Limpa o Flags
	this->reg_file.F() = 0;

	//Registro de destino
	uint8_t dest = opcode[1] & 0x07;

	if(dest == 6)
	{
		//Verifica o primeiro bit para overflow
		if ((*mem).read(this->reg_file.HL()) & 0x80)
			//Seta o flag
			this->reg_file.setFlagBit(c);

		//Executa RL
		(*mem).write(this->reg_file.HL(),((*mem).read(this->reg_file.HL()) << 1));

		//Copia o carry para bit 0
		(*mem).write(this->reg_file.HL(),(c_old | ((*mem).read(this->reg_file.HL()))));

		//Verifica o resultado é zero
		if((*mem).read(this->reg_file.HL()) == 0)
			this->reg_file.setFlagBit(z);

		//Atualiza o clock
		this->clk_elapsed = 16;

	}
	else
	{
		//Ponteiro para qual registro rotacionar
		uint8_t *p_reg = (this->reg_file.reg8[dest]);

		//Verifica o primeiro bit para overflow
		if ((*p_reg) & 0x80)
			//Seta o flag
			this->reg_file.setFlagBit(c);

		//Executa RL
		(*p_reg) = ((*p_reg) << 1);

		//Copia o carry para bit 0
		(*p_reg) |= c_old;

		//Verifica o resultado é zero
		if((*p_reg) == 0)
			this->reg_file.setFlagBit(z);

		//Atualiza o clock
		this->clk_elapsed = 8;
	}

	//Atualiza o contador de programa
	this->reg_file.PC += 2;

}

//RR reg8 - Rotaciona o registro de 8 bit para direita com carry
//Opcodes - 0x18 a 0x1F
void SimpleZ80::rrr8(uint8_t* opcode)
{
	//Variável temporária para guardar o carry
	bool c_old = (this->reg_file.getFlagBit(c));

	//Limpa o Flags
	this->reg_file.F() = 0;

	//Registro de destino
	uint8_t dest = opcode[1] & 0x07;

	if(dest == 6)
	{

		//Verifica o primeiro bit para overflow
		if ((*mem).read(this->reg_file.HL()) & 0x01)
			//Seta o flag
			this->reg_file.setFlagBit(c);

		//Executa RR
		(*mem).write(this->reg_file.HL(),((*mem).read(this->reg_file.HL()) >> 1));

		//Copia o carry para bit 7
		(*mem).write(this->reg_file.HL(),((c_old << 7) | (*mem).read(this->reg_file.HL())));

		//Verifica o resultado é zero
		if((*mem).read(this->reg_file.HL()) == 0)
			this->reg_file.setFlagBit(z);


		//Atualiza o clock
		this->clk_elapsed = 16;

	}
	else
	{
		//Ponteiro para qual registro rotacionar
		uint8_t *p_reg = (this->reg_file.reg8[dest]);

		//Verifica o primeiro bit para overflow
		if ((*p_reg) & 0x01)
			//Seta o flag
			this->reg_file.setFlagBit(c);

		//Executa RR
		(*p_reg) = ((*p_reg) >> 1);

		//Copia o carry para bit 7
		(*p_reg) |= (c_old << 7);

		//Verifica o resultado é zero
		if((*p_reg) == 0)
			this->reg_file.setFlagBit(z);


		//Atualiza o clock
		this->clk_elapsed = 8;	
	}

	//Atualiza o contador de programa
	this->reg_file.PC += 2;

}

//SLA reg8 - Deslocamento aritimético dos bits do registro de 8 bit para esquerda
//Opcodes - 0x20 a 0x27
void SimpleZ80::slar8(uint8_t* opcode)
{

	//Limpa o Flags
	this->reg_file.F() = 0;

	//Registro de destino
	uint8_t dest = opcode[1] & 0x07;

	if(dest == 6)
	{

		//Coloca o bit 7 no carry
		if((*mem).read(this->reg_file.HL()) & 0x80)
			this->reg_file.setFlagBit(c);

		//Executa SLA
		(*mem).write(this->reg_file.HL(),((*mem).read(this->reg_file.HL()) << 1));

		//Verifica o zero
		if ((*mem).read(this->reg_file.HL()) == 0x00)
			//Seta o flag
			this->reg_file.setFlagBit(z);

		//Atualiza o clock
		this->clk_elapsed = 16;	
	}
	else
	{
		//Ponteiro para qual registro rotacionar
		uint8_t *p_reg = (this->reg_file.reg8[dest]);

		//Coloca o bit 7 no carry
		if((*p_reg) & 0x80)
			this->reg_file.setFlagBit(c);

		//Executa SLA
		(*p_reg) = ((*p_reg) << 1);

		//Verifica o zero
		if ((*p_reg) == 0x00)
			//Seta o flag
			this->reg_file.setFlagBit(z);

		//Atualiza o clock
		this->clk_elapsed = 8;		
	}

	//Atualiza o contador de programa
	this->reg_file.PC += 2;

}

//SRA reg8 - Deslocamento aritimético dos bits do registro de 8 bit para direira
//Opcodes - 0x28 a 0x2F
void SimpleZ80::srar8(uint8_t* opcode)
{
	//Limpa o Flags
	this->reg_file.F() = 0;

	//Registro de destino
	uint8_t dest = opcode[1] & 0x07;

	if(dest == 6)
	{

		//Grava o estado do bit 7
		uint8_t b7 = ((*mem).read(this->reg_file.HL()) & 0x80);

		//Coloca o bit 0 no carry
		if((*mem).read(this->reg_file.HL()) & 0x01)
			this->reg_file.setFlagBit(c);

		//Executa SRA
		(*mem).write(this->reg_file.HL(),((*mem).read(this->reg_file.HL()) >> 1));

		//Restaura o valor do bit 7
		(*mem).write(this->reg_file.HL(),(b7 | (*mem).read(this->reg_file.HL())));

		//Verifica o zero
		if ((*mem).read(this->reg_file.HL()) == 0x00)
			//Seta o flag
			this->reg_file.setFlagBit(z);

		//Atualiza o clock
		this->clk_elapsed = 16;

	}
	else
	{
		//Ponteiro para qual registro rotacionar
		uint8_t *p_reg = (this->reg_file.reg8[dest]);

		//Grva o estado do bit 7
		uint8_t b7 = (*p_reg & 0x80);

		//Coloca o bit 0 no carry
		if((*p_reg) & 0x01)
			this->reg_file.setFlagBit(c);

		//Executa SRA
		(*p_reg) = ((*p_reg) >> 1);

		//Restaura o valor do bit 7
		(*p_reg) |= b7;

		//Verifica o zero
		if ((*p_reg) == 0x00)
			//Seta o flag
			this->reg_file.setFlagBit(z);

		//Atualiza o clock
		this->clk_elapsed = 8;

	}

	//Atualiza o contador de programa
	this->reg_file.PC += 2;


}

//SWAP reg8 - Troca os 4 bits mais significativos com os 4 menos significativos
//Opcodes - 0x30 a 0x37
void SimpleZ80::swapr8(uint8_t* opcode)
{
	//Limpa o Flags
	this->reg_file.F() = 0;

	//Registro de destino
	uint8_t dest = opcode[1] & 0x07;

	if(dest == 6)
	{

		//Grava o valor temporário do registro
		uint8_t r_temp = (*mem).read(this->reg_file.HL());

		//Troca os bits do registro
		(*mem).write(this->reg_file.HL(),((r_temp & 0xF0 ) >> 4) + ((r_temp & 0x0F) << 4));

		//Verifica o zero
		if ((*mem).read(this->reg_file.HL()) == 0x00)
			//Seta o flag
			this->reg_file.setFlagBit(z);

		//Atualiza o clock
		this->clk_elapsed = 16;		
	}
	else
	{
		//Ponteiro para qual registro rotacionar
		uint8_t *p_reg = this->reg_file.reg8[dest];

		//Grava o valor temporário do registro
		uint8_t r_temp = (*p_reg);

		//Troca os bits do registro
		(*p_reg) = ((r_temp & 0xF0 ) >> 4) + ((r_temp & 0x0F) << 4);

		//Verifica o zero
		if ((*p_reg) == 0x00)
			//Seta o flag
			this->reg_file.setFlagBit(z);

		//Atualiza o clock
		this->clk_elapsed = 8;		
	}

	//Atualiza o contador de programa
	this->reg_file.PC += 2;

}

//SRL reg8 - Deslocamento lógico dos bits do registro de 8 bit para esquerda
//Opcodes - 0x38 a 0x37
void SimpleZ80::srlr8(uint8_t* opcode)
{
	//Limpa o Flags
	this->reg_file.F() = 0;

	//Registro de destino
	uint8_t dest = opcode[1] & 0x07;

	if(dest == 6)
	{
		//Coloca o bit 7 no carry
		if((*mem).read(this->reg_file.HL())& 0x01)
			this->reg_file.setFlagBit(c);

		//Executa SLA
		(*mem).write(this->reg_file.HL(),((*mem).read(this->reg_file.HL()) >> 1));

		//Verifica o zero
		if ((*mem).read(this->reg_file.HL()) == 0x00)
			//Seta o flag
			this->reg_file.setFlagBit(z);

		//Atualiza o clock
		this->clk_elapsed = 16;	
	}
	else
	{
		//Ponteiro para qual registro rotacionar
		uint8_t *p_reg = (this->reg_file.reg8[dest]);

		//Coloca o bit 7 no carry
		if((*p_reg) & 0x01)
			this->reg_file.setFlagBit(c);

		//Executa SLA
		(*p_reg) = ((*p_reg) >> 1);

		//Verifica o zero
		if ((*p_reg) == 0x00)
			//Seta o flag
			this->reg_file.setFlagBit(z);

		//Atualiza o clock
		this->clk_elapsed = 8;		
	}

	//Atualiza o contador de programa
	this->reg_file.PC += 2;

}

//BIT b,reg8 - Testa o bit b no registro r8, seta a flag zero se o bit não está setado.
//Opcodes - 0x40 a 0x7F
void SimpleZ80::bitr8(uint8_t* opcode)
{
	//Limpa o Flags
	this->reg_file.F() &= 0x10;
	this->reg_file.setFlagBit(h);

	//Registro de destino
	uint8_t dest = opcode[1] & 0x07;

	//Bit para testar
	uint8_t bit = ((opcode[1] & 0x38) >> 3);

	if(dest == 6)
	{

		//Testa para setar a flag zero
		if(!((*mem).read(this->reg_file.HL()) & (1 << bit)))
			this->reg_file.setFlagBit(z);


		//Atualiza o clock
		this->clk_elapsed = 12;		
	}
	else
	{
		//Ponteiro para qual registro rotacionar
		uint8_t *p_reg = (this->reg_file.reg8[dest]);

		//Testa para setar a flag zero
		if(!(*p_reg & (1 << bit)))
			this->reg_file.setFlagBit(z);

		//Atualiza o clock
		this->clk_elapsed = 8;			
	}


	//Atualiza o contador de programa
	this->reg_file.PC += 2;

}

//RES b,reg8 - Coloca o bit b em 0
//Opcodes - 0x80 a 0xBF
void SimpleZ80::resr8(uint8_t* opcode)
{

	//Registro de destino
	uint8_t dest = opcode[1] & 0x07;

	//Bit para testar
	uint8_t bit = ((opcode[1] & 0x38) >> 3);

	if(dest == 6)
	{
	
		//Coloca o bit em 0
		(*mem).write(this->reg_file.HL(),((~(1 << bit)) & (*mem).read(this->reg_file.HL())));

		//Atualiza o clock
		this->clk_elapsed = 16;

	}else
	{
		//Ponteiro para qual registro rotacionar
		uint8_t *p_reg = (this->reg_file.reg8[dest]);

		//Coloca o bit em 0
		(*p_reg) &= (~(1 << bit));

		//Atualiza o clock
		this->clk_elapsed = 8;		
	}

	//Atualiza o contador de programa
	this->reg_file.PC += 2;

}

//SET b,reg8 - Coloca o bit b em 1
//Opcodes - 0xC0 a 0xFF
void SimpleZ80::setr8(uint8_t* opcode)
{
	//Registro de destino
	uint8_t dest = opcode[1] & 0x07;

	//Bit para testar
	uint8_t bit = ((opcode[1] & 0x38) >> 3);

	if(dest == 6)
	{

		//Coloca o bit em 0
		(*mem).write(this->reg_file.HL(),((1 << bit) | (*mem).read(this->reg_file.HL())));

		//Atualiza o clock
		this->clk_elapsed = 16;
	}
	else
	{
		//Ponteiro para qual registro rotacionar
		uint8_t *p_reg = (this->reg_file.reg8[dest]);

		//Coloca o bit em 0
		(*p_reg) |= (1 << bit);

		//Atualiza o clock
		this->clk_elapsed = 8;	
	}


	//Atualiza o contador de programa
	this->reg_file.PC += 2;

}

