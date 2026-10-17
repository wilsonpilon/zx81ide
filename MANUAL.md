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

## 5. Interface de Linha de Comando (CLI) e Modo Interativo

O **zx81ide** possui como módulo central a sua interface de linha de comando, permitindo tanto automação via argumentos quanto uma sessão interativa de console.

### 5.1. Argumentos de Linha de Comando
Ao executar o binário com parâmetros:
```bash
zx81ide [opcoes]
```

* `-h`, `--help`: Exibe a síntese de opções e ajuda da aplicação.
* `-v`, `--version`: Exibe a versão semântica, marcos de hardware e codenames.
* `--cli`: Inicia no console interativo CLI (padrão se nenhum argumento for fornecido).
* `--tui`: Inicia diretamente na interface gráfica de terminal (TUI retro MS-DOS).
* `--gui`: Inicia no modo desktop gráfico (em desenvolvimento).

### 5.2. Console Interativo CLI (`zx81ide>`)
Ao executar `zx81ide.exe` sem parâmetros, o console interativo é iniciado exibindo o banner do sistema e o prompt de comando:

```text
zx81ide> help
Comandos Disponiveis no Console CLI:
  help, ?           Exibe esta lista de comandos
  tui               Inicia a interface retro MS-DOS em modo texto (TUI)
  version, ver      Exibe os detalhes completos de versao e codenames
  clear, cls        Limpa a tela do console
  exit, quit, q     Encerra o zx81ide
```

### 5.3. Interface Retro de Terminal (TUI)
O comando `tui` (ou a flag `--tui`) aciona a interface inspirada nos softwares clássicos da era MS-DOS dos anos 1990 (Turbo Vision / Borland C++):
* **Paleta Sinclair ZX-81:** Estritamente **Preto**, **Branco** e **Cinza**.
* **Barra de Menus (Linha 1):** Fundo preto com texto branco.
  * `Arquivo` -> `Sair (Alt+X)`
  * `Help` -> `About... (F1)`
* **Desktop:** Área central com padrão quadriculado ASCII em cinza e branco (`░░▒▒`).
* **Barra de Status (Última Linha):** Fundo cinza com letras pretas, atalhos úteis e versão alinhada à direita (`| v0.1.1`).
* **Atalhos do Teclado:**
  * `Setas Esquerda / Direita`: Alterna entre os menus `Arquivo` e `Help`.
  * `Seta Abaixo / Enter`: Abre o submenu suspenso ou ativa o item selecionado.
  * `Esc`: Fecha submenus ou caixas de diálogo; sai do modo TUI quando no nível raiz.
  * `F1`: Abre imediatamente o diálogo modal Sobre (About).
  * `F10`: Alterna o foco da barra de menus.
  * `Alt+X` ou `Q`: Encerra o TUI e retorna ao console.

### 5.4. Gestão de Configurações e Temas (SQLite)
O **zx81ide** utiliza o **SQLite 3** como motor para armazenamento de parâmetros e preferências da aplicação no arquivo `zx81ide.db`:
* **Primeira Execução:** Se nenhum tema estiver configurado, o sistema abre um diálogo interativo solicitando a seleção:
  * `[1] normal`: Sinclair ZX-81 padrão (texto branco/cinza em fundo preto).
  * `[2] invertido`: Sinclair invertido (texto preto em fundo branco).
  * `[3] claro`: Tema claro.
  * `[4] escuro`: Tema escuro.
* **Comandos de Tema no Console CLI:**
  * `theme`: Exibe o tema atualmente ativo e a lista de temas suportados.
  * `theme <normal|invertido|claro|escuro>`: Altera instantaneamente o tema no banco SQLite.
* **Comandos de Configuração:**
  * `config` ou `config list`: Lista todas as configurações gravadas no banco `zx81ide.db`.
  * `config get <chave>`: Lê o valor de uma configuração.
  * `config set <chave> <valor>`: Salva ou atualiza uma configuração.

---

## 6. Sistema de Projetos Portáteis (`.zx81project`)

No **zx81ide**, os projetos são encapsulados em arquivos com extensão `.zx81project`. Cada arquivo `.zx81project` é um **banco de dados SQLite 3** autocontido e portátil, garantindo integridade ACID e facilitando o compartilhamento e backup do projeto em um único arquivo:

### 6.1. Estrutura Interna de um `.zx81project`
* `project_meta`: Tabela contendo metadados (nome, versão, autor, descrição, modelo ZX-81 alvo e fonte inicial).
* `project_settings`: Tabela chave-valor para parâmetros de compilação e preferências do projeto.
* `sources`: Fontes de código BASIC Sinclair, Dignified BASIC e Assembly Z80 (`main.bas`, `engine.asm`).
* `screens`: Telas gravadas nos formatos Standard 32x24 DFILE, ASCII, blocos e HRG.
* `binaries`: Imagens compiladas prontas (`.p`, `.bin`, `.hex`), com controle de tamanho e timestamp de build.
* `manuals`: Documentação do projeto, instruções de jogo e manuais em formato Markdown (`MANUAL.md`).
* `assets`: Mapas, dados binários de som e recursos gráficos.

### 6.2. Comandos CLI para Manipulação de Projetos
```text
zx81ide> project new <meu_jogo.zx81project> [nome] [autor]
zx81ide> project info <meu_jogo.zx81project>
zx81ide> project list <meu_jogo.zx81project>
zx81ide> project add-source <meu_jogo.zx81project> <arquivo.bas> [BASIC]
zx81ide> project view-source <meu_jogo.zx81project> <nome_fonte>
zx81ide> project export-source <meu_jogo.zx81project> <nome_fonte> <arquivo_saida>
zx81ide> project add-doc <meu_jogo.zx81project> <manual.md>
zx81ide> project view-doc <meu_jogo.zx81project> <titulo>
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
