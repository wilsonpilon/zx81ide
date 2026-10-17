# zx81ide

> **Ambiente Integrado de Desenvolvimento (IDE) para Sinclair ZX-81 e Clones**

[![C++17](https://img.shields.io/badge/C%2B%2B-17-blue.svg)](https://en.wikipedia.org/wiki/C%2B%2B17)
[![CMake](https://img.shields.io/badge/CMake-3.15%2B-brightgreen.svg)](https://cmake.org/)
[![PowerShell](https://img.shields.io/badge/PowerShell-7%2B-blueviolet.svg)](https://github.com/PowerShell/PowerShell)
[![License: BSD-3-Clause](https://img.shields.io/badge/License-BSD_3--Clause-yellow.svg)](LICENSE)
[![Release](https://img.shields.io/badge/Version-v0.1.11--3D_Monster_Maze-orange.svg)](RELEASE.md)
[![Database](https://img.shields.io/badge/Storage-SQLite_3-blue.svg)](https://www.sqlite.org/)

---

## 1. Visão Geral

O **zx81ide** é um ecossistema multiplataforma moderno para desenvolvimento de software voltado ao lendário microcomputador **Sinclair ZX-81** e seus clones nacionais e internacionais mais expressivos:

* **Sinclair ZX-81** / **ZX-80**
* **Timex Sinclair 1000** (TS1000 / TS1500)
* **Microdigital TK82C / TK83 / TK85** (Brasil)
* **Prológica CP200 / CP200S** (Brasil)
* **Ringo R-470** (Brasil)

Inspirado na arquitetura e filosofia do [fwMSX](https://github.com/wilsonpilon/fwMSX), o **zx81ide** unifica em uma única suíte:
* **Edição de Código:** Suporte a BASIC padrão Sinclair, BASIC moderno estruturado (Dignified BASIC) e Assembly Z80.
* **Compilação e Montagem:** Pipeline integrado de montagem Z80 de alta performance e gerador de snapshots binários `.p`.
* **Editor Gráfico Integrado:** Manipulação direta do *Display File* (32x24 texto/semigráficos, 64x48 blocos e modo HRG - High Resolution Graphics).
* **Emulação e Debug Bidirecional:** Núcleo de emulação embutido com inspeção de registradores Z80, visualização de memória em tempo real e injeção a quente de código.
* **Três Modos de Operação:**
  1. **CLI (Command-Line Interface):** Automação em pipelines de compilação contínua (CI/CD) e scripts.
  2. **TUI (Text User Interface):** Interface ágil em terminal para edições rápidas sem necessidade de servidor gráfico.
  3. **GUI (Graphical User Interface):** Ambiente desktop completo com janelas dockáveis, preview gráfico e emulador interativo.

---

## 2. Ferramentas e Ambiente de Desenvolvimento

Este projeto é desenvolvido com ferramentas modernas e práticas ágeis de engenharia de software:

| Categoria | Ferramenta / Tecnologia | Detalhes / Papel no Projeto |
| :--- | :--- | :--- |
| **Inteligência Artificial** | **Google Gemini & Antigravity IDE** | Pair-programming agentic, geração de scaffold, refatoração, documentação contínua e rastreamento em `OUTLINE.md`. |
| **Shell & Automação** | **PowerShell 7+ (`pwsh`)** | Script de build automatizado (`build.ps1`), orquestração de compilação, download de dependências e empacotamento de releases `.zip`. |
| **Linguagem Principal** | **C++17 / C99** | Alto desempenho, controle fino de memória e compatibilidade com emuladores e montadores existentes. |
| **Compiladores Homologados** | **GCC (MSYS2 UCRT64 16+) / Clang (22+) / MSVC** | Suporte multiplataforma (Windows nativo, Linux e macOS). |
| **Sistema de Build** | **CMake (>= 3.15)** | Gerenciamento multiplataforma de dependências e alvos de compilação. |

---

## 3. Créditos de Terceiros e Componentes Externos

O **zx81ide** presta homenagem e apoia-se em soluções consagradas criadas pela comunidade de entusiastas e desenvolvedores retro:

* **[fwMSX](https://github.com/wilsonpilon/fwMSX) (Wilson Pilon):** Inspiração estrutural e conceitual de ecossistema de desenvolvimento retrointegrado.
* **[SJAsmPlus](https://github.com/z00m128/sjasmplus) (Aprisobal, Pedro Gimeno, Pit/C4N):** Montador Z80 de alto desempenho, com macros ricas, suporte a arquivos `.sld` e integração Lua.
* **[EightyOne Core](https://github.com/Sinclair-ZX81/EightyOne) (Michael D Wynne):** Referência definitiva na emulação do hardware do ZX-81, ULA, temporização de ciclo e comportamento de clones.
* **[zmakebas](https://github.com/z88dk/z88dk/tree/master/src/z88dk/zmakebas) (Russell Marks):** Conversor de código BASIC Sinclair em texto puro para snapshots binários `.p`.
* **[z88dk](https://github.com/z88dk/z88dk):** Compilador C Small-C e biblioteca de ferramentas Z80 com rico suporte a target ZX81.
* **[Boriel ZX Basic Compiler](https://zxbasic.readthedocs.io/) (Jose Rodriguez-Rosa):** Compilador moderno de BASIC estruturado para Z80.
* **[FTXUI](https://github.com/ArthurSonzogni/FTXUI) (Arthur Sonzogni):** Biblioteca moderna de C++ para construção de interfaces no terminal (TUI).
* **[Dear ImGui](https://github.com/ocornut/imgui) (Omar Cornut):** Biblioteca gráfica *immediate-mode* ultrarrápida para a interface gráfica (GUI).
* **[SQLite 3](https://www.sqlite.org/) (D. Richard Hipp):** Motor de banco de dados relacional embarcado em C (Amalgamation) utilizado para o banco de configurações (`zx81ide.db`) e formato oficial de projetos portáteis (`.zx81project`).

---

## 4. Política de Versionamento (`X.Y.Z`)

Adotamos um esquema de versionamento semântico personalizado com homenagens históricas ao universo do ZX-81 e clones:

```text
      X       .       Y       .       Z
  [Modelo]        [Jogo Clássico]  [Subtítulo da Alteração]
```

* **`X` (Major):** Representa grandes conjuntos de funcionalidades e marcos arquiteturais consolidados.
  * *Homenagem:* Modelos históricos de ZX-81 e seus clones (ex: *Sinclair ZX81*, *Timex Sinclair 1000*, *Microdigital TK82C*, *Microdigital TK83*, *Microdigital TK85*, *Prológica CP200*).
* **`Y` (Minor):** Features novas que foram iniciadas ou concluídas.
  * *Homenagem:* Nomes de jogos clássicos de ZX-81 (ex: *3D Monster Maze*, *Mazogs*, *Flight Simulation*, *1K ZX Chess*, *Asteroids*).
* **`Z` (Patch / Build):** Cada compilação e iteração de desenvolvimento.
  * *Identificação:* Subtítulo curto e objetivo indicando o que foi alterado ou corrigido.

### Versão Atual
* **Versão:** `v0.1.4`
* **X (Modelo):** `Sinclair ZX81 (Foundation)`
* **Y (Jogo):** `3D Monster Maze`
* **Z (Build):** `Alinhamento e ampliacao da borda dos menus TUI`

---

## 5. Estrutura do Repositório

```text
zx81ide/
├── CMakeLists.txt              # Script mestre de compilação CMake
├── build.ps1                   # Script de automação PowerShell para build e release
├── SPEC.md                     # Especificação técnica (Live Document)
├── README.md                   # Apresentação do projeto e visão geral
├── MANUAL.md                   # Manual de usuário e guia de desenvolvimento
├── OUTLINE.md                  # Registro contínuo de conversas e decisões com a IA
├── CHANGELOG.md                # Histórico resumido de versões
├── RELEASE.md                  # Notas de release detalhadas
├── LICENSE                     # Licença BSD 3-Clause
├── include/                    # Headers públicos (.h / .hpp)
│   ├── core/                   # Versionamento, configuração e tipos fundamentais
│   ├── assembler/              # Módulo do montador Z80
│   ├── compiler/               # Pré-processador BASIC e gerador de .p
│   ├── emulator/               # Núcleo de emulação e protocolo de debug
│   ├── ui_tui/                 # Componentes TUI (FTXUI)
│   ├── ui_gui/                 # Componentes GUI (Dear ImGui)
│   └── cli/                    # Parser de linha de comando
├── src/                        # Código-fonte de implementação (.cpp / .c)
│   ├── main.cpp                # Ponto de entrada da aplicação
│   ├── core/                   # Lógica central
│   ├── assembler/              # Integração SJAsmPlus
│   ├── compiler/               # Lógica de pré-processamento BASIC
│   ├── emulator/               # Núcleo EightyOne / ZEsarUX
│   ├── ui_tui/                 # Telas TUI
│   ├── ui_gui/                 # Telas GUI
│   └── cli/                    # Comandos CLI
├── third_party/                # Bibliotecas e submódulos de terceiros
├── tests/                      # Bateria de testes unitários
├── tools/                      # Utilitários auxiliares de conversão e scripts
└── docs/                       # Documentação técnica e esquemáticos
```

---

## 6. Como Compilar e Executar

### Pré-requisitos
* Compilador C++17 (GCC 10+, Clang 11+ ou MSVC 2019+)
* CMake 3.15 ou superior
* PowerShell 7+ (recomendado no Windows)

### Build Automático via PowerShell (`build.ps1`)

O script `build.ps1` é a ferramenta recomendada de automação. Ele detecta os compiladores disponíveis, compila o projeto, organiza a pasta `dist/` e gera o pacote compactado `.zip` para distribuição:

```powershell
# Executar build padrão (compilação + dist/ + pacote ZIP)
.\build.ps1

# Limpar artefatos anteriores e recompilar
.\build.ps1 -Clean

# Gerar Release oficial (com verificação de tag e changelog)
.\build.ps1 -Release
```

### Build Manual com CMake

```bash
# 1. Configurar o build
cmake -B build -S .

# 2. Compilar
cmake --build build --config Release

# 3. Executar o binário
./build/zx81ide.exe
```

---

## 7. Roteiro e Status

Consulte o arquivo [SPEC.md](SPEC.md) para detalhes da arquitetura planejada e o [MANUAL.md](MANUAL.md) para o guia de uso e referência da API.

---

## 8. Licença

Este projeto é distribuído sob os termos da [Licença BSD 3-Clause](LICENSE).  
Copyright (c) 2026, Wilson "Barney" Pilon.
