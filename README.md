# Emulador CHIP-8

Um interpretador/emulador CHIP-8 escrito em C++ utilizando SDL2 para gráficos, áudio e entrada de dados. O emulador implementa o conjunto completo de instruções padrão do CHIP-8 e conta com um diálogo nativo do sistema operacional para seleção de ROMs.

## Funcionalidades

- Implementação completa de todos os 35 opcodes padrão do CHIP-8
- Display monocromático de 64×32 pixels renderizado via SDL2, escalado para 960×480
- Som de bipe em onda quadrada (440 Hz) acionado pelo temporizador de som
- Mapeamento do teclado hexadecimal de 16 teclas do CHIP-8 para o layout QWERTY
- Diálogo de arquivo nativo do SO (via tinyfiledialogs) para seleção de ROM na inicialização e em tempo de execução
- Troca de ROM em tempo real com `Ctrl Esquerdo`
- CPU emulada a ~900 Hz (15 ciclos por frame de ~16 ms), temporizadores a 60 Hz

## Arquitetura

```
CHIP-8-Emulator/
├── src/
│   ├── chip8.cpp           # Núcleo do CHIP-8: CPU, memória, display, temporizadores
│   ├── main.cpp            # Janela SDL2, renderizador, áudio, loop de entrada
│   ├── tinyfiledialogs.c   # Biblioteca de diálogo de arquivo nativo do SO
│   └── tinyfiledialogs.h
├── roms/                   # Coleção com 24 ROMs clássicos do CHIP-8
├── Makefile
└── README.md
```

### Especificações de Hardware do CHIP-8 (emuladas)

| Componente          | Detalhe                                      |
|---------------------|----------------------------------------------|
| RAM                 | 4 KB (4096 bytes)                            |
| Registers           | 16 × 8-bit de uso geral (V0–VF)              |
| Index Register      | 16-bit (I)                                   |
| Program Counter     | 16-bit, inicia em `0x200`                    |
| Stack               | Stack de sub-rotinas com 16 níveis           |
| Display             | 64 × 32 pixels, monocromático                |
| Keyboard            | Teclado hexadecimal de 16 teclas             |
| Timers              | Delay e Sound, ambos 8-bit a 60 Hz           |
| Font                | Conjunto de sprites embutido (0–F), armazenado em `0x050`–`0x09F` |

## Requisitos

| Dependência | Versão   |
|-------------|----------|
| Compilador C++ (g++) | C++17 ou superior |
| SDL2        | 2.x      |
| MSYS2 / MinGW (Windows) | Toolchain UCRT64 |

### Instalando o SDL2

**Windows (MSYS2 UCRT64):**
```bash
pacman -S mingw-w64-ucrt-x86_64-SDL2
```

**Linux (Debian/Ubuntu):**
```bash
sudo apt install libsdl2-dev
```

## Compilação

```bash
# Clone o repositório
git clone https://github.com/rodrigop07/CHIP-8-Emulator.git
cd CHIP-8-Emulator

# Compilar
make

# Executar
make run
```

> **Nota para Windows:** O Makefile espera os headers e bibliotecas do SDL2 em `C:/msys64/ucrt64`. Ajuste `INCLUDE_PATHS` e `LIBRARY_PATHS` no `Makefile` caso sua instalação esteja em outro diretório.

## Executando

Ao iniciar, um diálogo de arquivo nativo será aberto — navegue até a pasta `roms/` e selecione qualquer ROM.

```bash
./chip8        # Linux
chip8.exe      # Windows
```

## Mapeamento de Teclado

O CHIP-8 original utilizava um teclado hexadecimal de 16 teclas. Este emulador o mapeia para as seguintes teclas QWERTY:

```
Teclado CHIP-8      Mapeamento no Teclado
┌───┬───┬───┬───┐   ┌───┬───┬───┬───┐
│ 1 │ 2 │ 3 │ C │   │ 1 │ 2 │ 3 │ 4 │
├───┼───┼───┼───┤   ├───┼───┼───┼───┤
│ 4 │ 5 │ 6 │ D │   │ Q │ W │ E │ R │
├───┼───┼───┼───┤   ├───┼───┼───┼───┤
│ 7 │ 8 │ 9 │ E │   │ A │ S │ D │ F │
├───┼───┼───┼───┤   ├───┼───┼───┼───┤
│ A │ 0 │ B │ F │   │ Z │ X │ C │ V │
└───┴───┴───┴───┘   └───┴───┴───┴───┘
```

| Tecla Especial   | Ação                                      |
|------------------|-------------------------------------------|
| `Ctrl Esquerdo`  | Abrir diálogo de seleção de ROM (hot-swap) |

## ROMs Incluídos

A pasta `roms/` contém 24 programas clássicos de domínio público para CHIP-8:

