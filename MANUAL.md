# MANUAL DO zx81ide
**Ambiente Integrado de Desenvolvimento para Sinclair ZX-81 e Clones**

---

## Índice
1. [Introdução](#1-introdução)
2. [Requisitos de Sistema](#2-requisitos-de-sistema)
3. [Instalação e Compilação](#3-instalação-e-compilação)
4. [Arquitetura e Modos de Operação](#4-arquitetura-e-modos-de-operação)
5. [Interface de Linha de Comando (CLI)](#5-interface-de-linha-de-comando-cli)
6. [Estrutura de Projetos no zx81ide](#6-estrutura-de-projetos-no-zx81ide)
7. [Especificações Técnicas do ZX-81 e Clones](#7-especificações-técnicas-do-zx-81-e-clones)
8. [Formato de Snapshot `.p`](#8-formato-de-snapshot-p)
9. [Roteiro do Manual e Futuras Seções](#9-roteiro-do-manual-e-futuras-seções)

---

## 1. Introdução

O **zx81ide** é um ambiente integrado voltado à criação de jogos, utilitários e software de sistema para o computador **Sinclair ZX-81** e toda a sua família de clones compatíveis.

Seu objetivo é eliminar a fragmentação de ferramentas antigas, permitindo que o desenvolvedor escreva em **BASIC** (padrão ou estendido) ou **Assembly Z80**, monte/compile instantaneamente para arquivos binários `.p` e teste no emulador integrado com inspeção em tempo real de memória e registradores.

---

## 2. Requisitos de Sistema

### Ambientes Suportados
* **Windows:** Windows 10 ou Windows 11 (64-bit) com PowerShell 7+.
* **Linux:** Ubuntu 20.04+, Debian 11+, Fedora 36+ ou qualquer distribuição moderna com suporte a C++17.
* **macOS:** macOS 12 (Monterey) ou superior com Apple Clang ou LLVM.

### Ferramentas de Compilação
* **Compilador C++:** Suporte integral a **C++17**:
  * GCC 10 ou superior (no Windows, MSYS2 UCRT64 recomendado).
  * Clang 11 ou superior.
  * Microsoft Visual Studio 2019/2022 (MSVC v142/v143).
* **CMake:** Versão 3.15 ou superior.
* **PowerShell:** PowerShell 7.x (`pwsh`) para automação de tarefas via `build.ps1`.
* **Git:** Para controle de versão e obtenção dos submódulos.

---

## 3. Instalação e Compilação

### Clonando o Repositório
```bash
git clone https://github.com/wilsonpilon/zx81ide.git
cd zx81ide
```

### Compilando via `build.ps1` (Recomendado)
No Windows, abra o PowerShell no diretório do projeto e execute:
```powershell
# Execução padrão (compilação + geração da pasta dist/ + empacotamento ZIP)
.\build.ps1

# Limpeza e recompilação completa
.\build.ps1 -Clean

# Empacotamento de Release
.\build.ps1 -Release
```

### Compilando Manualmente via CMake
```bash
# Criar o diretório de build
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release

# Compilar
cmake --build build --config Release

# Executar a aplicação
./build/zx81ide.exe
```

---

## 4. Arquitetura e Modos de Operação

O **zx81ide** foi concebido em 3 camadas operacionais complementares:

### 4.1. Modo CLI (Command-Line Interface)
Focado em agilidade e automação. Permite:
* Montar arquivos `.asm` chamando o motor Z80 interno.
* Converter listagens BASIC `.bas` em snapshots `.p`.
* Empacotar projetos e gerar relatórios de uso de memória do ZX-81.
* Disparar o emulador diretamente com o snapshot carregado.

### 4.2. Modo TUI (Text User Interface)
Construído com base na biblioteca **FTXUI**, este modo oferece uma interface rica baseada em texto diretamente no console:
* Editor de código com realce de sintaxe BASIC/Z80.
* Gerenciador de arquivos do projeto.
* Terminal de saída de montagem integrado.
* Ideal para ambientes remotos (SSH) ou máquinas modestas.

### 4.3. Modo GUI (Graphical User Interface)
Construído com **Dear ImGui**:
* Janelas dockáveis e suporte a múltiplos monitores.
* Editor de blocos gráficos e caracteres ZX-81 (32x24 e pseudo-alta resolução 64x48).
* Painel de mapa de memória interativo.
* Janela do emulador EightyOne embutida com sincronização de frames e breakpoints.

---

## 5. Interface de Linha de Comando (CLI)

Na versão inicial (`v0.1.0`), a CLI executa o bootstrap do sistema e exibe as informações fundamentais de versão e linhagem de hardware:

```text
================================================================
  _____  _     ___  _   _     _ _       
 |_  / \/ /___( _ )/ | (_) __| (_) ___  
  / / \  // _ \/ _ \| | | |/ _` | |/ _ \ 
 /___//_/\___/\___/|_| |_|\__,_|_|\___/ 
  Integrated Development Environment for Sinclair ZX-81 & Clones
================================================================
Program Name   : zx81ide
Version        : 0.1.0
Major Model (X): Sinclair ZX81 (Foundation)
Game Codename(Y): 3D Monster Maze
Build Subtitle(Z): Estrutura inicial do projeto, documentacao e CLI bootstrap
----------------------------------------------------------------
Hello, Sinclair ZX-81 World! [READY]
================================================================
```

### Próximas Opções de Linha de Comando (Planejadas):
```text
Uso: zx81ide [opcoes] [arquivo_fonte]

Opcoes:
  -h, --help            Exibe esta mensagem de ajuda
  -v, --version         Exibe versao detalhada
  --tui                 Inicia em modo Text User Interface
  --gui                 Inicia em modo Graphical User Interface
  -a, --assemble        Monta arquivo Assembly Z80 (.asm) para .p
  -b, --basic           Compila codigo BASIC (.bas) para .p
  -e, --emulate         Inicia emulador com o arquivo fornecido
  -m, --model <nome>    Define o hardware alvo (zx81, tk85, cp200, ts1000)
  -r, --ram <tamanho>   Define a quantidade de RAM (1K, 2K, 16K, 32K, 64K)
```

---

## 6. Estrutura de Projetos no zx81ide

Um projeto padrão no **zx81ide** conterá a seguinte organização de arquivos:

```text
meu_projeto/
├── project.zx81        # Arquivo de configuracao do projeto (JSON ou TOML)
├── src/
│   ├── main.bas        # Codigo BASIC principal
│   └── routine.asm     # Rotinas auxiliares em Assembly Z80
├── assets/
│   └── tiles.scr       # Telas e blocos graficos gerados no editor de tela
└── bin/
    └── output.p        # Snapshot final pronto para carregar no hardware real ou emulador
```

---

## 7. Especificações Técnicas do ZX-81 e Clones

Para referência do desenvolvedor, o ambiente suporta as particularidades dos seguintes hardwares:

| Modelo / Clone | Fabricante | RAM Padrão | ROM | Particularidades de Hardware |
| :--- | :--- | :--- | :--- | :--- |
| **Sinclair ZX-81** | Sinclair Research (UK) | 1 KB (exp. 16 KB/64 KB) | 8 KB | ULA clássica com modos SLOW e FAST. |
| **TS1000 / TS1500** | Timex Sinclair (USA) | 2 KB / 16 KB | 8 KB | Saída NTSC padrão, layout de teclas similar ao ZX81. |
| **TK82C** | Microdigital (Brasil) | 2 KB | 8 KB | Clone inicial com circuito de vídeo transistorizado. |
| **TK83** | Microdigital (Brasil) | 2 KB (exp. 16K/64K) | 8 KB | Gabinete plástico refinado, teclas mecânicas estilizadas. |
| **TK85** | Microdigital (Brasil) | 16 KB / 48 KB | 8 KB/10 KB | Clone avançado com gabinete tipo ZX Spectrum e som embutido. |
| **CP200 / CP200S** | Prológica (Brasil) | 16 KB | 8 KB | Gabinete prateado robusto, teclado de membrana chiclete. |

---

## 8. Formato de Snapshot `.p`

O formato `.p` é uma cópia direta da área de memória do sistema do ZX-81, iniciando na variável de sistema `VERSN` (endereço `$4009` em decimal `16393`) até o final do display file ou variáveis do BASIC.

O **zx81ide** gerará snapshots em conformidade estrita com a ROM do Sinclair ZX-81, calculando automaticamente:
* Variáveis de sistema (`ERR_NR`, `FLAGS`, `DF_SZ`, etc.).
* O Display File (caracteres terminados com o delimitador `$76` / `NEWLINE`).
* A tabela de variáveis de usuário.

---

## 9. Roteiro do Manual e Futuras Seções

Este manual é um documento vivo e continuará sendo expandido a cada nova versão (`Y` e `Z`). As próximas seções a serem adicionadas incluem:
1. *Tutorial: Escrevendo seu primeiro jogo no zx81ide.*
2. *Guia do Montador SJAsmPlus e macros específicas para ZX-81.*
3. *Modos de Gráficos de Alta Resolução (HRG) no hardware não-modificado.*
4. *Protocolo de Comunicação e Depuração com o Emulador EightyOne.*
