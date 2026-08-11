# Emulador CHIP-8
![Static Badge](https://img.shields.io/badge/C%2B%2B-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white)
![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg?style=for-the-badge)

> Emulador completo do CHIP-8 escrito em C++, com depurador integrado, interface gráfica via Dear ImGui e suporte a áudio, tudo construído sobre SDL2.

---

## Índice

- [Sobre o Projeto](#-sobre-o-projeto)
- [Funcionalidades](#-funcionalidades)
- [Arquitetura do CHIP-8](#-arquitetura-do-chip-8)
- [Requisitos](#-requisitos)
- [Instalação das Dependências](#-instalação-das-dependências)
- [Como Compilar](#-como-compilar)
- [Como Executar](#-como-executar)
- [Controles](#-controles)
- [Depurador](#-depurador)
- [ROMs Incluídas](#-roms-incluídas)
- [Estrutura do Projeto](#-estrutura-do-projeto)
- [Tecnologias Utilizadas](#-tecnologias-utilizadas)
- [Licença](#-licença)

---

## Sobre o Projeto

O **CHIP-8** é uma linguagem de máquina interpretada desenvolvida na década de 1970 por Joseph Weisbecker, originalmente criada para facilitar a programação de jogos em microcomputadores como o COSMAC VIP e o Telmac 1800. Sua arquitetura simples — com apenas 35 opcodes — o torna um excelente ponto de partida para aprender desenvolvimento de emuladores.

Este projeto implementa um emulador completo do CHIP-8 em **C++17**, com foco em:

- **Fidelidade à especificação original** — todos os 35 opcodes implementados corretamente
- **Experiência de desenvolvedor** — depurador visual integrado com disassembler, visualizador de memória e inspetor de estado da CPU
- **Portabilidade** — suporte nativo a Linux e Windows (MinGW / MSYS2)

---

## Funcionalidades

### Emulação

- **CPU CHIP-8 completa** — 4 KB de RAM, 16 registradores de 8 bits (V0–VF), registrador de índice (I), contador de programa (PC), pilha de 16 níveis e timers de delay e som
- **Display monocromático 64×32** com renderização via SDL2
- **Teclado hexadecimal** de 16 teclas mapeado para o teclado do PC
- **Geração de áudio** — onda quadrada de 440 Hz produzida via callback SDL2 enquanto o timer de som estiver ativo

### Interface Gráfica (Dear ImGui)

- **Menu principal** — exibido na inicialização
- **Navegador de ROMs** — lista pesquisável das ROMs presentes na pasta `./roms/`; carregue com duplo clique ou seleção + botão "Load"
- **Ajuste de velocidade** — ciclos por frame configuráveis de 1 a 1000 (padrão: 15)
- **Botões de Reset e Load ROM** disponíveis a qualquer momento

### Depurador Integrado

Pressione `TAB` para abrir o painel de depuração em tela cheia:

- **Disassembler** — lista todas as instruções da ROM com a instrução atual (PC) destacada
- **Visualizador de memória** — dump hexadecimal dos 4 KB de espaço de endereçamento, com a instrução atual realçada em vermelho
- **Painel de estado da CPU** — visão ao vivo de PC, I, V0–VF, SP e o conteúdo da pilha de chamadas
- **Visualizador do teclado** — exibe o estado de cada uma das 16 teclas em tempo real
- **Pause / Step** — pause a emulação e avance instrução por instrução
- **Reset / Load ROM** — reinicie ou carregue uma nova ROM sem sair do emulador

---

## Arquitetura do CHIP-8

| Componente              | Especificação                                   |
|-------------------------|-------------------------------------------------|
| Memória                 | 4096 bytes (4 KB)                               |
| Registradores           | 16 registradores de 8 bits (V0–VF)             |
| Registrador de índice   | 16 bits (I)                                     |
| Contador de programa    | 16 bits (PC), início em `0x200`                |
| Pilha                   | 16 níveis de 16 bits                            |
| Timer de delay          | 8 bits, decrementado a 60 Hz                   |
| Timer de som            | 8 bits, gera beep quando > 0                  |
| Display                 | 64×32 pixels, 1 bit por pixel (monocromático)  |
| Teclado                 | 16 teclas hexadecimais (0–F)                   |
| Opcodes                 | 35 instruções de 16 bits                        |

> **Nota:** Os programas CHIP-8 são carregados a partir do endereço `0x200` (512), pois os primeiros 512 bytes são reservados para a fonte de sprites embutida (caracteres 0–F).

---

## Requisitos

| Dependência      | Versão mínima                         |
|------------------|---------------------------------------|
| Compilador C++   | C++17 ou superior (`g++` recomendado) |
| SDL2             | 2.x                                   |
| make             | Qualquer versão recente               |

---

## Instalação das Dependências

### Linux (Debian / Ubuntu)

```bash
sudo apt update
sudo apt install build-essential libsdl2-dev
```

### Linux (Arch / Manjaro)

```bash
sudo pacman -S base-devel sdl2
```

### Windows (MSYS2 / MinGW-w64)

Abra o terminal **MSYS2 UCRT64** e execute:

```bash
pacman -S mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-SDL2 make
```

> O Makefile detecta automaticamente o sistema operacional e ajusta os caminhos de include e link para `C:/msys64/ucrt64`.

---

## Como Compilar

Clone o repositório e execute `make` na raiz do projeto:

```bash
git clone https://github.com/rodrigop07/CHIP-8-Emulator.git
cd CHIP-8-Emulator
make
```

O executável `chip8` (ou `chip8.exe` no Windows) será gerado na raiz do projeto.

### Alvos do Makefile

| Alvo         | Descrição                               |
|--------------|-----------------------------------------|
| `make`       | Compila o emulador (alvo padrão)        |
| `make run`   | Compila (se necessário) e executa       |
| `make clean` | Remove todos os artefatos de compilação |

### Flags de compilação

O projeto é compilado com `-O3 -Wall -std=c++17`, garantindo otimizações de velocidade e avisos rigorosos do compilador.

---

## Como Executar

```bash
./chip8
```

Ao iniciar, o menu principal será exibido. Clique em **Load ROM** para abrir o navegador de ROMs e escolher um jogo da pasta `./roms/`.

Você também pode compilar e executar em um único comando:

```bash
make run
```

---

## Controles

### Teclas do emulador

| Tecla | Ação                               |
|-------|------------------------------------|
| `ESC` | Abre / fecha o menu principal      |
| `TAB` | Abre / fecha o painel de depuração |

### Mapeamento do teclado CHIP-8

O teclado hexadecimal original do CHIP-8 (4×4) é mapeado para as seguintes teclas do teclado do PC:

```
Tecla CHIP-8   →   Tecla do PC
────────────────────────────────
  1  2  3  C   →   1  2  3  4
  4  5  6  D   →   Q  W  E  R
  7  8  9  E   →   A  S  D  F
  A  0  B  F   →   Z  X  C  V
```

---

## Depurador

O depurador integrado é uma das principais características deste emulador. Para ativá-lo, pressione `TAB` durante a execução.

### Painel de Disassembly

Exibe todas as instruções da ROM traduzidas para mnemônicos legíveis (ex: `00E0 - CLS`, `6XNN - LD V0, 0x3C`). A instrução sendo executada é realçada automaticamente conforme o PC avança.

### Visualizador de Memória

Apresenta um dump hexadecimal completo dos 4096 bytes de memória, organizado em linhas de 16 bytes. O endereço da instrução atual é destacado em vermelho.

### Estado da CPU

Mostra em tempo real:
- **PC** — Program Counter (endereço da próxima instrução)
- **I** — Registrador de índice
- **V0–VF** — Os 16 registradores de propósito geral
- **SP** — Stack Pointer
- **Stack** — Conteúdo atual da pilha de chamadas (endereços de retorno)

### Keypad ao Vivo

Visualização em grade 4×4 do teclado CHIP-8, com indicação visual de quais teclas estão pressionadas no momento.

### Pause / Step

- **Pause** — congela a emulação no estado atual
- **Step** — avança exatamente um ciclo de CPU por vez (ideal para rastrear bugs em ROMs)

---

## ROMs Incluídas

O projeto já inclui **24 ROMs clássicas** na pasta `./roms/`:

| ROM                      | Descrição                                    |
|--------------------------|----------------------------------------------|
| `15PUZZLE`               | Quebra-cabeça deslizante de 15 peças         |
| `BLINKY`                 | Clone do Pac-Man                             |
| `BLITZ`                  | Jogo de bombardeiro aéreo                    |
| `BRIX`                   | Clone do Breakout                            |
| `CONNECT4`               | Jogo Connect Four (Liga 4)                   |
| `GUESS`                  | Jogo de adivinhar números                    |
| `HIDDEN`                 | Jogo de memória com cartas escondidas        |
| `INVADERS`               | Clone do Space Invaders                      |
| `KALEID`                 | Visualizador caleidoscópico                  |
| `MAZE`                   | Gerador de labirintos aleatórios             |
| `MERLIN`                 | Jogo de memória de sequências (Simon Says)   |
| `MISSILE`                | Defesa anti-míssil                           |
| `PONG`                   | Pong clássico (1 jogador vs CPU)             |
| `PONG2`                  | Pong para 2 jogadores                        |
| `PUZZLE`                 | Quebra-cabeça                                |
| `SYZYGY`                 | Jogo de habilidade                           |
| `TANK`                   | Jogo de tanque de guerra                     |
| `TETRIS`                 | Tetris clássico                              |
| `TICTAC.ch8`             | Jogo da velha                                |
| `UFO`                    | Atirar em OVNIs                              |
| `VBRIX`                  | Breakout vertical                            |
| `VERS`                   | Jogo de cobra (Snake)                        |
| `WIPEOFF`                | Breakout simplificado                        |
| `ultimatetictactoe.ch8`  | Jogo da velha definitivo                     |

---

## Estrutura do Projeto

```
CHIP-8-Emulator/
├── src/
│   ├── main.cpp            # Janela SDL2, áudio, interface ImGui e loop principal
│   ├── chip8.cpp           # CPU CHIP-8: fetch / decode / execute (35 opcodes)
│   ├── chip8.h             # Declaração da classe Chip8
│   ├── disassembler.cpp    # Disassembler de ROMs CHIP-8
│   ├── disassembler.h      # Declaração da classe Disassembler
│   └── imgui/              # Dear ImGui (biblioteca vendorizada)
├── roms/                   # 24 ROMs clássicas do CHIP-8
├── Makefile                # Build multiplataforma (Linux e Windows)
├── LICENSE                 # Licença do projeto
└── README.md               # Este arquivo
```

### Descrição dos arquivos principais

| Arquivo                | Responsabilidade                                                                  |
|------------------------|-----------------------------------------------------------------------------------|
| `src/chip8.h`          | Define a classe `Chip8` com todos os componentes de hardware do emulador          |
| `src/chip8.cpp`        | Implementa `loadROM()`, `reset()` e `cycle()` — o núcleo de emulação             |
| `src/disassembler.cpp` | Converte opcodes binários em mnemônicos legíveis para o painel de debug           |
| `src/main.cpp`         | Gerencia SDL2 (vídeo + áudio), Dear ImGui (UI) e o loop principal de emulação    |
| `Makefile`             | Detecta o SO automaticamente e configura compilação para Linux ou Windows         |

---

## Tecnologias Utilizadas

| Tecnologia     | Uso                                               |
|----------------|---------------------------------------------------|
| **C++17**      | Linguagem principal                               |
| **SDL2**       | Renderização de vídeo, entrada de teclado e áudio |
| **Dear ImGui** | Interface gráfica do depurador e menus            |
| **Make**       | Sistema de build multiplataforma                  |

---

## Licença

Este projeto está licenciado sob a Licença MIT — veja o arquivo [LICENSE](LICENSE) para mais detalhes.

---
