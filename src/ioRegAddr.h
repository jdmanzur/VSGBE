#ifndef IOREGADDR_H_
#define IOREGADDR_H_

//Endereço do banco de ROM 0
#define ROM0    0x0000

//Endereço de início do banco de ROM
#define ROMX    0x4000

//Endereço de início da VRAM
#define VRAM    0x8000

//Endereço de início do banco de RAM
#define RAMX    0xA000

//Endereço de RAM interna
#define RAM0    0xC000

//Espelho da RAM interna
#define ERAM    0xE000

//Enderenço de início da OAM
#define OAMS     0xFE00

//Endereços Vazios
#define EMP0    0xFEA0

//Portas de entrada e saída
#define IOPR    0xFF00

//Endereços Vazios
#define EMP1    0xFF4C

//Endereço de RAM interna
#define RAM1    0xFF80

//Endereço dos registros especiais na memória
//Em certos aspectos, é melhor referenciar pelo nomes,

#define P1      0xFF00   
#define SB      0xFF01   
#define SC      0xFF02   
#define DIV     0xFF04   
#define TIMA    0xFF05   
#define TMA     0xFF06   
#define TAC     0xFF07   
#define IF      0xFF0F   
#define NR10    0xFF10   
#define NR11    0xFF11   
#define NR12    0xFF12   
#define NR13    0xFF13   
#define NR14    0xFF14   
#define NR21    0xFF16   
#define NR22    0xFF17   
#define NR23    0xFF18   
#define NR24    0xFF19   
#define NR30    0xFF1A   
#define NR31    0xFF1B   
#define NR32    0xFF1C   
#define NR33    0xFF1D   
#define NR34    0xFF1E   
#define NR41    0xFF20   
#define NR42    0xFF21   
#define NR43    0xFF22   
#define NR44    0xFF23   
#define NR50    0xFF24   
#define NR51    0xFF25   
#define NR52    0xFF26   
//FF30 - FF3F (Wave Pattern RAM)
#define LCDC    0xFF40   
#define STAT    0xFF41   
#define SCY     0xFF42   
#define SCX     0xFF43   
#define LY      0xFF44   
#define LYC     0xFF45   
#define DMA     0xFF46   
#define BGP     0xFF47   
#define OBP0    0xFF48   
#define OBP1    0xFF49   
#define WY      0xFF4A   
#define WX      0xFF4B   
#define IE      0xFFFF   

#endif /* IOREGADDR_H_ */