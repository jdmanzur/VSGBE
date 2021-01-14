# VSGBE - Um Emulador de GameBoy

          _____                    _____                    _____                    _____                    _____          
         /\    \                  /\    \                  /\    \                  /\    \                  /\    \         
        /::\____\                /::\    \                /::\    \                /::\    \                /::\    \        
       /:::/    /               /::::\    \              /::::\    \              /::::\    \              /::::\    \       
      /:::/    /               /::::::\    \            /::::::\    \            /::::::\    \            /::::::\    \      
     /:::/    /               /:::/\:::\    \          /:::/\:::\    \          /:::/\:::\    \          /:::/\:::\    \     
    /:::/____/               /:::/__\:::\    \        /:::/  \:::\    \        /:::/__\:::\    \        /:::/__\:::\    \    
    |::|    |                \:::\   \:::\    \      /:::/    \:::\    \      /::::\   \:::\    \      /::::\   \:::\    \   
    |::|    |     _____    ___\:::\   \:::\    \    /:::/    / \:::\    \    /::::::\   \:::\    \    /::::::\   \:::\    \  
    |::|    |    /\    \  /\   \:::\   \:::\    \  /:::/    /   \:::\ ___\  /:::/\:::\   \:::\ ___\  /:::/\:::\   \:::\    \ 
    |::|    |   /::\____\/::\   \:::\   \:::\____\/:::/____/  ___\:::|    |/:::/__\:::\   \:::|    |/:::/__\:::\   \:::\____\
    |::|    |  /:::/    /\:::\   \:::\   \::/    /\:::\    \ /\  /:::|____|\:::\   \:::\  /:::|____|\:::\   \:::\   \::/    /
    |::|    | /:::/    /  \:::\   \:::\   \/____/  \:::\    /::\ \::/    /  \:::\   \:::\/:::/    /  \:::\   \:::\   \/____/ 
    |::|____|/:::/    /    \:::\   \:::\    \       \:::\   \:::\ \/____/    \:::\   \::::::/    /    \:::\   \:::\    \     
    |:::::::::::/    /      \:::\   \:::\____\       \:::\   \:::\____\       \:::\   \::::/    /      \:::\   \:::\____\    
    \::::::::::/____/        \:::\  /:::/    /        \:::\  /:::/    /        \:::\  /:::/    /        \:::\   \::/    /    
     ~~~~~~~~~~               \:::\/:::/    /          \:::\/:::/    /          \:::\/:::/    /          \:::\   \/____/     
                               \::::::/    /            \::::::/    /            \::::::/    /            \:::\    \         
                                \::::/    /              \::::/    /              \::::/    /              \:::\____\        
                                 \::/    /                \::/____/                \::/____/                \::/    /        
                                  \/____/                                           ~~                       \/____/         




## Conceito

Este é um pequeno projeto que eu comecei por hobby. Tudo começou com uma pequeno videogame caseiro baseado em um CPLD 
(Dispositivo Lógico Complexo Programável) com um Microntrolador ARM Cortex-M4F que eu realizei a alguns anos atrás, e
apesar do videogame não ser poderoso, percebi que seria possível a emulação de algum videogame de 8 bits. 
Escolhi o GameBoy justamente pela relativa simplicidade do hardware e por ser um videogame que tenho acesso ao hardware verdadeiro, 
sem contar a quantidade de informações disponíveis na internet. 
A primeira versão deste emulador foi escrita em Python, mas por uma questão de performance, eu decidi utilizar a linguagem C++.

## Desenvolvimento


## Sobre o emulador

Antes de mais nada, esse emulador não está perfeito, ou melhor dizendo, está incompleto. Simples assim: o código não está bem documentado, está
cheio de bugs e  da forma que se encontra, pouquíssimos jogos são compatíveis.
Esta versão suporta apenas Linux, mas espero fazer um Makefile decente que suporte o sistema operacional Windows em breve.

## Público Alvo 

Qualquer um que entenda de C++ e queira mexer em um código de emulação para isso. 
O código está em licença Z-lib, ou seja, pode ser utilizado para qualquer propósito. 
Porém, da forma que está, este sofware não é um produto pronto ou comercial, então se deseja apenas jogar um jogo no emulador, 
sem se importar com detalhes técnicos, este programa não é para você.

## Bugs
Como já foi dito, este código está bruto e possui diversos bugs comuns que um programa de computador pode ter, incluindo, mas não limitado à estouro de pilha, exceções não tratadas, ponteiros selvagens e etc. Este programa *não* deve ser utilizado de maneira alguma em modo de super usuário (root), e o autor não se responsabiliza por quaisquer danos, perda de dados ou acidente que possa ocorrer devido à utilização deste programa.

## Execução
### Compilação
Para a compilação é necessário o compilador g++ e a biblioteca SDL na sua versão 2.
Dentro da pasta principal, é necessário chamar o Makefile usando o comando make, como abaixo:

```
make
```
O arquivo de saída será ```vsgbe.out```. 
Para executar uma ROM com o emulador, o caminho para esta deve ser dado na linha de comando para chamar o arquivo. 
```
./vsgbe.out minha_rom.gb
```

### Controles

Direcional : Teclas direcionais 
Botão A: Tecla A 
Botão B: Tecla S 
Start: Tecla X
Mode: Tecla Z

## Screenshots

## Autores 
- [Adriano Emidio](https://github.com/adrianoemidio)
