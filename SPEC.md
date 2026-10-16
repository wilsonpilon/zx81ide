# SPEC.md - zx81ide Technical Specification
**Version:** 1.0.0
**Status:** Living Document / Planning Phase
**Inspiration:** fwMSX (`github.com/wilsonpilon/fwMSX`)

---

## 1. Visão Geral do Projeto
O **zx81ide** é um ecossistema integrado e multiplataforma de desenvolvimento para o Sinclair ZX-81 e seus principais clones (TK82/83/85, CP200, TS1000). A ferramenta unifica um ambiente de edição de texto com suporte a sintaxe estendida (Dignified BASIC / BASIC moderno), um editor de telas integrado (Standard 32x24 e HRG), um montador Z80, gerenciamento de projetos e um emulador embarcado com comunicação online bidirecional (troca de estado e debug em tempo real).

### Modos de Operação Desejados:
1. **CLI (Command Line Interface):** Automação de compilação, conversão de arquivos e empacotamento via linha de comando.
2. **TUI (Text User Interface):** Interface de terminal rica para edição rápida e gerenciamento leve.
3. **GUI (Graphical User Interface):** Ambiente visual completo (Desktop) com abas, painéis de inspeção de registradores e tela do emulador integrada.

---

## 2. Arquitetura e Linguagem Base
* **Linguagem Principal:** **C++17** (com partes em C puro para compatibilidade de bibliotecas de emulação).
* **Justificativa:** Reuso direto de componentes de emulação de ciclo a ciclo de CPU Z80, gerenciamento de memória flexível e alta performance para sincronização online com o emulador.

---

## 3. Stack Tecnológica: Ferramentas Principais e Alternativas

Cada componente crítico do ecossistema possui uma ferramenta principal recomendada e uma alternativa viável:

### 3.1. Montador Z80 (Z80 Assembler)
* **Principal:** **SJAsmPlus** (C++)
  * *Por que:* Suporte robusto a macros, inclusão condicional, arquivos de símbolos (`.sld`) e integração com scripts Lua. Pode ser embutido como biblioteca ou chamado via processo/pipe.
* **Alternativa:** **z80asm (z88dk)** (C)
  * *Por que:* Mais leve, ideal se houver restrições estritas de dependências no binário final.

### 3.2. Núcleo de Emulação e Comunicação Online
* **Principal:** **EightyOne Core (Adaptado em C++)**
  * *Por que:* É o emulador mais completo para ZX-81, suportando a ULA original, clocks variáveis, expansões de RAM e múltiplos clones.
* **Alternativa:** **ZEsarUX Core / Fuse Z80 Core**
  * *Por que:* Arquitetura modular excelente para extrair núcleos limpos de CPU e vídeo.

### 3.3. Pré-processador / Compilador BASIC
* **Principal:** **Custom Preprocessor + zmakebas (C)**
  * *Por que:* Permite traduzir a sintaxe estendida (Dignified Suite / estilo Boriel) para linhas BASIC padrão do ZX-81 e empacotá-las diretamente no formato de snapshot `.p`.
* **Alternativa:** **Boriel ZX BASIC Compiler (Backend customizado)**
  * *Por que:* Oferece um ecossistema maduro de geração de código intermediário Z80.

### 3.4. Editor de Telas (Screen/Tile Editor)
* **Principal:** **Módulo Interno Nativo (Matriz 32x24 + Bloco Gráfico de 64x48 / Modo HRG)**
  * *Por que:* Desenhado especificamente para mapear o *Display File* do ZX-81 e gerar código `DEFB` ou blocos `DATA`.
* **Alternativa:** **ZX81-Screen-Editor (James Bray)** como referência de UI/UX para grade de caracteres.

### 3.5. Camadas de Interface (TUI / GUI)
* **TUI Principal:** **FTXUI** (C++ moderno, baseado em componentes).
* **TUI Alternativa:** **NCurses / PDCurses**.
* **GUI Principal:** **Dear ImGui** (leve, rápido, excelente para ferramentas de desenvolvimento e visualização de memória/emulador em tempo real).
* **GUI Alternativa:** **Qt 6**.

---

## 4. Estrutura de Diretórios Proposta (`zx81ide`)

```text
zx81ide/
├── CMakeLists.txt              # Build system principal
├── include/
│   ├── core/                   # Gerenciamento de projeto e configs
│   ├── assembler/              # Integração com SJAsmPlus
│   ├── compiler/               # Pré-processador BASIC e gerador `.p`
│   ├── emulator/               # Núcleo do emulador e protocolo de comunicação
│   ├── ui_tui/                 # Interfaces de terminal (FTXUI)
│   └── ui_gui/                 # Interface gráfica (Dear ImGui)
├── src/                        # Implementações correspondentes
├── third_party/                # Dependências externas (SJAsmPlus core, zmakebas, etc.)
├── tests/                      # Testes unitários (Parser, Compilador, Gerador `.p`)
└── SPEC.md                     # Este documento de especificação