| ROM               | Descrição                    |
|-------------------|------------------------------|
| `PONG`            | Pong clássico (1 jogador)    |
| `PONG2`           | Pong (2 jogadores)           |
| `TETRIS`          | Clone de Tetris              |
| `INVADERS`        | Clone de Space Invaders      |
| `BLINKY`          | Clone de Pac-Man             |
| `BRIX`            | Clone de Breakout            |
| `TANK`            | Jogo de batalha de tanques   |
| `CONNECT4`        | Conecta 4                    |
| `TICTAC`          | Jogo da velha                |
| `15PUZZLE`        | Quebra-cabeça deslizante     |
| `MAZE`            | Gerador de labirintos        |
| `KALEID`          | Demo caleidoscópio           |
| `MERLIN`          | Clone de Genius (Simon Says) |
| `GUESS`           | Jogo de adivinhar números    |
| `MISSILE`         | Clone de Missile Command     |
| `UFO`             | Atirador de OVNIs            |
| `VERS`            | Jogo de cobra                |
| `WIPEOFF`         | Variante de Breakout         |
| `VBRIX`           | Breakout vertical            |
| `BLITZ`           | Jogo de bombardeio           |
| `SYZYGY`          | Jogo de puzzle               |
| `HIDDEN`          | Jogo de cartas ocultas       |
| `PUZZLE`          | Jogo de puzzle               |
| `ultimatetictactoe` | Jogo da velha supremo      |

## Opcodes Implementados

| Opcode   | Mnemônico        | Descrição                                                |
|----------|------------------|----------------------------------------------------------|
| `00E0`   | CLS              | Limpa o display                                          |
| `00EE`   | RET              | Retorna de sub-rotina                                    |
| `1NNN`   | JP addr          | Salta para o endereço NNN                                |
| `2NNN`   | CALL addr        | Chama sub-rotina em NNN                                  |
| `3xkk`   | SE Vx, byte      | Pula próxima instrução se Vx == kk                       |
| `4xkk`   | SNE Vx, byte     | Pula próxima instrução se Vx != kk                       |
| `5xy0`   | SE Vx, Vy        | Pula próxima instrução se Vx == Vy                       |
| `6xkk`   | LD Vx, byte      | Carrega kk em Vx                                         |
| `7xkk`   | ADD Vx, byte     | Adiciona kk a Vx                                         |
| `8xy0`   | LD Vx, Vy        | Define Vx = Vy                                           |
| `8xy1`   | OR Vx, Vy        | Define Vx = Vx OR Vy                                     |
| `8xy2`   | AND Vx, Vy       | Define Vx = Vx AND Vy                                    |
| `8xy3`   | XOR Vx, Vy       | Define Vx = Vx XOR Vy                                    |
| `8xy4`   | ADD Vx, Vy       | Define Vx = Vx + Vy, VF = carry                          |
| `8xy5`   | SUB Vx, Vy       | Define Vx = Vx - Vy, VF = NOT borrow                    |
| `8xy6`   | SHR Vx           | Desloca Vx à direita, VF = LSB                           |
| `8xy7`   | SUBN Vx, Vy      | Define Vx = Vy - Vx, VF = NOT borrow                    |
| `8xyE`   | SHL Vx           | Desloca Vx à esquerda, VF = MSB                          |
| `9xy0`   | SNE Vx, Vy       | Pula próxima instrução se Vx != Vy                       |
| `Annn`   | LD I, addr       | Define I = nnn                                           |
| `Bnnn`   | JP V0, addr      | Salta para nnn + V0                                      |
| `Cxkk`   | RND Vx, byte     | Define Vx = byte aleatório AND kk                        |
| `Dxyn`   | DRW Vx, Vy, n    | Desenha sprite de n bytes em (Vx, Vy), VF = colisão      |
| `Ex9E`   | SKP Vx           | Pula próxima instrução se a tecla Vx estiver pressionada |
| `ExA1`   | SKNP Vx          | Pula próxima instrução se a tecla Vx não estiver pressionada |
| `Fx07`   | LD Vx, DT        | Define Vx = valor do temporizador de delay               |
| `Fx0A`   | LD Vx, K         | Aguarda pressionamento de tecla e armazena em Vx         |
| `Fx15`   | LD DT, Vx        | Define temporizador de delay = Vx                        |
| `Fx18`   | LD ST, Vx        | Define temporizador de som = Vx                          |
| `Fx1E`   | ADD I, Vx        | Define I = I + Vx                                        |
| `Fx29`   | LD F, Vx         | Define I = localização do sprite do dígito Vx            |
| `Fx33`   | LD B, Vx         | Armazena BCD de Vx em I, I+1 e I+2                       |
| `Fx55`   | LD [I], Vx       | Armazena V0–Vx na memória a partir de I                  |
| `Fx65`   | LD Vx, [I]       | Lê V0–Vx da memória a partir de I                        |

## Licença

Este projeto está licenciado sob a Licença MIT — veja o arquivo [LICENSE](LICENSE) para mais detalhes.
